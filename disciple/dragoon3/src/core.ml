open Log
open Run
open Win32

let modules = (Hashtbl.create 0)
let are_loading = (Hashtbl.create 0)

let modules_infos = ref []

let while_loading () = ()

let current_module = ref "Boot"

let on_module_register modinfos =
	modules_infos := !modules_infos@[modinfos];
	Hashtbl.add modules modinfos.m_name ();
	log MSG ("Module loaded : "^modinfos.m_name)

let rec load_module modname =
	try
		Hashtbl.find modules modname
	with
		Not_found ->
		try
			Hashtbl.add modules modname ();
			Hashtbl.add are_loading modname ();			
			current_module := ("Loading "^modname);
			Dynlink.loadfile (modname^".cma");	
			Hashtbl.remove are_loading modname;
		with
			Dynlink.Error(Dynlink.Unavailable_unit(depend)) |
			Dynlink.Error(Dynlink.Linking_error(_,Dynlink.Undefined_global(depend))) as e ->
				try
					(Hashtbl.find are_loading depend);
					failwith ("Crossing with "^depend);
				with
					Not_found ->
					(load_module depend);
					(Hashtbl.remove modules modname);
					(load_module modname)

let load_file ~data =
	let modname = (String.capitalize (String.lowercase (Filename.chop_extension data.fd_file_name))) in
		(load_module modname);
		true

let load_modules () =
	(Win32.find_files "*.cma" load_file)

let exc_to_string where e =
	("Exception in <"^where^"> :\n"^
		match e with
			Dynlink.Error e -> ("Dynlink.Error("^(Dynlink.error_message e)^")")
			| Win32.Error id -> ("Win32.Error(\""^(format_message (Fms_system id) Fmw_max_width None)^"\","^(string_of_int id)^")")
			| error -> (Printexc.to_string error)
	)

let init_modules () =	
	List.iter (fun m -> 			
		match m.m_init with
		| None -> ()
		| Some func ->
			current_module := "Init "^m.m_name;
			try
				func();
			with
				e -> log ERROR (exc_to_string !current_module e)	
	) !modules_infos

let error_to_string n =
	format_message
		~source:(Fms_system n)
		~width:Fmw_none
		~args:None

let preloaded modname =
	Hashtbl.add modules modname ()

let rec handle e =
	try log ERROR (exc_to_string !current_module e); with e -> handle e

let rec main () =
	try
		while true do
			List.iter
				(fun m ->
					match m.m_main with
					| None -> ()
					| Some func ->
						current_module := "Running "^m.m_name;
						func()) !modules_infos;
		done;	
	with
		| Exception_no_display -> main()
		| Exit_application -> ()
		| e -> handle e; main()

let close () =
	List.iter
		(fun m ->
			match m.m_close with
			| None -> ()
			| Some func ->
				current_module := "Closing "^m.m_name;
				try
					func();
				with
					e -> log ERROR (exc_to_string !current_module e)
		) (List.rev !modules_infos)
;;
try
	current_module := "Load";
	preloaded "Run";
	preloaded "Win32";
	preloaded "Log";
	preloaded "Core";
	if Array.length Sys.argv > 1 && (Array.get Sys.argv 1) = "-d" then preloaded "Dragoon3" else preloaded "Disciple";
	Run.on_register on_module_register;
	Run.on_exc (fun msg e -> log ERROR (exc_to_string msg e));
	load_modules();
	init_modules();
	(try
		ignore(List.find (fun m -> m.m_main <> None) !modules_infos);
		main();
	with
		Not_found -> ());
	close();
	log MSG "[Exit]";
	Gc.full_major();
with
	e -> (log ERROR (exc_to_string !current_module e))
