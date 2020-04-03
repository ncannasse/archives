(********************************************************************************)
(* Includes *)

open Mtlib
open Protocol
open Xml (* import / export *)
open Win32 (* for hardlink *)

(********************************************************************************)
(* Interface *)

type fid = int
type fh_r = fid * in_channel
type fh_w = fid * out_channel
type fdep = int
type ftype = int
type fver = int

type fileinfos = {	
	fi_id : fid;
	fi_name : string;
	fi_parent : fid;
	fi_ver : fver;
	fi_t : ftype;
}

type filehard = {
	fh_ext : string list;
	fh_import : fid -> string -> unit;
}

type fevent = fid -> unit

type filetype = {
	ft_name : string;
	ft_t : ftype;	
	ft_icon : Osiris.hres;
	ft_new : fevent option;
	ft_select : fevent option;
	ft_unselect : fevent option;
	ft_open : fevent option;
	ft_show : fevent option;
	ft_hide : fevent option;
	ft_close : fevent option;
	ft_context : (string * fevent) list;
	ft_delete : fevent option;
	ft_hardlinked : filehard option;
	ft_haschilds : bool;
	ft_export : ((fid -> Xml.xml) * (fid -> Xml.xml -> unit)) option;
}

exception No_such_file_id of fid
exception No_such_hlink of fid
exception No_such_file of string
exception File_already_exist of string
exception Invalid_parent of fid
exception Invalid_file_name of string
exception Type_already_registred
exception Unknown_ftype of string
exception Cannot_delete of fid
exception Cannot_rename of fid * string
exception Operation_not_supported
exception Hlink_no_data of fid

(********************************************************************************)
(* Types *)

type finfos = {
	mutable name : string;
	mutable parent : fid;
	mutable ver : fver;
	t : ftype;
}

type fdata = {
	mutable id : fid;
	infos : finfos;
	childs : fid MList.t;
	deps : fid MList.t;
	mutable isdel : bool
}

type hlink = {
	mutable hid : fid;
	mutable hver : fver;
	hname : string;
	mutable hfile : string option;
	mutable htime : int64;
}

type msg =
	| Create of string * fid * ftype * fid
	| Movefile of fid * fid * fid
	| Rename of fid * string * string
	| Update of fid
	| Deletefile of fid
	| Undelete of fid
	| CreateLink of fid

exception Continue

(********************************************************************************)
(* Globals *)

let config_filename = "config.xml"
let config = try (Xml.parse_file config_filename) with Xml.Error e -> failwith ("Invalid config file "^config_filename^" : "^(Xml.error e))


let fdata_file = "files"
let fdata_filedata = "data"
let fdeps_file = "deps"
let hlink_file = "links"
let msgs_file = "msgs"

let get_tree att = try Xml.attrib config att with Xml.No_attribute _ -> failwith "Incomplete config file"

let project_name = get_tree "project"

let file_path = project_name
let host_name = get_tree "host"
let user_name = get_tree "user"
let user_pass = get_tree "pass"
let islocal = try ((String.lowercase (Xml.attrib config "local" )) = "true") with Xml.No_attribute _ -> false
let isdebug = try ((String.lowercase (Xml.attrib config "debug")) = "true") with Xml.No_attribute _ -> false

let max_id = ref 1
let base_tmp_id = 1 lsl 29
let tmp_id = ref base_tmp_id
let root_bind = Global.empty "Vfs ROOT"

let all_files_data = Hashtbl.create 0
let tmp_files_data = Hashtbl.create 0
let all_files = MList.empty()
let tmp_files = MList.empty()
let all_types = MList.empty()
let hlinks = MList.empty()
let messages = MList.empty()

let on_refresh = ref (fun () -> ())

let do_refresh () =
	!on_refresh()

let path = file_path^"/"

let is_tmp fid = fid >= base_tmp_id

let find_file fid = 
	try
		MList.find (fun data -> data.id = fid) (if is_tmp fid then tmp_files else all_files) 
	with
		Not_found -> raise (No_such_file_id fid)

let root () = find_file (Global.get root_bind)

let rec get_file_path id =
	if id = Global.get root_bind then
		":"
	else
		let f = (find_file id).infos in
			((get_file_path f.parent)^"/"^f.name)	

let string_of_ftype t =
	(Printf.sprintf "%c%c%c" (Char.chr (t land 255)) (Char.chr ((t lsr 8) land 255)) (Char.chr ((t lsr 16) land 255)))

let string_of_finfos i =
	(Printf.sprintf "{ name = \"%s\"; parent = %d; ver = %d; type = %s; }" i.name i.parent i.ver (string_of_ftype i.t))

let string_of_fdata d =
	(Printf.sprintf "{ id = %d; infos = %s; childs <...>; deps <...>; }" d.id (string_of_finfos d.infos))

let string_of_msg m =
	match m with
	| Create(name,parent,ftype,newid) -> Printf.sprintf "Creating %s/%s (%s)" (get_file_path parent) name (string_of_ftype ftype)
	| Movefile(id,idfrom,idto) -> Printf.sprintf "Moving %s/%s to %s" (get_file_path idfrom) (find_file id).infos.name (get_file_path idto) 
	| Rename(id,oldname,newname) -> Printf.sprintf "Renaming %s/%s to %s" (get_file_path (find_file id).infos.parent) oldname newname
	| Update(id) -> Printf.sprintf "Check out %s" (get_file_path id)
	| Deletefile(id) -> Printf.sprintf "Deleting %s" (get_file_path id)
	| Undelete(id) -> Printf.sprintf "Restoring %s" (get_file_path id)
	| CreateLink(id) -> Printf.sprintf "Creating hlink %s" (get_file_path id)

let name_of_file id =
	(find_file id).infos.name

let hash_key_1 = 0x6e5ea73858134343
let hash_key_2 = 0xb34e8f99a2ec9ef5

let fid_hash id =
	let k = ref id in
		k := (!k lxor ((hash_key_1 lxor !k) asr 31)) * hash_key_1;
		k := (!k lxor ((hash_key_2 lxor !k) asr 30)) * hash_key_2;
		k := (!k lxor ((hash_key_1 lxor !k) asr 31));
		!k

let fid_compare ida idb =
	compare (name_of_file ida) (name_of_file idb)

(********************************************************************************)
(* LOAD & SAVE *)

let do_sync_local = ref (fun () -> ())

let full_file_name file =
	file_path^"/."^file

let read_fdata file =
	let id = (input_value file : fid) in
	let infos = (input_value file : finfos) in
	let clist = (input_value file : fid list) in
	let isdel = (input_value file : bool ) in
	{
		id = id;
		infos = infos;
		childs = MList.from_list clist;
		deps = MList.empty();
		isdel = isdel
	}

let read_tree () =
	let f = (open_in_bin (full_file_name fdata_file)) in
	let n = (input_binary_int f) in	
	let data = (read_fdata f) in
	Global.set root_bind data.id;
	MList.add all_files data;
	max_id := data.id;
	for i = 1 to (n-1) do
		let data = (read_fdata f) in
		if !max_id < abs(data.id) then max_id := abs(data.id);
		MList.add all_files data;
	done;
	(close_in f)

let read_datas () =
	let fh = open_in_bin (full_file_name fdata_filedata) in
	let (h:(fid,string) Hashtbl.t) = input_value fh in
		Hashtbl.clear all_files_data;
		Hashtbl.iter (fun a b -> Hashtbl.add all_files_data a b) h;
		close_in fh

let read_deps () =
	let f = open_in_bin (full_file_name fdeps_file) in
	let n = (input_binary_int f) in	
	for i = 1 to n do
		let id = input_binary_int f in
		MList.copy_list (find_file id).deps (input_value f);		
	done;
	close_in f

let read_hlinks () =
	let f = open_in_bin (full_file_name hlink_file) in
	MList.copy_list hlinks (input_value f);
	close_in f


let read_messages () =
	let f = open_in_bin (full_file_name msgs_file) in
	MList.copy_list messages (input_value f);
	close_in f

let save_datas () =
	try
		let fh = open_out_bin (full_file_name fdata_filedata) in
		output_value fh all_files_data;
		close_out fh;
	with
		Sys_error msg -> failwith("Cannot save filesystem datas ("^msg^")")

let save_tree () =
	let output_fdata file data =
		(output_value file data.id);
		(output_value file data.infos);
		(output_value file (MList.to_list data.childs));
		(output_value file data.isdel)
	in
	try
		let f = open_out_bin (full_file_name fdata_file) in
		let r = root() in
		if islocal then begin
			!do_sync_local();
			MList.clear messages;
		end;
		output_binary_int f (MList.length all_files);
		output_fdata f r;
		MList.iter (fun data -> if data.id <> r.id then (output_fdata f data)) all_files;
		close_out f;
		do_refresh();
	with
		Sys_error msg -> do_refresh(); failwith("Cannot save filesystem ("^msg^")")

let save_messages () =
	try
		let f = open_out_bin (full_file_name msgs_file) in
		output_value f (MList.to_list messages);
		close_out f;
	with
		Sys_error msg -> failwith("Cannot load messages ("^msg^")")

let save_deps () =
	try
		let f = open_out_bin (full_file_name fdeps_file) in
		output_binary_int f (MList.length all_files);
		MList.iter (fun data ->
							output_binary_int f data.id;
							output_value f (MList.to_list data.deps))
					all_files;
		close_out f;
	with
		Sys_error msg -> failwith("Cannot save filesystem deps ("^msg^")")

let save_hlinks () =
	try
		let f = open_out_bin (full_file_name hlink_file) in
		output_value f (MList.to_list hlinks);
		close_out f;
	with
		Sys_error msg -> failwith("Cannot save filesystem hlinks ("^msg^")")

(********************************************************************************)
(* Funcs *)

let find_child parent name =
	try
		let found = ref None in
		ignore(MList.find
			(fun x ->
				let data = (find_file x) in
				if not data.isdel && data.infos.name = name then begin
					found := Some data;
					true;
				end else
					false)
			 parent.childs);
		(match !found with
		| None -> assert false;
		| Some p -> p)
	with
		Not_found -> raise (No_such_file name)

let rec has_parent id pid =
	if id = Global.get root_bind then false else
	let data = (find_file id) in
		data.infos.parent = pid || (has_parent data.infos.parent pid)

let rec check_before_delete id =
	let data = (find_file id) in
	try
		let f = MList.find 
			(fun x ->
				if x.isdel || x.id = id then false else begin
					try
						ignore (MList.find (fun d -> d = id) x.deps);
						true;
					with
						Not_found -> false;
				end
			) all_files 
		in
		failwith ("Cannot delete, this file is used by "^(get_file_path f.id));
	with
		Not_found -> MList.iter check_before_delete data.childs
			
let rec do_delete_file id =
	(check_before_delete id);
	let data = (find_file id) in
		data::(List.concat (List.map do_delete_file (MList.to_list data.childs)))

let check_char c =
	(c >= 'a' && c <='z')
	|| (c >= 'A' && c <='Z')
	|| (c >= '0' && c <='9')
	|| c = ' '
	|| c = '!'
	|| c = 'é'
	|| c = 'è'
	|| c = 'à'
	|| c = '.'
	|| c = '-'
	|| c = '_'
	|| c = '('
	|| c = ')'

let check_file_name name =
	if (String.length name) = 0 then false
	else begin
		let result = ref true in
		for i = 0 to (String.length name)-1 do
			if not (check_char (String.get name i)) then result := false
		done;
		!result;
	end

let not_allowed_for_tmp id =
	if is_tmp id then raise Operation_not_supported

(********************************************************************************)
(* File Types Func *)

let get_ftype s =
	(Char.code (String.get s 0))+((Char.code (String.get s 1)) lsl 8)+((Char.code (String.get s 2)) lsl 16)

let get_ftype_infos ft =
	try
		MList.find (fun x -> x.ft_t = ft) all_types;
	with
		Not_found -> raise (Unknown_ftype (string_of_ftype ft))

let get_group t =
	List.map (fun f -> f.id) (List.filter (fun f -> not f.isdel && f.infos.t = t) (MList.to_list all_files))

let register ft =
	try
		ignore (MList.find (fun x -> x.ft_t = ft.ft_t) all_types);
		raise Type_already_registred;
	with
		Not_found ->
			MList.add all_types ft

let enum_ftype func =
	MList.iter func all_types

(********************************************************************************)
(* Files Func *)

let update_file id =
	try
		ignore (MList.find
			(fun m ->
				match m with
					Update(x) -> x = id
					| _ -> false)
			messages)
	with
		Not_found ->
			let version = (find_file id).infos.ver in
				MList.add messages (Update(id));
				save_messages()

let refresh_callback func =
	on_refresh := func

let get_root () = Global.get root_bind

let get_all_files () =
	List.map (fun f -> f.id) (List.filter (fun f -> not f.isdel) (MList.to_list all_files))

let enum_childs func id =
	let f = (find_file id) in
	let l = ref [] in
	MList.iter (fun x -> if not (find_file x).isdel then l := (func x)::!l;) f.childs;
	List.rev !l

let get_file_infos id =
	let i = (find_file id).infos in 
	{
		fi_id = id;
		fi_name = i.name; 
		fi_parent = i.parent;
		fi_ver = i.ver;
		fi_t = i.t;
	}

let read id =
	try
		Marshal.from_string (Hashtbl.find (if is_tmp id then tmp_files_data else all_files_data) (fid_hash id)) 0;
	with 
		Not_found -> failwith ("Reading a non written file "^(get_file_path id))

let write id data =
	let tmp = is_tmp id in
	Hashtbl.replace (if tmp then tmp_files_data else all_files_data) (fid_hash id) (Marshal.to_string data [Marshal.Closures]);
	if not tmp then begin 
		update_file id;
		save_datas();
	end

let rename_file id name =
	not_allowed_for_tmp id;
	let f = (find_file id) in
	if id = get_root() then raise (Cannot_rename (id,name));
	if not(check_file_name name) then raise (Invalid_file_name name);
	let p = (find_file f.infos.parent) in
		try
			ignore(find_child p name);
			raise (File_already_exist name);
		with
			No_such_file _ ->
				let old = f.infos.name in
				f.infos.name <- name;
				MList.add messages (Rename(id,old,name));
				save_tree()

let move_file ~file ~parent =
	not_allowed_for_tmp file;
	if file = parent || (has_parent parent file) then raise (Invalid_parent parent);	
	let p = (find_file parent) in	
	let f = (find_file file) in
	let oldp = (find_file f.infos.parent) in
		f.infos.parent <- parent;
		MList.add messages (Movefile(file,oldp.id,parent));
		MList.remove oldp.childs file;
		MList.add p.childs file;
		MList.sort fid_compare p.childs;
		save_tree();
		save_messages()

let get_fid path =
	let rec find_child_rec parent path =
		try
			let name,path = MString.split_char path '/' in
			(find_child_rec (find_child parent name) path);
		with 
			Not_found -> (find_child parent path) in
	try
		(find_child_rec (root()) path).id
	with
		No_such_file _ -> raise (No_such_file path)

let has_childs id =
	let data = (find_file id) in
	try
		ignore(MList.find (fun id -> not (find_file id).isdel) data.childs);
		true;
	with
		Not_found -> false

let do_create ~name ~parent ~ftype ~istmp ~hlink =
	ignore (
	try
		let t = MList.find (fun x -> x.ft_t = ftype) all_types in
		if hlink <> (t.ft_hardlinked <> None) then failwith ("The file "^name^" should hasn't the good link type");
	with
		Not_found -> raise (Unknown_ftype (string_of_ftype ftype))
	);
	let parent_data = (find_file parent) in
	try
		ignore (find_child parent_data name);		
		raise (File_already_exist name)
	with
		No_such_file _ ->
			if not(check_file_name name) then raise (Invalid_file_name name);		
			if istmp then incr tmp_id else incr max_id;
			let baseid = (if istmp then !tmp_id else !max_id) in
			let id = (if istmp || islocal then baseid else -baseid) in
			let data = {
				id = id;
				infos = { name = name; parent = parent; ver = 0; t = ftype; };
				childs = MList.empty();
				deps = MList.empty();
				isdel = false;
				} in
			if not istmp then begin
				MList.add messages (Create(name,parent,ftype,data.id));
				MList.add all_files data;
				MList.add parent_data.childs data.id;
				MList.sort fid_compare parent_data.childs;
				save_tree();
				save_messages();
			end else
				MList.add tmp_files data;
			data.id

let create_file ~name ~parent ~ftype =
	not_allowed_for_tmp parent;
	do_create ~name ~parent ~ftype ~istmp:false ~hlink:false

let create_tmp ~name ~ftype =
	do_create ~name ~parent:(get_root()) ~ftype ~istmp:true ~hlink:false

let delete_file id =
	not_allowed_for_tmp id;
	if id = get_root() then raise (Cannot_delete id);
	let l = do_delete_file id in
		(List.iter (fun data ->
			data.isdel <- true;
			(match (get_ftype_infos (data.infos.t)).ft_delete with
				None -> ()
				| Some func -> func data.id);
			MList.add messages (Deletefile(data.id))
		) (List.rev l));
	save_tree();
	save_messages()

(********************************************************************************)
(* File Deps *)

let add_dep id depid =
	let data = (find_file id) in	
	try
		MList.index_of data.deps depid;
	with
		Not_found ->
			update_file id;
			try				
				let pos = MList.index_of data.deps 0 in
					MList.set data.deps pos depid;
					save_deps();
					pos;
			with
				Not_found ->
					MList.add data.deps depid;
					save_deps();
					(MList.length data.deps)-1

let enum_deps func id =
	let data = (find_file id) in
	MList.iter (fun x -> if x <> 0 then func x) data.deps

let get_dep id dep =
	let data = (find_file id) in	
	try
		MList.at_index data.deps dep
	with
		Invalid_index n -> failwith ("Invalid dependency for file "^(get_file_path id)^"@"^(itos n))

let del_dep id depid =
	let data = (find_file id) in
	MList.set data.deps depid 0;
	update_file id;
	save_deps()

let clear_deps id =
	let data = (find_file id) in
	MList.clear data.deps;
	update_file id;
	save_deps()

(********************************************************************************)
(* Hardlink *)

let get_file_time file =
	let h = Win32.create_file ~name:file ~access:(Win32.access_mask_of_right Win32.GENERIC_READ) ~share:[] ~security:None ~creation:Win32.OPEN_EXISTING ~attributes:[] ~flags:[] ~sqos:[] ~template:Win32.null_handle in
	let date = Win32.get_file_time h in
		Win32.close_handle h;
		date.ft_last_write_time

let find_hlink id =
	try
		MList.find (fun h -> h.hid = id) hlinks;
	with
		Not_found -> raise (No_such_hlink id)

let create_hlink ~name ~file ~parent ~ftype = 
	not_allowed_for_tmp parent;
	let p = (try String.rindex file '\\' with Not_found -> (-1)) in
	let filename = String.sub file (p+1) ((String.length file)-p-1) in	
	let id = do_create ~name ~parent ~ftype ~istmp:false ~hlink:true in	
	MList.add hlinks { hid = id; hname = filename; hfile = Some file; htime = Int64.zero; hver = 0 };
	MList.add messages (CreateLink(id));
	save_hlinks();
	save_messages();
	id

let open_in_hlink id =
	let h = find_hlink id in
	match h.hfile with
	| None -> raise (Hlink_no_data id)
	| Some f ->
		try
			open_in_bin f
		with
			Sys_error _ -> raise (Hlink_no_data id)

let name_of_hlink id =
	let h = find_hlink id in
	h.hname

let size_of_hlink id =
	let ch = open_in_hlink id in
	let data = read_all_file ch in
	close_in ch;
	String.length data

let find_hlink_local locid name =
	let pid = (if locid != get_root() then (find_file locid).infos.parent else locid) in
	try
		(MList.find (fun h -> h.hname = name && (find_file h.hid).infos.parent = pid) hlinks).hid
	with
		Not_found ->
			(MList.find (fun h -> h.hname = name ) hlinks).hid

(********************************************************************************)
(* Import / Export *)

let export_fdep d = itos d
let import_fdep s = int_of_string s

let rec export_tree id =
	let infos = find_file id in
	let data = infos.infos in
	Element (
		"item",
		[("id",itos id);("name",data.name);("type",string_of_ftype data.t);("ver",itos data.ver);("isdel",if infos.isdel then "true" else "false")],
		List.map export_tree (MList.to_list infos.childs)
	)

let rec export_dep infos =
	Element (
		"dep",
		[("id",itos infos.id)],
		List.map (fun d -> Element ("did",[("id",itos d)],[])) (MList.to_list infos.deps)
	)

let rec export_data infos =
	let efunc = get_ftype_infos infos.infos.t in
	Element (
		"data",
		[("id",itos infos.id)],
		[(match efunc.ft_export with
			None -> Element ("nodata",[],[])
			| Some (exp,_) -> exp infos.id)
		]
	)

let export_hlink h =
	Element (
		"hlink",
		[("id",itos h.hid);
			("name",h.hname);
			("file",match h.hfile with None -> "" | Some f -> f);
			("time",Int64.to_string h.htime);
			("version",itos h.hver)
		],
		[]
	)

let export_message m =
	let msg_data = function
		| Create(name,pid,ftype,newid) -> [("type","create");("name",name);("parent",itos pid);("ftype",string_of_ftype ftype);("id",itos newid)]
		| Movefile(id,oldp,newp) -> [("type","move");("id",itos id);("src",itos oldp);("dst",itos newp)]
		| Rename(id,oldname,newname) -> [("type","rename");("id",itos id);("old",oldname);("new",newname)]
		| Update(id) -> [("type","update");("id",itos id)]
		| Deletefile(id) -> [("type","delete");("id",itos id)]
		| Undelete(id) -> [("type","undelete");("id",itos id)]
		| CreateLink(id) -> [("type","link");("id",itos id)]
	in
	Element("msg",msg_data m,[])

let has_dep data dataid =
	try
		ignore(MList.find (fun id -> id = dataid.id) data.deps);
		true;
	with
		Not_found -> false

let rec dep_sorting_func l =
	match l with
	| [] -> []
	| a::l ->
		try
			let item = List.find (has_dep a) l in
			let list = MList.from_list l in
				MList.remove list item;
				dep_sorting_func (item::a::(MList.to_list list));
		with
			Not_found -> a::(dep_sorting_func l)

let export() =
	let root = get_root() in
		Element (
			"vfs",
			[],
			[export_tree root]@
				(List.map export_dep (MList.to_list all_files))@
				(List.map export_hlink (MList.to_list hlinks))@
				(List.map export_message (MList.to_list messages))@
				(List.map export_data
					(dep_sorting_func (MList.to_list all_files)))
		)

let get_attrib x att =
	(try
		Xml.attrib x att
	with
		No_attribute _ -> failwith ("Missing attribute for "^(Xml.tag x)^": "^att))

let get_int x att =
	try
		int_of_string (get_attrib x att)
	with
		Failure _ -> failwith ("The attribute "^att^" of "^(Xml.tag x)^" should be an integer")

let get_bool x att =
	match get_attrib x att with
	| "true" -> true
	| "false" -> false
	| _ -> failwith ("The attribute "^att^" of "^(Xml.tag x)^" should be a boolean")

let rec import_tree parent x =
	let get_attrib = get_attrib x in
	let get_int = get_int x in
	let get_bool = get_bool x in
	let id = get_int "id"
		and name = get_attrib "name"
		and t = get_attrib "type"
		and ver = get_int "ver"
		and isdel = get_bool "isdel" in
		MList.add all_files {
			id = id;
			infos = { name = name; parent = parent; ver = ver; t = get_ftype t };
			childs = MList.from_list (List.map (import_tree id) (Xml.children x));
			deps = MList.empty();
			isdel = isdel;
		};
		id
			
let import_data x =
	let get_attrib = get_attrib x in
	match Xml.tag x with
	| "item" -> ignore(import_tree 0 x)
	| "dep" ->
		let id = get_int x "id" in
		MList.copy_list (find_file id).deps (List.map (fun xdep -> get_int xdep "id") (Xml.children x));
	| "hlink" ->
		let id = get_int x "id" and
			time = Int64.of_string (get_attrib "time") and
			file = get_attrib "file" and
			name = get_attrib "name" and
			version = get_int x "version" in
			MList.add hlinks {
				hid = id;
				htime = time;
				hfile = if String.length file = 0 then None else Some file;
				hname = name;
				hver = version;
				}
	| "msg" ->
		MList.add messages (match get_attrib "type" with
		| "create" -> Create(get_attrib "name",get_int x "parent",get_ftype (get_attrib "ftype"),get_int x "id")
		| "move" -> Movefile(get_int x "id",get_int x "src",get_int x "dst")
		| "rename" -> Rename(get_int x "id",get_attrib "old",get_attrib "new")
		| "update" -> Update(get_int x "id")
		| "delete" -> Deletefile(get_int x "id")
		| "undelete" -> Undelete(get_int x "id")
		| "link" -> CreateLink(get_int x "id")
		| _ -> failwith ("Invalid message name: "^(get_attrib "type"))
		)
	| "data" ->
		let id = get_int x "id" in
		(match (get_ftype_infos (find_file id).infos.t).ft_export with
			| None -> ()
			| Some(_,imp) -> imp id (List.hd (Xml.children x));
		)
	| _ -> failwith ("Invalid xml item: "^(Xml.tag x))

let import x =
	let saved_files = MList.to_list all_files and
		saved_data = Hashtbl.copy all_files_data and
		saved_hlinks = MList.to_list hlinks and
		saved_msgs = MList.to_list messages
		in
		try
			MList.clear all_files;
			MList.clear hlinks;
			MList.clear messages;
			Hashtbl.clear all_files_data;			
			if Xml.tag x <> "vfs" then failwith "Not a vfs exported file";
			List.iter import_data (Xml.children x);
			save_tree();
			save_datas();
			save_deps();
		with
			e ->
				MList.copy_list all_files saved_files;
				MList.copy_list messages saved_msgs;
				MList.copy_list hlinks saved_hlinks;
				Hashtbl.iter (Hashtbl.replace all_files_data) saved_data;
				raise e

(********************************************************************************)
(* Synchronization *)

let change_id locid realid =
	let sync_messages () =
		MList.copy_list messages (List.map (fun msg ->
			match msg with
				| Create(name,parent,ftype,newid) ->
					if parent = locid then Create(name,realid,ftype,newid) else msg
				| Movefile(id,idfrom,idto) ->
					if id = locid then Movefile(realid,idfrom,idto)
					else if idfrom = locid then Movefile(id,realid,idto)
					else if idto = locid then Movefile(id,idfrom,realid)
					else msg
				| Rename(id,oldname,newname) ->
					if id = locid then Rename(realid,oldname,newname)
					else msg
				| Update(id) -> 
					if id = locid then Update(realid)
					else msg
				| Deletefile(id) ->
					if id = locid then Deletefile(realid)
					else msg
				| Undelete(id) ->
					if id = locid then Undelete(realid)
					else msg
				| CreateLink(id) ->
					if id = locid then CreateLink(realid)
					else msg
		) (MList.to_list messages))
	and
	sync_deps () =
		MList.iter
			(fun f ->
				try
					let p = MList.index_of f.deps locid in
					MList.set f.deps p realid
				with Not_found -> ()
			)	
			all_files
	and
	sync_hlinks () =
		MList.iter (fun h -> if h.hid = locid then h.hid <- realid) hlinks
	and
	sync_data () = 
		try
			let data = (Hashtbl.find all_files_data (fid_hash locid)) in
			Hashtbl.remove all_files_data (fid_hash locid);
			Hashtbl.add all_files_data (fid_hash realid) data;
			save_datas();
		with
			Not_found -> ()
	in
		sync_messages();
		sync_hlinks();
		sync_deps();
		sync_data()

let really_delete_file id =
	let infos = (find_file id) in
	let pinfos = (find_file infos.infos.parent) in
		MList.remove pinfos.childs id;
		MList.remove all_files infos;
		Hashtbl.remove all_files_data (fid_hash id);		
		save_datas();
		save_tree();
		(try
			MList.remove hlinks (MList.find (fun h -> h.hid = id) hlinks);
			save_hlinks();
		with
			Not_found -> ())

let do_sync_send_msg func h =
	MList.iter
	(fun msg ->
		func (string_of_msg msg);
		match msg with
			| Create(name,parent,ftype,locid) ->
				let infos = (find_file locid) in
				let pinfos = (find_file infos.infos.parent) in
				let p = try (MList.index_of pinfos.childs locid) with Not_found -> -1 in (* if file deleted ! *)
				let id,name = Protocol.create h name parent ftype in
				MList.set pinfos.childs p id;
				change_id locid id;
				infos.id <- id;
				infos.infos.name <- name;
				MList.iter (fun f -> (find_file f).infos.parent <- id) infos.childs;
				MList.remove messages msg;
				save_tree();
				save_messages();
				save_hlinks();
				save_deps();
				raise Continue;
			| Rename(id,oldname,newname) ->
				if not (Protocol.rename h id newname) then begin
					(find_file id).infos.name <- oldname;
					Osiris.message_box ("Failed to rename "^(get_file_path id)^" to "^newname);
				end;
				MList.remove messages msg;
				save_tree();
				save_messages();
			| Deletefile(id) ->
				Protocol.delete h id;
				really_delete_file id;
				MList.remove messages msg;
				save_messages();
			| Undelete(id) ->
				Protocol.undelete h id (find_file id).infos.parent;
				MList.remove messages msg;
				save_tree();
				save_messages();
			| Movefile(id,oldp,newp) ->
				Protocol.move h id newp;
				MList.remove messages msg;				
				save_messages();
			| Update(id) ->
				let inf = find_file id in
				let data = (try Hashtbl.find all_files_data (fid_hash id) with Not_found -> "") in
				if Protocol.update ~phandle:h ~id ~version:inf.infos.ver ~deps:(MList.to_list inf.deps) ~data then begin
					inf.infos.ver <- inf.infos.ver + 1;
					save_tree();
				end;
				MList.remove messages msg;
				save_messages();
			| CreateLink(id) ->
				let link = find_hlink id in
				Protocol.create_link h id link.hname;
				MList.remove messages msg;
				save_messages();
	) messages

let sync_local () =
	let do_save_deps = ref false in
	MList.iter
		(fun msg ->
			match msg with
				| Deletefile(id) ->
					really_delete_file id;
					do_save_deps := true;
				| _ -> ()
		) messages;
	if !do_save_deps then save_deps();
	MList.clear messages;
	save_messages();
;;
do_sync_local := sync_local

let rec	do_sync_send func h =
	try
		do_sync_send_msg func h;
	with
		| Protocol.ID_out(-1) ->
			(* auto restore deleted file *)			
			let id = Protocol.get_last_id() in
			ignore(
			try
				let infos = (find_file id) in
				MList.push messages (Undelete id);
				do_sync_send func h;
			with
				No_such_file_id _ -> failwith ("Failed to restore #"^(itos id))
			);
		| Protocol.Invalid_version(id,ver) ->
			(* prompt for replace *)
			let msg = Printf.sprintf "The file %s has local version %d but has server version %d\nDo you want to replace it on server ?"
						(get_file_path id) (find_file id).infos.ver ver in
			(match Osiris.question_cancel_box msg with
				| Osiris.QR_YES -> (find_file id).infos.ver <- ver; save_tree(); do_sync_send func h;
				| Osiris.QR_NO -> MList.remove_at_index messages 0; save_messages(); do_sync_send func h;
				| Osiris.QR_CANCEL -> failwith "Synchronize aborted";
			);
		| Continue -> do_sync_send func h

let do_sync_hlink func h link =	
	match link.hfile with
	| None -> ()
	| Some f ->
		try 
			let time = get_file_time f in
			if time > link.htime then begin
				func ("Checking out data : "^(get_file_path link.hid));
				let ch = open_in_bin f in
				let data = read_all_file ch in
				close_in ch;
				Protocol.update_link h link.hid link.hver data;
				link.hver <- link.hver + 1;
				link.htime <- time;
				save_hlinks();
			end
		with
			Win32.Error _ ->
				link.hfile <- None

let checkout_hlink func id =
	let data = find_hlink id in	
	let h = connect host_name project_name user_name user_pass in
	try
		let link = Protocol.get_link h id in
		(match data.hfile with
		| None -> ()
		| Some filename ->
			if Sys.file_exists filename && data.hver >= link.pver then raise Exit;
		);
		(match Protocol.checkout_link h id with
		| None -> failwith "No hlink data";
		| Some linkdata ->
			func ("Getting Data : "^data.hname);
			let filename = full_file_name ((itos id)^"@"^data.hname) in
			let f = open_out_bin filename in
			output_string f linkdata;
			close_out f;
			data.hfile <- Some filename;
			data.hver <- link.pver;
			data.htime <- get_file_time filename;
			save_hlinks();
		);
		raise Exit;
	with
		| Exit -> close h
		| e -> close h; raise e

let rec do_sync_recv files func h id =
	let infos = Protocol.get_infos h id in
	let childs = Protocol.get_childs h id in	
	let deps = ref [] in
		(try
			let inf = find_file id in
			if inf.infos.ver < infos.pver then raise (No_such_file_id id);
			deps := MList.to_list inf.deps;
		with
			No_such_file_id _ -> 
				func ("Checking out "^infos.pname);
				(match Protocol.checkout h id with
				| None -> ()
				| Some data ->
					Hashtbl.replace all_files_data (fid_hash id) data.pdata;
					deps := data.pdeps);
		);
		let ftype = if infos.ptype = 0 then get_ftype "DIR" else infos.ptype in		
		MList.add files {
			id = id;
			infos = {
				name = infos.pname;
				parent = infos.pparent;
				t = ftype;
				ver = infos.pver; };
			childs = MList.from_list childs;
			deps = MList.from_list !deps;
			isdel = false;
		};
		if (get_ftype_infos ftype).ft_hardlinked <> None then begin
			let link = Protocol.get_link h id in
			(try
				let found = find_hlink id in
				if found.hver < link.pver then found.hfile <- None;
			with
				No_such_hlink _ ->
					MList.add hlinks { hid = id; hname = link.pname; hfile = None; htime = Int64.zero; hver = link.pver };
			);
			save_hlinks();
		end;
		match childs with
		| [] -> ()
		| _ ->
			if infos.pparent <> 0 then func ("Getting "^(get_file_path id));
			List.iter (do_sync_recv files func h) childs

let sync func =
	let h = connect host_name project_name user_name user_pass in
	try
		do_sync_send func h;
		MList.iter (do_sync_hlink func h) hlinks;
		let files = MList.empty() in
		do_sync_recv files func h (Global.get root_bind);
		MList.copy_list all_files (MList.to_list files);
		save_deps();
		save_tree();
		raise Exit;
	with
		| Exit -> close h
		| e -> close h; raise e


let rec export_tree path id =
	let f = find_file id in	
	if not f.isdel then begin
		if (get_ftype_infos f.infos.t).ft_haschilds then begin		
			if f.id <> get_root() then begin
				let newpath = (path^f.infos.name^"/") in
				Win32.create_directory newpath None;
				MList.iter (export_tree newpath) f.childs;
			end else
				MList.iter (export_tree path) f.childs;
		end;
		try
			let h = find_hlink id in
			if Some (path^h.hname) <> h.hfile then begin
				let ch = open_in_hlink id in
				let data = read_all_file ch in
				close_in ch;
				if Sys.file_exists (path^h.hname) then
					(match Osiris.question_cancel_box ("The file "^(path^h.hname)^" already exists : overwrite it ?") with
						| Osiris.QR_YES -> ()
						| Osiris.QR_NO -> h.hfile <- Some (path^h.hname); save_hlinks(); raise Exit
						| Osiris.QR_CANCEL -> failwith "Export aborted"
					);
				let ch = open_out_bin (path^h.hname) in
				output_string ch data;
				close_out ch;
				(match h.hfile with
					| None -> assert false
					| Some filename -> Win32.delete_file filename);
				h.hfile <- Some (path^h.hname);
				save_hlinks();
			end;		
		with
			| Exit -> ()
			| No_such_hlink _ -> ()
			| Hlink_no_data _ -> checkout_hlink (fun _ -> ()) id; export_tree path id;
	end

(********************************************************************************)
(* Init *)

let init () =
	try
		read_tree();
		read_deps();
		read_datas();
		read_hlinks();
		read_messages();
	with		
		Sys_error s ->
			if not islocal then begin
				let h = connect host_name project_name user_name user_pass in
				let rid = Protocol.get_root h in
				Global.set root_bind rid;				
				do_sync_recv all_files (fun _ -> ()) h rid;			
				close h;
			end else begin
				let rid = 1 in
				Global.set root_bind rid;
				MList.add all_files {
					id = rid;
					infos = {
						name = "/";
						parent = 0;
						t = get_ftype "DIR";
						ver = 0; };
					childs = MList.empty();
					deps = MList.empty();
					isdel = false;
				};
			end;
			save_tree();
			save_datas();
			save_deps();
			save_hlinks();
			save_messages();
;;
Callback.register "vfs.get_file_type" (fun id -> (get_file_infos id).fi_t);
Callback.register "vfs.get_file_path" get_file_path;
Callback.register "vfs.get_hlink"
	(fun id ->
		try
			let h = find_hlink id in
			match h.hfile with			
			| Some file when Sys.file_exists file -> h.hfile
			| _ -> raise (Hlink_no_data id)
		with
			No_such_hlink _ -> None
	);

Callback.register "vfs.find_hlink" (fun name -> try Some (find_hlink_local (get_root()) name) with Not_found -> None);

open Run;;

register {
	m_name = "Vfs";
	m_init = Some init;
	m_close = None;
	m_main = None;
}

(********************************************************************************)
