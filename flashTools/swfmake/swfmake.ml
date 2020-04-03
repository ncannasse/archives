open Swf
open Printf
open ExtString

type data = {
	swf : swf option;
	max_cid : int;
}

let dtd = "
	<!ELEMENT swf (mtypes|base|include|mark|obfu|output|clip|xml)*>

	<!ELEMENT mtypes (file|dir|classpath|params)*>
	<!ATTLIST mtypes file CDATA #REQUIRED>
	<!ATTLIST mtypes zip (true|false) #IMPLIED>
	<!ATTLIST mtypes trace CDATA #IMPLIED>

	<!ELEMENT file EMPTY>
	<!ELEMENT dir EMPTY>
	<!ELEMENT classpath EMPTY>
	<!ELEMENT params EMPTY>
	<!ATTLIST file value CDATA #REQUIRED>
	<!ATTLIST dir value CDATA #REQUIRED>
	<!ATTLIST dir expand (true|false) #DEFAULT \"true\">
	<!ATTLIST classpath value CDATA #REQUIRED>
	<!ATTLIST params value CDATA #REQUIRED>

	<!ELEMENT base EMPTY>
	<!ATTLIST base file CDATA #REQUIRED>
	<!ATTLIST base fps CDATA #IMPLIED>
	<!ATTLIST base width CDATA #IMPLIED>
	<!ATTLIST base height CDATA #IMPLIED>
	<!ATTLIST base version CDATA #IMPLIED>

	<!ELEMENT include EMPTY>
	<!ATTLIST include file CDATA #REQUIRED>
	<!ATTLIST include link CDATA #DEFAULT \"*\">
	<!ATTLIST include exclude CDATA #IMPLIED>
	<!ATTLIST include frame CDATA #IMPLIED>

	<!ELEMENT obfu (protect)*>
	<!ATTLIST obfu debug CDATA #DEFAULT \"0\">
	<!ATTLIST obfu key CDATA #IMPLIED>

	<!ELEMENT protect EMPTY>
	<!ATTLIST protect file CDATA #REQUIRED>

	<!ELEMENT output EMPTY>
	<!ATTLIST output file CDATA #REQUIRED>
	<!ATTLIST output zip (true|false) #IMPLIED>

	<!ELEMENT clip (pic*)>
	<!ATTLIST clip name CDATA #REQUIRED>

	<!ELEMENT pic EMPTY>
	<!ATTLIST pic file CDATA #REQUIRED>

	<!ELEMENT xml EMPTY>
	<!ATTLIST xml file CDATA #REQUIRED>
	<!ATTLIST xml var CDATA #REQUIRED>

	<!ELEMENT mark (protect)*>
	<!ATTLIST mark file CDATA #REQUIRED>
	<!ATTLIST mark output CDATA #REQUIRED>
	<!ATTLIST mark key CDATA #IMPLIED>
	<!ATTLIST mark debug CDATA #IMPLIED>

"

let msvc = ref false

let getswf ctx =
	match ctx.swf with
	| None -> assert false
	| Some s -> s

let load_file ctx file =
	let ch , file = Tools.open_file true file in
	Tools.verbose_msg ("Reading swf file " ^ file);
	let swf = Swf.parse (IO.input_channel ch) in
	close_in ch;
	swf

let create file fps width height version ctx =
	let swf = load_file ctx file in
	let head = fst swf in
	(match fps with None -> () | Some fps -> head.h_fps <- to_float16 (float fps));
	(match version with None -> () | Some v -> head.h_version <- v);
	let width , height =
		(match width with None -> head.h_size.right | Some v -> v * 20),
		(match height with None -> head.h_size.bottom | Some v -> v * 20)
	in
	head.h_size <- {
		rect_nbits = if (max width height) >= 1 lsl 14 then 16 else if max width height >= 1 lsl 13 then 15 else 14;
		left = 0;
		top = 0;
		right = width;
		bottom = height;
	};
	let max = ref (-1) in
	Tools.scan_cids (fun _ id ->
		if id > !max then max := id;
		id
	) swf;
	let swf = fst swf , (match snd swf with
		| { tdata = TFilesAttributes _ } :: _ as l -> l
		| l when version = Some 8 ->
			let id = !max in
			incr max;
			{
				tid = id;
				textended = false;
				tdata = TFilesAttributes { fa_network = false; fa_as3 = false; fa_metadata = false; fa_gpu = false; fa_direct_blt = false };
			} :: l
		| l -> l
	) in
	{
		swf = Some swf;
		max_cid = !max;
	}

let save file zip ctx =
	let ch = open_out_bin file in
	Tools.verbose_msg ("Saving " ^ file);
	let data = getswf ctx in
	let cmp = (fst data).h_compressed in
	(match zip with
	| None -> ()
	| Some z -> (fst data).h_compressed <- z);
	Swf.write (IO.output_channel ch) data;
	(fst data).h_compressed <- cmp;
	close_out ch;
	ctx

let merge_tags ctx ?frame swf =
	let max = ref 0 in
	Tools.scan_cids (fun t id ->
		if t.tid < 0 && id > !max then max := id;
		id
	) swf;
	Tools.scan_cids (fun t id -> id + !max) (getswf ctx);
	let swf = Tools.unify ?frame (getswf ctx) swf in
	{
		swf = Some swf;
		max_cid = !max;
	}

let include_file file frame excl ctx =
	let swf = load_file ctx file in
	Tools.verbose_msg "merging";
	let exps , ids = Tools.make_tables swf in
	let h = Hashtbl.create 0 in
	List.iter (fun e -> if e <> "" && e.[0] != '#' then Hashtbl.add h e (ref false)) excl;
	Hashtbl.iter (fun name id ->
		try
			let t = Hashtbl.find h name in
			t := true;
		with Not_found ->
			Tools.mark ids id
	) exps;
	Hashtbl.iter (fun e t ->
		if not !t then failwith ("This exclude was not found in " ^ file ^ " : " ^ e);
	) h;
	merge_tags ctx ?frame swf

let include_file_item file link ctx =
	let swf = load_file ctx file in
	Tools.verbose_msg "merging";
	let exps , ids = Tools.make_tables swf in
	let id = (try Hashtbl.find exps link with Not_found -> failwith ("No linkage name "^link^" in "^file)) in
	Tools.mark ids id;
	merge_tags ctx swf

let obfusc c ctx =
	Tools.verbose_msg "obfu";
	{ ctx with swf = Some (Obfu.apply c (getswf ctx)) }

let exec cmd ctx =
	Tools.verbose_msg ("exec " ^ cmd);
	let r = Sys.command cmd in
	if r <> 0 then failwith ("Exit code " ^ string_of_int r);
	ctx

let mark file out protects key debug ctx =
	Tools.verbose_msg ("mark " ^ file);
	let swf = load_file ctx file in
	let mark = Filename.chop_extension file ^ ".mark" in
	let ch , _ = Tools.open_file true mark in
	let ch = IO.input_channel ch in
	let rec nread f n =
		if n = 0 then
			[]
		else
			let x = f() in
			x :: nread f (n - 1)
	in
	let read_bool() = IO.read_byte ch <> 0 in
	let read_fun() =
		let p = IO.read_i32 ch in
		p , nread read_bool (IO.read_i32 ch)
	in
	let funs = nread read_fun (IO.read_i32 ch) in
	let pool_size = IO.read_i32 ch in
	let flags = nread read_bool pool_size in
	IO.close_in ch;
	let octx = Obfu.context debug false in
	(match key with
	| None -> ()
	| Some k -> Obfu.set_key octx k);
	Obfu.add_file octx "haxe.v";
	List.iter (Obfu.add_file octx) protects;
	Obfu.obfu_arrowbase octx true;
	Obfu.set_filter octx (fun s -> s.[0] != '_');
	let obfu s f =
		if f then
			s
		else
			Obfu.hash octx s
	in
	let rec mark_code t =
		match t.tdata with
		| TDoAction acts ->
			(match try DynArray.get acts 0 with _ -> AEnd with
			| AStringPool l when List.length l = pool_size ->
				DynArray.set acts 0 (AStringPool (List.map2 obfu l flags));
				List.iter (fun (i,l) ->
					match (try DynArray.get acts (i - 1) with _ -> AEnd) with
					| AFunction2 f ->
						if List.length f.f2_args <> List.length l then failwith "Inconstitent function";
						DynArray.set acts (i - 1) (AFunction2 { f with f2_args = List.map2 (fun (r,s) f -> (r,obfu s f)) f.f2_args l });
					| _ -> failwith "Function not found"
				) funs;
			| _ ->
				DynArray.iteri (fun i a ->
					DynArray.set acts i (Obfu.apply_action octx a)
				) acts;
			);
			t
		| TClip data ->
			{ t with tdata = TClip { data with c_tags = List.map mark_code data.c_tags } }
		| TButton2 _
		| TPlaceObject2 _
		| TPlaceObject3 _
		| TEditText _ ->
			Obfu.apply_tag octx t
		| _ -> t
	in
	let swf = fst swf , List.map mark_code (snd swf) in
	Sys.remove mark;
	let ch = IO.output_channel (open_out_bin out) in
	Swf.write ch swf;
	IO.close_out ch;
	ctx

let list_mt dir expand =
	let files = Array.to_list (Sys.readdir dir) in
	let files = List.filter (fun s -> String.length s > 3 && String.sub s (String.length s - 3) 3 = ".mt") files in
	let files =
		if expand then
			List.map (fun f -> dir ^ "/" ^ f) files
		else
			files
	in
	String.concat " " files

let load_pic file id =
	let file = Tools.resolve_file file in
	try
		SwfPic.load_picture file id
	with
		SwfPic.Error msg -> failwith ("Error in " ^ file ^ " : " ^ SwfPic.error_msg msg)

let load_pics pl =
	let id = ref 1 in
	let pics = ref [] in
	List.iter (fun pic ->
		let file = Xml.attrib pic "file" in
		try
			let f1 , f2 = String.split file "%all" in
			let i = ref 1 in
			(try
				while true do
					pics := load_pic (sprintf "%s%.2d%s" f1 !i f2) !id :: !pics;
					incr i;
					incr id;
				done;
			with
				SwfPic.File_not_found _ -> ());
		with
			Invalid_string ->
				try
					pics := load_pic file !id :: !pics;
					incr id;
				with
					SwfPic.File_not_found _ -> failwith ("File not found " ^ file)
	) pl;
	List.rev !pics

let xml_to_actions x =
	let ctx = {
		swf = None;
		max_cid = -1;
	} in
	let bases = ref [] in
	let rec loop = function
		| [] -> []
		| x :: l ->
			let act = (
				match Xml.tag x with
				| "base" ->
					let fps = try Some (int_of_string (Xml.attrib x "fps")) with _-> None in
					let width = try Some (int_of_string (Xml.attrib x "width")) with _-> None in
					let height = try Some (int_of_string (Xml.attrib x "height")) with _-> None in
					let version = try Some (int_of_string (Xml.attrib x "version")) with _-> None in
					let file = Xml.attrib x "file" in
					bases := file :: !bases;
					create file fps width height version
				| "mtypes" ->
					let trace = (try "-trace " ^ Xml.attrib x "trace" with _ -> "") in
					let zip = (try if Xml.attrib x "zip" = "true" then "-swf-zip" else "" with _ -> "") in
					let out = "-swf " ^ Xml.attrib x "file" in
					let args = List.map (fun x ->
						let v = Xml.attrib x "value" in
						match Xml.tag x with
						| "classpath" -> "-cp " ^ v
						| "dir" -> list_mt v (Xml.attrib x "expand" = "true")
						| "file" -> v
						| "params" -> v
						| _ -> assert false
					) (Xml.children x) in
					let args = [
						"mtypes";
						trace;
						zip;
						out;
						if !msvc then "-msvc" else "";
						if !Tools.verbose then "-v" else ""
					] @ args in
					let cmd = String.concat " " (List.filter ((<>) "") args) in
					exec cmd
				| "include" ->
					let file = Xml.attrib x "file" in
					let link = Xml.attrib x "link" in
					let frame = try Some (int_of_string (Xml.attrib x "frame")) with _ -> None in
					if link = "*" then begin
						let excl = (try Some (Xml.attrib x "exclude") with _ -> None) in
						let l = (match excl with
							| None -> []
							| Some file ->
								let ch, _ = Tools.open_file false file in
								let l = Std.input_list ch in
								close_in ch;
								l
						) in
						include_file file frame l
					end else
						include_file_item file link
				| "obfu" ->
					let c = Obfu.context (Xml.attrib x "debug" = "1") false in
					(try Obfu.set_key c (Xml.attrib x "key") with _ -> ());
					(fun ctx ->
						List.iter (fun x -> Obfu.add_file c (Xml.attrib x "file")) (Xml.children x);
						obfusc c ctx
					)
				| "output" ->
					let out = Xml.attrib x "file" in
					let zip = (try Some (Xml.attrib x "zip" = "true") with _ -> None) in
					if not (List.mem out !bases) then (try Sys.remove out with _ -> ());
					save out zip
				| "clip" ->
					let clname = Xml.attrib x "name" in
					let pics = load_pics (Xml.children x) in
					let swf = SwfPic.make_clip clname pics (1 + List.length pics) in
					(fun ctx -> merge_tags ctx ((),swf))
				| "mark" ->
					let file = Xml.attrib x "file" in
					let out = Xml.attrib x "output" in
					let files = List.map (fun x -> Xml.attrib x "file") (Xml.children x) in
					let key = (try Some (Xml.attrib x "key") with _ -> None) in
					mark file out files key (try Xml.attrib x "debug" = "1" with _ -> false)
				| "xml" ->
					let file = Xml.attrib x "file" in
					let v = Xml.attrib x "var" in
					let report e =
						failwith ("Error in " ^ file ^ " : " ^ e)
					in
					let x = (try
						Xml.parse_file (Tools.resolve_file file)
					with
						| Xml.Error e -> report (Xml.error e)
						| Dtd.Parse_error e -> report (Dtd.parse_error e)
						| Dtd.Check_error e -> report (Dtd.check_error e)
						| Dtd.Prove_error e -> report (Dtd.prove_error e))
					in
					let xdata = Xml.to_string x in
					let rec loop s =
						let l = String.length s in
						let maxl = 65000 in
						if l < maxl then
							[APush [PString s]]
						else
							let op = APush [PString (String.sub s 0 maxl)] in
							let acc = loop (String.sub s maxl (l - maxl)) in
							op :: acc @ [AAdd]
					in
					(fun ctx ->
						let header, swf = getswf ctx in
						let cl = { tid = 0; textended = false; tdata = TDoAction (DynArray.of_list (APush [PString v] :: loop xdata @ [ASet])) } in
						{ ctx with swf = Some (header,cl :: swf) }
					)
				| t ->
					failwith ("Invalid tag " ^ t)
			) in
			act :: (loop l)
	in
	ctx , loop (Xml.children x)

let normalize_path p =
	let l = String.length p in
	if l = 0 then
		"./"
	else match p.[l-1] with
		| '\\' | '/' -> p
		| _ -> p ^ "/"

;;
let report err =
	prerr_endline err;
	exit 1
in
let usage = "SWFMAKE 1.5 - (c)2004-2010 Motion-Twin" in
let makefile = ref "swfmake.xml" in
let dumps = ref [] in
let did_something = ref false in
SwfParser.init Extc.input_zip Extc.output_zip;
Tools.paths := normalize_path (Extc.executable_path()) :: "/usr/lib/swfmake/" :: !Tools.paths;
let args = [
	("-v",Arg.Unit (fun () -> Tools.verbose := true),"verbose mode");
	("-f",Arg.String (fun f -> makefile := f),"makefile");
	("-d",Arg.String (fun f -> dumps := f :: !dumps),"dump file");
	("-dump-size",Arg.Unit (fun () -> Tools.dump_sizes := true),"dump sizes");
	("-dump-nocode",Arg.Unit (fun () -> Tools.dump_code := false),"don't dump as3 code");
	("-dump-as3-pos",Arg.Unit (fun () -> As3parse.dump_code_size := false),"alternate as3 code dump");
	("-check",Arg.Unit (fun () -> Tools.check_as3 := true),"check as3 code");
	("-p",Arg.String (fun p -> Tools.paths := normalize_path p :: !Tools.paths),"additional search path");
	("-msvc",Arg.Unit (fun () -> msvc := true),"use mtypes -msvc");
	("-partial",Arg.Unit (fun () -> SwfParser.full_parsing := false),"don't fully parse the SWF");
	("-io",Arg.String (fun file ->
		let ch , _ = Tools.open_file true file in
		let data = Swf.parse (IO.input_channel ch) in
		close_in ch;
		let ch = IO.output_string() in
		Swf.write ch data;
		let ch = IO.input_string (IO.close_out ch) in
		ignore(Swf.parse ch);
		did_something := true
	),"<file> : read/write/read an SWF in order to check library");
	("-mark",Arg.String (fun file ->
		let file, key = (try ExtString.String.split file "@" with _ -> file, file) in		
		let protect = (try Filename.chop_extension file with _ -> file) ^ ".v" in		
		mark file file (if Sys.file_exists protect then [protect] else []) (Some key) (key = "") ();
		did_something := true
	),"<file[@key]> : obfuscate the haXe/Flash8 SWF file")
] in
try
	Arg.parse args (fun s -> raise (Arg.Bad s)) usage;
	(match !dumps with
	| [] when not !did_something ->
		let dtd = Dtd.check(Dtd.parse_string dtd) in
		let x = Xml.parse_file (Tools.resolve_file (!makefile)) in
		let x = Dtd.prove dtd "swf" x in
		let ctx, actions = xml_to_actions x in
		let rec loop ctx = function
			| [] -> ()
			| act :: l ->
				let ctx = act ctx in
				loop ctx l
		in
		loop ctx actions
	| _ ->
		List.iter Tools.dump (List.rev !dumps));
with
	| Xml.Error e -> report (Xml.error e)
	| Xml.File_not_found f -> report ("File not found " ^ f)
	| Dtd.Parse_error e -> report (Dtd.parse_error e)
	| Dtd.Check_error e -> report (Dtd.check_error e)
	| Dtd.Prove_error e -> report (Dtd.prove_error e)
	| Failure msg -> report ("Error : "^msg)
