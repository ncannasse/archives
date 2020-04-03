open Hsx
open Interface

type hsx_edit = {
	mutable hdata : Hsx.t;
	mutable hmodified : bool;
	mutable hfile : string option;
}

let hsx_label id name =
	match id with
	| None -> "*"^name
	| Some (_,idname) -> idname

let hsx_label = function
	| Mts file -> Filename.basename file 
	| Mtw file -> Filename.basename file
	| Pic p -> hsx_label p.pic_id (Filename.basename p.pic_file)
	| Font f -> hsx_label f.font_id f.font_face
	| Dir name -> name
	| Obj o -> hsx_label o.obj_id o.obj_name
	| Infos _ -> "Library"

let hsx_compare a b =
	let aname = hsx_label a in
	let bname = hsx_label b in
	compare (String.lowercase aname) (String.lowercase bname)

let hsx = 
	{
		hdata = Hsx.default_hsx();
		hmodified = false;
		hfile = None;
	}

let actions = (Interface.actions() : Hsx.item intf_actions)
let intf = Interface.create actions

let file_extension f =
	try
		let p = String.rindex f '.' in
		String.uppercase (String.sub f (p+1) (String.length f - (p+1)))
	with
		Not_found -> ""

let switch_ext f ext =
	let base = (try 
		let p = String.rindex f '.' in
		String.sub f 0 p 
	with
		Not_found -> f) in
	base ^ ext

let exec cmd wait =
	let pout, pin, perr = Unix.open_process_full cmd (Unix.environment()) in
	match wait with
	| false ->
		close_in pout;
		close_in perr;
		close_out pin;
		false
	| true ->
		let edata = Std.input_lines perr in
		let data = Std.input_lines pout in
		intf.i_log_clear ();
		intf.i_log_show true;
		Enum.iter intf.i_log edata;
		Enum.iter intf.i_log data;
		match Unix.close_process_full (pout, pin, perr) with
		| Unix.WEXITED 0 -> true
		| Unix.WEXITED exitcode -> false
		| _ -> failwith "Build aborted by signal"

let do_save() =
	let do_save file =
		let ch = open_out file in
		Hsx.save ch hsx.hdata;
		close_out ch;
		hsx.hmodified <- false;
		intf.i_save_enabled false
	in
	match hsx.hfile with
	| Some file ->
		do_save file;
		true
	| None ->
		match Osiris.create_file "library.hsx" [("Honsen Library",["*.hsx"])] with
		| None -> false
		| Some file ->
			hsx.hfile <- Some file;
			hsx.hdata.hpath <- Filename.dirname file ^ "/";
			do_save file;
			true

let do_build() =
	match do_save() with
	| false -> false
	| true ->
		match hsx.hfile with
		| None -> assert false
		| Some file ->
			exec (Hsx.exe_dir ^ "/hsmake.exe " ^ file) true

let do_open file =
	hsx.hdata <- Hsx.read file;
	Tree.sort hsx.hdata.htree hsx_compare;
	hsx.hfile <- Some file;
	hsx.hmodified <- false;
	intf.i_save_enabled false;
	intf.i_set_file_name (Some (switch_ext (Filename.basename file) ".hs"));
	intf.i_refresh()

let prompt_save() =
	match hsx.hmodified with
	| false -> true
	| true ->
		let filename = (match hsx.hfile with None -> "<noname>" | Some f -> f) in
		match Osiris.question_cancel_box ("File "^filename^" have been modified.\nSave first ?") with
		| Osiris.QR_YES -> do_save()
		| Osiris.QR_NO -> true
		| Osiris.QR_CANCEL -> false

let is_movable t =
	match Tree.data t with
	| Infos _ | Obj _ -> false
	| _ -> true

let get_target t =
	let rec loop = function
		| None -> Tree.root hsx.hdata.htree
		| Some it ->
			match Tree.data it with
			| Infos _ | Dir _ -> it
			| _ ->
				loop (Tree.parent it)
	in
	loop t

let modified() =
	hsx.hmodified <- true;
	intf.i_save_enabled true;
	intf.i_refresh()

let on_new() =
	if prompt_save() then begin
		hsx.hdata <- Hsx.default_hsx();
		Tree.sort hsx.hdata.htree hsx_compare;
		hsx.hfile <- None;
		modified();
		hsx.hmodified <- false;
	end

let on_save() =
	ignore (do_save())

let on_exit() =
	if prompt_save() then Osiris.exit_application()

let on_open() =
	if prompt_save() then
		match Osiris.open_file [("Honsen Library",["*.hsx"])] with
		| None -> ()
		| Some file -> do_open file

let on_insert() =
	match Osiris.open_file [("Honsen File",["*.mts";"*.mtw";"*.png";"*.jpg"]);
					 ("MotionScript File",["*.mts"]);
					 ("Motion3D File",["*.mtw"]);
					 ("Picture",["*.png";"*.jpg"])] with
	| None -> ()
	| Some file ->
		let pfile = Path.make (Filename.dirname file) in
		let phsx = Path.make hsx.hdata.hpath in
		let prel = Path.relative pfile phsx in
		let file = Path.to_string prel ^ Filename.basename file in
		let new_item = (match file_extension file with
			| "MTS" -> Tree.item (Mts file) []
			| "MTW" -> Hsx.load_mtw hsx.hdata file []
			| "PNG" | "JPG" -> Tree.item (Pic { pic_file = file; pic_id = None }) []
			| _ -> failwith ("Don't know how to insert "^file^" in the library")
		) in
		let target = get_target (intf.i_get_sel()) in
		Tree.insert new_item target;
		modified()

let on_newdir() =
	let target = get_target (intf.i_get_sel()) in
	let new_item = Tree.item (Dir "New directory") [] in	
	Tree.insert new_item target;
	modified()

let on_delete() =
	match intf.i_get_sel() with
	| None -> ()
	| Some t -> 
		if is_movable t then begin
			Tree.remove t;
			modified();
		end

let on_prefs() =
	()

let on_start() =
	if do_build() then 
		match hsx.hfile with
		| None -> assert false
		| Some f ->
			ignore(exec (Hsx.exe_dir ^ "/hsrun.exe "^(switch_ext f ".hs")) false)

let on_build() =
	ignore(do_build())

let on_refresh() = Some (Tree.root hsx.hdata.htree)

let printer =
	let idir = Some (intf.i_get_icon "DIR") in
	let imts = Some (intf.i_get_icon "MTS") in
	let ipic = Some (intf.i_get_icon "PIC") in
	let imtw = Some (intf.i_get_icon "MTW") in
	let ifont = Some (intf.i_get_icon "FONT") in
	let get_icon = function
		| Mts _ -> imts
		| Dir _ | Infos _ -> idir
		| Pic _ -> ipic
		| Mtw _ | Obj _ -> imtw
		| Font _ -> ifont
	in
	(fun t -> hsx_label t, get_icon t)

let can_drag it = is_movable it

let drag_drop item dest =
	try
		Tree.insert item (get_target (Some dest));
		modified();
	with
		| Tree.Recursive_insert -> ()

let error = function
	| Hsx.Error msg -> Osiris.error_box msg
	| Failure msg -> Osiris.error_box msg
	| e -> 
		let s = Printexc.to_string e in
		prerr_endline s;
		Osiris.error_box s

let rec loop w =
	try
		intf.i_loop()
	with
		e ->
			error e;
			loop w
;;
try
	if Array.length Sys.argv > 1 then
		do_open (Sys.argv.(1))
	else begin
		intf.i_save_enabled true;
		Tree.sort hsx.hdata.htree hsx_compare;
	end;
	actions.on_new <- on_new;
	actions.on_open <- on_open;
	actions.on_save <- on_save;
	actions.on_exit <- on_exit;
	actions.on_newdir <- on_newdir;
	actions.on_insert <- on_insert;
	actions.on_delete <- on_delete;
	actions.on_build <- on_build;
	actions.on_start <- on_start;
	actions.on_prefs <- on_prefs;
	actions.on_refresh <- on_refresh;
	actions.on_closequery <- prompt_save;
	actions.printer <- printer;
	actions.act_can_drag <- can_drag;
	actions.act_get_drag_target <- (fun it -> Some (get_target it));
	actions.act_drag_drop <- drag_drop;
	intf.i_refresh();
(*	Win32.free_console();*)
	loop()
with
	e -> error e
