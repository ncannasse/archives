open Win32

type logtype =
	MSG
	| WARNING
	| ERROR
	| TRACE

type logmsg = logtype * string

let event = ref (fun () -> ())

let messages = ref []

let fetch () =
	match !messages with 
	[] -> None
	| x::l -> messages := l; Some x

let log mtype msg =
	ignore(
		match mtype with
		MSG ->
			(print_endline ("Message> "^msg))
		| WARNING ->
			(print_endline ("Warning> "^msg))
		| TRACE ->
			(print_endline ("Trace> "^msg))
		| ERROR ->			
			ignore (message_box ~wnd:null_hwnd ~text:msg ~caption:"Error:" ~options:[MB_OK;MB_ICONERROR;MB_TOPMOST])
	);
	messages := (!messages)@[(mtype,msg)];
	(flush stdout);
	(!event)()
