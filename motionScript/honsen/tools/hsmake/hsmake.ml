open ExtString
open Png

type context = {
	mutable mtv_id : int;
	mutable childs : Mtw.t list;
	mutable mts : string list;
	mutable paths : string list;
	resources : (int,string * string) Hashtbl.t;
	file : string;
}

let hs_version = 1

let dtd_data = "
	
	<!ELEMENT honsen (mts|swf|mtv|m3d|mtypes|pic|path)*>
	
	<!ATTLIST honsen width CDATA #REQUIRED>
	<!ATTLIST honsen height CDATA #REQUIRED>
	<!ATTLIST honsen fps CDATA #REQUIRED>
	<!ATTLIST honsen bgcolor CDATA #DEFAULT \"0xFFFFFF\">

	<!ELEMENT mts EMPTY>
	<!ATTLIST mts file CDATA #REQUIRED>

	<!ELEMENT swf EMPTY>
	<!ATTLIST swf file CDATA #REQUIRED>
	<!ATTLIST swf res CDATA #REQUIRED>

	<!ELEMENT mtv EMPTY>
	<!ATTLIST mtv file CDATA #REQUIRED>
	<!ATTLIST mtv res CDATA #REQUIRED>

	<!ELEMENT m3d EMPTY>
	<!ATTLIST m3d file CDATA #REQUIRED>

	<!ELEMENT mtypes (classpath)*>
	<!ATTLIST mtypes file CDATA #REQUIRED>

	<!ELEMENT pic EMPTY>
	<!ATTLIST pic file CDATA #REQUIRED>
	
	<!ELEMENT classpath EMPTY>
	<!ATTLIST classpath value CDATA #REQUIRED>

	<!ELEMENT path EMPTY>
	<!ATTLIST path value CDATA #REQUIRED>
"

let context file = {
	mtv_id = 0;
	childs = [];
	mts = [];
	paths = ["."];
	resources = Hashtbl.create 0;
	file = file;
}

let printable data =
	let b = Buffer.create 0 in
	let rec loop k i =
		if i = String.length data then
			()
		else if k = 21 then
			Buffer.add_string b "..."
		else
			let c = String.unsafe_get data i in
			let code = int_of_char c in
			if code >= 32 && code <= 126 && c <> '\\' then begin
				Buffer.add_char b c;
				loop k (i+1)
			end else begin
				Buffer.add_string b (Printf.sprintf "\\%.2X" code);
				loop (k+1) (i+1);
			end;
	in
	loop 0 0;
	Buffer.contents b

let switch_ext f ext =
	try
		Filename.chop_extension f ^ ext
	with _ -> 
		f ^ ext

let xmlint x n =
	try
		int_of_string (Xml.attrib x n)
	with
		_ -> failwith ("Invalid value integer " ^ Xml.attrib x n ^ " for attribute " ^ n ^ " of " ^ Xml.tag x)

let xmlbool x n =
	let a = Xml.attrib x n in
	a = "1" || a = "true"

let compress d =
	let ch = IO.output_string() in
	Mtw.write ch d;
	Mtw.Data ("HSGZ", Extc.zip (IO.close_out ch))

let delete_file f =
	try
		Sys.remove f
	with
		_ -> ()

let data_string d = d ^ "\000"

let mid_hash s = 
	let acc = ref 0 in
	for i = 0 to String.length s - 1 do
		acc := 223 * !acc + Char.code (String.unsafe_get s i)
	done;
	acc := !acc land ((1 lsl 31) - 1);
	!acc

let resource_id ctx name file =
	let id = mid_hash name in
	try
		let name2 , file2 = Hashtbl.find ctx.resources id in
		failwith ("Resources conflicts : " ^ name ^ "(" ^ file ^ ") and " ^ name2 ^ "(" ^ file2 ^ ")");
	with
		Not_found ->
			Hashtbl.add ctx.resources id (name,file);
			Mtw.Number ("rsid", Int32.of_int id)

let rec process_resources ctx file = function
	| Mtw.Group(name,childs) as g ->
		(try
			let n = Mtw.get_data "RSNM" g in
			let l = String.length n in
			if l = 0 || n.[l - 1] <> '\000' then failwith ("Invalid resource name " ^ printable n ^ " in " ^ file);
			Mtw.Group (name, resource_id ctx (String.sub n 0 (l - 1)) file :: List.map (process_resources ctx file) childs)
		with
			Not_found -> 
				Mtw.Group (name, List.map (process_resources ctx file) childs))
	| x -> x

let open_file ctx file =
	let rec loop = function
		| [] -> failwith ("File not found : " ^ file)
		| p :: l ->
			try
				IO.input_channel (open_in_bin (p ^ "/" ^ file))
			with
				_ -> loop l
	in
	loop ctx.paths

let picture_data ctx file req =
	let ch = (try open_file ctx file with _ -> failwith ("File not found " ^ file ^ " required by " ^ req)) in
	match String.lowercase (try snd (String.split file ".") with Invalid_string -> "") with
	| "jpg" | "jpeg" ->
		let data = IO.read_all ch in
		IO.close_in ch;
		data , "jPEG"
	| "png" ->
		let png = Png.parse ch in
		IO.close_in ch;
		let header = Png.header png in
		let data = Extc.unzip (Png.data png)in
		let data = Png.filter png data in
		let w = header.png_width in
		let h = header.png_height in
		let p = ref 0 in
		for k = 1 to w*h do
			let a = String.unsafe_get data !p in
			let r = String.unsafe_get data (!p + 1) in
			let g = String.unsafe_get data (!p + 2) in
			let b = String.unsafe_get data (!p + 3) in
			String.unsafe_set data !p b;
			String.unsafe_set data (!p + 1) g;
			String.unsafe_set data (!p + 2) r;
			String.unsafe_set data (!p + 3) a;
			p := !p + 4;
		done;
		let header = IO.output_string() in
		IO.write_i32 header w;
		IO.write_i32 header h;
		let header = IO.close_out header in
		header ^ data , "bRAW"
	| _ ->
		IO.close_in ch;
		failwith ("Unknown picture extension for file " ^ file ^ " required by " ^ req)

let rec load_m3d_textures ctx file = function
	| Mtw.Group(name,childs) ->
		List.iter (load_m3d_textures ctx file) childs;
	| Mtw.Data("TEX0",n) -> 
		let l = String.length n in
		if l = 0 || n.[l - 1] <> '\000' then failwith ("Invalid texture name " ^ printable n ^ " in " ^ file);
		let tfile = String.sub n 0 (l - 1) in
		let data , ident = picture_data ctx tfile file in
		let rsnm = Filename.basename (try Filename.chop_extension tfile with _ -> tfile) in
		let mtw = Mtw.Group ("Pict",[
			Mtw.Data("RSNM",data_string rsnm);
			Mtw.Ident ident;
			compress (Mtw.Data("DATA",data));
		]) in
		ctx.childs <- process_resources ctx tfile mtw :: ctx.childs
	| _ ->
		()

let rec process_item ctx x =
	match String.uppercase (Xml.tag x) with
	| "MTS" ->
		ctx.mts <- Xml.attrib x "file" :: ctx.mts
	| "SWF" ->
		let file = Xml.attrib x "file" in
		let res = Xml.attrib x "res" in
		let ch = open_file ctx file in 
		let swf = Swf.parse ch in
		IO.close_in ch;
		ctx.mtv_id <- ctx.mtv_id + 1;
		let mtw = process_resources ctx file (Swf2mtv.process swf res ctx.mtv_id) in
		ctx.childs <- mtw :: ctx.childs
	| "MTV" ->
		let file = Xml.attrib x "file" in
		let res = Xml.attrib x "res" in
		let ch = open_file ctx file in 
		let mtw = Mtw.read ch in
		IO.close_in ch;
		ctx.mtv_id <- ctx.mtv_id + 1;
		let mtw = (match mtw with
			| Mtw.Group("Mtvd",childs) as g -> Mtw.Group("Mtvd", Mtw.Number("mtvi",Int32.of_int ctx.mtv_id) :: Mtw.Data ("RSNM",data_string res) :: childs)
			| _ -> failwith ("Invalid MTV file " ^ file)
		) in
		let mtw = process_resources ctx file mtw in
		ctx.childs <- mtw :: ctx.childs
	| "M3D" ->
		let file = Xml.attrib x "file" in
		let ch = open_file ctx file in
		let mtw = Mtw.read ch in
		IO.close_in ch;
		let mtw = process_resources ctx file mtw in
		load_m3d_textures ctx file mtw;
		ctx.childs <- mtw :: ctx.childs;
	| "MTYPES" ->
		let file = Xml.attrib x "file" in
		let cp = "-cp mts" :: List.map (fun x -> "-cp " ^ Xml.attrib x "value") (Xml.children x) in
		let cmd = Printf.sprintf "mtypes -mtx %s -mtx-gen-only %s %s" file (String.concat " " cp) file in
		if Sys.command cmd <> 0 then failwith "Aborted";
		ctx.mts <- switch_ext file ".mts" :: ctx.mts
	| "PIC" ->
		let file = Xml.attrib x "file" in		
		let data , ident = picture_data ctx file ctx.file in
		let rsnm = Filename.basename (try Filename.chop_extension file with _ -> file) in
		let mtw = Mtw.Group ("Pict",[
			Mtw.Data("RSNM",data_string rsnm);
			Mtw.Ident ident;
			compress (Mtw.Data("DATA",data));
		]) in
		ctx.childs <- process_resources ctx file mtw :: ctx.childs
	| "PATH" ->
		ctx.paths <- (Xml.attrib x "value" ^ "/") :: ctx.paths
	| _ ->
		failwith ("Unhandled item " ^ Xml.tag x)	

let build_mts ctx =
	let mts = List.rev ctx.mts in
	let exe_dir = Extc.executable_path() in
	let mts_cc = (try ignore(String.index exe_dir ' '); "\"" ^ exe_dir ^ "mts\"" with _ -> exe_dir ^ "mts") in
	if Sys.command (mts_cc ^ " -c " ^ (String.concat " " mts)) <> 0 then failwith "Aborted.";
	let out_mtx = switch_ext ctx.file ".mtx" in
	let out_res = switch_ext ctx.file ".res" in
	let ch = open_out_bin out_res in
	Hashtbl.iter (fun id (name,_) ->
		output_string ch name;
		output_char ch '\n';
	) ctx.resources;
	close_out ch;
	let r = Sys.command (mts_cc ^ " -ldbg -l " ^ out_mtx ^ " -r " ^ out_res ^ " " ^ (String.concat " " (List.map (fun f -> switch_ext f ".mto") mts))) in
	List.iter (fun f -> delete_file (switch_ext f ".mto")) mts;
	delete_file out_res;
	if r <> 0 then failwith "Aborted.";
	let data = Std.input_file ~bin:true out_mtx in
	ctx.childs <- compress (Mtw.Data ("CODE",data)) :: ctx.childs;
	delete_file out_mtx

let process x file =
	let w = xmlint x "width" in
	let h = xmlint x "height" in
	let fps = xmlint x "fps" in
	let bgcolor = xmlint x "bgcolor" in
	let h =	Mtw.Group ("Head",[
		Mtw.Number ("hver", Int32.of_int hs_version);
		Mtw.Number ("wdth", Int32.of_int w);
		Mtw.Number ("hght", Int32.of_int h);
		Mtw.Number ("nfps", Int32.of_int fps);
		Mtw.Number ("bgcl", Int32.of_int bgcolor);
	]) in
	let ctx = context file in
	List.iter (process_item ctx) (Xml.children x);
	ctx.childs <- List.rev ctx.childs;
	build_mts ctx;
	ctx.childs <- h :: ctx.childs;
	ctx , Mtw.Group ("Hsen",ctx.childs)

let dump_mtw file =
	let ch = open_file (context file) file in
	let mtw = Mtw.read ch in
	IO.close_in ch;
	let ch = IO.output_channel (open_out (switch_ext file ".txt")) in
	IO.printf ch "Dump of %s (%d bytes)\n" file (Mtw.size mtw);
	let pos = ref 0 in
	let rec loop tabs = function
		| Mtw.Ident t ->
			IO.printf ch "%.6X%s%s\n" !pos tabs t;
		| Mtw.Data ("HSGZ",data) ->
			let udata = Extc.unzip data in
			let sub = Mtw.read (IO.input_string udata) in
			IO.printf ch "%.6X%s%s %d -> %d (%.1f%%)\n" !pos tabs "HSGZ" (String.length udata) (String.length data) (float (String.length data * 100) /. float (String.length udata));
			pos := 0;
			loop (tabs ^ "  ") sub
		| Mtw.Data (t,data) ->
			IO.printf ch "%.6X%s%s [%d] %s\n" !pos tabs t (String.length data) (printable data)
		| Mtw.Number (t,n) ->
			IO.printf ch "%.6X%s%s = %ld (0x%lX)\n" !pos tabs t n n
		| Mtw.Group (t,subs) as x ->
			IO.printf ch "%.6X%s%s [%d]\n" !pos tabs t (Mtw.size x);
			pos := !pos + 8;
			List.iter (fun s ->
				let old = !pos in
				loop (tabs ^ "  ") s;
				pos := old + Mtw.size s
			) subs;
	in
	loop "    " mtw;
	IO.close_out ch

let main() =
	let usage = "HSMake v2.01 - Copyright (C)2005 Motion-Twin\nUsage : hsmake <file>\n Options :" in
	let args_spec = [
		("-d",Arg.String (fun file -> dump_mtw file),"<file> : dump mtw file content into text");
		("-v",Arg.Unit (fun() -> Swf2mtv.verbose := true),": verbose");
	] in
	let display_usage() = Arg.usage args_spec usage; raise Exit in
	let dtd = Dtd.check (Dtd.parse_string dtd_data) in
	Arg.parse args_spec (fun file ->
		let xml = Xml.parse_file file in
		let xml = Dtd.prove dtd "honsen" xml in
		let ctx , mtw = process xml file in
		let ch = IO.output_channel (open_out_bin (switch_ext file ".hs")) in
		Mtw.write ch (compress mtw);
		IO.close_out ch;		
	) usage

let error f msg =
	prerr_endline (f msg);
	exit 1

;;
try
	main()
with
	| Exit -> ()
	| Xml.File_not_found f -> error (fun f -> "File not found : " ^ f) f
	| Xml.Error e -> error Xml.error e
	| Dtd.Prove_error e -> error Dtd.prove_error e
	| Dtd.Parse_error e -> error Dtd.parse_error e
	| Dtd.Check_error e -> error Dtd.check_error e
	| Swf.Error msg -> error (fun msg -> "Swf error : " ^ msg) msg
	| Png.Error msg -> error Png.error_msg msg
	| Failure msg -> error (fun msg -> msg) msg

