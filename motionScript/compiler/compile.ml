open Types
open Bytecode

type code = {
	mutable ccode : instr list;
	mutable csize : int;
}

type faddr =
	| FunAddr of int
	| ImportAddr of int
	| PrimAddr of (string * prim_flag)

type fdecl = {
	nargs : int;
	mutable addr : faddr;
	mutable used : bool;
}

type fhidden = {
	h_nargs : int;
	h_impid : int;
}

type data_closure = {
	cname : string;
	cnargs : int;
	caddr : int;
	cexport : bool;
}

type data =
	| VString of string
	| VFloat of string
	| VClosure of data_closure

type decl = 
	| Used
	| Bindable
	| FunDecl of fdecl
	| FunCall of int
	| FunHidden of fhidden
	| Resource of int

type global = {
	mutable gid : int;
	mutable gpos : pos;
	mutable gdecl : decl;
}

type env = {
	esp : int;
	elocalsp : int;
	epos : int;
}

type local = {
	sp : int;
	mutable is_ref : bool;
}

type var = 
	| Local of local
	| Env of env
	| Global of global

type interp_env = {
	mutable code : code;
	mutable code_stack : code list;
	mutable funs_code : instr list list;
	mutable total_size : int;
	mutable stack_size : int;
	mutable env_size : int;
	mutable limit : int;
	mutable in_function : bool;
	mutable super : (int * string) option;
	mutable break_limit : int;
	mutable breaks : (instr list * int * pos) list;
	mutable locals : (string,var) Hashtbl.t;
	mutable global_context : (string,global) Hashtbl.t;
	mutable global_count : int;
	mutable imps : string list;
	mutable datas : (int * data) list;
	mutable obj_ids : (string,int) Hashtbl.t;
}

type error_msg =
	| No_global_break
	| No_global_return
	| Invalid_assignment
	| Invalid_import
	| Import_not_found
	| Function_redefinition of string
	| Multiple_functions
	| Class_redefinition of string
	| Unknown_class of string
	| No_superclass
	| Unknown_function of string * int
	| Primitive_not_found of string
	| Duplicate_mid of string * string
	| Reference_on_global

exception Error of error_msg * pos

let error m p = raise (Error (m,p))

let error_msg = function
	| No_global_break -> "break statement outside loop"
	| No_global_return -> "return statement outside loop"
	| Invalid_assignment -> "invalid assignment"
	| Import_not_found -> "import file not found"
	| Invalid_import -> "invalid imported file"
	| Class_redefinition name -> "class redefinition ("^name^")"
	| No_superclass -> "no super class"
	| Unknown_class name -> "unknown class " ^ name
	| Unknown_function (name,n) -> "unknown function ("^name^") with "^ string_of_int n ^ " arguments"
	| Primitive_not_found name -> "primitive not found ("^name^")"
	| Function_redefinition name -> "function redefinition ("^name^")"
	| Multiple_functions -> "this global can represent several functions"
	| Duplicate_mid (name,name2) -> "the class members '"^name^"' and '"^name2^"' are generating the same compilation identifiers, please rename one of the two"
	| Reference_on_global -> "reference on global variables are forbiden"

let jmp_size = instr_size (JmpNot 0)

let csize env = env.code.csize

let empty_env () =
	{
		code = { ccode = []; csize = 0 };
		stack_size = -1;
		break_limit = -1;
		total_size = 0;
		env_size = 0;
		global_count = 0;
		locals = Hashtbl.create 0;
		global_context = Hashtbl.create 0;
		obj_ids = Hashtbl.create 0;
		limit = -1;
		super = None;
		breaks = [];
		in_function = false;
		funs_code = [];
		code_stack = [];
		imps = [];
		datas = [];
	} 

let gen_mid env name p =
	try
		Hashtbl.find env.obj_ids name
	with
		Not_found ->
			let mid = mid_hash name in
			Hashtbl.iter (fun name2 id ->
				if id = mid then error (Duplicate_mid (name,name2)) p;
			) env.obj_ids;
			Hashtbl.add env.obj_ids name mid;
			mid

let array_get env = gen_mid env "at"
let array_set env = gen_mid env "set_at"

let emit env instr =
	env.code.ccode <- instr :: env.code.ccode;
	env.code.csize <- env.code.csize + instr_size instr;
	match instr with
	| Push | PushNull -> env.stack_size <- env.stack_size + 1
	| Pop n | Call n -> env.stack_size <- env.stack_size - n
	| CallObj n -> env.stack_size <- env.stack_size - (n + 1)
	| Array n -> env.stack_size <- env.stack_size - n
	| MakeOp _ | SetObj _ -> env.stack_size <- env.stack_size - 1
	| AccThis | AccNot | AccNull | AccInt _ | AccStack _ | AccGlobal _ 
	| AccRef _ | AccObj _ | Swap | AccEnv _ | SetEnv _ 
	| JmpNot _ | JmpIf _ | Jmp _ | Ret _ | NewObj _ | MakeRef _
	| SetGlobal _ | SetStack _ | SetRef _ | Incr | Decr | AccEnvRaw _
	| Closure _
		-> ()

let emit_tmp env instr =
	emit env instr;
	env.code.ccode

let modify (instr : instr list) (newinstr : instr) =
	Obj.set_field (Obj.repr instr) 0 (Obj.repr newinstr)

let add_break env p =	
	let op = emit_tmp env (Jmp 0) in
	env.breaks <- (op , csize env, p) :: env.breaks

let save_breaks env =
	let old_breaks = env.breaks in
	let old_limit = env.break_limit in
	env.break_limit <- env.stack_size;
	env.breaks <- [];
	old_limit , old_breaks

let process_breaks env (old_limit,old_breaks) =
	List.iter (fun (brk,pos,_) -> 
		modify brk (Jmp (csize env - pos)) 
	) env.breaks;
	env.break_limit <- old_limit;
	env.breaks <- old_breaks

let is_function g =
	match g.gdecl with
	| FunCall _ | Used | Bindable | FunHidden _ | Resource _ -> false
	| FunDecl _ -> true

let alloc_global env name p decl =
	(*/* global not found , alloc a new one */*)
	let id = (match decl with
		| FunDecl f when not f.used -> -1
		| _ -> 
			let n = env.global_count in
			env.global_count <- env.global_count + 1;
			n)
	in
	let g = {
		gid = id;
		gpos = p;
		gdecl = decl;
	} in
	Hashtbl.add env.global_context name g;
	g

let alloc_local env name is_ref =
	Hashtbl.add env.locals name (Local { sp = env.stack_size; is_ref = is_ref })

let alloc_data env data =
	try
		fst (List.find (fun (_,d) -> data = d) env.datas)
	with
		Not_found ->
			let n = env.global_count in
			env.global_count <- env.global_count + 1;
			env.datas <- (n , data) :: env.datas;
			n

let use_global env g f =
	f.used <- true;
	if g.gid = -1 then begin
		let n = env.global_count in
		env.global_count <- env.global_count + 1;
		g.gid <- n;
	end

let resolve_global env name p decl =
	match decl , Hashtbl.find_all env.global_context name with
	| _ , [] ->
		alloc_global env name p decl
	| FunCall n , l ->
		(try
			List.find (fun g ->
				match g.gdecl with
				| Bindable ->
					true
				| Used ->
					g.gdecl <- decl;
					g.gpos <- p;
					true
				| FunCall n2 when n = n2 ->
					true
				| FunDecl f when f.nargs = n || f.nargs == (-1) ->
					use_global env g f;
					true
				| FunHidden _
				| FunCall _
				| FunDecl _
				| Resource _ ->
					false) l
		with
			Not_found ->
				alloc_global env name p decl)
	| FunDecl f , l ->
		(try
			List.find (fun g ->
				match g.gdecl with
				| Bindable ->
					false
				| Used ->
					f.used <- true;
					g.gdecl <- decl;
					true
				| FunCall n when n = f.nargs ->
					f.used <- true;
					g.gdecl <- decl;
					true
				| FunDecl f2 when f2.nargs = f.nargs && name <> "" ->
					if name.[0] = '#' then
						error (Class_redefinition (String.sub name 1 (String.length name - 1))) p
					else
						error (Function_redefinition name) p
				| FunHidden f2 when f2.h_nargs = f.nargs ->
					(match f.addr with
					| ImportAddr impid when f2.h_impid = impid -> true
					| _ -> false)
				| FunHidden _
				| FunCall _
				| FunDecl _
				| Resource _ ->
					false) l
		with
			Not_found ->
				alloc_global env name p decl)
	| FunHidden f , l ->
		(try
			List.find (fun g ->
				match g.gdecl with
				| FunHidden f2 when f2.h_nargs = f.h_nargs && f2.h_impid = f.h_impid -> 
					true
				| FunDecl ({ addr = ImportAddr impid } as f2) when f2.nargs = f.h_nargs && impid = f.h_impid ->
					use_global env g f2;
					true
				| _ ->
					false) l
		with
			Not_found ->
				alloc_global env name p decl)
	| Resource _ , l ->
		(try
			List.find (fun g ->
				match g.gdecl with
				| Resource _ ->
					true
				| _ ->
					false) l
		with
			Not_found ->
				alloc_global env name p decl)
	| Used , [g]	| Bindable , [g] ->
		(match g.gdecl with
		| FunDecl f -> use_global env g f
		| _ -> ());
		g
	| Used , _ :: _ :: _ 
	| Bindable , _ :: _ :: _ ->
		error Multiple_functions p

let resolve env name p decl =
	try
		(*/* if we try to access outside of the function stack limit,
		  the the local need to be stored into a closure environment */*)
		let alloc_env sp =
			let pos = env.env_size in
			let v = Env { esp = sp; elocalsp = env.limit; epos = pos } in
			env.env_size <- pos + 1;
			Hashtbl.add env.locals name v;
			v
		in
		match Hashtbl.find env.locals name with
		| Local l when l.sp < env.limit -> alloc_env l.sp
		| Env e when e.elocalsp < env.limit -> alloc_env e.elocalsp
		| Local _
		| Env _ as v -> v
		| _ -> assert false
	with
		Not_found ->
			Global (resolve_global env name p decl)

let alloc_function env name p nargs =
	let fcode = { ccode = []; csize = 0 } in
	let g = resolve_global env name p (FunDecl {
				nargs = nargs;
				addr = FunAddr (-1);
				used = true }) in
	env.code_stack <- env.code :: env.code_stack;
	env.code <- fcode;
	g

let alloc_primitive env name p nargs addr =
	let f = {
		nargs = nargs;
		addr = PrimAddr addr;
		used = false;
	} in
	let g = resolve_global env name p (FunDecl f) in
	if not f.used then begin
		use_global env g f;
		f.used <- false;
	end

let alloc_import_id env libname =
	let impid = ref (-1) in
	try
		ignore(List.find (fun imp -> incr impid; imp = libname) (List.rev env.imps));
		!impid
	with
		Not_found ->
			incr impid;
			env.imps <- libname :: env.imps;
			!impid

let alloc_import env p libid g =
	let alloc name nargs =	
		ignore(resolve_global env name p (FunDecl 
				{
					nargs = nargs;
					addr = ImportAddr libid;
					used = false;
				}))
	in
	match g with
	| GNamed name -> alloc name (-1)
	| GPrimitive (g,_,PrimStatic) -> alloc g.g_name (-1)
	| GPrimitive (g,_,_) -> alloc g.g_name g.g_nargs
	| GFunction g -> alloc g.g_name g.g_nargs
	| GClosure (g,ClExported) -> alloc g.g_name g.g_nargs
	| GNull
	| GClosure _
	| GImport _
	| GString _
	| GFloat _
	| GResource _ ->
		()

let alloc_import_namespace env p libname g =
	let libid = alloc_import_id env libname in
	let alloc name nargs =	
		resolve_global env name p (FunHidden
				{
					h_nargs = nargs;
					h_impid = libid;
				})
	in
	match g with
	| GNamed name -> alloc name (-1)
	| GPrimitive (g,_,_) -> alloc g.g_name g.g_nargs
	| GFunction g -> alloc g.g_name g.g_nargs
	| GClosure (g,ClExported) -> alloc g.g_name g.g_nargs
	| GNull
	| GClosure _
	| GImport _
	| GString _
	| GFloat _ 
	| GResource _ ->
		assert false

let closure_global env name g export =
	let nargs , addr = 
		match g.gdecl with
		| FunDecl ({ addr = FunAddr addr } as f)  -> f.nargs , addr
		| _ -> assert false
	in
	let rec loop l =
		let fg = Hashtbl.find env.global_context name in
		Hashtbl.remove env.global_context name;
		if fg <> g then	
			loop (fg :: l)
		else begin
			List.iter (Hashtbl.add env.global_context name) l;
			env.datas <- (g.gid,VClosure { cname = name; cnargs = nargs; caddr = addr; cexport = export}) :: env.datas;
		end;
	in
	loop []

let commit_function env g =
	match env.code_stack with
	| [] -> assert false
	| c :: l ->
		let addr = env.total_size in
		(match g.gdecl with
		| FunDecl f -> f.addr <- FunAddr addr
		| _ -> assert false);
		env.total_size <- env.total_size + env.code.csize;
		env.funs_code <- env.code.ccode :: env.funs_code;
		env.code <- c;
		env.code_stack <- l;
		addr

(*/* Before cleaning locals, we're pushing the closure environment 
  on the stack. Since we're resolving again the locals, this will
  eventually create environment in upper function. */*)
let make_env env oldlimit oldenvsize =
	let envs = ref [] in
	let curlimit = env.limit in
	env.env_size <- oldenvsize;
	env.limit <- oldlimit;
	Hashtbl.iter (fun name l ->
		match l with
		| Env e when e.elocalsp = curlimit ->
			Hashtbl.remove env.locals name;
			envs := (e.epos,resolve env name null_pos Used) :: !envs;
		| _ -> ()
	) env.locals;
	List.iter (fun (_,g) ->
		match g with
		(*/* theses cannot be put in an environment since they are globals */*)
		| Global _ -> assert false
		| Local l when l.is_ref -> 
			emit env (AccStack (env.stack_size - l.sp));
			emit env Push;
		| Local l ->
			l.is_ref <- true;
			emit env (MakeRef (env.stack_size - l.sp));
			emit env Push;
		| Env e ->
			emit env (AccEnvRaw e.epos);
			emit env Push;
	) (List.sort (fun (p1,_) (p2,_) -> compare p1 p2) !envs)

(*/* remove locals that have been allocated on the stack */*)
let cleanup_locals env limit =
	Hashtbl.iter (fun name l ->
		match l with
		| Local l when l.sp > limit ->
			Hashtbl.remove env.locals name;
		| _ -> ()
	) env.locals

let gen_accobj env name p =
	match name.[0] with
	| '_' ->
		emit env Push;
		emit env (AccObj (gen_mid env ("get"^name) p));
		emit env (CallObj 0);
	| '$' ->
		emit env Push;
		emit env (AccInt (gen_mid env name p));
		emit env Swap;
		emit env Push;
		emit env (AccObj (gen_mid env "get" p));
		emit env (CallObj 1)
	| _ ->
		emit env (AccObj (gen_mid env name p))

let gen_setobj env name p =
	match name.[0] with
	| '_' ->
		emit env Push;
		emit env (AccObj (gen_mid env ("set"^name) p));
		emit env (CallObj 1);
	| '$' ->
		emit env Push;
		emit env (AccInt (gen_mid env name p));
		emit env Swap;
		emit env Push;
		emit env (AccObj (gen_mid env "set" p));
		emit env (CallObj 2);
	| _ ->
		emit env (SetObj (gen_mid env name p))

let gen_callobj nargs env name p =
	match name.[0] with
	| '_' ->
		emit env Push;
		emit env (AccObj (gen_mid env ("get"^name) p));
		emit env (CallObj 0);
		emit env (Call nargs);
	| '$' ->
		emit env Push;
		emit env (AccInt (gen_mid env name p));
		emit env Swap;
		emit env Push;
		emit env (AccObj (gen_mid env "get" p));
		emit env (CallObj 1);
		emit env (Call nargs);
	| _ ->
		emit env Push;
		emit env (AccObj (gen_mid env name p));
		emit env (CallObj nargs)

let compile_acc env name decl p =
	match resolve env name p decl with
	| Global g -> emit env (AccGlobal g.gid)
	| Env e -> emit env (AccEnv e.epos)
	| Local l when l.is_ref -> emit env (AccRef (env.stack_size - l.sp))
	| Local l -> emit env (AccStack (env.stack_size - l.sp))	

let compile_const env p = function
	| Null -> emit env  AccNull
	| Int n -> emit env (AccInt n)
	| Float f ->
		let n = alloc_data env (VFloat f) in
		emit env (AccGlobal n)
	| String s ->
		let n = alloc_data env (VString s) in
		emit env (AccGlobal n)
	| Ident "this" ->
		emit env AccThis
	| Ident name ->
		compile_acc env name Used p

let rec compile_ident env flast (id,p) =
	match id with 
	| Id name ->
		flast env name p
	| IDot (name,next) ->
		gen_accobj env name p;
		compile_ident env flast next

let compile_op env = function
	| Assign
	| AssignOp _ | BAnd | BOr -> assert false
	| op ->
		emit env (MakeOp op)

let rec compile_function env name params e p =
	let nargs = List.length params in
	let g = alloc_function env name p nargs in
	let oldlimit = env.limit in
	let mylimit = env.stack_size in
	let oldenvsize = env.env_size in
	env.env_size <- 0;
	env.stack_size <- env.stack_size + 1;
	env.limit <- env.stack_size;
	List.iter (fun (p,flag) ->
		env.stack_size <- env.stack_size + 1;
		alloc_local env p (match flag with ArgNormal -> false | ArgReference -> true)
	) params;
	let in_function = env.in_function in
	env.in_function <- true;
	compile_expr env e;
	env.in_function <- in_function;
	(*/* if constructor, return this */*)
	if (try ignore(String.index name '#'); true with Not_found -> false) then emit env AccThis;
	emit env (Ret (env.stack_size - env.limit));
	let addr = commit_function env g in
	env.stack_size <- env.limit - 1;
	let myenv = env.env_size in
	make_env env oldlimit oldenvsize;
	cleanup_locals env mylimit;
	(*/* if an environment is needed (has been created by make_env
		then we need to create the corresponding closure on the stack
		and rebind the function name so it refers to the closure and
		not to the global.
		The global is erased but we keep its name and address in a
		closure data block.
	*/*)
	if myenv > 0 then begin
		emit env (AccInt myenv);
		emit env (Closure g.gid);
		env.stack_size <- env.stack_size - myenv;
		closure_global env name g (not in_function);
		if in_function then begin
			if name <> "" then begin
				alloc_local env name false;
				emit env Push;
			end;
			Local { sp = env.stack_size; is_ref = false }
		end else begin
			emit env (SetGlobal g.gid);
			Global g
		end;
	end else
		Global g

and compile_val env (v,p) =
	match v with	
	| EConst c -> compile_const env p c
	| EParenthesis v -> compile_val env v
	| ECall ((EConst (Ident "super"),ep),vl) ->
		(match env.super with
		| None -> error No_superclass ep
		| Some (sid,sname) ->
			let nargs = List.length vl in
			List.iter (fun v -> compile_val env v; emit env Push) vl;
			emit env (AccGlobal sid);
			emit env (AccObj (gen_mid env (sname^"#"^(string_of_int nargs)) p));
			emit env (Call nargs))
	| ECall ((EConst (Ident name),ep),vl) ->
		let nargs = List.length vl in
		List.iter (fun v -> compile_val env v; emit env Push) vl;
		compile_acc env name (FunCall nargs) p;
		emit env (Call nargs)
	| ECall ((EDot ((EConst (Ident "super"),sp),id),ep),vl) ->
		(match fst id with
		| Id _ -> ()
		| IDot _ -> Parser.error Parser.Invalid_expression ep);
		(match env.super with
		| None -> error No_superclass ep
		| Some (sid,_) ->
			let nargs = List.length vl in
			List.iter (fun v -> compile_val env v; emit env Push) vl;
			emit env (AccGlobal sid);
			compile_ident env gen_accobj id;
			emit env (Call nargs));
	| ECall ((EDot (v,id),ep),vl) ->
		let nargs = List.length vl in
		List.iter (fun v -> compile_val env v; emit env Push) vl;
		compile_val env v;
		compile_ident env (gen_callobj nargs) id;
	| ECall (v,vl) -> 
		List.iter (fun v -> compile_val env v; emit env Push) vl;
		compile_val env v;
		emit env (Call (List.length vl));
	| EOp (AssignRef,v,v2) ->
		compile_val env v;
		emit env Push;
		compile_val env v2;
		emit env (SetRef 0);
		emit env (Pop 1);
	| EOp (Assign,(EConst (Ident name),ep),v) ->
		if name = "this" || name = "super" then error Invalid_assignment p;
		compile_val env v;
		(match resolve env name ep Bindable with
		| Global g when is_function g -> error Invalid_assignment p
		| Global g -> emit env (SetGlobal g.gid)
		| Env e -> emit env (SetEnv e.epos)
		| Local l when l.is_ref -> emit env (SetRef (env.stack_size - l.sp))
		| Local l -> emit env (SetStack (env.stack_size - l.sp)))
	| EOp (Assign,(EDot (v,id),_),nv) ->
		compile_val env nv;
		emit env Push;
		compile_val env v;
		compile_ident env gen_setobj id;
	| EOp (Assign,(EAccess (a,b),ep),v) ->
		compile_val env b;
		emit env Push;
		compile_val env v;
		emit env Push;
		compile_val env a;
		emit env Push;
		emit env (AccObj (array_set env ep));
		emit env (CallObj 2)
	| EOp (AssignOp op,(EConst (Ident name),ep),v) ->
		if name = "this" || name = "super" then error Invalid_assignment p;
		(match resolve env name ep Bindable with
		| Global g when is_function g -> error Invalid_assignment p
		| Global g ->
			emit env (AccGlobal g.gid);
			emit env Push;
			compile_val env v;
			compile_op env op;
			emit env (SetGlobal g.gid)
		| Env e ->
			emit env (AccEnv e.epos);
			emit env Push;
			compile_val env v;
			compile_op env op;
			emit env (SetEnv e.epos);
		| Local l ->
			emit env (if l.is_ref then AccRef (env.stack_size - l.sp) else AccStack (env.stack_size - l.sp));
			emit env Push;
			compile_val env v;
			compile_op env op;
			emit env (if l.is_ref then SetRef (env.stack_size - l.sp) else SetStack (env.stack_size - l.sp)))
	| EOp (AssignOp op,(EDot (v,id),_),nv) ->
		let saved = ref ("",null_pos) in
		compile_val env v;
		compile_ident env (fun env name p -> saved := (name,p); emit env Push; gen_accobj env name p) id;
		emit env Push;
		compile_val env nv;
		compile_op env op;
		emit env Swap;
		gen_setobj env (fst !saved) (snd !saved);
	| EOp (AssignOp op,(EAccess (a,b),ep),v) ->
		compile_val env b;
		emit env Push;
		emit env PushNull; (*/* empty space */*)
		emit env Push;
		compile_val env a;
		emit env (SetStack 1); (*/* fill the empty space */*)
		emit env Push;
		emit env (AccObj (array_get env ep));
		emit env (CallObj 1);
		emit env Push;
		compile_val env v;
		compile_op env op;
		emit env Swap;
		emit env Push;
		emit env (AccObj (array_set env ep));
		emit env (CallObj 2);
	| EUnop ((EConst (Ident name),ep),(UIncr as uop),way)
	| EUnop ((EConst (Ident name),ep),(UDecr as uop),way) ->
		if name = "this" || name = "super" then error Invalid_assignment p;
		let a, b = (match resolve env name ep Bindable with
			| Global g when is_function g -> error Invalid_assignment p
			| Global g ->
				AccGlobal g.gid , SetGlobal g.gid
			| Env e -> 
				AccEnv e.epos, SetEnv e.epos
			| Local l ->
				let sp = env.stack_size - l.sp in 
				if l.is_ref then AccRef sp , SetRef sp else AccStack sp, SetStack sp)
		in
		emit env a;
		emit env (if uop = UIncr then Incr else Decr);
		emit env b;
		if way = Postfix then emit env (if uop = UDecr then Incr else Decr);
	| EUnop ((EDot (v,id),_),(UIncr as uop),way)
	| EUnop ((EDot (v,id),_),(UDecr as uop),way) ->
		compile_val env v;
		emit env Push;
		compile_ident env gen_accobj id;
		emit env (if uop = UIncr then Incr else Decr);
		emit env Swap;
		compile_ident env gen_setobj id;
		if way = Postfix then emit env (if uop = UDecr then Incr else Decr);
	| EUnop ((EAccess (a,b),ep),(UIncr as uop),way)
	| EUnop ((EAccess (a,b),ep),(UDecr as uop),way) ->
		compile_val env b;
		emit env Push;
		emit env PushNull; (*/* empty space */*)
		emit env Push;
		compile_val env a;
		emit env (SetStack 1); (*/* fill the empty space */*)
		emit env Push;
		emit env (AccObj (array_get env ep));
		emit env (CallObj 1);
		emit env (if uop = UIncr then Incr else Decr);
		emit env Swap;
		emit env Push;
		emit env (AccObj (array_set env ep));
		emit env (CallObj 2);
		if way = Postfix then emit env (if uop = UDecr then Incr else Decr);
	| EUnop (_,UIncr,_)
	| EUnop (_,UDecr,_)
	| EOp (Assign,_,_)
	| EOp (AssignOp _,_,_) ->
		error Invalid_assignment p
	| EOp (BAnd,a,b) ->
		compile_val env a;
		let jmp = emit_tmp env (JmpNot 0) in
		let pos = csize env in
		compile_val env b;
		modify jmp (JmpNot (csize env - pos))
	| EOp (BOr,a,b) ->
		compile_val env a;
		let jmp = emit_tmp env (JmpIf 0) in
		let pos = csize env in
		compile_val env b;
		modify jmp (JmpIf (csize env - pos))
	| EOp (op,a,b) ->
		compile_val env a;
		emit env Push;
		compile_val env b;
		compile_op env op;		
	| EUnop (v,Minus,_) ->
		emit env (AccInt 0);
		emit env Push;
		compile_val env v;
		compile_op env Sub;
	| EUnop (v,Not,_) ->
		compile_val env v;
		emit env AccNot
	| ENew (mname,cname,args) ->
		let gid = env.global_count in
		let nargs = List.length args in
		let gid = if cname = "Object" then
			-1
		else match mname with
			| Some libname ->
				let g = alloc_import_namespace env p libname (GNamed ("#"^cname)) in
				g.gid
			| None ->
				match resolve env ("#"^cname) p (FunCall 0) with
				| Local _ | Env _ -> assert false
				| Global g ->					
					g.gid
		in
		if gid <> -1 then
			List.iter (fun v ->
				compile_val env v;
				emit env Push;
			) args;
		emit env (NewObj gid);
		if gid <> -1 then begin
			emit env Push;
			emit env (AccObj (gen_mid env (cname^"#"^(string_of_int nargs)) p));
			emit env (CallObj nargs)
		end
	| EArray el ->
		List.iter (fun v ->			
			compile_val env v;
			emit env Push;
		) (List.rev el);
		emit env (Array (List.length el));
	| EObject fl ->
		emit env (NewObj (-1));
		emit env Push;
		List.iter (fun (fname,fv) ->
			emit env Push;
			compile_val env fv;
			emit env Swap;
			emit env (SetObj (gen_mid env fname (pos fv)));
			emit env (AccStack 0);
		) fl;
		emit env (Pop 1)
	| EAccess (a,b) ->
		compile_val env b;
		emit env Push;
		compile_val env a;
		emit env Push;
		emit env (AccObj (array_get env p));
		emit env (CallObj 1)
	| EDot (v,id) ->
		compile_val env v;
		compile_ident env gen_accobj id
	| EImported (mname,fname,None) ->
		let g = alloc_import_namespace env p mname (GNamed fname) in
		emit env (AccGlobal g.gid);
	| EImported (mname,fname,Some args) ->
		let nargs = List.length args in
		let g = alloc_import_namespace env p mname (GFunction { g_name = fname; g_nargs = nargs; g_addr = -1 }) in
		List.iter (fun v -> compile_val env v; emit env Push) args;
		emit env (AccGlobal g.gid);
		emit env (Call nargs)
	| ERes name ->
		(match resolve env name p (Resource (gen_mid env name p)) with
		| Global g -> emit env (AccGlobal g.gid)
		| _ -> assert false)
	| ELambda (params,e) ->
		(match compile_function env "" params e p with
		| Local l -> ()
		| Global g -> emit env (AccGlobal g.gid)
		| _ -> assert false)
	| EQuestion (e,e1,e2) ->
		compile_val env e;
		let jmp = emit_tmp env (JmpNot 0) in
		let pos = csize env in
		compile_val env e1;
		modify jmp (JmpNot (csize env - pos + jmp_size));
		let jmp = emit_tmp env (Jmp 0) in
		let pos = csize env in
		compile_val env e2;
		modify jmp (Jmp (csize env - pos))
	| ERef name ->
		match resolve env name p Used with
		| Global g ->
			(*/* we could allow a direct pointer since the global is
			  always available but we need to allocate a block, so this
			  will cause problems because we already generated no-ref
			  global usage code. */*)
			error Reference_on_global p
		| Local l when l.is_ref ->
			emit env (AccStack (env.stack_size - l.sp));
		| Local l ->
			l.is_ref <- true;
			emit env (MakeRef (env.stack_size - l.sp));
		| Env e ->
			emit env (AccEnvRaw e.epos)

and compile_expr env (e,p) =
 	match e with
	| ENext (a,b) ->
		compile_expr env a;
		compile_expr env b
	| EVal v ->
		compile_val env (v,p)
	| EIfThenElse (v,e,e2) ->
		compile_val env v;
		let jmp = emit_tmp env (JmpNot 0) in
		let pos = csize env in
		compile_expr env e;
		(match e2 with
		| None ->
			modify jmp (JmpNot (csize env - pos));	
		| Some e ->
			modify jmp (JmpNot (csize env - pos + jmp_size));
			let jmp = emit_tmp env (Jmp 0) in
			let pos = csize env in
			compile_expr env e;
			modify jmp (Jmp (csize env - pos)))
	| EWhile (v,e,Normal) ->
		let begin_pos = csize env in
		let old_limit = save_breaks env in
		compile_val env v;
		let jmp = emit_tmp env (JmpNot 0) in
		let pos = csize env in
		compile_expr env e;
		emit env (Jmp (begin_pos - csize env - jmp_size));
		modify jmp (JmpNot (csize env - pos));
		process_breaks env old_limit
	| EWhile (v,e,DoWhile) ->
		let begin_pos = csize env in
		let old_limit = save_breaks env in
		compile_expr env e;
		compile_val env v;
		emit env (JmpIf (begin_pos - csize env - jmp_size));
		process_breaks env old_limit
	| EFor (inits,tests,bcls,e) ->
		let old_limit = save_breaks env in
		List.iter (compile_val env) inits;
		let begin_pos = csize env in
		let jmps = List.map (fun t ->
			compile_val env t;
			let jmp = emit_tmp env (JmpNot 0) in
			jmp , csize env
		) tests in
		compile_expr env e;
		List.iter (compile_val env) bcls;
		emit env (Jmp (begin_pos - csize env - jmp_size));
		List.iter (fun (jmp,pos) -> modify jmp (JmpNot (csize env - pos)) ) jmps;
		process_breaks env old_limit
	| ESwitch (v,cases,def) ->
		compile_val env v;
		emit env Push;
		let old_limit = save_breaks env in
		let exprs = List.map (fun (v,e) ->
			emit env (AccStack 0);
			emit env Push;
			compile_val env v;
			emit env (MakeOp Eq);
			let jmp = emit_tmp env (JmpIf 0) in
			let pos = csize env in
			(jmp , pos , e)
		) cases in
		let jmp_default = emit_tmp env (Jmp 0) in
		let def_pos = csize env in
		List.iter (fun (j,p,e) ->
			modify j (JmpIf (csize env - p));
			compile_expr env (EBlock [e],pos e);
		) exprs;
		modify jmp_default (Jmp (csize env - def_pos));
		(match def with
		| None -> ()
		| Some e -> compile_expr env e);
		process_breaks env old_limit;
		emit env (Pop 1)
	| EReturn v ->
		(match v with None -> () | Some v -> compile_val env v);
		if not env.in_function then error No_global_return p;
		emit env (Ret (env.stack_size - env.limit))
	| EBreak ->
		if env.break_limit <> env.stack_size then begin
			let instr = Pop (env.stack_size - env.break_limit) in
			env.code.ccode <- instr :: env.code.ccode;
			env.code.csize <- env.code.csize + instr_size instr;
		end;
		add_break env p
	| EBlock el ->
		let oldlimit = env.limit in
		let mylimit = env.stack_size in
		List.iter (compile_expr env) el;
		if env.stack_size <> mylimit then
			(*/* this will restore also stack_size */*)
			emit env (Pop (env.stack_size - mylimit));
		cleanup_locals env mylimit;
	| EFunction (name,params,e) ->
		ignore(compile_function env name params e p)
	| EVars vars ->
		List.iter (fun (name,v) ->
			(match v with
			| None -> emit env PushNull;
			| Some v ->
				compile_val env v;
				emit env Push);
			alloc_local env name false
		) vars
	| EPrimitive (name,nargs,addr,flag) ->
		alloc_primitive env (if flag = PrimClass then "#"^name else name) p nargs (addr,flag)
	| EImport (libname,None) ->
		(*/* full include : we need to load the mto file */*)
		let file = String.uncapitalize libname ^ ".mto" in
		(try
			let ch , _ = !Tools.loader file in 
			let header = Binary.read_header (fun () -> input_byte ch) in
			let impid = alloc_import_id env libname in
			Array.iter (fun g -> ignore(alloc_import env p impid g)) header.globals;
			close_in ch;
		with
			| Tools.File_not_found _ -> error Import_not_found p
			| Binary.Error _ -> error Invalid_import p)
	| EImport (libname,Some (fname,nargs)) ->
		(*/* partial include : only a forward definition so we
		  can recurse within several modules */*)
		let impid = alloc_import_id env libname in
		ignore(alloc_import env p impid (match nargs with 
			| -1 -> GNamed fname
			| n -> GFunction { g_name = fname; g_nargs = nargs; g_addr = -1 }))
	| EClass (cname,csuper,mlist) ->
		let gid = env.global_count in
		let superid , csuper = (match csuper with
			| None -> -1 , None
			| Some (supername,p) ->
				let g = resolve_global env ("#"^supername) p (FunCall 0) in
				if g.gid == gid then error (Unknown_class supername) p;
				g.gid , Some (g.gid,supername))
		in
		let g = resolve_global env ("#"^cname) p (FunDecl { nargs = 0; addr = FunAddr (-2); used = true }) in
		let class_gid = g.gid in
		emit env (NewObj superid);
		emit env (SetGlobal class_gid);
		emit env (AccGlobal class_gid);
		let methods = Hashtbl.create 0 in
		let gen_methods (e,p) =
			match e with
			| EFunction (name,args,code) ->
				let nargs = List.length args in
				let cname = (if name = cname then name^"#"^(string_of_int nargs) else name) in
				if Hashtbl.mem methods cname then error (Function_redefinition name) p;
				Hashtbl.add methods cname ();
				compile_expr env (EFunction (cname,args,code),p);
				(match resolve env cname p (FunCall nargs) with
				| Local l when l.sp = env.stack_size - 1 ->
					(*/* the function created a closure : this will
						store the closure in the object and pop the stack.
						since we are 'in-function' the function will not get
						exported */*)
					emit env (SetObj (gen_mid env cname p));
					Hashtbl.remove env.locals cname;
				| Global g ->
					emit env (AccGlobal g.gid);
					emit env Push;
					emit env (AccGlobal class_gid);
					emit env (SetObj (gen_mid env cname p));
					closure_global env cname g false;
				| Local _ 
				| Env _ ->
					assert false);
			| _ ->
				assert false
		in
		let in_function = env.in_function in
		let old_super = env.super in
		env.in_function <- true;
		env.super <- csuper;
		List.iter gen_methods mlist;
		env.in_function <- in_function;
		env.super <- old_super

let compile expr =
	let env = empty_env() in
	env.total_size <- jmp_size;
	if (snd expr).pfile <> "std.mts" then compile_expr env (EImport ("Std",None),{ pfile = "<std.mts>"; pmin = 0; pmax = 0 });
	compile_expr env expr;
	List.iter (fun (_,_,p) -> error No_global_break p) env.breaks;
	let globals = Array.create env.global_count (GNamed "") in
	let gindexes = Array.create env.global_count (-1) in
	let gindex = ref (-1) in
	Hashtbl.iter (fun name g ->
		match g.gdecl with
		| Used
		| Bindable ->
			incr gindex;
			gindexes.(g.gid) <- !gindex;
			globals.(!gindex) <- GNamed name;
		| Resource id ->
			incr gindex;
			gindexes.(g.gid) <- !gindex;
			globals.(!gindex) <- GResource id;
		| FunCall n ->
			error (Unknown_function (name,n)) g.gpos
		| FunHidden f ->
			incr gindex;
			gindexes.(g.gid) <- !gindex;
			globals.(!gindex) <- GImport { g_name = name; g_nargs = f.h_nargs; g_addr = f.h_impid }
		| FunDecl f when not f.used && g.gid = -1 ->
			()
		| FunDecl f ->
			incr gindex;
			gindexes.(g.gid) <- !gindex;
			globals.(!gindex) <-
				match f.addr with
				| FunAddr (-2) -> GNamed name
				| FunAddr (-1) -> assert false
				| FunAddr a -> GFunction { g_name = name; g_nargs = f.nargs; g_addr = a; }
				| PrimAddr (a,cflag) -> GPrimitive ({ g_name = name; g_nargs = f.nargs; g_addr = a; },(if f.used then PrimUsed else PrimNotUsed),cflag)
				| ImportAddr a -> GImport { g_name = name; g_nargs = f.nargs; g_addr = a; }
	) env.global_context;
	List.iter (fun (gid,data) ->
		incr gindex;
		gindexes.(gid) <- !gindex;
		globals.(!gindex) <-
			match data with
			| VString s -> GString s
			| VFloat s -> GFloat s
			| VClosure c -> GClosure ({ g_name = c.cname; g_nargs = c.cnargs; g_addr = c.caddr },if c.cexport then ClExported else ClInternal)
	) (List.rev env.datas);
	let oids = Hashtbl.fold (fun name _ acc -> name :: acc) env.obj_ids [] in
	let code = List.rev (List.concat (env.code.ccode :: env.funs_code)) in
	let code = (Jmp (env.total_size - jmp_size)) :: code in
	({
		globals = Array.sub globals 0 (!gindex + 1);
		imports = Array.of_list (List.rev env.imps);
		oids = Array.of_list oids;
		codesize = env.total_size + env.code.csize;
	} , code) , (fun n -> gindexes.(n))
