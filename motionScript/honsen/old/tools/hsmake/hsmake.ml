open Mtw
open Hsx

type hs_file = {
	hmts : string list;
	hdata : Mtw.t list;
}

let open_file fname =
	try
		open_in_bin fname
	with
		_ -> failwith ("File not found : "^fname)

let read_file fname =
	let buf_len = 8192 in
	let input_all ch =
		let rec loop acc total buf ofs =
			let n = input ch buf ofs (buf_len - ofs) in
			if n = 0 then
				let res = String.create total in
				let pos = total - ofs in
				String.blit buf 0 res pos ofs;
				let coll pos buf =
					let new_pos = pos - buf_len in
					String.blit buf 0 res new_pos buf_len;
					new_pos
				in
				ignore(List.fold_left coll pos acc);
				res
			else
				let new_ofs = ofs + n in
				let new_total = total + n in
				if new_ofs = buf_len then
					loop (buf :: acc) new_total (String.create buf_len) 0
				else
					loop acc new_total buf new_ofs
		in
		loop [] 0 (String.create buf_len) 0
	in
	let ch = open_file fname in
	let s = input_all ch in
	close_in ch;
	s

let data_string s =
	s ^ "\000"

let switch_ext ext f =
	let base = (try 
		let p = String.rindex f '.' in
		String.sub f 0 p 
	with
		Not_found -> f) in
	base ^ "." ^ ext

let command cmd =
	Sys.command cmd

let delete_file f =
	try
		Sys.remove f
	with 
		_ -> ()

let process_rsid id items =
	match id with
	| None -> items
	| Some (id,_) ->
		Number ("rsid",Int32.of_int id) :: items

let do_compress = ref true

let rec process_item mts acc it =
	let mtw tag childs =
		Mtw.Group(tag,childs)
	in
	let childs() =
		List.fold_left (process_item mts) [] (Tree.childs it)
	in
	match Tree.data it with
	| Mts file ->
		mts := file :: !mts;
		acc
	| Mtw m
	| Atw m ->
		mtw "Mtwt" (List.rev (childs())) :: acc		
	| Pic p ->
		let data = read_file p.pic_file in
		mtw "Pict" (process_rsid p.pic_id [
			Data ("RSNM",data_string (Filename.basename p.pic_file));
			Data ("DATA",data)
		] @ (if p.pic_wrap then [Ident "wRAP"] else [])) :: acc;
	| Font f ->
		let flags = ref 0 in
		if f.font_bold then flags := !flags lor 1;
		if f.font_italic then flags := !flags lor 2;
		let filedata = (match f.font_file with
			| None -> []
			| Some file ->
				let data = read_file file in
				[Data ("DATA",data)])
		in
		mtw "Font" (process_rsid f.font_id [
				Data ("FACE",data_string f.font_face);
				Number ("size",Int32.of_int f.font_size);
				Number ("flag",Int32.of_int !flags);
			] @ filedata) :: acc
	| Dir d ->
		childs() @ acc
	| Infos i ->
		do_compress := i.inf_compress;
		childs() @ mtw "Head" [
			Number ("wdth", Int32.of_int i.inf_width);
			Number ("hght", Int32.of_int i.inf_height);
			Number ("bgcl", Int32.of_int i.inf_bgcolor);
			Number ("nfps", Int32.of_int i.inf_fps)
		] :: acc
	| Obj o ->
		match o.obj_id with
		| None -> mtw o.obj_tag (o.obj_data @ (List.rev (childs())) ) :: acc
		| Some (id,_) -> mtw o.obj_tag (Number ("rsid",Int32.of_int id) :: o.obj_data @ (List.rev (childs())) ) :: acc

external hslib_zip : string -> int -> string -> int -> int = "caml_hslib_zip"

let process h out_file =
	let mts = ref [] in
	let datas = List.rev (process_item mts [] (Tree.root h.htree)) in
	let mts = List.rev !mts in
	let out_mtx = switch_ext "mtx" out_file in
	let out_res = switch_ext "res" out_file in
	if command (exe_dir ^ "\\mts -c " ^ (String.concat " " mts)) <> 0 then failwith "Aborted.";
	let ch = open_out_bin out_res in
	Hashtbl.iter (fun _ name ->
		output_string ch name;
		output_char ch '\n';
	) h.hres;
	close_out ch;
	if command (exe_dir ^ "\\mts -ldbg -l " ^ out_mtx ^ " -r " ^ out_res ^ " " ^ (String.concat " " (List.map (switch_ext "mto") mts))) <> 0 then begin
		delete_file out_res;
		failwith "Aborted.";
	end;
	delete_file out_res;
	let hs_code = Data ("CODE",read_file out_mtx) in
	let datas = (match datas with [] -> [hs_code] | h :: data -> h :: hs_code :: data) in
	let datas = (match !do_compress with
		| true ->
			print_endline "Compressing";
			flush stdout;
			let ch = IO.output_string() in
			Mtw.write ch datas;
			let str = IO.close_out ch in
			let cstr = String.create (String.length str * 2 + 30) in
			let clen = hslib_zip str (String.length str) cstr (String.length cstr) in
			if clen = -1 then failwith "Zip failure";
			[(Mtw.Data("HSGZ",String.sub cstr 0 clen))]
		| false ->
			datas
	) in
	let ch = IO.output_channel (open_out_bin out_file) in
	Mtw.write ch datas;
	IO.close_out ch;
	delete_file out_mtx

let main() =
	let usage = "HSMAKE v1.0 - Copyright (C)2004 Motion-Twin" in
	let display_usage() = failwith (usage^"\r\n"^" Usage : hsmake <file>") in
	let file = ref None in
	let args = [] in
	Arg.parse args (fun arg ->
		match !file with
		| Some file -> display_usage()
		| None -> file := Some arg
	) usage;
	let file = (match !file with None -> display_usage() | Some f -> f) in
	let h = Hsx.read file in	
	process h (switch_ext "hs" file)
;;
try
	main()
with
	| Hsx.Error msg -> prerr_endline msg; exit 1
	| Failure msg -> prerr_endline msg; exit 1
