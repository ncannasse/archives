open Types 
open Bytecode

type error_msg =
	| Invalid_header
	| Invalid_data
	| Invalid_opcode
	| Invalid_opcode_param
	| Invalid_index
	| Incomplete_file

exception Error of error_msg

let error e =
	raise (Error e)

let error_msg = function
	| Invalid_header -> "invalid bytecode header"
	| Invalid_data -> "invalid data identifier"
	| Invalid_index -> "invalid index"
	| Invalid_opcode -> "invalid opcode"
	| Invalid_opcode_param -> "invalid opcode parameter"
	| Incomplete_file -> "truncated file"

let string_write ch s =
	for i = 0 to String.length s - 1 do
		ch (int_of_char s.[i]);
	done;
	ch 0

let int_write ch i =
	ch (i land 0xFF);
	ch ((i lsr 8) land 0xFF);
	ch ((i lsr 16) land 0xFF);
	ch ((i asr 24) land 0xFF)

let write0 ch op =
	let id = ((op_int op) lsl 2) lor 3 in 
	ch id

let write1 ch op n =
	let id = (op_int op) lsl 2 in
	match n with
	| 0 -> ch id
	| 1 -> ch (id lor 1)
	| n when n >= 0 && n <= 0xFF ->
		ch (id lor 2);
		ch n
	| n ->
		ch (id lor 3);
		int_write ch n

let opcode_write gtbl ch = function
	| PushNull -> write0 ch OPushNull
	| Push -> write0 ch OPush
	| Pop n -> write1 ch OPop n
	| JmpNot n -> write1 ch OJmpNot n
	| JmpIf n -> write1 ch OJmpIf n
	| Jmp n -> write1 ch OJmp n
	| Call n -> write1 ch OCall n
	| CallObj n -> write1 ch OCallObj n
	| Ret n -> write1 ch ORet n
	| Swap -> write0 ch OSwap
	| AccNull -> write0 ch OAccNull
	| AccThis -> write0 ch OAccThis
	| AccNot -> write0 ch OAccNot
	| AccInt n -> write1 ch OAccInt n
	| AccStack n -> write1 ch OAccStack n
	| AccGlobal n ->
		let n = gtbl n in
		if n = -1 then assert false;
		write1 ch OAccGlobal n
	| AccRef n -> write1 ch OAccRef n
	| AccObj n -> write1 ch OAccObj n
	| AccEnv n -> write1 ch OAccEnv n
	| AccEnvRaw n -> write1 ch OAccEnvRaw n
	| SetGlobal n ->
		let n = gtbl n in
		if n = -1 then assert false;
		write1 ch OSetGlobal n
	| SetStack n -> write1 ch OSetStack n
	| SetRef n -> write1 ch OSetRef n
	| SetObj n -> write1 ch OSetObj n
	| SetEnv n -> write1 ch OSetEnv n
	| Incr -> write0 ch OIncr
	| Decr -> write0 ch ODecr
	| Closure n ->
		let n = gtbl n in
		if n = -1 then assert false;
		write1 ch OClosure n
	| NewObj n ->
		let n = (if n = -1 then 0 else
			let n = gtbl n in
			if n = -1 then assert false;
			n + 1)
		in
		write1 ch ONewObj n
	| MakeRef n -> write1 ch OMakeRef n
	| Array n -> write1 ch OArray n
	| MakeOp op ->
		match op with
		| Add -> write0 ch OpAdd
		| Sub -> write0 ch OpSub
		| Mult -> write0 ch OpMult
		| Div -> write0 ch OpDiv
		| Eq -> write0 ch OpEq
		| Neq -> write0 ch OpNeq
		| Shl -> write0 ch OpShl
		| Shr -> write0 ch OpShr
		| Or -> write0 ch OpOr
		| And -> write0 ch OpAnd
		| Xor -> write0 ch OpXor
		| Lt -> write0 ch OpLt
		| Lte -> write0 ch OpLte
		| Gt -> write0 ch OpGt
		| Gte -> write0 ch OpGte
		| Mod -> write0 ch OpMod
		| BAnd
		| BOr
		| Assign
		| AssignRef
		| AssignOp _ -> assert false

let hglob_write ch g n f =
	ch n;
	string_write ch g.g_name;
	ch (g.g_nargs + 1);
	f ch g.g_addr

let global_write ch = function
	| GNull ->
		ch 0;
	| GNamed s ->
		ch 1;
		string_write ch s
	| GFloat s ->
		ch 2;
		string_write ch s
	| GString s ->
		ch 3;
		string_write ch s
	| GFunction g ->
		hglob_write ch g 4 int_write
	| GClosure (g,flag) ->
		let mask = (match flag with ClExported -> 1 | ClInternal -> 0) in
		hglob_write ch g 5 (fun ch i -> int_write ch ((i lsl 1) lor mask))
	| GPrimitive (g,flag,cflag) ->
		hglob_write ch g 6 string_write;
		ch (match cflag,flag with
			| PrimFunction, PrimNotUsed -> 0
			| PrimFunction, PrimUsed -> 1
			| PrimClass, PrimNotUsed -> 2
			| PrimClass, PrimUsed -> 3
			| PrimStatic, PrimNotUsed -> 4
			| PrimStatic, PrimUsed -> 5)
	| GImport g ->
		hglob_write ch g 7 int_write
	| GResource id ->
		ch 8;
		int_write ch id

let write_header ch h =
	ch (int_of_char 'M');
	ch (int_of_char 'T');
	ch (int_of_char 'S');
	ch 0;
	int_write ch h.codesize;
	int_write ch (Array.length h.globals);
	int_write ch (Array.length h.imports);
	int_write ch (Array.length h.oids);
	Array.iter (global_write ch) h.globals;
	Array.iter (string_write ch) h.imports;
	Array.iter (string_write ch) h.oids

let write ch (h,code) gtbl =
	write_header ch h;
	List.iter (opcode_write gtbl ch) code;
	write0 ch OLast

let write_bin ch (h,code) =
	write_header ch h;
	let pos = ref 0 in
	while !pos < h.codesize do
		let op = int_op (Tools.env_get code !pos) in
		if is_long_opcode op then begin
			write1 ch op (Tools.env_get code (!pos+1));
			pos := !pos + 2;
		end else begin
			write0 ch op;
			incr pos;
		end;		
	done;
	write0 ch OLast		

let int_read ch =
	let a = ch() in
	let b = ch() in
	let c = ch() in
	let d = ch() in
	a lor (b lsl 8) lor (c lsl 16) lor (d lsl 24)

let string_read ch =
	let b = Buffer.create 0 in
	let rec loop () =
		match ch() with
		| 0 -> Buffer.contents b
		| c -> Buffer.add_char b (char_of_int c); loop()
	in
	loop()

let hglob_read ch f =
	let name = string_read ch in
	let nargs = ch() - 1 in
	let addr = f ch in
	{
		g_name = name;
		g_nargs = nargs;
		g_addr = addr;
	}

let global_read ch =
	match ch() with
	| 0 -> GNull
	| 1 -> GNamed (string_read ch)
	| 2 -> GFloat (string_read ch)
	| 3 -> GString (string_read ch)
	| 4 -> GFunction (hglob_read ch int_read)
	| 5 ->
		let param = ref ClInternal in
		GClosure (hglob_read ch (fun ch ->
			let n = int_read ch in
			param := (if n land 1 = 1 then ClExported else ClInternal);
			n lsr 1
		), !param)
	| 6 ->
		let h = hglob_read ch string_read in
		let cflag, flag = (match ch() with
			| 0 -> PrimFunction, PrimNotUsed
			| 1 -> PrimFunction, PrimUsed
			| 2 -> PrimClass, PrimNotUsed
			| 3 -> PrimClass, PrimUsed
			| 4 -> PrimStatic, PrimNotUsed
			| 5 -> PrimStatic, PrimUsed
			| _ -> error Invalid_data)
		in
		GPrimitive (h,flag,cflag)
	| 7 -> GImport (hglob_read ch int_read)
	| 8 -> GResource (int_read ch)
	| _ -> error Invalid_data

let read_header ch =
	try
		let _m = ch() in
		let _t = ch() in
		let _s = ch() in
		let _z = ch() in
		if _m <> int_of_char 'M' ||
		   _t <> int_of_char 'T' ||
		   _s <> int_of_char 'S' || 
		   _z <> 0 then error Invalid_header;
		let codesize = int_read ch in
		let nglobs = int_read ch in
		let nincludes = int_read ch in
		let noids = int_read ch in
		if codesize <= 0 || codesize > 1000000 ||
		   nglobs < 0 || nglobs > 100000 ||
		   nincludes < 0 || nincludes > 10000 ||
		   noids < 0 || noids > 10000 then error Invalid_header;
		let globals = Array.init nglobs (fun _ -> global_read ch) in
		let imports = Array.init nincludes (fun _ -> string_read ch) in
		let oids = Array.init noids (fun _ -> string_read ch) in
		{
			globals = globals;
			imports = imports;
			oids = oids;
			codesize = codesize;
		}
	with
		| End_of_file ->
			error Incomplete_file

let read_code code start glob_table size ch =
	let rec loop n =
		if n - start > size then error Invalid_header;
		let opcode = ch() in
		let bits = opcode land 3 in
		let opcode = opcode lsr 2 in
		if opcode < op_int OLast then begin
			Tools.env_set code n opcode;
			let opcode = int_op opcode in
			if bits = 3 && not (is_long_opcode opcode) then
				loop (n+1)
			else begin
				let arg = (if bits <= 1 then bits else if bits = 2 then ch() else int_read ch) in
				let arg = (match opcode with
					| OAccGlobal | OSetGlobal | OClosure -> glob_table arg
					| ONewObj -> if arg = 0 then 0 else (glob_table (arg - 1)) + 1
					| _ -> arg)
				in
				Tools.env_set code (n+1) arg;
				loop(n+2)
			end;
		end else
			if opcode = op_int OLast then n else error Invalid_opcode
	in
	try
		if loop start <> size + start then error Incomplete_file;
	with
		| End_of_file ->
			error Incomplete_file

let read ch =
	let header = read_header ch in
	let code = Tools.new_env 0 header.codesize in
	read_code code 0 (fun n -> n) header.codesize ch;
	header, code
