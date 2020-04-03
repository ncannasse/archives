open Arg
open Printf

type mode =
	| Default
	| Compile
	| Link
	| Dump

type to_compile = {
	modname : string;
	filename : string;
	mutable parsed : bool;
	mutable compiled : bool;
}

let cur_file = ref ""
let verbose = ref false

let remove_ext f =
	try
		Filename.chop_extension f
	with
		_ -> f

let rec check_deps files = function
	| Types.EImport (mname,None) ->
		List.iter (fun f ->
			if f.modname = String.lowercase mname && not f.compiled then begin
				if f.parsed then
					failwith "Recursive dependencies"
				else
					compile files f
			end
		) files
	| _ ->
		()
and
  compile files f =
	if not f.compiled then begin
		let basename = Filename.basename f.filename in
		cur_file := basename;
		let ch , path = (try !Tools.loader f.filename with _ -> failwith ("File not found "^f.filename)) in
		let e = Parser.parse basename (Lexing.from_channel ch) in
		close_in ch;
		Parser.check e;
		f.parsed <- true;
		Types.expr_rec (check_deps files) e;
		if !Linker.verbose then printf "Compiling %s\n" basename;
		flush stdout;
		let c , gtbl = Compile.compile e in
		let output = path ^ (remove_ext f.filename)^".mto" in
		let ch = open_out_bin output in
		Binary.write (output_byte ch) c gtbl;
		close_out ch;
		f.compiled <- true;
	end

let dump f =
	printf "Dumping %s\n" f;
	flush stdout;
	let ch = open_in_bin f in
	let c = Binary.read (fun () -> input_byte ch) in
	close_in ch;
	print_endline (Bytecode.s_bytecode c)

let load_res file res =
	let ch = open_in file in
	let rec loop() =
		try
			let name = input_line ch in
			let id = Bytecode.mid_hash name in
			Hashtbl.add res id name;
			loop()
		with
			End_of_file -> ()
	in
	loop();
	close_in ch

try
	let usage = "Motion Script Compiler - (C)2004-2005 Motion-Twin\n"
				^" version 1.21 - Nicolas Cannasse" in
	let files = ref [] and dumps = ref [] and mode = ref Default and out_file = ref "" and link_dbg = ref false in
	let res = Hashtbl.create 0 in
	let set_mode m =
		if !mode <> Default then raise (Bad Sys.argv.(!Arg.current));
		mode := m;
	in
	let args = [
		("-c", Unit (fun () -> set_mode Compile ), " : compile source files.");
		("-d", Unit (fun () -> set_mode Dump), " : dump compiled bytecode.");
		("-l", String (fun out -> set_mode Link; out_file := out), " <output> : link to output file.");
		("-r", String (fun f -> load_res f res), "<file> : add ressource list.");
		("-ldbg",Unit (fun () -> link_dbg := true), " : keep debug symbols.");
		("-v",Unit (fun () -> Linker.verbose := true), " : verbose mode.");
	] in
	Arg.parse args (fun f -> files := f :: !files) usage;
	Tools.loader := Tools.make_loader !files;
	let files = List.rev !files in
	match !mode with
	| Default when files = [] ->
		Arg.usage args usage;
	| Default
	| Compile ->
		let files = List.map (fun f -> {
			modname = String.lowercase (remove_ext (Filename.basename f));
			filename = f;
			parsed = false;
			compiled = false; }) files
		in
		List.iter (compile files) files
	| Dump ->
		List.iter dump files;
	| Link ->
		let linked = Linker.link !link_dbg res (List.map remove_ext files) in
		let ch = open_out_bin !out_file in
		Binary.write_bin (output_byte ch) linked;
		close_out ch;
		Tools.close_opens()
with
	| Failure msg ->
		prerr_endline msg;
		exit 1;
	| Parser.Error (m,p) ->
		let epos = Lexer.get_error_pos p in
		prerr_endline (sprintf "Compiling %s" (Filename.basename (Lexer.get_current_file())));
		prerr_endline (sprintf "Parse error %s : %s" epos (Parser.error_msg m));
		exit 1;
	| Lexer.Error (m,p) ->
		let file = Lexer.get_current_file() in
		let ll = Lexer.get_lines file in
		let l, p = Lexer.find_line p ll in
		prerr_endline (sprintf "Compiling %s" (Filename.basename file));
		prerr_endline (sprintf "Syntax error line %d character %d : %s" l p (Lexer.error_msg m));
		exit 1;
	| Compile.Error (m,p) ->
		let epos = Lexer.get_error_pos p in
		if !cur_file <> p.Types.pfile then prerr_endline (sprintf "In file %s" p.Types.pfile);
		prerr_endline (sprintf "Compilation error %s : %s" epos (Compile.error_msg m));
		exit 1;
	| Binary.Error m ->
		prerr_endline (sprintf "Loading error : %s" (Binary.error_msg m));
		exit 1;
	| Linker.Error m ->
		prerr_endline (sprintf "Linker error : %s" (Linker.error_msg m));
		exit 1;