open Printf

type loads = {
	to_compile : (Expr.type_path,unit) Hashtbl.t;
	types : (Expr.type_path,Types.ttype list) Hashtbl.t;
	mutable class_path : string list;
}

type p_style =
	| StyleJava
	| StyleMSVC

let print_style = ref StyleJava
	
let rec split l str =
	let rec loop = function
		| [] -> -1
		| x :: l ->
			try
				let p = String.index str x in
				let p2 = loop l in
				if p2 = -1 || p2 > p then
					p
				else
					p2
			with
				Not_found -> loop l
	in
	let p = loop l in
	if p = -1 then
		[str]
	else if p = 0 then
		split l (String.sub str 1 (String.length str - 1))
	else
		let sub = String.sub str 0 p in
		sub :: (split l (String.sub str (p+1) (String.length str - (p+1))))

let class_name file =
	let path = Filename.dirname file in
	let path = split ['/';'\\'] path in
	let file = Filename.basename file in
	path , try
		Filename.chop_extension file
	with
		_ -> file	

let mk_path p =
	let l = String.length p in
	if l = 0 then
		"./"
	else match p.[l-1] with 
		| '\\' | '/' -> p
		| _ -> p ^ "/"

let add_file l file =
	Hashtbl.replace l.to_compile (class_name file) ();
	let path = mk_path (Filename.dirname file) in
	if not (List.exists (( = ) path) l.class_path) then
		l.class_path <- path :: l.class_path

let rec parse_class_path l path =
	if path = "" then
		[]
	else
		try
			let p = String.index path ';' in
			let fp = mk_path (String.sub path 0 (p-1)) in
			fp :: (parse_class_path l (String.sub path (p+1) (String.length path - p - 1)));
		with
			Not_found ->
				[mk_path path]

let rec open_class_file (cpath,cname) = function
	| [] -> raise (Typer.Cannot_load_class (cpath,cname))
	| path :: l ->
		try
			let path = path ^ match cpath with [] -> "" | _ -> String.concat "/" cpath ^ "/" in
			let file = path ^ cname ^ ".as" in
			file , open_in file
		with
			_ -> open_class_file (cpath,cname) l

let loader l t path =
	let file , ch = open_class_file path ("" :: l.class_path) in
	Hashtbl.remove l.to_compile path;
	let el = Parser.parse (Lexing.from_channel ch) file in
	close_in ch;
	let c = Checker.context() in
	List.iter (Checker.check c) el;
	let types = List.map (fun e -> Typer.type_expr t e) el in
	Hashtbl.add l.types path types

let rec process l t =
	try
		Hashtbl.iter (fun path () ->
			Typer.load t path;
			raise Exit;
		) l.to_compile;
	with
		| Exit -> process l t

let print_types l =
	let iter f = Hashtbl.iter (fun _ tl -> List.iter f tl) l.types in
	let m = Types.begin_mark() in
	iter (Types.mark m);
	iter Types.end_mark;
	iter (fun t -> print_endline ((if !Typer.print_debug then Types.s_ttype else Types.to_string) ~print_class:true t))

let report (msg,p) etype printer =
	let error_printer file line =
		match !print_style with
		| StyleJava -> sprintf "%s:%d:" file line
		| StyleMSVC -> sprintf "%s(%d):" file line
	in
	let epos = Lexer.get_error_pos error_printer p in
	prerr_endline (sprintf "%s : %s %s" epos etype (printer msg));
	exit 1

;;
try
	let usage = "ASML v1.3 - (c)2003-2004 Motion-Twin" in
	let to_load = {
		to_compile = Hashtbl.create 0;
		types = Hashtbl.create 0;
		class_path = [mk_path ((Filename.dirname (OSDep.exe_name())) ^ "/std/")];
	} in
	let do_print_types = ref false in
	let args_spec = [
		("-cp",Arg.String (fun path -> to_load.class_path <- parse_class_path to_load path @ to_load.class_path)," <paths> : add classpath");
		("-p",Arg.Unit (fun () -> do_print_types := true)," : print types");
		("-d",Arg.Unit (fun () -> Typer.print_debug := true)," : turn debug on");
		("-msvc",Arg.Unit (fun () -> print_style := StyleMSVC)," : use MSVC style errors");
	] in
	Arg.parse args_spec (add_file to_load) usage;
	to_load.class_path <- List.rev to_load.class_path;
	let t = Typer.create (loader to_load) in
	process to_load t;
	if !do_print_types then print_types to_load;
with
	| Lexer.Error (m,p) -> report (m,p) "syntax error" Lexer.error_msg
	| Parser.Error (m,p) -> report (m,p) "parse error" Parser.error_msg
	| Checker.Error (m,p) -> report (m,p) "check error" Checker.error_msg
	| Typer.Error (m,p) -> report (m,p) "type error" Typer.error_msg
	| Typer.Cannot_load_class path ->
		prerr_endline (sprintf "Cannot load class %s" (Expr.s_type_path path));
		exit 1
	| Failure msg ->
		prerr_endline msg;
		exit 1;
