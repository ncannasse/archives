open ExtString

let normalize_path p =
	let l = String.length p in
	if l = 0 then
		"./"
	else match p.[l-1] with 
		| '\\' | '/' -> p
		| _ -> p ^ "/"

;;
let usage = "Obfuscator v1.5 - (c)2004-2007 Motion-Twin" in
Tools.paths := normalize_path (Extc.executable_path()) :: !Tools.paths;
Random.self_init();
SwfParser.init SwfZip.inflate SwfZip.deflate;
let output = ref None in
let input = ref None in
let key = ref None in
let debug = ref false in
let vars = ref ["flash7.v"] in
let dump = ref false in
let random_key() =
	String.init 64 (fun i -> char_of_int (Random.int 256))
in
let args = [
	("-vars",Arg.String (fun s -> vars := s :: !vars),"<file> : add a file containing variables that will be protected");
	("-key",Arg.String (fun k -> key := Some k),"<key> : specify key for obfuscating swf");
	("-rndkey",Arg.Unit (fun k -> key := Some (random_key())),"use a random key");
	("-d",Arg.String (fun p -> Tools.paths := normalize_path p :: !Tools.paths),"<path> : add an additional search path");
	("-o",Arg.String (fun s -> output := Some s),"<file> : specify output swf file");
	("-dbg",Arg.Unit (fun () -> debug := true),"debug mode");
	("-dump",Arg.Unit (fun () -> dump := true),"dump a vars.txt file containing all variables");
	("-v",Arg.Unit (fun () -> Tools.verbose := true),"verbose mode");
] in
Arg.parse args (fun s -> 
	input := Some s;
	if !output = None then output := Some (Filename.chop_extension s ^ "_obfu.swf")
) usage;
try
	match !input , !output with
	| None , _ | _ , None -> Arg.usage args usage
	| Some input , Some output -> 
		let ch , file = Tools.open_file true input in
		Tools.verbose_msg ("Reading swf file " ^ input);
		let swf = Swf.parse (IO.input_channel ch) in
		close_in ch;
		let c = Obfu.context !debug !dump in
		(match !key with None -> () | Some k -> Obfu.set_key c k);
		List.iter (fun f -> Obfu.add_file c f) !vars;
		let swf = Obfu.apply c swf in
		let ch = open_out_bin output in
		Tools.verbose_msg ("Saving " ^ output);
		Swf.write (IO.output_channel ch) swf;
		close_out ch
with
	Failure msg -> 
		prerr_endline ("Error : " ^ msg);
		exit 1