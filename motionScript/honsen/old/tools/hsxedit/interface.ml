open Osiris

type icon = hres

type 'a intf = {
	i_set_file_name : (string option -> unit);
	i_save_enabled : (bool -> unit);
	i_refresh : (unit -> unit);
	i_get_sel : (unit -> 'a Tree.tree option);
	i_log : (string -> unit);
	i_log_show : (bool -> unit);
	i_log_clear : (unit -> unit);
	i_loop : (unit -> unit);
	i_get_icon : (string -> icon);
}

type event = unit -> unit

type 'a intf_actions = {
	mutable on_new : event;
	mutable on_open : event;
	mutable on_save : event;
	mutable on_exit : event;

	mutable on_newdir : event;
	mutable on_insert : event;
	mutable on_delete : event;

	mutable on_build : event;
	mutable on_start : event;
	mutable on_prefs : event;
	
	mutable on_refresh : unit -> 'a Tree.tree option;
	mutable on_closequery : (unit -> bool);

	mutable printer : 'a -> string * icon option;
	mutable act_can_drag : ('a Tree.tree -> bool);
	mutable act_get_drag_target : ('a Tree.tree option -> 'a Tree.tree option);
	mutable act_drag_drop : ('a Tree.tree -> 'a Tree.tree -> unit);
}

let no_event _ = ()

let actions() = {
	on_new = no_event;
	on_open = no_event;
	on_save = no_event;
	on_exit = no_event;
	on_newdir = no_event;
	on_insert = no_event;
	on_delete = no_event;
	on_build = no_event;
	on_start = no_event;
	on_prefs = no_event;
	on_closequery = (fun _ -> true);
	on_refresh = (fun () -> None);
	printer = (fun _ -> "",None);
	act_can_drag = (fun _ -> false);
	act_get_drag_target = (fun _ -> None);
	act_drag_drop = (fun _ _ -> ());
}

let drag = ref false
let dragypos = ref 0

let on_startdrag tree dragitem can_drag =
	let x,y = mouse_pos() in
	match tree#hittest x y with
	| None -> ()
	| Some p ->
		match can_drag p with
		| false -> ()
		| true ->
			dragypos := y;
			drag := true;
			dragitem := Some p

let on_dragmove tree get_target =
	if !drag then begin
		let x,y = mouse_pos() in
		tree#set_selected (get_target (tree#hittest x y))
	end

let on_enddrag tree dragitem drag_drop =
	if !drag then
		drag := false;
		let nx,ny = mouse_pos() in
		match tree#hittest nx ny with
		| None -> ()
		| Some dest ->
			let _,y = mouse_pos() and ny = !dragypos in
			match !dragitem with
			| None -> ()
			| Some p ->
				if p <> dest && abs(y-ny) > 5 then drag_drop p dest

let create actions =
	let dragitem = ref None in
	let file_menu = new_menu() in
	let fnew = new_menuitem file_menu in
	let fopen = new_menuitem file_menu in
	let fsave = new_menuitem file_menu in
	let fsep = new_menuitem file_menu in
	let fexit = new_menuitem file_menu in
	fnew#caption "New...";
	fnew#on_click (fun () -> actions.on_new());
	fopen#caption "Open...";
	fopen#on_click (fun () -> actions.on_open());
	fsave#caption "Save";
	fsave#on_click (fun () -> actions.on_save());
	fsep#break true;
	fexit#caption "Exit";
	fexit#on_click (fun () -> actions.on_exit());
	let edit_menu = new_menu() in
	let finsert = new_menuitem edit_menu in
	let fnewdir = new_menuitem edit_menu in
	let fsep = new_menuitem edit_menu in
	let fdelete = new_menuitem edit_menu in
	fnewdir#caption "New Directory";
	fnewdir#on_click (fun () -> actions.on_newdir());
	finsert#caption "Insert...";
	finsert#on_click (fun () -> actions.on_insert());
	fsep#break true;
	fdelete#caption "Delete";
	fdelete#on_click (fun () -> actions.on_delete());
	let build_menu = new_menu() in
	let fbuild = new_menuitem build_menu in
	let fstart = new_menuitem build_menu in
	let fsep = new_menuitem build_menu in
	let fprefs = new_menuitem build_menu in
	fprefs#caption "Preferences...";
	fsep#break true;
	fprefs#on_click (fun () -> actions.on_prefs());
	fstart#on_click (fun () -> actions.on_start());
	fbuild#on_click (fun () -> actions.on_build());
	let w = new_window() in
	let file = new_menuitem w#menu in
	let edit = new_menuitem w#menu in
	let build = new_menuitem w#menu in
	let left_pan = new_panel w in
	let left_split = new_splitter w in
	let up_pan = new_panel w in
	let big_pan = new_panel w in
	let tree = new_treeview left_pan in
	let icons = new_resources 16 16 in
	icons#add_source "hsxicons.dll";
	left_pan#border BdNone;
	left_pan#width 250;
	left_pan#align AlLeft;
	left_pan#span 10;
	up_pan#border BdNone;
	up_pan#align AlTop;
	up_pan#span 10;
	big_pan#border BdNone;
	big_pan#align AlClient;
	big_pan#span 10;
	tree#align AlClient; 
	tree#set_icons (Some icons);
	tree#printer (fun t -> let s,i = actions.printer (Tree.data t) in s,i,Tree.childs t);
	tree#on_ldown (fun () -> on_startdrag tree dragitem actions.act_can_drag);
	tree#on_lup (fun () -> on_enddrag tree dragitem actions.act_drag_drop);
	tree#on_mousemove (fun _ _ -> on_dragmove tree actions.act_get_drag_target);
	file#caption "File...";
	file#submenu (Some file_menu);
	edit#caption "Edit...";
	edit#submenu (Some edit_menu);
	build#caption "Build...";
	build#submenu (Some build_menu);
	w#caption "HSX Editor - (c)2003 Motion-Twin";
	w#width 600;
	w#height 600;
	w#x 200;
	w#y 200;
	w#visible true;
	w#on_closequery (fun () -> actions.on_closequery());
	{
		i_get_icon = icons#get_bitmap;
		i_save_enabled = fsave#enable;
		i_refresh = (fun () -> tree#set_tree (actions.on_refresh()); tree#redraw);
		i_loop = (fun () -> w#loop);
		i_get_sel = (fun () -> tree#selected);
		i_set_file_name = (function
			| None ->
				fstart#caption "Run";
				fbuild#caption "Build";
			| Some f ->
				fstart#caption ("Run "^f);
				fbuild#caption ("Build "^f);
		);
		i_log = no_event;
		i_log_clear = no_event;
		i_log_show = no_event;
	}
