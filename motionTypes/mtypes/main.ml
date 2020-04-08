open Printf

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
	let first_upper s =
		if s.[0] >= 'a' && s.[0] <= 'z' then s.[0] <- char_of_int (int_of_char s.[0] - int_of_char 'a' + int_of_char 'A');
		s
	in
	let path = Filename.dirname file in	
	let path = (match split ['/';'\\'] path with "." :: l -> l | l -> l) in
	let file = Filename.basename file in
	path , try
		first_upper (Filename.chop_extension file)
	with
		_ -> first_upper file

let normalize_path p =
	let l = String.length p in
	if l = 0 then
		"./"
	else match p.[l-1] with 
		| '\\' | '/' -> p
		| _ -> p ^ "/"

let rec parse_class_path base_path path =
	if path = "" then
		[]
	else
		let relative fp l =
			if String.length fp >= 2 && (fp.[0] = '/' || fp.[0] = '.' || fp.[1] = ':') then
				fp :: l
			else
				fp :: (base_path ^ fp) :: l
		in
		try
			let p = String.index path ';' in
			let fp = normalize_path (String.sub path 0 (p-1)) in
			let remains = String.sub path (p+1) (String.length path - p - 1) in
			relative fp (parse_class_path base_path remains)
		with
			Not_found ->
				relative (normalize_path path) []

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
	let usage = "MotionTypes v0.3 - (c)2004 Motion-Twin\n Usage : mtypes.exe [options] <files...>\n Options :" in
	let base_path = normalize_path (Extc.executable_path()) in
	let files = ref [] in
	let dump = ref false in
	let time = Sys.time() in
	Plugin.class_path := [base_path;""];
	let args_spec = [
		("-cp",Arg.String (fun path -> Plugin.class_path := parse_class_path base_path path @ !Plugin.class_path),"<paths> : add classpath");
		("-msvc",Arg.Unit (fun () -> print_style := StyleMSVC),": use MSVC style errors");
		("-v",Arg.Unit (fun () -> Typer.verbose := true; Plugin.verbose := true),": turn on verbose mode");
		("-dump",Arg.Unit (fun () -> dump := true),": dump types definitions after typing");
		("-debug",Arg.Unit (fun () -> Typer.print_debug := true),": debug printing (more informations)");
		("-trace", Arg.String (fun s -> Plugin.trace := Some s), "<func> : specify trace function")
	] @ !Plugin.options in
	Arg.parse args_spec (fun file -> files := file :: !files) usage;
	if !files = [] then begin
		Arg.usage args_spec usage
	end else begin
		let typer = Typer.create !Plugin.class_path in
		List.iter (fun file ->			
			let path = class_name file in
			ignore(Typer.load_class typer path Expr.null_pos);
		) (List.rev !files);
		Typer.finalize typer;
		if !Typer.verbose then print_endline ("Time spent : " ^ string_of_float (Sys.time() -. time));
		if !dump then Typer.dump typer;
		List.iter (fun f -> f typer) !Plugin.calls;
	end;
with	
	| Lexer.Error (m,p) -> report (m,p) "syntax error" Lexer.error_msg
	| Parser.Error (m,p) -> report (m,p) "parse error" Parser.error_msg
	| Typer.Error (m,p) -> report (m,p) "type error" Typer.error_msg
	| Checker.Error (m,p) -> report (m,p) "check error" Checker.error_msg
	| Typer.File_not_found file ->
		prerr_endline (sprintf "File not found %s" file);
		exit 1	
	| Failure msg ->
		prerr_endline msg;
		exit 1;
