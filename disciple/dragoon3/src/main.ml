open Osiris
open Vfs
open Log

let wnd = "main.win"

let obj = Dialogs.create None wnd

let run = ref true

let before_sync_func = ref (fun () -> ())

let before_sync f = before_sync_func := f

let on_sync x str =
	(log MSG str);
	(x#label "sync.text")#caption str

let add_tool name func =
	Dialogs.create_in obj "menu.tools" name;
	(obj#menuitem name)#on_click func

let export_all() =
	let file = Osiris.create_file "vfs.xml" [("XML Document",["*.xml"])] in
	(match file with
	| None -> ()
	| Some f ->
		let x = Vfs.export() in
		let h = open_out f in
		output_string h (Xml.to_string_fmt x);
		close_out h;
		message_box "Export done !";)

let import_all() =
	let file = Osiris.open_file [("XML Document",["*.xml"])] in
	(match file with
	| None -> ()
	| Some f ->
		Vfs.import (Xml.parse_file f);
		message_box "Import done !";)

let main () =
	let mainwnd = obj#window "main.win" in
	try
		if  (mainwnd#is_destroyed || not !run || not (mainwnd#process false)) then raise Run.Exit_application
	with
		Hlink_no_data id ->
			let x = Dialogs.create None "vfs.sync" in
			Vfs.checkout_hlink (fun s -> on_sync x s) id;
			Dialogs.close x	

let close () =
	let mainwnd = obj#window "main.win" in
	mainwnd#on_closequery event_true;
	mainwnd#destroy

let init () =
	(obj#window "main.win")#on_closequery (fun () -> run := false; false);
	add_tool "dialogs.reload" Dialogs.reload;
	add_tool "vfs.export" export_all;
	add_tool "vfs.import" import_all;	
	(obj#menuitem "menu.sync")#enable (not Vfs.islocal);
	(obj#menuitem "menu.sync")#on_click
		(fun () -> 
			!before_sync_func();
			let x = Dialogs.create None "vfs.sync" in
			Vfs.sync (fun s -> on_sync x s);			
			Dialogs.close x;
		);
;;
open Run;;

register {
	m_name = "Main";
	m_init = Some init;	
	m_main = Some main;
	m_close = Some close;
}
	