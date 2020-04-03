open Mtlib
open Vfs
open Xgui
open Osiris

exception No_such_document of fid

let documents = (Hashtbl.create 0)
let hiddens = MList.empty()
let current_bind = Global.empty "current document"
let panel = Global.empty "document panel"
let topbar = ref null_xobj

let apply func param = 
	match func with
	None -> ()
	| Some f -> f param

let show_current b =
	try
		let (id,x) = Global.get current_bind in
			x#self#visible b;
			if not b then begin
				MList.add hiddens id;
				apply (get_ftype_infos (get_file_infos id).fi_t).ft_hide id;
			end else begin
				(!topbar#label "doc.name")#caption (get_file_path id);
				apply (get_ftype_infos (get_file_infos id).fi_t).ft_show id;
			end
	with
		Global_not_initialized _ -> ()

let dopen id =
	try
		let x = (Hashtbl.find documents id) in
			show_current false;
			Global.set current_bind (id,x);
			MList.remove hiddens id;
			show_current true;
	with
		Not_found ->
			let data = Vfs.get_file_infos id in
			let tdata = Vfs.get_ftype_infos data.fi_t in
			match tdata.ft_open with
				None -> ()
				| Some func -> func id

let get_document id =
	try
		(Hashtbl.find documents id)
	with
		Not_found -> raise (No_such_document id)

let create id x initfunc =
	(Global.get panel)#visible false;
	let xobj = Xgui.create (Some (XPanel (Global.get panel))) x in
	(Hashtbl.add documents id xobj);
	xobj#self#align AlClient;
	show_current false;
	Global.set current_bind (id,xobj);
	show_current true;
	initfunc xobj;
	(Global.get panel)#visible true

let show_last() =
	if MList.length hiddens > 0 then begin
		let id = MList.last hiddens in
		MList.remove hiddens id;
		Global.set current_bind (id,(get_document id));
		show_current true;
	end

let kill id =
	let hash_length h =
		let l = ref 0 in
		Hashtbl.iter (fun _ _ -> incr l) h;
		!l in
	let x = get_document id in
	Hashtbl.remove documents id;
	(try
		MList.remove hiddens id;
	with
		Not_found -> ());
	(Global.get panel)#visible false;
	x#self#destroy;
	if hash_length documents = 0 then
		Global.undef current_bind
	else begin 
		if Global.isdef current_bind then begin
			let (n,_) = Global.get current_bind in
			if n = id then show_last();
		end else
			show_last();
		(Global.get panel)#visible true;
	end


let close id =
	let x = get_document id in
	let data = Vfs.get_file_infos id in
	let tdata = Vfs.get_ftype_infos data.fi_t in
	ignore(match tdata.ft_close with
			None -> ()
			| Some func -> func id);
	kill id

let get id =
	get_document id

let current () =
	match Global.opt current_bind with
	| None -> None
	| Some (id,_) -> Some id

let close_all () =
	Hashtbl.iter (fun id _ -> close id) documents	

let init () =
	Global.set panel (Main.obj#panel "main.panel");	
	let x = Dialogs.create (Some (XPanel (Global.get panel))) "main.doc" in
		topbar := x;
		(x#button "doc.close")#on_click (fun() -> let id,_ = Global.get current_bind in close id);
	Main.add_tool "document.kill" (fun() -> match current() with None -> () | Some k -> kill k);
	Main.before_sync close_all;
;;
open Run;;

register {
	m_name = "Document";
	m_init = Some init;
	m_close = Some close_all;
	m_main = None;
}
