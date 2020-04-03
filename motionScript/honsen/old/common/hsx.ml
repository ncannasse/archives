type rsid = int * string

type mts = string

type id = {
	id_obj : string;
	id_id : rsid;
	mutable id_bound : bool;
}

type obj = {
	obj_tag : string;
	obj_name : string;
	obj_data : Mtw.t list;
	obj_id : rsid option;
}

type mtw = string

type atw = string

type pic = {
	pic_file : string;
	pic_id : rsid option;
	pic_wrap : bool;
}

type font = {
	font_file : string option;
	font_id : rsid option;
	font_bold : bool;
	font_italic : bool;
	font_size : int;
	font_face : string;
}

type infos = {
	inf_width : int;
	inf_height : int;
	inf_bgcolor : int;
	inf_fps : int;
	inf_compress : bool;
}

type dir = string

type item =
	| Infos of infos
	| Mts of mts
	| Mtw of mtw
	| Atw of atw
	| Pic of pic 
	| Font of font
	| Dir of dir
	| Obj of obj

type t = {
	mutable htree : item Tree.t;
	mutable hpath : string;
	hres : (int,string) Hashtbl.t;
}

exception Error of string

let exe_dir = Filename.dirname Sys.argv.(0)

let hs_dtd_data = "
	<!ELEMENT honsen (mts|mtw|pic|font|dir|atw)*>
	<!ATTLIST honsen width CDATA #REQUIRED>
	<!ATTLIST honsen height CDATA #REQUIRED>
	<!ATTLIST honsen fps CDATA #REQUIRED>
	<!ATTLIST honsen bgcolor CDATA #DEFAULT \"0xFFFFFF\">
	<!ATTLIST honsen compress CDATA #IMPLIED>

	<!ELEMENT mts EMPTY>
	<!ATTLIST mts file CDATA #REQUIRED>

	<!ELEMENT pic EMPTY>
	<!ATTLIST pic file CDATA #REQUIRED>
	<!ATTLIST pic res CDATA #IMPLIED>
	<!ATTLIST pic wrap CDATA #IMPLIED>

	<!ELEMENT mtw (id)*>
	<!ATTLIST mtw file CDATA #REQUIRED>
	<!ATTLIST mtw compress CDATA #DEFAULT \"4\">

	<!ELEMENT atw (id)*>
	<!ATTLIST atw file CDATA #REQUIRED>
	<!ATTLIST atw compress CDATA #DEFAULT \"4\">

	<!ELEMENT font EMPTY>
	<!ATTLIST font face CDATA #REQUIRED>
	<!ATTLIST font size CDATA #REQUIRED>
	<!ATTLIST font italic CDATA #IMPLIED>
	<!ATTLIST font bold CDATA #IMPLIED>
	<!ATTLIST font res CDATA #IMPLIED>
	<!ATTLIST font file CDATA #IMPLIED>

	<!ELEMENT dir (mts|mtw|pic|font|dir|atw)*>
	<!ATTLIST dir name CDATA #REQUIRED>

	<!ELEMENT id EMPTY>
	<!ATTLIST id res CDATA #REQUIRED>
	<!ATTLIST id name CDATA #REQUIRED>
"

let error msg =
	raise (Error msg)

let open_file fname =
	try
		IO.input_channel (open_in_bin fname)
	with
		_ -> error ("File not found : "^fname)

let mid_hash s = 
	let acc = ref 0 in
	for i = 0 to String.length s - 1 do
		acc := 223 * !acc + Char.code (String.unsafe_get s i)
	done;
	acc := !acc land ((1 lsl 31) - 1);
	!acc

let itos = string_of_int

let stoi x v =
	try
		int_of_string (Xml.attrib x v)
	with
		_ -> error ("Invalid integer value for "^v)

let attrib_bool_opt x name def =
	try
		match String.uppercase (Xml.attrib x name) with
		| "TRUE" | "1" -> true
		| "FALSE" | "0" -> false
		| _ -> def
	with
		Xml.No_attribute _ -> def

let attrib_option x name =
	try
		Some (Xml.attrib x name)
	with
		Xml.No_attribute _ -> None

let process_rsid h x =
	try
		let name = Xml.attrib x "RES" in
		let id = mid_hash name in
		(try
			let name2 = Hashtbl.find h.hres	id in
			if name = name2 then error ("Resource name "^name^" is used several times.");
			error ("Resource name collision between "^name^" and "^name2^" : please rename one of the two.")
		with
			Not_found ->
				Hashtbl.add h.hres id name;
				Some (id,name))
	with
		Xml.No_attribute _ ->
			None

let process_mts h x =
	Tree.item (Mts (Xml.attrib x "FILE")) []

let process_pic h x =
	Tree.item (Pic {
		pic_file = Xml.attrib x "FILE";
		pic_id = process_rsid h x;
		pic_wrap = attrib_bool_opt x "WRAP" false;
	}) []

let rec process_obj tag h ids = function
	| Mtw.Group (t,data) as it when t = tag ->
		let oname = (try Mtw.get_data "NAME" it with Not_found -> error (tag ^ " without name")) in
		let oname = String.sub oname 0 (String.length oname - 1) in
		let oid = (try Some (List.find (fun i ->
							if i.id_obj = oname then begin
								i.id_bound <- true;
								true;
							end else
								false)
						ids).id_id
					with
						Not_found -> None)
		in
		let obj_data , data = List.partition (fun it -> match it with Mtw.Group (t,_) when t = tag -> true | _ -> false) data in
		Tree.item (Obj { 
			obj_tag = tag;
			obj_name = oname; 
			obj_id = oid; 
			obj_data = data
		}) (List.map (process_obj tag h ids) obj_data)
	| it ->
		error ("Invalid MTW element "^(Mtw.tag it))

let process_id h x =
	match process_rsid h x with
	| None -> error "Invalid ID";
	| Some id ->
		{
			id_obj = Xml.attrib x "NAME";
			id_id = id;
			id_bound = false;
		}

external hslib_compress : int -> string -> int -> string -> int -> int = "caml_hslib_compress"

let compress_data cmp data =
	let newdata = String.create (String.length data * 2) in
	let newlen = hslib_compress cmp data (String.length data) newdata (String.length newdata) in
	if newlen = -1 then
		error "Compression failed"
	else
		String.sub newdata 0 newlen

let rec compress cmp inframe = function
	| Mtw.Group (name,l) ->
		Mtw.Group (name,List.map (compress cmp (name = "Fram")) l)
	| Mtw.Data ("DATA",data) when inframe ->
		Mtw.Data ("DATA",compress_data cmp data)
	| Mtw.Data ("VECT" as name,data)
	| Mtw.Data ("MAP0" as name,data) ->
		Mtw.Data (name,compress_data cmp data)
	| Mtw.Data _
	| Mtw.Ident _ 
	| Mtw.Number _ as x ->
		x

let load_mtw h file cmp ids =
	let ch = open_file (h.hpath ^ file) in
	let data = (try Mtw.read ch with _ -> error ("Invalid MTW file : "^file)) in
	let data = List.map (compress cmp false) data in
	IO.close_in ch;
	let t = Tree.item (Mtw file) (List.map (process_obj "Objt" h ids) data) in
	List.iter (fun id ->
		if not id.id_bound then error ("Object not found "^id.id_obj^" in "^file);
	) ids;
	t

let load_atw h file cmp ids =
	let ch = open_file (h.hpath ^ file) in
	let data = (try Mtw.read ch with _ -> error ("Invalid MTW file : "^file)) in
	let data = List.map (compress cmp false) data in
	IO.close_in ch;
	let t = Tree.item (Atw file) (List.map (process_obj "Anim" h ids) data) in
	List.iter (fun id ->
		if not id.id_bound then error ("Object not found "^id.id_obj^" in "^file);
	) ids;
	t

let process_mtw h x =
	load_mtw h (Xml.attrib x "FILE") (stoi x "COMPRESS") (List.map (process_id h) (Xml.children x))

let process_atw h x = 
	load_atw h (Xml.attrib x "FILE") (stoi x "COMPRESS") (List.map (process_id h) (Xml.children x))

let process_font h x =
	Tree.item (Font {
		font_bold = attrib_bool_opt x "BOLD" false;
		font_italic = attrib_bool_opt x "ITALIC" false;
		font_face = Xml.attrib x "FACE";
		font_size = stoi x "size";
		font_file = attrib_option x "FILE";
		font_id = process_rsid h x;
	}) []

let rec process_dir h x =
	Tree.item (Dir (Xml.attrib x "NAME")) (List.map (process_child h) (Xml.children x))

and process_child h x =
	match String.uppercase (Xml.tag x) with
	| "MTS" -> process_mts h x
	| "PIC" -> process_pic h x
	| "MTW" -> process_mtw h x
	| "ATW" -> process_atw h x
	| "FONT" -> process_font h x
	| "DIR" -> process_dir h x
	| _ -> error ("Invalid child : "^ Xml.tag x)

let process h x =
	Tree.item (Infos {
		inf_width = stoi x "width";
		inf_height = stoi x "height";
		inf_bgcolor = stoi x "bgcolor";
		inf_fps = stoi x "FPS";
		inf_compress = attrib_bool_opt x "compress" true
	}) (List.map (process_child h) (Xml.children x))

let default_hsx() =
	{
		htree = Tree.create (Infos {
			inf_width = 100;
			inf_height = 100;
			inf_fps = 100;
			inf_bgcolor = 0xFFFFFF;
			inf_compress = true;
		});
		hres = Hashtbl.create 0;
		hpath = "";
	}

let read file =
	try
		let dtd = Dtd.check (Dtd.parse_string hs_dtd_data) in
		let x = Dtd.prove dtd "HONSEN" (Xml.parse_file file) in	
		let h = default_hsx() in
		h.hpath <- Filename.dirname file ^ "/";
		h.htree <- Tree.of_root (process h x);
		h
	with
		| Xml.Error e -> error (Xml.error e)
		| Xml.File_not_found f -> error ("File not found : "^f)
		| Dtd.Parse_error e -> error ("Dtd parse error : " ^ Dtd.parse_error e )
		| Dtd.Check_error e -> error ("Dtd check error : " ^ Dtd.check_error e )
		| Dtd.Prove_error e -> error ("Dtd prove error : " ^ Dtd.prove_error e )

let save ch h =
	let xml name attribs childs =
		Xml.Element (name,attribs,childs)
	in
	let save_id = function
		| None -> []
		| Some (_,name) -> [("name",name)]
	in
	let save_bool name v def =
		if v = def then
			[]
		else
			[(name,if v then "true" else "false")]
	in
	let save_opt name = function
		| None -> []
		| Some v -> [(name,v)]
	in
	let rec save_item it =
		match Tree.data it with
		| Mts file -> xml "mts" [("file",file)] []
		| Pic p -> xml "pic" (("file",p.pic_file) :: save_id p.pic_id) []
		| Mtw file -> xml "mtw" [("file",file)] (List.map save_item (Tree.childs it))
		| Atw file -> xml "atw" [("file",file)] (List.map save_item (Tree.childs it))
		| Font f -> xml "font"
				(("face",f.font_face) ::
				("size",itos f.font_size) ::
				save_bool "bold" f.font_bold false @ 
				save_bool "italic" f.font_italic false @ 
				save_opt "file" f.font_file @
				save_id f.font_id)
			[]
		| Dir name -> xml "dir" [("name",name)] (List.map save_item (Tree.childs it))
		| Infos i -> xml "honsen"
			[("width",itos i.inf_width);("height",itos i.inf_height);("bgcolor",Printf.sprintf "0x%.6X" i.inf_bgcolor);("fps",itos i.inf_fps)] 
			(List.map save_item (Tree.childs it))
		| Obj o ->
			match o.obj_id with
			| None -> Xml.PCData ""
			| Some (_,idname) -> xml "id" [("object",o.obj_name);("name",idname)] []
	in
	let x = save_item (Tree.root h.htree) in
	output_string ch (Xml.to_string_fmt x)
