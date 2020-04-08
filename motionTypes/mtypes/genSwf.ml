open Type
open Typetools
open Swf
open SwfZip
open ExtHashtbl
open ExtString

type kind = 
	| VarReg of int
	| VarStr
	| VarObj
	| VarVolatile

type local_ctx = {
	reg : int;
	sp : int;
}

type context =  {
	t : Typer.context;
	idents : (string,int) Hashtbl.t;
	ops : action DynArray.t;
	locals : (string,local_ctx) Hashtbl.t;
	superfuns : (string,Type.class_context * string * bool ref) Hashtbl.t;
	mutable current : Type.class_context;
	mutable stack : int;
	mutable code_pos : int;
	mutable ident_count : int;
	mutable reg_count : int;
	mutable stack_size : int;
	mutable cur_block : texpr;
	mutable breaks : (unit -> unit) list;
	mutable continue_pos : int;
	mutable opt_push : bool;
	mutable curmethod : string;
}

type push_style =
	| VStr of string
	| VInt of int
	| VInt32 of int32
	| VFloat of float
	| VReg of int
	| VThis
	| VNull
	| VSuper

let stack_delta = function
	| APush l -> List.length l
	| ASetReg _ -> 0
	| AAdd | ADivide | ASubtract | AMultiply | AMod -> -1
	| AAnd | AOr | AXor | AShl | AShr | AAsr -> -1
	| ACompare | AGreater -> -1
	| AEval | ANot | AJump _ | AToInt | AToString | ATry _ -> 0
	| ACondJump _ -> -1
	| AEqual | APhysEqual -> -1
	| ANew -> -1 (** only if 0 params **)
	| AObject | AInitArray -> 0 (** calculated outside **)
	| ASet -> -2
	| APop -> -1
	| AFunction2 _ -> 1	
	| ADup -> 1
	| AObjGet -> -1
	| AObjSet -> -3
	| ALocalVar -> -1
	| ALocalAssign | AExtends -> -2
	| AReturn -> -1
	| AIncrement | ADecrement -> 0
	| AObjCall | ACall | ANewMethod -> assert false
	| ACast | AInstanceOf | AThrow -> -1
	| op -> failwith ("Unknown stack delta for " ^ (ActionScript.action_string (fun _ -> "") 0 op))

let write ctx op =
	let write b op =
		DynArray.add ctx.ops op;
		ctx.code_pos <- ctx.code_pos + 1;
		ctx.stack_size <- ctx.stack_size + stack_delta op;
		ctx.opt_push <- b
	in
	match op with
	| APush l when ctx.opt_push ->
		(match DynArray.last ctx.ops with
		| (APush l2) as a ->
			ctx.code_pos <- ctx.code_pos - 1;
			ctx.stack_size <- ctx.stack_size - stack_delta a;
			DynArray.delete_last ctx.ops;
			write true (APush (l2 @ l))
		| _ ->
			assert false)
	| APush _ ->
		write true op
	| _ ->
		write false op

let call ctx kind n =
	let op , n = (match kind with
		| VarReg r ->
			write ctx (APush [PReg r;PUndefined]);
			AObjCall , n + 2
		| VarStr -> 
			ACall , n + 1
		| VarObj ->
			AObjCall , n + 2
		| VarVolatile ->
			assert false
	) in
	DynArray.add ctx.ops op;
	ctx.opt_push <- false;
	ctx.code_pos <- ctx.code_pos + 1;
	ctx.stack_size <- ctx.stack_size - n

let new_call ctx kind n  =
	let op , n = (match kind with
		| VarReg n ->
			write ctx (APush [PReg n;PUndefined]);
			ANewMethod , n + 2
		| VarStr -> 
			ANew , n + 1
		| VarObj ->
			ANewMethod , n + 2
		| VarVolatile ->
			assert false
	) in
	DynArray.add ctx.ops op;
	ctx.opt_push <- false;
	ctx.code_pos <- ctx.code_pos + 1;
	ctx.stack_size <- ctx.stack_size - n

let push ctx items =
	write ctx (APush (List.map (fun i ->
		match i with
		| VStr str ->
			let n = (try
				Hashtbl.find ctx.idents str
			with Not_found ->
				let n = ctx.ident_count in
				ctx.ident_count <- n + 1;
				Hashtbl.add ctx.idents str n;
				n
			) in
			if n <= 0xFF then 
				PStack n
			else
				PStack2 n
		| VInt n ->
			PInt (Int32.of_int n)
		| VInt32 n ->
			PInt n
		| VFloat f ->
			PDouble f
		| VThis ->
			PReg 1
		| VNull ->
			PNull
		| VSuper ->
			PReg 2
		| VReg n ->
			PReg n
	) items))

let rec pop ctx n =
	if n > 0 then begin
		write ctx APop;
		pop ctx (n-1);
	end

let cjmp ctx =
	write ctx (ACondJump 0);
	let start_pos = ctx.code_pos in
	let op_pos = DynArray.length ctx.ops - 1 in
	(fun() ->
		let delta = ctx.code_pos - start_pos in
		DynArray.set ctx.ops op_pos (ACondJump delta);
		ctx.opt_push <- false
	)

let jmp ctx =
	write ctx (AJump 0);
	let start_pos = ctx.code_pos in
	let op_pos = DynArray.length ctx.ops - 1 in
	(fun() ->
		let delta = ctx.code_pos - start_pos in
		DynArray.set ctx.ops op_pos (AJump delta);
		ctx.opt_push <- false
	)

let rec is_volatile ctx t v =
	let rec check_class c =
		try 
			let f = List.find (fun f -> f.cf_name = v) c.c_class_fields in
			f.cf_volatile
		with
			Not_found ->
				match c.c_super with
				| None -> false
				| Some (c,_) -> check_class c
	in
	let check_name name =
		try	
			let c = Typer.getclass ctx.t name in
			check_class c
		with
			Not_found ->
				false
	in
	let check c =
		match c.c_name with
		| None -> false
		| Some name -> check_name name
	in
	match t.Type.t with
	| TLink t -> is_volatile ctx t v
	| TObject o ->
		(match o.o_sup with
		| None -> false
		| Some c -> check c) || check o.o_inf
	| TAbbrev (path,_,_) ->
		check_name path
	| _ ->
		false

let error p =
	raise (Typer.Error (Typer.Custom "Malformed expression",p))

let do_jmp ctx pos =
	write ctx (AJump (pos - (ctx.code_pos + 1)))

let func ctx args constructor =
	let default_flags = [ThisRegister;ArgumentsNoVar] in
	let f = {
		f2_name = "";
		f2_args = args;
		f2_codelen = 0;
		f2_nregs = 0;
		f2_flags = (if constructor then SuperRegister :: default_flags else SuperNoVar :: default_flags);
	} in
	write ctx (AFunction2 f);
	let start_pos = ctx.code_pos in
	(fun nregs ->
		let delta = ctx.code_pos - start_pos in
		f.f2_codelen <- delta;
		f.f2_nregs <- nregs
	)

let setvar ?(retval=false) ctx = function
	| VarReg n -> write ctx (ASetReg n); if not retval then write ctx APop
	| VarStr -> write ctx ASet
	| VarObj -> write ctx AObjSet
	| VarVolatile ->
		push ctx [VInt 1];
		write ctx AInitArray;
		ctx.stack_size <- ctx.stack_size - 1;
		write ctx AObjSet

let getvar ctx = function
	| VarReg n -> push ctx [VReg n]
	| VarStr -> write ctx AEval
	| VarObj -> write ctx AObjGet
	| VarVolatile ->
		write ctx AObjGet;
		push ctx [VInt 0];
		write ctx AObjGet

let clean_stack ctx stack =
	Hashtbl.iter (fun name r ->
		if r.sp > stack then Hashtbl.remove ctx.locals name
	) ctx.locals;
	ctx.stack <- stack

let open_block ctx e =
	let old_block = ctx.cur_block in
	let old_stack = ctx.stack in
	let start_size = ctx.stack_size in
	ctx.stack <- ctx.stack + 1;
	ctx.cur_block <- e;
	(fun() ->
		clean_stack ctx old_stack;
		pop ctx (ctx.stack_size - start_size);
		ctx.cur_block <- old_block
	)

exception Found

let rec used_in_block ctx curblock vname e =
	let loop e =
		match e.texpr with
		| TBuiltin SuperConstructor when curblock && vname = "super" -> raise Found
		| TMember (_,c) when curblock && vname = "super" && c != ctx.current -> raise Found
		| TLocal v when curblock && v = vname -> raise Found
		| TLambda (_,e,_) ->
			if used_in_block ctx true vname e then raise Found;
			raise Exit
		| _ -> ()
	in
	try
		expr_iter loop e;
		false
	with
		Found -> true

let generate_package ctx = function
	| [] -> VarStr
	| p :: l ->		
		push ctx [VStr p];
		write ctx AEval;
		List.iter (fun p -> push ctx [VStr p]; write ctx AObjGet) l;
		VarObj

let rec generate_package_register ctx = function
	| [] -> ()
	| p :: [] ->
		ignore(generate_package ctx (p :: []));
		write ctx ANot;
		write ctx ANot;
		let j = cjmp ctx in
		push ctx [VStr p; VInt 0; VStr "Object"];
		write ctx ANew;
		write ctx ASet;
		j()
	| p :: l ->
		let lrev = List.rev l in
		let all_but_last , last = List.rev (List.tl lrev), List.hd lrev in
		generate_package_register ctx (p :: all_but_last);
		ignore(generate_package ctx (p :: l));
		write ctx ANot;
		write ctx ANot;
		let j = cjmp ctx in
		push ctx [VStr p];
		write ctx AEval;
		List.iter (fun p -> push ctx [VStr p]; write ctx AObjGet) all_but_last;
		push ctx [VStr last; VInt 0; VStr "Object"];
		write ctx ANew;
		write ctx AObjSet;
		j()

let unescape_chars s = 
	let s = String.concat "\n" (String.nsplit s "\\n") in
	let s = String.concat "\r" (String.nsplit s "\\r") in
	let s = String.concat "\t" (String.nsplit s "\\t") in
	let s = String.concat "\"" (String.nsplit s "\\\"") in
	let s = String.concat "'" (String.nsplit s "\\'") in
	let s = String.concat "\\" (String.nsplit s "\\\\") in
	s

let generate_constant ctx p = function
	| TConstInt str -> push ctx [VInt32 (try Int32.of_string str with _ -> error p)]
	| TConstFloat str -> push ctx [VFloat (try float_of_string str with _ -> error p)]
	| TConstString s -> push ctx [VStr (unescape_chars s)]
	| TConstBool b -> write ctx (APush [PBool b])
	| TConstThis -> push ctx [VThis]
	| TConstNull -> push ctx [VNull]

let generate_breaks ctx olds =
	List.iter (fun f -> f()) ctx.breaks;
	ctx.breaks <- olds

let rec generate_access ?(forcall=false) ctx e =
	match e.texpr with
	| TBuiltin SuperConstructor ->
		if not forcall then error (epos e);
		push ctx [VSuper];
		write ctx (APush [PUndefined]);
		VarObj
	| TLocal s ->
		(try
			let l = Hashtbl.find ctx.locals s in
			if l.reg = 0 then begin
				push ctx [VStr s];
				VarStr
			end else
				VarReg l.reg
		with Not_found -> assert false)
	| TMember (s,c) ->
		push ctx [(if c == ctx.current then VThis else VSuper);VStr s];
		if is_volatile ctx (mk_obj c.c_inst None) s then
			VarVolatile
		else
			VarObj
	| TField (v,s) ->
		generate_expr ctx v;
		push ctx [VStr s];
		if is_volatile ctx v.ttype s then
			VarVolatile
		else
			VarObj
	| TClass c ->
		(match c.c_path with
		| [] , "Xml" ->
			push ctx [VStr "XML"];
			VarStr
		| [] , "XmlNode" ->
			push ctx [VStr "XMLNode"];
			VarStr
		| [] , "XmlSocket" ->
			push ctx [VStr "XMLSocket"];
			VarStr
		| p , s ->
			let k = generate_package ctx p in
			push ctx [VStr s];
			k)
	| TArray (va,vb) ->
		generate_expr ctx va;
		generate_expr ctx vb;
		VarObj
	| _ ->
		if not forcall then error (epos e);
		generate_expr ctx e;
		write ctx (APush [PUndefined]);
		VarObj

and generate_binop retval ctx op e1 e2 =
	let gen a =
		generate_expr ctx e1;
		generate_expr ctx e2;
		write ctx a
	in
	match op with
	| OpAssign ->
		let k = generate_access ctx e1 in
		generate_expr ctx e2;
		setvar ~retval ctx k
	| OpAssignOp op ->
		let k = generate_access ctx e1 in
		generate_binop true ctx op e1 e2;
		setvar ~retval ctx k
	| OpAdd -> gen AAdd
	| OpMult -> gen AMultiply
	| OpDiv -> gen ADivide
	| OpSub -> gen ASubtract
	| OpEq -> gen AEqual
	| OpNotEq -> 
		gen AEqual;
		write ctx ANot
	| OpGt -> gen AGreater
	| OpGte ->
		gen ACompare;
		write ctx ANot
	| OpLt -> gen ACompare
	| OpLte ->
		gen AGreater;
		write ctx ANot
	| OpAnd -> gen AAnd
	| OpOr -> gen AOr
	| OpXor -> gen AXor
	| OpBoolAnd ->
		generate_expr ctx e1;
		write ctx ADup;
		write ctx ANot;
		let jump_end = cjmp ctx in
		write ctx APop;
		generate_expr ctx e2;
		jump_end()
	| OpBoolOr ->
		generate_expr ctx e1;
		write ctx ADup;
		let jump_end = cjmp ctx in
		write ctx APop;
		generate_expr ctx e2;
		jump_end()
	| OpShl -> gen AShl
	| OpShr -> gen AShr
	| OpUShr -> gen AAsr
	| OpMod -> gen AMod

and generate_local_var ctx (vname,_,vinit) =
	if used_in_block ctx false vname ctx.cur_block then begin
		push ctx [VStr vname];
		Hashtbl.add ctx.locals vname { reg = 0; sp = ctx.stack };
		match vinit with
		| None -> write ctx ALocalVar
		| Some v ->
			generate_expr ctx v;
			write ctx ALocalAssign
	end else begin
		ctx.reg_count <- ctx.reg_count + 1;
		let r = ctx.reg_count in
		Hashtbl.add ctx.locals vname { reg = r; sp = ctx.stack };
		match vinit with
		| None -> 
			()
		| Some v ->
			generate_expr ctx v;
			setvar ctx (VarReg r)
	end

and generate_builtin ctx = function
	| SuperConstructor ->
		assert false
	| ToInt e ->
		generate_expr ctx e;
		write ctx AToInt
	| ToString e ->
		generate_expr ctx e;
		write ctx AToString
	| Callback (e,name,args) ->
		let nargs = 2 + List.length args in
		List.iter (generate_expr ctx) (List.rev args);
		push ctx [VStr name];
		generate_expr ctx e;
		push ctx [VInt nargs];
		push ctx [VStr "Std"];
		write ctx AEval;
		push ctx [VStr "callback"];
		call ctx VarObj nargs
	| Downcast e 
	| Upcast e
	| Cast e ->
		generate_expr ctx e
	| PrintType e ->
		()
	| Trace (arg,file,line) ->
		(match !Plugin.trace with
		| None -> ()
		| Some f ->
			let rec loop f =
				try
					let p , f = String.split f "." in
					let p2 , f = loop f in
					p :: p2 , f
				with
					Invalid_string ->
						[] , f
			in
			let p , f = loop f in
			let pos = epos arg in
			let e = mk_e (TClass { empty_class_context with c_path = (p,f) }) t_void pos in
			generate_expr ctx (mk_e (TCall(e,[
					arg;
					mk_e (TConst (TConstString (s_type_path ctx.current.c_path))) t_void pos;
					mk_e (TConst (TConstString file)) t_void pos;
					mk_e (TConst (TConstInt (string_of_int line))) t_void pos
				])) t_void pos)
		)

and generate_expr ?(retval=true) ctx e =
	match e.texpr with
	| TVars vl ->
		List.iter (generate_local_var ctx) vl
	| TBlock el ->
		let block_end = open_block ctx e in
		List.iter (generate_expr ctx) el;
		block_end()
	| TFor (inits,conds,incrs,e) ->
		let block_end = open_block ctx e in
		List.iter (generate_expr ctx) inits;
		let test = jmp ctx in
		let start_pos = ctx.code_pos in
		let old_continue = ctx.continue_pos in
		let old_breaks = ctx.breaks in
		ctx.breaks <- [];
		ctx.continue_pos <- start_pos;
		ctx.opt_push <- false;
		List.iter (generate_expr ~retval:false ctx) incrs;
		test();
		let jumps = ref [] in
		List.iter (fun cond -> 
			generate_expr ctx cond;
			write ctx ANot;
			jumps := cjmp ctx :: !jumps;
		) conds;
		generate_expr ctx e;
		do_jmp ctx start_pos;
		List.iter (fun j -> j()) !jumps;
		generate_breaks ctx old_breaks;
		ctx.continue_pos <- old_continue;
		block_end()
	| TIf (v,e,eelse) ->
		generate_expr ctx v;
		write ctx ANot;
		let jump_else = cjmp ctx in
		generate_expr ctx e;
		(match eelse with
		| None -> jump_else()
		| Some e ->
			let jump_end = jmp ctx in
			jump_else();
			let old = ctx.stack_size in
			(* does not count else stack delta, since it's supposed to be
			   same as previous expression : only = 1 for a?b:c *)
			generate_expr ctx e;
			ctx.stack_size <- old;
			jump_end())
	| TDiscard v ->
		let s = ctx.stack_size in
		generate_expr ~retval:false ctx v;
		pop ctx (ctx.stack_size - s)
	| TWhile (v,e,flag) ->
		let jump_begin = (match flag with NormalWhile -> (fun()->()) | DoWhile -> jmp ctx) in
		let start_pos = ctx.code_pos in
		let old_continue = ctx.continue_pos in
		let old_breaks = ctx.breaks in
		ctx.breaks <- [];
		ctx.opt_push <- false;
		ctx.continue_pos <- start_pos;
		generate_expr ctx v;
		write ctx ANot;
		let jump_end = cjmp ctx in
		jump_begin();
		generate_expr ctx e;
		do_jmp ctx start_pos;
		generate_breaks ctx old_breaks;
		ctx.continue_pos <- old_continue;
		jump_end()
	| TBreak ->
		ctx.breaks <- jmp ctx :: ctx.breaks
	| TContinue ->
		do_jmp ctx ctx.continue_pos
	| TReturn None ->
		write ctx (APush [PUndefined]);
		write ctx AReturn	
	| TReturn (Some v) ->
		generate_expr ctx v;
		write ctx AReturn
	| TSwitch (v,cases,edefault) ->
		generate_expr ctx v;
		write ctx (ASetReg 0);
		let old_breaks = ctx.breaks in
		let first_case = ref true in
		ctx.breaks <- [];
		let cases = List.map (fun (v,e) ->
			if !first_case then
				first_case := false
			else
				push ctx [VReg 0];
			generate_expr ctx v;
			write ctx APhysEqual;
			cjmp ctx , e
		) cases in
		let jump_default = jmp ctx in
		List.iter (fun (j,e) ->
			j();
			generate_expr ctx e
		) cases;
		jump_default();
		(match edefault with
		| None -> ()
		| Some e ->
			generate_expr ctx e);
		generate_breaks ctx old_breaks
	| TTry (e,name,path,e2) ->
		let tdata = {
			tr_style = TryRegister 0;
			tr_trylen = 0;
			tr_catchlen = None;
			tr_finallylen = None;
		} in
		write ctx (ATry tdata);
		let p = ctx.code_pos in
		generate_expr ctx e;
		let jump_end = jmp ctx in
		tdata.tr_trylen <- ctx.code_pos - p;
		let p = ctx.code_pos in
		Hashtbl.add ctx.locals name { reg = 0; sp = ctx.stack };
		write ctx APop;
		(match path with
		| None -> ()
		| Some (p,s) ->
			(*// if !(e instanceof class) throw e *)
			push ctx [VReg 0];
			let k = generate_package ctx p in
			push ctx [VStr s];
			getvar ctx k;
			write ctx ACast;
			push ctx [VNull];
			write ctx AEqual;
			let jump_ok = cjmp ctx in
			push ctx [VReg 0];
			write ctx AThrow;
			jump_ok();
		);
		push ctx [VStr name;VReg 0];
		write ctx ALocalAssign;
		generate_expr ctx e2;
		Hashtbl.remove ctx.locals name;
		tdata.tr_catchlen <- Some (ctx.code_pos - p);
		jump_end()
	| TParenthesis v ->
		generate_expr ctx v
	| TObjDecl fields ->
		let nfields = List.length fields in
		List.iter (fun (s,v) ->
			push ctx [VStr s];
			generate_expr ctx v
		) fields;
		push ctx [VInt nfields];
		write ctx AObject;
		ctx.stack_size <- ctx.stack_size - (nfields * 2);
	| TArrayDecl vl ->
		let nfields = List.length vl in
		List.iter (generate_expr ctx) (List.rev vl);
		push ctx [VInt nfields];
		write ctx AInitArray;
		ctx.stack_size <- ctx.stack_size - nfields;
	| TUnop (Not,_,v) -> 
		generate_expr ctx v;
		write ctx ANot
	| TUnop (Neg,_,({ texpr = TConst (TConstInt s) } as e)) ->
		push ctx [VInt32 (Int32.neg (try Int32.of_string s with _ -> error e.tpos))]
	| TUnop (Neg,_,({ texpr = TConst (TConstFloat f) } as e)) ->
		push ctx [VFloat (0. -. (try float_of_string f with _ -> error e.tpos))]
	| TUnop (Neg,_,v) ->
		push ctx [VInt 0];
		generate_expr ctx v;
		write ctx ASubtract
	| TUnop (NegBits,_,v) ->
		generate_expr ctx v;
		push ctx [VInt (-1)]; 
		write ctx AXor
	| TUnop (op,flag,v) ->
		if retval && flag = Postfix then begin
			let k = generate_access ctx v in
			getvar ctx k
		end;
		let k = generate_access ctx v in
		ignore(generate_access ctx v);
		getvar ctx k;
		write ctx (match op with Increment -> AIncrement | Decrement -> ADecrement | _ -> assert false);
		if retval && flag = Prefix then write ctx (ASetReg 0);
		setvar ctx k;
		if retval && flag = Prefix then push ctx [VReg 0]
	| TClass _
	| TMember _
	| TLocal _
	| TField _
	| TArray _ ->
		let k = generate_access ctx e in
		getvar ctx k
	| TConst c ->
		generate_constant ctx (epos e) c
	| TCall ( { texpr = TMember(f,c) },eargs) when c != ctx.current ->
		let nargs = List.length eargs + 1 in
		List.iter (generate_expr ctx) (List.rev eargs);
		push ctx [VThis];
		let fname = (match fst c.c_path with [] -> snd c.c_path | l -> String.concat "_" l ^ "_" ^ snd c.c_path) ^ "__" ^ f in
		push ctx [VInt nargs;VStr fname];
		write ctx AEval;
		push ctx [VStr "call"];
		call ctx VarObj nargs;
		if not (Hashtbl.mem ctx.superfuns fname) then Hashtbl.add ctx.superfuns fname (c,f,ref false)
	| TCall	(e,eargs) ->
		let nargs = List.length eargs in
		List.iter (generate_expr ctx) (List.rev eargs);
		push ctx [VInt nargs];
		let k = generate_access ~forcall:true ctx e in
		call ctx k nargs
	| TNew (c,_,eargs) ->
		let nargs = List.length eargs in
		List.iter (generate_expr ctx) (List.rev eargs);
		push ctx [VInt nargs];
		let k = generate_access ~forcall:true ctx (mk_e (TClass c) t_void null_pos) in
		new_call ctx k nargs
	| TBinop (op,e1,e2) ->
		generate_binop retval ctx op e1 e2
	| TBuiltin t ->
		generate_builtin ctx t
	| TLambda (args,e,_) ->
		generate_function ctx "" args e

and generate_function ?(constructor=false) ctx fname fargs e =
	let old_name = ctx.curmethod in
	let stack_base , old_nregs = ctx.stack , ctx.reg_count in
	let reg_super = used_in_block ctx true "super" e in
	ctx.reg_count <- (if reg_super then 2 else 1);
	if fname <> "" then ctx.curmethod <- fname;
	ctx.stack <- ctx.stack + 1;
	let args = List.map (fun (aname,_) ->
		let r = 
			(if used_in_block ctx false aname e then
				0
			else begin
				ctx.reg_count <- ctx.reg_count + 1;
				ctx.reg_count
			end)
		in
		Hashtbl.add ctx.locals aname { reg = r; sp = ctx.stack };
		r , aname
	) fargs in		
	let fdone = func ctx args reg_super in
	generate_expr ctx e;
	fdone (ctx.reg_count + 1);
	clean_stack ctx stack_base;
	ctx.reg_count <- old_nregs;
	ctx.curmethod <- old_name

let generate_class_code ctx c =
	let cpath , cname = c.c_path in
	generate_package_register ctx cpath;
	let k = generate_package ctx cpath in
	push ctx [VStr cname];
	(match Typetools.constructor c with
	| None -> 
		let fdone = func ctx [] true in
		fdone 3;
	| Some (args,e,_) ->
		generate_function ~constructor:true ctx "new" args e);	
	write ctx (ASetReg 0);
	setvar ctx k;
	(match c.c_super with
	| None -> ()
	| Some (csuper,_) ->
		push ctx [VReg 0];
		getvar ctx (generate_access ctx (mk_e (TClass csuper) t_void null_pos));
		write ctx AExtends);
	if List.exists (fun cf -> cf.cf_expr <> None && not cf.cf_static) c.c_class_fields then begin
		push ctx [VReg 0; VStr "prototype"];
		getvar ctx VarObj;
		write ctx (ASetReg 1);
		write ctx APop;
	end;
	List.iter (fun cf ->
		match cf.cf_expr with
		| Some { texpr = TLambda(args,e,_) } when cf.cf_name <> "new" ->
			push ctx [VReg (if cf.cf_static then 0 else 1)];
			push ctx [VStr cf.cf_name];
			generate_function ctx cf.cf_name args e;
			setvar ctx VarObj;
		| _ -> ()
	) c.c_class_fields;
	Hashtbl.iter (fun fname (c,f,b) ->
		if not !b then begin
			b := true;
			push ctx [VStr fname];
			getvar ctx (generate_access ctx (mk_e (TClass c) t_void null_pos));
			push ctx [VStr "prototype"];
			write ctx AObjGet;
			push ctx [VStr f];
			write ctx AObjGet;
			write ctx ASet
		end;
	) ctx.superfuns;
	List.iter (fun cf ->
		if cf.cf_static then match cf.cf_expr with
		| Some { texpr = TLambda _ } ->	()
		| Some e ->
			push ctx [VReg 0];
			push ctx [VStr cf.cf_name];
			generate_expr ctx e;
			setvar ctx VarObj;
		| None -> ()
	) c.c_class_fields

let to_utf8 str =
	try
		UTF8.validate str;
		str;
	with
		UTF8.Malformed_code -> 
			let b = UTF8.Buf.create 0 in
			String.iter (fun c -> UTF8.Buf.add_char b (UChar.of_char c)) str;
			UTF8.Buf.contents b

let generate file compress t =
	let file , linkage =
		(try
			let f,l = String.split file "@" in
			f , Some l
		with
			Invalid_string ->
				file , None)
	in
	let ctx = {
		t = t;
		idents = Hashtbl.create 0;
		superfuns = Hashtbl.create 0;
		ops = DynArray.create();
		current = empty_class_context;
		code_pos = 1;
		ident_count = 0;
		stack = 0;
		reg_count = 0;
		locals = Hashtbl.create 0;
		stack_size = 0;
		cur_block = mk_e TBreak t_void null_pos;
		breaks = [];
		continue_pos = 0;
		opt_push = false;
		curmethod = "";
	} in
	DynArray.add ctx.ops (AStringPool []);
	Typer.generate t (fun c ->
		ctx.current <- c;
		if not (is_native c) then generate_class_code ctx c;
	);
	let idents = Hashtbl.fold (fun ident pos acc -> (ident,pos) :: acc) ctx.idents [] in
	let idents = List.sort (fun (_,p1) (_,p2) -> compare p1 p2) idents in
	DynArray.set ctx.ops 0 (AStringPool (List.map (fun (id,_) -> to_utf8 id) idents));	
	let ch = IO.output_channel (open_out_bin file) in
	let header = {
		h_version = 7;
		h_size = {
			rect_nbits = 15;
			left = 0;
			right = 14000;
			top = 0;
			bottom = 9600;
		};
		h_fps = 10240;
		h_frame_count = 1;
		h_compressed = compress;
	} in
	let tag d = {
		tid = 0;
		textended = false;
		tdata = d;
	} in
	let data = List.map tag 
		(TSetBgColor { cr = 0xFF; cg = 0xFF; cb = 0xFF } ::
		(match linkage with
		| None -> 
			[TDoAction ctx.ops ; TShowFrame]
		| Some link ->
			[
				TClip {
					c_id = 1;
					c_frame_count = 1;
					c_tags = [
						tag (TDoAction ctx.ops);
						tag TShowFrame
					]
				};
				TExport [{ exp_id = 1; exp_name = link }];
				TShowFrame
			]))
	in
	Swf.write ch (header,data);
	IO.close_out ch

;;
let output = ref None in
let zip = ref false in
Plugin.add
	[
		("-swf",Arg.String (fun f -> output := Some f),"<file> : generate SWF into target file");
		("-swf-zip",Arg.Unit (fun () -> zip := true),": compress generated SWF");
	]
	(fun t -> 
		match !output with
		| None -> ()
		| Some file -> generate file !zip t
	)