(********************************************************************************)
(* Header *)

open Osiris
open Vfs
open String
open Log
open Mtlib

let type_ml = Vfs.get_ftype "OML"
let type_prj = Vfs.get_ftype "OPR"

(********************************************************************************)
(* Project *)

type project = {
	p_opt : bool;
	p_dbg : bool;
	p_cma : bool;
	p_args : string;
	p_path : string;
}

let save_project id (data:project) =
	Vfs.write id data

let load_project id =
	(Vfs.read id : project)

let get_current_options x =
	{
		p_opt = (x#checkbox "opt")#get_state;
		p_dbg = (x#checkbox "dbg")#get_state;
		p_cma = (x#checkbox "cma")#get_state;
		p_args = (x#edit "args")#get_caption;		
		p_path = (x#edit "path")#get_caption;
	}

let rec make_file_list id =
	let f = get_file_infos id in
	if f.fi_t = type_ml then
		(name_of_hlink id)::(List.concat (Vfs.enum_childs make_file_list id))
	else
		List.concat (Vfs.enum_childs make_file_list id)

let log_process command =
	let read_all_in func chin =
		try
			while true do
				func (input_line chin)
			done;
			assert false;
		with
			End_of_file -> close_in chin
	in	
	let pin,pout,perr = Unix.open_process_full command (Unix.environment()) in
		read_all_in (fun s -> log MSG (String.sub s 0 ((String.length s)-1))) pin;
		read_all_in (fun s -> log WARNING (String.sub s 0 ((String.length s)-1))) perr;
		match Unix.close_process_full (pin,pout,perr) with
		| Unix.WEXITED exitcode -> ()
		| _ -> failwith "Build aborted by signal"

let run_ocamake id addparam =
	let p = get_current_options (Document.get id)
	and l = make_file_list id in
	let args =
		(if p.p_opt then " -opt" else "")^
		(if p.p_dbg then " -g" else "")^
		(if p.p_cma then " -a" else "")^
		" "^p.p_args in
	let cmd = "ocamake"^args^" "^(String.concat " " l)^" -o "^(name_of_file id)^(if p.p_cma then ".cma" else ".exe")^" "^addparam in	
	let cwd = Sys.getcwd() in
	if String.length p.p_path > 1 then
		Sys.chdir p.p_path;
	try
		log_process cmd;
		Sys.chdir cwd;
	with e -> Sys.chdir cwd; raise e

let run_compile id = run_ocamake id ""

let run_clean id = run_ocamake id "-clean"
		
let run_output id =
	let p = get_current_options (Document.get id) in
	if p.p_cma then failwith "Cannot run a CMA alone" else
	let cwd = Sys.getcwd() in
	if String.length p.p_path > 1 then
		Sys.chdir p.p_path;
	try		
		ignore(Win32.shell_execute Win32.null_hwnd Win32.Shell_open ((name_of_file id)^".exe") None (Some p.p_path) Win32.SW_SHOWNORMAL);
		Sys.chdir cwd;
	with e -> Sys.chdir cwd; raise e


let open_project id =
	let p = load_project id in
	Document.create id (Dialogs.get "ocaml.project") (fun x ->
		(x#checkbox "opt")#state p.p_opt;
		(x#checkbox "dbg")#state p.p_dbg;
		(x#checkbox "cma")#state p.p_cma;
		(x#edit "args")#caption p.p_args;
		(x#edit "path")#caption p.p_path;
		(x#button "compile")#on_click (fun () -> run_compile id);
		(x#button "run")#on_click (fun () -> run_output id);
		(x#button "clean")#on_click (fun () -> run_clean id)
	)	

let new_project parent =
	let id = create_file ~name:"new_project" ~parent:parent ~ftype:type_prj in
	save_project id { p_opt = false; p_dbg = false; p_cma = false; p_args = "win32.cma orisis.cma"; p_path = "" };
	open_project id

let close_project id =
	let x = Document.get id in
	let p = get_current_options x in
		save_project id p

(********************************************************************************)
(* OCaml *)

let ocaml_import pid file =
	let p = String.rindex file '\\' in
	let filename = String.sub file (p+1) ((String.length file)-p-1) in
	let id = create_hlink ~name:filename ~file ~parent:pid ~ftype:type_ml in		
		Document.dopen id

let keywords = Hashtbl.create 0

let add_keyword k =	Hashtbl.add keywords k true
;;
List.iter add_keyword ["and";"as";"assert";"begin";"class";"constraint";"do";"done";"downto";"else";"end";
	"exception";"external";"false";"for";"fun";"function";"functor";"if";"in";"include";
	"inherit";"initializer";"lazy";"let";"match";"method";"module";"mutable";"new";"not";"object";"of";
	"open";"or";"private";"rec";"sig";"struct";"then";"to";"true";"try";"type";"val";"virtual";"when";
	"while";"with"];;

type state =
	Normal
	| Comment
	| String

let col_normal = make_richcolor 0 0 0
let col_keyword = make_richcolor 0 0 255
let col_comment = make_richcolor 0 128 0
let do_colorize = ref false

let colorize r part =
	let state = ref Normal in
	let default = if part = None then None else Some col_normal in
	let func t str pos =
		match !state with 
		| Normal -> 
			(match t with
				| ST_ALPHANUM -> (try ignore(Hashtbl.find keywords str); Some col_keyword; with Not_found -> default)
				| ST_OTHER ->
					let l = length str in
					if l >=2 && get str 0 = '(' && get str 1 = '*' then begin
						if get str (l-1) <> ')' || get str (l-2) <> '*' then state := Comment;
						Some col_comment;
					end else
						default;
				| _ -> default;
			);
		| Comment ->
			(match t with
				| ST_OTHER ->
					let l = length str in
					if l >= 2 && get str (l-1) = ')' && get str (l-2) = '*' then state := Normal;
					Some col_comment;
				| _ -> Some col_comment;
			)
		| _ -> default;
	in
		r#colorize func part

let ocaml_open id =
	Document.create id (Xml.parse_string "<richedit name='src' align='client'/>") (fun x ->
		let r = (x#richedit "src") in
			r#font#face "Courier";
			r#font#size 10;
		let f = open_in_hlink id in
		r#caption (Mtlib.read_all_file f);
		colorize r None;
		r#on_change (fun () ->		
			if !do_colorize then begin
				let y = r#line_from_char r#get_selection.smin in		
				let min = r#char_from_line y and max = r#char_from_line (y+1) in
				colorize r (Some {smin = min; smax = max-1});
				do_colorize := false;
			end else
				do_colorize := true;
		);
		close_in f;
	)

(********************************************************************************)
(* Register *)
	
;;
let data = {
	ft_name = "OCaml File";
	ft_t = type_ml;
	ft_icon = Gfxres.bitmap "BMP_OCAML";
	ft_new = None;
	ft_select = None;
	ft_unselect = None;
	ft_open = Some ocaml_open;
	ft_show = None;
	ft_delete = None;
	ft_hide = None;
	ft_close = None;
	ft_context = [];
	ft_export = None;
	ft_hardlinked = Some {
		fh_ext = ["*.ml";"*.mli";"*.mll";"*.mly"];
		fh_import = ocaml_import;
	};
	ft_haschilds = false;
} in
	Vfs.register data;

let data = {
	ft_name = "OCaml Project";
	ft_t = type_prj;
	ft_icon = Gfxres.bitmap "BMP_OCAML";
	ft_new = Some new_project;
	ft_select = None;
	ft_unselect = None;
	ft_open = Some open_project;
	ft_show = None;
	ft_delete = None;
	ft_hide = None;
	ft_close = Some close_project;
	ft_context = [];
	ft_export = None;
	ft_hardlinked = None;
	ft_haschilds = true;
} in
	Vfs.register data;

Run.fast_register "P_ocaml";

(********************************************************************************)
