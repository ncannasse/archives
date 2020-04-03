open Mtlib
open Osiris
open M3d

let main_wnd = Global.empty "Main Window"
let render_panel = Global.empty "3D Render Panel"
let model = Global.empty "Current Model"
let anims = MList.empty()
let fxlibs = MList.empty()
let effects = MList.empty()
let stoi = int_of_string

let refresh_anims = ref (fun()->())
let refresh_effects = ref (fun()->())

let last_file = ref ""
let cur_file = ref ""
let cur_path = ref ""
let vsync = ref true
let fullscreen = ref false

let deffile = ref "default.mtw"

let new_fid =
	let fid = ref 0 in
	(fun () -> incr fid; Obj.magic !fid)

let get_extension fname =
	let len = String.length fname in
	String.sub fname (len-3) 3

let get_ftype s =
	(Char.code (String.get s 0))+((Char.code (String.get s 1)) lsl 8)+((Char.code (String.get s 2)) lsl 16)

let close_model() =
	match Global.opt model with
	| None -> ()
	| Some i ->
		Inst.detach i;
		Global.undef model;
		MList.clear anims

let open_file file =
	close_model();
	last_file := file;
	cur_file := file;
	cur_path := Filename.dirname file;
	let i = Inst.attach (new_fid()) in
	MList.copy_list anims (Inst.anims i);
	!refresh_anims();
	Camera.reset();
	Global.set model i

let open_lib file =
	match FX.load_library file with
	| true ->
		MList.copy_list effects (FX.get_names());
		cur_path := Filename.dirname file;
		!refresh_effects();
		MList.add fxlibs file
	| false -> failwith ("Unable to load FX library "^file)

let open_model() =
	match Osiris.open_file [("Motion-Twin 3D Model",["*.mtw"]);("Motion-Twin FX Library",["*.dll"])] with
	| None -> ()
	| Some file ->
		match Filename.check_suffix file "mtw" with
		| true -> open_file file
		| false -> open_lib file

let drag_x = ref 0 and
	drag_y = ref 0 and
	drag = ref false and
	can_drag = ref false and
	zoom_y = ref 0 and
	zoom = ref false and
	pan = ref false

let begin_drag () =	
	can_drag := true

let begin_pan () =
	can_drag := true;
	pan := true

let begin_zoom () =
	let _,y = mouse_pos() in
	zoom_y := y;
	zoom := true

let end_drag () =
	can_drag := false;
	drag := false;
	pan := false

let end_zoom () =
	zoom := false

let do_dragzoom x y =
	if !drag then begin
		let x,y = mouse_pos() in
		if not !pan then
			Render.do_transform (Matrix.rot {x=0.0; y=(float_of_int (y- !drag_y))/.100.0; z=(float_of_int (!drag_x-x))/.100.0;})
		else
			Render.do_transform (Matrix.trans { z= (float_of_int (!drag_y-y)); y=0.; x= (float_of_int (!drag_x-x)); });
		drag_x := x; 
		drag_y := y;
	end else if !zoom then begin
		let _,y = mouse_pos() in
		let zfact = (1.0+.(float_of_int (!zoom_y-y))/.100.0) in			
		Camera.zoom zfact;
		zoom_y := y;
	end else if !can_drag then begin
		let p = Global.get render_panel in
		let xp = ((float_of_int x) *. 2.0) /. (float_of_int p#get_width) -. 1.0 in
		let yp = ((float_of_int y) *. 2.0) /. (float_of_int p#get_height) -. 1.0 in
		let x,y = mouse_pos() in
		drag_x := x;
		drag_y := y;
		drag := true;
	end

let init() =
	Win32.free_console();
	(*--- WND + MENUS ---*)
	let wnd = new_window() in
	Global.set main_wnd wnd;
	wnd#width 800;
	wnd#height 600;
	wnd#caption "MTW Model Viewer - (c)2002 Motion-Twin";
	wnd#state Maximize;
	let files = new_menuitem wnd#menu in
	let files_sub = new_menu() in
	let fopen = new_menuitem files_sub in
	let fclose = new_menuitem files_sub in
	let fexit = new_menuitem files_sub in
	files#caption "Files...";
	files#submenu (Some files_sub);
	fopen#caption "Open";
	fopen#on_click open_model;
	fclose#caption "Close";
	fclose#on_click close_model;
	fexit#caption "Exit";
	fexit#on_click (fun () -> wnd#destroy);
	(*--- LOGBOX ---*)
	let msgs = MList.empty() in
	let logbox = new_listbox wnd#container in
	logbox#height 50;
	logbox#align AlBottom;
	logbox#listprint (fun () -> MList.to_list msgs);
	Callback.register "logfunc" (fun msg -> MList.add msgs msg; logbox#redraw);	
	(*--- ANIMATIONS ---*)
	let panel = new_panel wnd#container in
	let label = new_label panel#container in
	let animlist = new_combobox panel#container in
	panel#align AlTop;
	panel#height 40;
	panel#space 10;
	panel#span 5;
	label#caption "Animation:";
	label#align AlLeft;
	label#width 50;
	animlist#align AlLeft;
	animlist#width 130;
	animlist#listprint (fun () -> MList.to_list anims);
	animlist#on_selchange (fun () ->
		match animlist#selected with
		| None -> ()
		| Some p -> Inst.anim (Global.get model) (Some (MList.at_index anims p))
	);
	let stop = new_button panel#container in
	let reset = new_button panel#container in
	stop#caption "Stop";
	reset#caption "Reset";
	stop#align AlLeft;
	reset#align AlLeft;
	stop#on_click (fun () -> match Global.opt model with None -> () | Some i -> Inst.anim i None);
	reset#on_click (fun () -> match Global.opt model with None -> () | Some i -> Inst.reset i);

	(*--- SPEED ---*)
	(new_label panel#container)#align AlLeft;

	let label = new_label panel#container in
	let speed = new_edit panel#container in
	let modspeed = new_button panel#container in
	label#caption "Speed:";
	label#align AlLeft;
	label#width 50;
	speed#align AlLeft;
	speed#caption "100";
	speed#width 50;
	modspeed#align AlLeft;
	modspeed#caption "Update";
	modspeed#on_click (fun () ->
		match Global.opt model with
		| None -> ()
		| Some i ->
			try 
				Inst.speed i ((itof (stoi speed#get_caption))/.100.);
			with
				_ -> speed#caption "100"
	);
	refresh_anims := (fun () -> animlist#redraw);

	(*---- OPTIONS ---*)
	(new_label panel#container)#align AlLeft;

	let options = new_menuitem wnd#menu in
	let optmenu = new_menu() in
	options#caption "Options...";
	options#submenu (Some optmenu);

	let togshade = new_menuitem optmenu in	
	togshade#caption "Shading";	
	togshade#check true;
	togshade#on_click (fun () ->
		let shade = not togshade#is_checked in
		togshade#check shade;
		Render.set_option (ODoShading shade)
	);
	let toginfos = new_menuitem optmenu in	
	toginfos#caption "Show Infos";	
	toginfos#on_click (fun () ->
		let infos = not toginfos#is_checked in
		toginfos#check infos;
		Render.set_option (OShowInfos infos)
	);	
	let leftpan = new_panel wnd#container in
	let refresh = new_button leftpan#container in
	leftpan#align AlLeft;
	leftpan#width 150;	
	leftpan#span 10;
	refresh#caption "Refresh";
	refresh#align AlTop;
	refresh#on_click (fun () ->
		match Global.opt model with
		| None -> ()
		| Some m ->
			Inst.detach m;
			Global.undef model;
			Gc.full_major();
			Render.garbage();
			open_file !last_file;
			MList.iter open_lib fxlibs
	);
	(*------- FX ----------*)
	(new_label panel#container)#align AlLeft;

	let label = new_label panel#container in
	let fxlist = new_combobox panel#container in
	label#caption "Special FX:";
	label#align AlLeft;
	label#width 60;
	fxlist#align AlLeft;
	fxlist#width 130;
	fxlist#listprint (fun () -> MList.to_list effects);
	fxlist#on_selchange (fun () ->
		match fxlist#selected with
		| None -> ()
		| Some p ->
			FX.play (MList.at_index effects p) [("_default",FX.PBool true)];
	);
	refresh_effects := (fun () -> fxlist#redraw);
	(*------ RENDER -------*)
	let rp = new_panel wnd#container in
	Global.set render_panel rp;	
	rp#align AlClient;
	rp#on_ldown begin_drag;
	rp#on_lup end_drag;
	rp#on_rdown begin_zoom;
	rp#on_rup end_zoom;
	rp#on_mdown begin_pan;
	rp#on_mup end_drag;
	rp#on_mousemove do_dragzoom;
	wnd#visible true;
	let handle = (if !fullscreen then wnd#handle else rp#handle) in
	Render.set_option (OFullScreen !fullscreen);
	if not (M3d.init handle !vsync) then failwith "Failed to Init M3D";
	Render.start();
	(try open_lib "fxlib.dll"; with _ -> ());
	(try if Sys.file_exists !deffile then open_file !deffile; with _ -> ())

let main() =
	let wnd = Global.get main_wnd in
	M3d.handle_events();
	wnd#process false

let close() =
	Global.undef model;	
	Render.stop();
	M3d.close();
	let wnd = Global.get main_wnd in
	Global.undef main_wnd;
	if not wnd#is_destroyed then wnd#destroy

;;	
Callback.register "vfs.get_file_type" (fun id ->
	match String.uppercase (get_extension !cur_file) with
	| "JPG" | "PNG" | "GIF" | "BMP" -> get_ftype "PIC"
	| "MTW" -> get_ftype "MTW"
	| _ -> get_ftype "???"
);
Callback.register "vfs.get_file_path" (fun x -> !cur_file);
Callback.register "vfs.get_hlink" (fun id -> Some !cur_file);
Callback.register "vfs.find_hlink" (fun name -> cur_file := !cur_path^"\\"^name; Some (new_fid()));

try
	if Array.length Sys.argv > 1 then
		(match (Array.get Sys.argv 1) with
		| "-cpu" -> vsync := false
		| "-full" -> fullscreen := true
		| f -> deffile := f);
	init();
	while main() do (); done;
	close();
with
	e -> Osiris.message_box (Printexc.to_string e); close()