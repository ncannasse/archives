open Swf

type context = {
	mutable error : bool;
	mutable key : int array;
	dump : bool;
	debug : bool;
	cache : (string,string) Hashtbl.t;
	mutable links : (string * string) list;
	mutable filter : string -> bool;
	hashed : (int,string) Hashtbl.t;
	mutable obfu_arrowbase : bool;
}

let context dbg du = {
	dump = du;
	error = false;
	filter = (fun _ -> true);
	debug = dbg;
	key = [|0|];
	links = [];
	cache = Hashtbl.create 0;
	hashed = Hashtbl.create 0;
	obfu_arrowbase = false;
}

let obfu_arrowbase ctx f = 
	ctx.obfu_arrowbase <- f

let set_filter c f =
	c.filter <- f

let is_valid_char c = 
	if c >= char_of_int 128 then
		false
	else if c = ':' || c = '/' || c = '\000' then
		false
	else if c >= '0' && c <= '9' then
		true
	else if c = ' ' || c = '(' || c = ')' || c = '{' || c = '}' || c = '[' || c = ']' || c = ',' || c = ';' || c = '-' || c ='+' || c = '*' || c = '=' then
		true
	else
		false

let first_chars, all_chars =
	let fc = ref [] in
	let ac = ref [] in
	for i = 0 to 255 do
		let c = char_of_int i in
		if is_valid_char c then begin
			fc := c :: !fc;
			ac := c :: !ac;
		end else if (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c = '_' then
			ac := c :: !ac;
	done;
	Array.of_list (List.rev !fc) , Array.of_list (List.rev !ac)

let add_file c f =
	let ch , file = Tools.open_file false f in
	Tools.verbose_msg ("Reading obfu file " ^ file);
	let rec loop() =
		let l = input_line ch in
		let len = String.length l in
		if len > 0 && l.[0] <> '#' then
			(try
				let rep = "REPLACE=" in
				let replen = String.length rep in
				if String.length l <= replen || String.sub l 0 replen <> rep then raise Not_found;
				let s = String.index l ':' in
				Hashtbl.add c.cache (String.sub l replen (s - replen)) (String.sub l (s+1) (len - s - 1))
			with
				Not_found -> Hashtbl.add c.cache l l);
		loop()
	in
	try
		loop()
	with
		End_of_file ->
			close_in ch

let char_code ctx c =
	if c >= 'a' && c <= 'z' then
		int_of_char c - int_of_char 'a' + 1
	else if c >= 'A' && c <= 'Z' then
		int_of_char c - int_of_char 'A' + 27
	else if c >= '0' && c <= '9' then
		int_of_char c - int_of_char '0' + 53
	else if c = '_' then
		63
	else if ctx.obfu_arrowbase && c = '@' then
		64
	else
		raise Exit

let set_key c s =
	let key = Base64.str_encode (s ^ "#") in
	c.key <- Array.init (String.length key) (fun i -> try char_code c key.[i] with Exit -> 63)

let make_id id =
	let b = Buffer.create 0 in	
	let flen = Array.length first_chars in
	let len = Array.length all_chars in	
	Buffer.add_char b (first_chars.(id mod flen));
	let id = ref (id / flen) in
	while !id > 0 do
		Buffer.add_char b (all_chars.(!id mod len));
		id := !id / len;
	done;
	Buffer.contents b

let hash c s =
	if String.length s = 0 || not (c.filter s) then
		s
	else
		try
			Hashtbl.find c.cache s
		with
			Not_found ->
				try
					let n = ref 0 in
					let l = String.length s - 1 in
					if s.[0] >= '0' && s.[0] <= '9' then raise Exit;
					for i = 0 to l do
						n := !n * 51;
						let cc = char_code c (String.unsafe_get s i) in
						n := !n + c.key.(i mod (Array.length c.key)) lxor cc;
					done;
					let n = (if !n < 0 then - !n else !n) in
					(try 
						let s2 = Hashtbl.find c.hashed n in
						if s <> s2 then begin
							c.error <- true;
							prerr_endline ("Obfu conflict between " ^ s ^ " and " ^ s2);
						end;
					with
						Not_found -> Hashtbl.add c.hashed n s);			
					if c.debug then 
						"@" ^ s ^ "#"
					else begin
						let s2 = make_id n in
						Hashtbl.add c.cache s s2;
						s2
					end	
				with
					Exit ->
						s

let rec hash_path c s =
	try
		let p = String.index s '.' in
		let s1 = String.sub s 0 p in
		(hash c s1) ^ "." ^ (hash_path c (String.sub s (p+1) (String.length s - p - 1)));
	with
		Not_found -> hash c s

let apply_action c = function
	| AStringPool sl -> AStringPool (List.map (hash c) sl)
	| AGotoLabel s -> AGotoLabel (hash c s)
	| AFunction f -> AFunction { f with
			f_name = hash c f.f_name;
			f_args = List.map (hash c) f.f_args;
		}
	| AFunction2 f -> AFunction2 { f with
			f2_name = hash c f.f2_name;
			f2_args = List.map (fun (n,s) -> n,hash c s) f.f2_args
		}
	| APush pl -> APush (List.map (function PString s -> PString (hash c s) | p -> p) pl)
	| a -> a

let apply_actions c a = DynArray.map (apply_action c) a

let apply_export c e =
	let h = hash c e.exp_name in
	let hu = String.uppercase h in
	List.iter (fun (e2,h2) -> 
		if e2 <> e.exp_name && hu = h2 then failwith ("Linkage names " ^ e.exp_name ^ " and " ^ e2 ^ " conflicts.");
	) c.links;
	c.links <- (e.exp_name,hu) :: c.links;
	{ e with exp_name = hash c e.exp_name }

let apply_event c e =
	{
		e with cle_actions = apply_actions c e.cle_actions
	}

let rec apply_tag c t =
	let tag d = { t with tdata = d } in
	match t.tdata with
	| TClip cl -> tag (TClip { cl with c_tags = List.map (apply_tag c) cl.c_tags })
	| TFrameLabel (s,flags) -> tag (TFrameLabel (hash c s,flags))
	| TButton2 b2 -> tag (TButton2 {
			b2 with bt2_actions = List.map 
				(fun a ->
					{ a with bta_actions = apply_actions c a.bta_actions }
				) b2.bt2_actions
		})
	| TPlaceObject2 po2 -> tag (TPlaceObject2 {
			po2 with 
				po_inst_name = (match po2.po_inst_name with None -> None | Some inst -> Some (hash c inst));
				po_events = (match po2.po_events with None -> None | Some evts -> Some (List.map (apply_event c) evts))
		})
	| TPlaceObject3 po2 -> tag (TPlaceObject3 {
			po2 with 
				po_inst_name = (match po2.po_inst_name with None -> None | Some inst -> Some (hash c inst));
				po_events = (match po2.po_events with None -> None | Some evts -> Some (List.map (apply_event c) evts))
		})
	| TEditText t -> tag (TEditText {
			t with edt_variable = hash_path c t.edt_variable
		})
	| TDoAction a -> tag (TDoAction (apply_actions c a))
	| TDoInitAction a -> tag (TDoInitAction { a with dia_actions = apply_actions c a.dia_actions })
	| TExport el -> 
		tag (TExport (List.map (apply_export c) el))
	| _ ->
		t

let apply c (h,tags) =
	let data = (h,List.map (apply_tag c) tags) in
	if c.dump then begin
		let ch = open_out "vars.txt" in
		let l = Hashtbl.fold (fun _ s acc -> s :: acc) c.hashed [] in
		let l = List.sort compare l in
		List.iter (fun s -> output_string ch (s ^ "\n")) l;
		close_out ch
	end;
	if c.error then failwith "Obfu has errors";
	data