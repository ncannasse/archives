(********************************************************************************)
(* LIBS *)

open Osiris
open Vfs
open Mtlib

(********************************************************************************)
(* GLOBAL *)

let files = ref Empty

let last_selected = Global.empty "Workspace : last selected"

let wspace_bind = ref Xgui.null_xobj

let wspace() =
	!wspace_bind#treeview "workspace"

let file_at_index p =
	let pos = ref p in
	let found = ref None in
	let rec at_index tree =
		(match tree with
			| Empty -> false
			| Tree((_,infos,_),childs) ->
				if !pos = 0 then begin
					found := Some infos;
					true;
				end else begin
					decr pos;
					try
						ignore(List.find at_index childs);
						true;
					with
						Not_found -> false;
				end) in
		ignore(at_index !files);
		match !found with
		| None -> failwith ("Cannot find file at position "^(itos p))
		| Some inf -> inf


let rec do_refresh id =	
	let infos = get_file_infos id in	
	let t = get_ftype_infos infos.fi_t in
	let childs = enum_childs (fun x -> do_refresh x) id in
		Tree ((id,infos,Some t.ft_icon),childs)
		
let refresh() =
	let w = wspace() in		
		let root = Global.empty "Workspace root refresh" in
		if (try
				Global.set root (get_root());
				true
			with
				Global_not_initialized _ -> false) then files := do_refresh (Global.get root);
		(* check if last_selected has been deleted *)
		ignore(
		match Global.opt last_selected with
			None -> ()
			| Some id -> try ignore(get_file_infos id) with No_such_file_id _ -> Global.undef last_selected);
		w#redraw

let print_tree() =
	let rec print_item t =
		match t with
		| Empty -> Empty
		| Tree((id,infos,ico),childs) ->			
			if Vfs.isdebug then
				Tree((infos.fi_name^" ("^(itos (Obj.magic id : int))^")",ico),List.map print_item childs)
			else
				Tree((infos.fi_name,ico),List.map print_item childs)
	in
		print_item !files

let apply func param = 
	match func with
	None -> ()
	| Some f -> f param

(********************************************************************************)
(* MENU FUNCTIONS *)

let rec parent_dir data =
	if (get_ftype_infos data.fi_t).ft_haschilds then data else parent_dir (get_file_infos data.fi_parent)

let new_file ft =
	let parent =
		(match (wspace())#selected with
			None -> get_root()
			| Some p -> let data = (file_at_index p) in (parent_dir data).fi_id)
		in
		apply (get_ftype_infos ft).ft_new parent

let import_file ft =
	let parent =
		(match (wspace())#selected with
			None -> get_root()
			| Some p -> let data = (file_at_index p) in (parent_dir data).fi_id)
		in
		match ft.ft_hardlinked with
		None -> ()
		| Some h ->
			let str = Osiris.open_file [(ft.ft_name,h.fh_ext)] in
			match str with
			None -> ();
			| Some file -> h.fh_import parent file

let rename_file() =
	match (wspace())#selected with
	None -> ()
	| Some p ->
		let data = (file_at_index p) in
		let d = Dialogs.create None "file.rename" in
		(d#label "file.oldname")#caption ("(From) "^(get_file_path data.fi_id));
		(d#edit "file.name")#caption data.fi_name;
		if d#process then begin
			let name = (d#edit "file.name")#get_caption in
			Dialogs.close d;
			if name <> data.fi_name then rename_file data.fi_id name;
		end else
			Dialogs.close d

let delete_file() =
	match (wspace())#selected with
	None -> ()
	| Some p ->
		let data = (file_at_index p) in
			(try Document.close data.fi_id; with Document.No_such_document _ -> ());		
			delete_file data.fi_id;
			(wspace())#set_selected (Some(p-1))

(********************************************************************************)
(* ACTIONS *)

let action_dblclick() =
	match (wspace())#selected with
	None -> ()
	| Some p -> let data = (file_at_index p) in		
		Document.dopen data.fi_id		

let is_selected id =
	match Global.opt last_selected with
	None -> false
	| Some p -> p = id

let action_unselect newsel =
	ignore(
		match Global.opt last_selected with
		None -> ()
		| Some p -> apply (get_ftype_infos (get_file_infos p).fi_t).ft_unselect p
	);
	match newsel with
	| None -> Global.undef last_selected
	| Some s -> Global.set last_selected s

let action_select() =
	match (wspace())#selected with
	None -> action_unselect None
	| Some p -> let data = (file_at_index p) in
		if not (is_selected data.fi_id) then begin 
			action_unselect (Some data.fi_id);
			apply (get_ftype_infos data.fi_t).ft_select data.fi_id;
		end

let action_context() =
	match (wspace())#selected with
	None -> ()
	| Some p -> let data = (file_at_index p) in		
		(match (get_ftype_infos data.fi_t).ft_context with
		| [] -> 
			(Main.obj#menuitem "menu.custom")#enable false;			
		| l ->
			let m = Main.obj#menuitem "menu.custom" in
			let actions = new_menu() in
			m#enable true;
			m#submenu (Some actions);
			List.iter (fun (str,act) ->
					let item = new_menuitem actions in
					item#caption str;
					item#on_click (fun() -> act data.fi_id);
				) l);
		(Main.obj#menu "menu.actions")#popup


let action_key k =
		 if k = keys.k_del then delete_file()
	else if k = keys.k_fun+2 (*F2*) then rename_file()
	else ()


(********************************************************************************)
(* DRAG'N DROP *)

let drag = ref false
let dragitem = ref (-1)
let dragypos = ref 0

let action_startdrag() =
	let x,y = mouse_pos() in
	match (wspace())#hittest x y with
	| None -> ()
	| Some p ->
		dragypos := y;
		drag := true;
		dragitem := p

let action_dragmove _ _ =
	if !drag then begin
		let x,y = mouse_pos() in
		let w = wspace() in
		w#set_selected (w#hittest x y)
	end

let action_enddrag() =
	if !drag then begin
		drag := false;
		let nx,ny = mouse_pos() in
		match (wspace())#hittest nx ny with
		| None -> ()
		| Some dest ->
			let _,y = mouse_pos() and ny = !dragypos in
			if !dragitem <> dest && abs(y-ny) > 5 then 
				(try
					move_file (file_at_index !dragitem).fi_id (parent_dir (file_at_index dest)).fi_id
				with
					Invalid_parent _ -> ());
	end

(********************************************************************************)
(* INIT *)

let init() =
	(* workspace creation *)
	let p = Main.obj#panel "main.left" in
		p#visible true;
		wspace_bind := (Dialogs.create (Some (Xgui.XPanel (Main.obj#panel "main.left"))) "workspace");

	(* workspace actions init *)
	let w = wspace() in
		w#set_icons (Some Gfxres.gfx);
		w#treeprint print_tree;
		w#on_key_down action_key;
		w#on_dblclick action_dblclick; 
		w#on_selchange action_select;
		w#on_rdown action_context;
		w#on_mousemove action_dragmove;
		w#on_ldown action_startdrag;
		w#on_lup action_enddrag;

	Vfs.refresh_callback refresh;

	(* menu actions init *)
	let menu = Main.obj#menu "menu.new" in
	let import = Main.obj#menu "menu.import" in
		(enum_ftype	(fun x ->
				(match x.ft_new with
				None -> ()					
				| Some _ ->
					let m = new_menuitem menu in
					m#caption x.ft_name;
					m#on_click (fun y -> new_file x.ft_t;));
				(match x.ft_hardlinked with
				None -> ()
				| Some _ ->
					let m = new_menuitem import in
					m#caption x.ft_name;
					m#on_click (fun y -> import_file x;));
				));
	let menu = Main.obj#menuitem "menu.delete" in
		menu#on_click delete_file;
	let menu = Main.obj#menuitem "menu.rename" in
		menu#on_click rename_file;
	refresh();
;;
open Run;;

register {
	m_name = "Workspace";
	m_init = Some init;
	m_close = None;
	m_main = None;
}

(********************************************************************************)
