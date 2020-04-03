open Types
open Printf

type instr = 
	| PushNull
	| Push
	| Pop of int

	| JmpNot of int
	| JmpIf of int
	| Jmp of int
	| Call of int
	| CallObj of int
	| MakeOp of op
	| Incr
	| Decr
	| Ret of int

	| AccNull
	| AccNot
	| AccThis
	| AccInt of int
	| AccStack of int
	| AccGlobal of int
	| AccRef of int
	| AccEnv of int
	| AccEnvRaw of int
	| AccObj of int
	| Swap

	| SetGlobal of int
	| SetStack of int
	| SetRef of int
	| SetObj of int
	| SetEnv of int

	| NewObj of int

	| Closure of int
	| MakeRef of int
	| Array of int

type opcode =
	| OPushNull
	| OPush
	| OPop
	| OJmpNot
	| OJmpIf
	| OJmp
	| OCall
	| OCallObj
	| ORet
	
	| OAccNull
	| OAccNot
	| OAccInt
	| OAccStack
	| OAccGlobal
	| OAccRef
	| OAccObj
	| OAccThis
	| OAccEnv
	| OAccEnvRaw
	| OSwap

	| OSetGlobal
	| OSetStack
	| OSetRef
	| OSetObj
	| OSetEnv
	| OIncr
	| ODecr
	| OClosure
	| ONewObj
	| OMakeRef
	| OArray

	| OpAdd
	| OpSub
	| OpMult
	| OpDiv
	| OpMod
	| OpEq
	| OpNeq
	| OpShl
	| OpShr
	| OpOr
	| OpAnd
	| OpXor
	| OpLt
	| OpLte
	| OpGt
	| OpGte

	| OLast

type 'a hglob_decl = {
	g_name : string;
	g_nargs : int;
	g_addr : 'a;
}

type closure_flag =
	| ClExported
	| ClInternal

type prim_use_flag =
	| PrimNotUsed
	| PrimUsed

type hglob =
	| GNull
	| GNamed of string 
	| GFloat of string
	| GString of string
	| GFunction of int hglob_decl
	| GClosure of int hglob_decl * closure_flag
	| GPrimitive of string hglob_decl * prim_use_flag * prim_flag
	| GImport of int hglob_decl
	| GResource of int

type header = {
	globals : hglob array;
	imports : string array;
	oids : string array;
	codesize : int;
}

external op_int : opcode -> int = "%identity"
external int_op : int -> opcode = "%identity"

let mid_hash s = 
	let acc = ref 0 in
	for i = 0 to String.length s - 1 do
		acc := 223 * !acc + Char.code (String.unsafe_get s i)
	done;
	acc := !acc land ((1 lsl 31) - 1);
	!acc

let find_id oids id =
	let rec loop n =
		if n = -1 then
			string_of_int id
		else if mid_hash oids.(n) = id then
			oids.(n)
		else
			loop (n-1)
	in
	loop (Array.length oids - 1)

let is_long_opcode = function
	| OPushNull | OPush | OAccNull | OAccNot | OpAdd
	| OpSub | OpMult | OpDiv | OpMod | OpEq | OpNeq
	| OpShl | OpShr | OpOr | OpAnd | OpXor | OSwap
	| OpLt | OpLte | OpGt | OpGte | OLast | OIncr | ODecr | OAccThis -> false
	| OPop | OJmpNot | OJmpIf | OJmp | OCall | ORet | OAccInt | OAccEnv | OSetEnv
	| OAccStack | OAccGlobal | OAccRef | OAccObj | OCallObj | OMakeRef | OArray
	| OSetGlobal | OSetStack | OSetRef | OSetObj | OClosure | ONewObj | OAccEnvRaw -> true

let instr_size = function
	| JmpNot _ | JmpIf _ | Jmp _ | Ret _ | Pop _ | Call _ | CallObj _ -> 2
	| AccInt _ | AccStack _ | AccGlobal _ | AccRef _ | AccObj _ | AccEnv _ | AccEnvRaw _ -> 2
	| AccNot | AccNull | PushNull | Push | MakeOp _ | Incr | Decr | AccThis | Swap -> 1
	| SetGlobal _ | SetStack _ | SetRef _ | SetObj _  | SetEnv _ -> 2
	| Closure _ | NewObj _ | MakeRef _ | Array _ -> 2

let s_global oids = function
	| GNull -> "NULL"
	| GNamed s -> s
	| GFloat s -> sprintf "FLOAT %s" s
	| GString s -> sprintf "STRING \"%s\"" (escape s)
	| GFunction g -> sprintf "FUNCTION %s NARGS:%d ADDR:%d" g.g_name g.g_nargs g.g_addr
	| GClosure (g,flag) -> 
		sprintf "CLOSURE %s NARGS:%d ADDR:%d EXPORT:%b" g.g_name g.g_nargs g.g_addr (match flag with ClExported -> true | ClInternal -> false)
	| GPrimitive (g,flag,cflag) -> 
		let pt = match cflag with
			| PrimFunction -> ""
			| PrimClass -> "CLASS " 
			| PrimStatic -> "STATIC "
		in
		sprintf "PRIMITIVE %s%s NARGS:%d ADDR:%s USED:%b" pt g.g_name g.g_nargs g.g_addr (match flag with PrimUsed -> true | PrimNotUsed -> false)
	| GImport g -> sprintf "IMPORT %s NARGS:%d IMPLIB:%d" g.g_name g.g_nargs g.g_addr
	| GResource id ->
		sprintf "RESOURCE %s" (find_id oids id)

let s_opcode op =
	if op_int op < 0 || op > OLast then
		"UNKNOWN"
	else
	match op with
	| OPushNull -> "PUSHNULL"
	| OPush -> "PUSH"
	| OPop -> "POP"
	| OJmpNot -> "JMPNOT"
	| OJmpIf -> "JMPIF"
	| OJmp -> "JMP"
	| OCall -> "CALL"
	| OCallObj -> "CALLOBJ"
	| ORet -> "RET"
	| OAccThis -> "ACCTHIS"
	| OAccNull -> "ACCNULL"
	| OAccNot -> "ACCNOT"
	| OAccInt -> "ACCINT"
	| OAccStack -> "ACCSTACK"
	| OAccGlobal -> "ACCGLOBAL"
	| OAccObj -> "ACCOBJ"
	| OAccRef -> "ACCREF"
	| OAccEnv -> "ACCENV"
	| OAccEnvRaw -> "ACCENVRAW"
	| OSwap -> "SWAP"
	| OSetGlobal -> "SETGLOBAL"
	| OSetStack -> "SETSTACK"
	| OSetRef -> "SETREF"
	| OSetObj -> "SETOBJ"
	| OSetEnv -> "SETENV"
	| OIncr -> "INCR"
	| ODecr -> "DECR"
	| OClosure -> "CLOSURE"
	| ONewObj -> "NEWOBJ"
	| OMakeRef -> "MAKEREF"
	| OArray -> "ARRAY"

	| OpAdd -> "ADD"
	| OpSub -> "SUB"
	| OpMult -> "MULT"
	| OpDiv -> "DIV"
	| OpMod -> "MOD"
	| OpEq -> "EQ"
	| OpNeq -> "NEQ"
	| OpShl -> "SHL"
	| OpShr -> "SHR"
	| OpOr -> "OR"
	| OpAnd -> "AND"
	| OpXor -> "XOR"
	| OpLt -> "LT"
	| OpLte -> "LTE"
	| OpGt -> "GT"
	| OpGte -> "GTE"

	| OLast -> "LAST"

let s_header h b =
	let p = Buffer.add_string b in
	p (sprintf "codesize : %d\n" h.codesize);
	p "imports :\n";
	Array.iter (fun s ->
		p (sprintf "  %s\n" s);
	) h.imports;
	p "globals :\n";
	Array.iteri (fun gid g ->
		p (sprintf "  G#%d : %s\n" gid (s_global h.oids g));
	) h.globals

let s_bytecode (header,code) =
	let b = Buffer.create 0 in
	s_header header b;
	Buffer.add_string b "code :\n";
	let size = header.codesize in
	let rec loop n =
		if n < size then begin
			let op = int_op (Tools.env_get code n) in
			Buffer.add_string b (sprintf "  %.4d    " n);
			Buffer.add_string b (s_opcode op);
			if op = OSetObj || op = OAccObj then begin
				let param = Tools.env_get code (n+1) in
				Buffer.add_string b " ";
				Buffer.add_string b (find_id header.oids param);
				Buffer.add_char b '\n';
				loop (n+2);
			end else if op = ONewObj then begin
				let param = Tools.env_get code (n+1) in
				Buffer.add_string b " ";
				Buffer.add_string b (string_of_int (param - 1));
				Buffer.add_char b '\n';
				loop (n+2);
			end else if is_long_opcode op then begin
				let param = Tools.env_get code (n+1) in
				Buffer.add_string b " ";
				Buffer.add_string b (string_of_int param);
				Buffer.add_char b '\n';
				loop (n+2);
			end else begin
				Buffer.add_char b '\n';
				loop (n+1);
			end;
		end;
	in
	loop 0;
	Buffer.contents b

let log x =
	prerr_endline x;
	flush stderr

