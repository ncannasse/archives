open Bytecode

type error_msg =
	| Duplicate_mid of string * string
	| Import_not_found of string
	| Primitive_not_found of string
	| Module_not_found of string
	| Duplicate_main of string * string
	| Resource_not_found of string

type tmodule = {
	m_name : string;
	m_header : header;
	m_prims : (string hglob_decl * Types.prim_flag, int) Hashtbl.t;
	m_ch : (unit -> int);
	mutable m_iglob : int;
	mutable m_icode : int;
}

exception Error of error_msg

let verbose = ref false

let error_msg = function
	| Module_not_found name -> "module not found "^name
	| Primitive_not_found name -> "primitive not found "^name
	| Import_not_found name -> "imported global not found "^name
	| Duplicate_mid (name,name2) -> "the class members '"^name^"' and '"^name2^"' are generating the same compilation identifiers, please rename one of the two"
	| Duplicate_main (name,name2) -> "function main declared twice in "^name^" and "^name2
	| Resource_not_found name -> "unknown resource : "^name

let error e = raise (Error e)

let link dbg res files = 
	let globsize = ref 0 in
	let codesize = ref 0 in
	let entry_point = ref None in
	let modules = Hashtbl.create 0 in
	let ids = Hashtbl.create 0 in
	let local_res = Hashtbl.create 0 in
	let set_entry_point m n =
		match !entry_point with
		| None -> entry_point := Some (m,n)
		| Some (m2,_) -> error (Duplicate_main (m.m_name,m2.m_name))
	in
	let count_imports oids acc = function
		| GImport _
		| GPrimitive (_,PrimNotUsed,_) -> acc + 1
		| GResource id ->
			if not (Hashtbl.mem local_res id) then begin
				if not (Hashtbl.mem res id) then error (Resource_not_found (find_id oids id));
				let n = !globsize in
				incr globsize;
				Hashtbl.add local_res id n;
			end;
			acc + 1
		| _ -> acc
	in
	let rec load_loop acc name =
		name.[0] <- Char.uppercase name.[0];
		if Hashtbl.mem modules name then
			acc
		else begin
			if !verbose then begin
				Printf.printf "Linking %s\n" name;
				flush stdout;
			end;
			let file_name = String.copy name ^ ".mto" in
			file_name.[0] <- Char.lowercase file_name.[0];
			let ch , _ = (try !Tools.loader file_name with Tools.File_not_found _ -> error (Module_not_found name))in
			let ch = (fun () -> input_byte ch) in
			let h = Binary.read_header ch in
			let m = {
				m_name = name;
				m_header = h;
				m_ch = ch;
				m_iglob = 0;
				m_icode = 0;
				m_prims = Hashtbl.create 0;
			} in
			Hashtbl.add modules name m;
			let acc = Array.fold_left load_loop (name :: acc) h.imports in
			Array.iteri (fun n g ->
				match g with
				| GNamed "main" -> set_entry_point m n
				| GFunction f when f.g_name = "main" && f.g_nargs = 0 ->
					set_entry_point m n
				| GClosure (c,ClExported) when c.g_name = "main" && c.g_nargs = 0 ->
					set_entry_point m n
				| _ ->
					()
			) h.globals;
			Array.iter (fun id ->
				let mid = mid_hash id in
				try
					let id2 = Hashtbl.find ids mid in
					if id <> id2 then error (Duplicate_mid (id,id2));
				with
					Not_found ->
						Hashtbl.add ids mid id) h.oids;
			let nskip = Array.fold_left (count_imports h.oids) 0 h.globals in
			m.m_iglob <- !globsize;
			m.m_icode <- !codesize;
			globsize := !globsize + Array.length h.globals - nskip;
			codesize := !codesize + h.codesize;
			acc
		end;
	in
	let resolve_prim m p =
		try
			Hashtbl.find m.m_prims p
		with
			Not_found ->
				let n = !globsize in
				incr globsize;
				Hashtbl.add m.m_prims p n;
				n
	in
	let resolve_import libname fname nargs =
		try
			let m = Hashtbl.find modules libname in (* has been loaded *)
			let nmax = Array.length m.m_header.globals in
			let gindex = ref m.m_iglob in
			let rec loop n =
				let found() =
					let index = !gindex in
					incr gindex;
					max index (loop (n+1))
				in
				if n = nmax then
					-1
				else match m.m_header.globals.(n) with
					| GNamed name when name = fname && nargs = -1 ->
						found()
					| GFunction f when f.g_name = fname && f.g_nargs = nargs ->
						found()
					| GPrimitive (p,flag,cflag) when p.g_name = fname && (p.g_nargs = nargs || p.g_nargs = -1 || (nargs = -1 && fname.[0] = '#') || (nargs = -1 && cflag = Types.PrimStatic)) ->
						(match flag with
						| PrimUsed -> found()
						| PrimNotUsed ->
							let index = loop (n+1) in
							if index = -1 then resolve_prim m (p,cflag) else index)
					| GClosure (c,ClExported) when c.g_name = fname && c.g_nargs = nargs ->
						found()
					| GImport _ | GPrimitive(_,PrimNotUsed,_) | GResource _ ->
						loop (n+1)
					| GNull
					| GFloat _
					| GString _
					| GNamed _
					| GFunction _
					| GClosure _
					| GPrimitive _ ->
						incr gindex;
						loop (n+1)
			in
			let n = loop 0 in
			if n = -1 then error (Import_not_found (
				if fname.[0] == '#' then
					"class "^libname^"."^(String.sub fname 1 (String.length fname - 1))
				else
					libname^"."^fname^"#"^(string_of_int nargs)
			));
			n
		with
			Not_found -> assert false
	in
	let mod_list = List.fold_left load_loop [] files in
	let globals = Array.create !globsize GNull in
	if !entry_point <> None then codesize := !codesize + 4;
	let code = Tools.new_env 0 !codesize in
	List.iter (fun name ->
		let m = Hashtbl.find modules name in
		let gindexes = Array.create (Array.length m.m_header.globals) (-1) in
		let gindex = ref m.m_iglob in
		Array.iteri (fun gid g ->
			let is_import = ref false in
			(match g with
			| GNull -> ()
			| GNamed _ when not dbg -> ()
			| GFunction f ->
				globals.(!gindex) <- GFunction {
					g_name = if dbg then f.g_name else "";
					g_nargs = f.g_nargs;
					g_addr = f.g_addr + m.m_icode;
				}
			| GClosure (c,_) ->
				globals.(!gindex) <- GFunction {
					g_name = if dbg then c.g_name else "";
					g_nargs = c.g_nargs;
					g_addr = c.g_addr + m.m_icode;
				}
			| GResource id ->
				is_import := true;
				gindexes.(gid) <- Hashtbl.find local_res id
			| GPrimitive (_,PrimNotUsed,_) ->
				is_import := true
			| GPrimitive (p,flag,cflag) ->
				globals.(!gindex) <- GPrimitive ({
					g_name = "";
					g_nargs = p.g_nargs;
					g_addr = p.g_addr;
				},flag,cflag)
			| GNamed _
			| GFloat _
			| GString _ ->
				globals.(!gindex) <- g;			
			| GImport i ->
				is_import := true;
				gindexes.(gid) <- resolve_import m.m_header.imports.(i.g_addr) i.g_name i.g_nargs
			);
			if not !is_import then begin
				gindexes.(gid) <- !gindex;
				incr gindex;
			end;
		) m.m_header.globals;
		(* translate the bytecode *)
		Binary.read_code code m.m_icode (fun n -> gindexes.(n)) m.m_header.codesize m.m_ch;
		(* translate the entry point *)
		(match !entry_point with
		| Some (md,n) when md == m ->
			entry_point := Some (m,gindexes.(n))
		| _ -> ())
	) mod_list;
	(* add found primitives *)
	let globals = Array.init !globsize (fun i -> if i < Array.length globals then globals.(i) else GNull) in
	List.iter (fun name ->
		let m = Hashtbl.find modules name in
		Hashtbl.iter (fun (g,cflag) gindex ->
			globals.(gindex) <- GPrimitive ({ g with g_name = ""},PrimUsed,cflag)
		) m.m_prims;
	) mod_list;
	(* add resources *)
	Hashtbl.iter (fun id gid ->
		globals.(gid) <- GResource id
	) local_res;
	(* add entry point call *)
	(match !entry_point with
	| None -> ()
	| Some (_,n) ->
		Tools.env_set code (!codesize - 4) (op_int OAccGlobal);
		Tools.env_set code (!codesize - 3) n;
		Tools.env_set code (!codesize - 2) (op_int OCall);
		Tools.env_set code (!codesize - 1) 0;
	);
	let ids = if dbg then begin
		let l = Hashtbl.fold (fun _ i acc -> i :: acc) ids [] in
		Array.of_list l
	end else
		[||]
	in
	{
		globals = globals;
		imports = [||];
		oids = ids;
		codesize = !codesize;
	} , code
