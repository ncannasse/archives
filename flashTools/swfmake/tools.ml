open Swf

let printf = IO.printf

let dump_sizes = ref false
let check_as3 = ref false
let verbose = ref false
let dump_code = ref true

let verbose_msg msg =
	if !verbose then begin
		print_endline msg;
		flush stdout;
	end

let paths = ref [""]

let dump_header ch h =
	printf ch "HEADER:\n  version = %d\n  size = %d x %d\n  nframes = %d\n  compressed = %b\n\n" 
		h.h_version
		h.h_size.right
		h.h_size.bottom
		h.h_frame_count
		h.h_compressed

let open_file bin f =
	let rec loop = function
		| [] -> failwith ("Cannot open file " ^ f)
		| p :: l ->
			try
				(if bin then open_in_bin else open_in) (p ^ f) , p ^ f				
			with
				_ -> loop l
	in
	loop (List.rev !paths)

let resolve_file f =
	let rec loop = function
		| [] -> f
		| p :: l ->
			let f = p ^ f in
			if Sys.file_exists f then
				f
			else
				loop l
	in
	loop (List.rev !paths)

let hex_data s =
	let s , f = (if String.length s > 200 then String.sub s 0 200, true else s, false) in
	let b = Buffer.create 0 in
	for i = 0 to String.length s - 1 do
		Buffer.add_string b (Printf.sprintf "%.2X " (int_of_char s.[i]));
	done;
	let s = Buffer.contents b in
	if f then
		s ^ "..."
	else
		s
		

let rec dump_tags ch pos delta t =
	let tid = ref None in
	let deps = ref [] in
	SwfParser.scan (fun id -> tid := Some id; id) (fun id -> deps := id :: !deps; id) t;
	let name = SwfParser.tag_name t.tdata in
	let name = (match !tid, !deps with
		| None, [] -> name
		| _ -> 
			let len = String.length name in
			if len >= 20 then name else name ^ String.make (20-len) ' '
	) in
	printf ch "%.5X %s%s" pos (if t.tid < 0 then "M " ^ delta else "  " ^ delta) name;
	(match !tid with
	| None -> ()
	| Some id -> printf ch " id:%d" id);
	(match !deps with
	| [] -> ()
	| l -> printf ch " ref:%s" (String.concat "," (List.map string_of_int l)));
	if !dump_sizes then printf ch "(%d)" (SwfParser.tag_length t);
	(match t.tdata with
	| TPlaceObject3 { po_inst_name = Some n }
	| TPlaceObject2 { po_inst_name = Some n } -> printf ch " name:%s" n
	| TFrameLabel (n,_) -> printf ch " label:%s" n
	| TUnknown (_,d) -> printf ch " data:%s" (hex_data d)
	| _ -> ());
	printf ch "\n";
	(match t.tdata with
	| TClip c ->
		ignore(List.fold_left (fun acc t -> dump_tags ch acc (delta ^ "  ") t) pos c.c_tags)
	| TExport el ->
		List.iter (fun e -> printf ch "        %s  %s:%d\n" delta e.exp_name e.exp_id) el
	| TF9Classes l ->
		List.iter (fun c -> printf ch "        %s  %s:%d\n" delta c.f9_classname (match c.f9_cid with None -> 0 | Some id -> id)) l
	| TActionScript3 (id,a) ->
		if !dump_code then As3parse.dump ch a id else Array.iteri (As3parse.dump_class a ch) a.As3.as3_classes;
		if !check_as3 then ignore(As3hlparse.flatten (As3hlparse.parse a));
	| _ ->
		());
	pos + SwfParser.tag_length t

let dump_contents (head,tags) file =
	let ch = open_out (file ^ ".txt") in
	let out = IO.output_channel ch in
	printf out "FILE = %s\n\n" file;
	dump_header out head;
	ignore(List.fold_left (fun acc t -> dump_tags out acc "" t) (SwfParser.header_length head) tags);
	close_out ch

let dump file =
	let ch , _ = open_file true file in
	let data = Swf.parse (IO.input_channel ch) in
	close_in ch;
	dump_contents data file

let rec scan_tag f t =
	SwfParser.scan (f t) (f t) t;
	match t.tdata with
	| TClip c -> List.iter (scan_tag f) c.c_tags
	| _ -> ()

let scan_cids f (h,tags) =
	List.iter (scan_tag f) tags

let make_tables (h,tags) =
	let h = Hashtbl.create 0 in
	let exps = Hashtbl.create 0 in
	let loop t =
		let reg id = Hashtbl.add h id t in
		SwfParser.scan (fun id -> reg id; id) (fun id -> id) t;
		match t.tdata with
		| TExport el ->
			List.iter (fun e -> 
				reg (-e.exp_id);
				Hashtbl.add exps e.exp_name e.exp_id
			) el
		| TF9Classes l ->
			List.iter (fun c ->
				match c.f9_cid with
				| None -> ()
				| Some id ->
					reg (-id);
					Hashtbl.add exps c.f9_classname id
			) l;
		| TFontAlignZones c ->
			reg (-c.cd_id)
		| TDoInitAction a ->
			reg (-a.dia_id)
		| _ -> ()
	in
	List.iter loop tags;
	exps, h

let rec mark tables id =
	let rec mark_tag t = 
		if t.tid > 0 then begin
			t.tid <- -t.tid;
			scan_tag (fun t id -> mark tables id; id) t;
			List.iter mark_tag (Hashtbl.find_all tables (-id)); (* mark deps *)
		end;
	in
	let t = (try Hashtbl.find tables id with Not_found -> failwith ("Missing CID " ^ string_of_int id)) in
	mark_tag t

let mark_jpeg_tables swf =
	List.exists (fun t ->
		match t.tdata with
		| TJPEGTables _ ->
			if t.tid > 0 then t.tid <- -t.tid;
			true
		| _ ->
			false
	) (snd swf)

let merge_jpeg_tables swf =
	let table = ref None in
	List.iter (fun t ->
		match t.tdata with
		| TJPEGTables t ->
			table := Some t
		| _ ->
			()
	) (snd swf);
	match !table with
	| None -> ()
	| Some table ->
		List.iter (fun t ->
			match t.tdata with
			| TBitsJPEG b ->
				t.tdata <- TBitsJPEG2 {
					bd_id = b.jpg_id;
					bd_table = Some table;
					bd_data = b.jpg_data;
					bd_alpha = None;
					bd_deblock = None;
				}
			| _ ->
				()
		) (snd swf)		

let unify ?frame swf_main swf_added =
	let head = fst swf_main in
	let has_tables = mark_jpeg_tables swf_main in
	if has_tables then merge_jpeg_tables swf_added else ignore(mark_jpeg_tables swf_added);
	let insert = List.filter (fun t -> t.tid < 0) (snd swf_added) in
	let rec loop f = function
		| [] ->
			let sf = { tdata = TShowFrame; tid = 0; textended = false } in
			if f = 0 then
				insert @ [sf]
			else
				sf :: loop (f - 1) []
		| { tdata = TShowFrame } as x :: l ->
			if f = 0 then
				insert @ x :: l
			else
				x :: loop (f - 1) l
		| x :: l ->
			x :: loop f l
	in
	let tags = match frame with
		| Some f ->
			if f < 0 then assert false;
			loop f (snd swf_main)
		| None ->
		(match snd swf_main with
		| ({ tdata = TFilesAttributes _ } as sb) :: ({ tdata = TSetBgColor _ } as bg) :: l -> sb :: bg :: insert @ l
		| ({ tdata = TSetBgColor _ } as bg) :: l -> bg :: insert @ l
		| _ -> assert false)
	in
	head , tags
