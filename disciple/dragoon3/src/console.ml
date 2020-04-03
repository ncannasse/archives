open Osiris
open Mtlib

let messages = MList.empty()

let console() = Main.obj#listbox "console"

let onmessage () =
	match Log.fetch() with
	None -> false
	| Some ((mtype,msg) as m) ->
		MList.add messages m;
		(console())#redraw;
		(console())#set_top_index ((MList.length messages)-1);
		true

let print_message m =
	let _,s = m in s

let print_messages () =
	MList.to_list (MList.map print_message messages)

let init () =
	let c = Main.obj#panel "main.bottom" in
		c#visible true;
		Dialogs.create_in Main.obj "main.bottom" "console";
		(console())#listprint print_messages;	
	Log.event := (fun x -> ignore(onmessage x));
	while onmessage() do (); done;
;;
open Run;;

register {
	m_name = "Console";
	m_init = Some init;
	m_close = Some (fun () -> Log.event := (fun () -> ()));
	m_main = None;
}

