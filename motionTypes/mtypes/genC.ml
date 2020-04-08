open Expr

type context = {
	mutable cur_frame : int;
	locals : (string,int) Hashtbl.t;
	mutable classes : Class.context list;
	mutable current : Class.context;
	mutable ch : unit IO.output;
	allocs : (string list,int) Hashtbl.t;
	arrays : (int,unit) Hashtbl.t;
	lambdas : (pos,int * string list) Hashtbl.t;
	mutable alloc_count : int;
	mutable lambda_count : int;
	mutable level : int;
	mutable wrote : bool;
}

let write ctx s =
	if not ctx.wrote then begin
		ctx.wrote <- true;
		IO.nwrite ctx.ch (String.make ctx.level '\t');
	end;
	IO.nwrite ctx.ch s

let next ?(sep=true) ctx =
	if ctx.wrote then begin
		ctx.wrote <- false;
		if sep then IO.write ctx.ch ';';
		IO.write ctx.ch '\n';		
	end

let open_block ctx =
	ctx.level <- ctx.level + 1;
	next ~sep:false ctx

let close_block ctx =
	ctx.level <- ctx.level - 1;
	next ~sep:false ctx

let add_local ctx n =
	Hashtbl.add ctx.locals n ctx.cur_frame

let is_local ctx s =
	Hashtbl.mem ctx.locals s

let new_stack_frame ctx =
	let s = ctx.cur_frame in
	ctx.cur_frame <- ctx.cur_frame + 1;
	s

let clean_stack_frame ctx frame =
	Hashtbl.iter (fun name f ->
		if f > frame then Hashtbl.remove ctx.locals name;
	) ctx.locals;
	ctx.cur_frame <- frame

let class_name (p,name) =
	String.concat "_" p ^ (if p = [] then name else "_" ^ name)

let static_name p n =
	let cname = class_name p in
	if n = "new" then
		"new_" ^ cname
	else
		cname ^ "_" ^ n

let get_alloc ctx fl =
	Hashtbl.find ctx.allocs (List.map fst fl)

let add_alloc ctx fl =
	let n = ctx.alloc_count in
	ctx.alloc_count <- n + 1;
	Hashtbl.add ctx.allocs (List.map fst fl) n;
	n

let rec capture_vars (e,p) =
	match e with
	| EVars (_,vl) -> List.map (fun (n,_,_) -> n) vl
	| EFor (el,_,_,_) -> List.fold_left (fun acc e -> acc @ capture_vars e) [] el
	| _ -> []

let rec generate_list sep f ctx = function
	| [] -> ()
	| [x] -> f x
	| x :: l ->
		f x;
		write ctx sep;
		generate_list sep f ctx l

let generate_method_head ctx clctx f =
	IO.printf ctx.ch "value %s(" (static_name (Class.path clctx) f.fname);
	let args = (if f.fstatic = IsMember then ("this",None) :: f.fargs else f.fargs) in
	generate_list ", " (fun (aname,_) -> IO.printf ctx.ch "value %s" aname) ctx args;
	IO.nwrite ctx.ch ")"

let rec generate_constant ctx = function
	| Int s -> write ctx ("Int("^s^")");
	| Float s -> write ctx ("Float("^s^")");
	| String s -> write ctx ("String(\"" ^ s ^ "\")")
	| Name _ -> assert false
	| Ident "this" -> write ctx "this"
	| Ident "true" -> write ctx "Val_true"
	| Ident "false" -> write ctx "Val_false"
	| Ident "null" -> write ctx "Val_null"
	| Ident s ->
		if is_local ctx s then 
			write ctx s
		else
			try
				if Class.getvar ctx.current s = IsMember then raise Not_found;
				write ctx (static_name (Class.path ctx.current) s)
			with
				Not_found ->
					generate_val ctx (EField ((EConst (Ident "this"),null_pos),s),null_pos)

and generate_val ?(parenthesis=true) ctx (v,p) =
	match v with
	| EConst (Name s) ->
		generate_val ctx (EStatic ([],s),p)
	| EConst c ->
		generate_constant ctx c
	| EArray (v1,v2) ->
		write ctx "A(";
		generate_val ctx v1;
		write ctx ",";
		generate_val ctx v2;
		write ctx ")"
	| EBinop (op,v1,v2) ->
		generate_binop ctx op v1 v2
	| EField (v,s) ->
		write ctx "Get(";
		generate_val ctx v;
		write ctx (",id_" ^ s);
		write ctx ")"
	| EStatic p ->
		write ctx (class_name (Class.resolve ctx.current p))
	| ECall ((EConst (Ident "upcast"),_),[v])
	| ECall ((EConst (Ident "downcast"),_),[v])
	| EParenthesis v when parenthesis ->
		write ctx "(";
		generate_val ctx v;
		write ctx ")";
	| EParenthesis v ->
		generate_val ctx v
	| EObjDecl fl ->
		let id = (try get_alloc ctx fl with Not_found -> assert false) in
		write ctx (Printf.sprintf "Obj_%d(" id);		
		generate_list ", " (fun (_,v) -> generate_val ctx v) ctx fl;
		write ctx ")";
	| ECall ((EField (v,s),_),vl) ->
		write ctx (Printf.sprintf "OC%d(" (List.length vl));
		generate_val ctx v;
		write ctx (", id_" ^ s);
		if vl <> [] then write ctx ", ";
		generate_list ", " (generate_val ctx) ctx vl;
		write ctx ")";
	| ECall (((EConst (Ident s),p2) as v),vl) ->
		(try
			match Class.getvar ctx.current s with
			| IsMember -> generate_val ctx (ECall ((EField ((EConst (Ident "this"),p2),s),p2),vl),p)
			| IsStatic -> generate_val ctx (ECall ((EField ((EStatic (Class.path ctx.current),p2),s),p2),vl),p)
		with
			Not_found ->
				generate_val ctx (ECall ((EParenthesis v,p2),vl),p))
	| ECall (v,vl) ->
		write ctx (Printf.sprintf "C%d(" (List.length vl));
		generate_list ", " (generate_val ctx) ctx (v :: vl);
		write ctx ")";
	| EQuestion (v1,v2,v3) ->
		generate_val ctx v1;
		write ctx "?";
		generate_val ctx v2;
		write ctx ":";
		generate_val ctx v3;
	| EUnop (Not,_,v) ->
		write ctx "!";
		generate_val ctx v;
	| EUnop (Neg,_,v) ->
		write ctx "OpNeg(";
		generate_val ctx v;
		write ctx ")";
	| EUnop (NegBits,_,v) ->
		write ctx "OpNegBits(";
		generate_val ctx v;
		write ctx ")";
	| EUnop (Increment,Prefix,v) ->
		write ctx "IncrPre(";
		generate_val ctx v;
		write ctx ")";
	| EUnop (Increment,Postfix,v) ->
		write ctx "Incr(";
		generate_val ctx v;
		write ctx ")";
	| EUnop (Decrement,Prefix,v) ->
		write ctx "DecrPre(";
		generate_val ctx v;
		write ctx ")";
	| EUnop (Decrement,Postfix,v) ->
		write ctx "Decr(";
		generate_val ctx v;
		write ctx ")";
	| ENew (p,_,vl) ->
		write ctx (static_name p "new");
		write ctx "(";
		generate_list ", " (generate_val ctx) ctx vl;
		write ctx ")";
	| EArrayDecl vl ->
		write ctx (Printf.sprintf "Arr_%d(" (List.length vl));
		generate_list ", " (generate_val ctx) ctx vl;
		write ctx ")";
	| ELambda f ->
		()

and generate_binop ctx op v1 v2 =
	let gen f =
		write ctx f;
		write ctx "(";
		generate_val ctx v1;
		write ctx ",";
		generate_val ctx v2;
		write ctx ")"
	in
	let cmp f =
		write ctx "Cmp(";
		generate_val ctx v1;
		write ctx ",";
		generate_val ctx v2;
		write ctx ") ";
		write ctx f;
		write ctx " 0"
	in
	match op with
	| OpAdd -> gen "Add"		
	| OpMult -> gen "Mult"
	| OpDiv -> gen "Div"
	| OpSub -> gen "Sub"
	| OpEq -> cmp "=="
	| OpNotEq -> cmp "!="
	| OpGt -> cmp ">"
	| OpGte -> cmp ">="
	| OpLt -> cmp "<"
	| OpLte -> cmp "<="
	| OpAnd -> gen "OpAnd"
	| OpOr -> gen "OpOr"
	| OpXor -> gen "OpXor"
	| OpBoolAnd ->
		generate_val ctx v1;
		write ctx " && ";
		generate_val ctx v2;
	| OpBoolOr ->
		generate_val ctx v1;
		write ctx " || ";
		generate_val ctx v2;
	| OpShl -> gen "OpShl"
	| OpShr -> gen "OpShr"
	| OpUShr -> gen "OpUShr"
	| OpMod -> gen "OpMod"
	| OpAssign ->
		()
	| OpAssignOp op ->
		()

let rec generate_expr ctx (e,p) =
	match e with
	| EVars (_,vl) ->
		List.iter (fun (n,_,v) ->			
			match v with
			| None -> ()
			| Some v ->
				write ctx n;
				write ctx " = ";
				generate_val ctx v;
				next ctx
		) vl
	| EBlock el ->
		let sf = new_stack_frame ctx in
		write ctx "{";
		open_block ctx;
		let vars = List.fold_left (fun acc e -> acc @ capture_vars e) [] el in
		if vars <> [] then begin
			List.iter (add_local ctx) vars;
			write ctx ("value " ^ String.concat ", " vars);
			next ctx;
		end;
		List.iter (fun e ->
			generate_expr ctx e;
			next ctx;
		) el;
		close_block ctx;
		clean_stack_frame ctx sf;
		write ctx "}"
	| EFor (el,conds,incrs,e) ->
		write ctx "for(";
		generate_list "," (generate_expr ctx) ctx el;
		write ctx ";";
		generate_list "," (generate_val ctx) ctx conds;
		write ctx ";";
		generate_list "," (generate_val ctx) ctx incrs;
		write ctx ")";
		gen_block ctx e;
	| EIf (v,e,eo) ->
		write ctx "if( ";
		generate_val ~parenthesis:false ctx v;
		write ctx " )";
		gen_block ctx e;
		(match eo with
		| None -> () 
		| Some e ->
			write ctx "else";
			gen_block ctx e)
	| EWhile (v,e,DoWhile) ->
		write ctx "do";
		gen_block ctx e;
		write ctx "while( ";
		generate_val ~parenthesis:false ctx v;
		write ctx " )";
	| EWhile (v,e,NormalWhile) ->
		write ctx "while( ";
		generate_val ~parenthesis:false ctx v;
		write ctx " )";
		gen_block ctx e;
	| ESwitch (v,cases,def) ->
		()(*assert false*)
	| EReturn None ->
		write ctx "return";
	| EReturn (Some v) ->
		write ctx "return ";
		generate_val ctx v;
	| EBreak ->
		write ctx "break"
	| EContinue ->
		write ctx "continue"
	| EVal v ->
		generate_val ctx v
	| EFunction _
	| EImport _
	| EClass _ 
	| EInterface _ ->
		assert false

and	gen_block ctx e =
	match fst e with
	| EBlock _ ->
		write ctx " ";
		generate_expr ctx e
	| _ ->
		open_block ctx;
		generate_expr ctx e;
		next ctx;
		close_block ctx

let rec generate_lambda_val ctx (v,p) =
	match v with
	| EConst _ 
	| EStatic _ ->
		()
	| EArray (v1,v2)
	| EBinop (_,v1,v2) ->
		generate_lambda_val ctx v1;
		generate_lambda_val ctx v2;
	| EField (v,_)
	| EUnop (_,_,v)
	| EParenthesis v ->
		generate_lambda_val ctx v;
	| ECall (v,vl) ->
		List.iter (generate_lambda_val ctx) (v :: vl);
	| ENew (_,_,vl) ->
		List.iter (generate_lambda_val ctx) vl;
	| EQuestion (v,v1,v2) ->
		generate_lambda_val ctx v;
		generate_lambda_val ctx v1;
		generate_lambda_val ctx v2;
	| ELambda f ->
		let e = (match f.fexpr with None -> assert false | Some e -> e) in
		generate_lambda_expr ctx e;
		let id = ctx.lambda_count in
		ctx.lambda_count <- ctx.lambda_count + 1;
		let sf = new_stack_frame ctx in
		List.iter (fun (n,_) -> add_local ctx n) f.fargs;
		IO.nwrite ctx.ch "static ";
		generate_method_head ctx ctx.current { f with fargs = ("this",None) :: f.fargs ; fname = "Lambda" ^ string_of_int id };
		IO.write ctx.ch ' ';
		generate_expr ctx e;
		List.iter (fun (n,_) -> Hashtbl.remove ctx.locals n) f.fargs;
		clean_stack_frame ctx sf;
		IO.nwrite ctx.ch "\n\n";
	| EArrayDecl vl ->
		List.iter (generate_lambda_val ctx) vl;
		let n = List.length vl in
		if not (Hashtbl.mem ctx.arrays n) then begin
			Hashtbl.add ctx.arrays n ();
			IO.printf ctx.ch "static value Arr_%d(" n;
			for i = 0 to n - 1 do
				IO.printf ctx.ch "p%d" i;
				if i <> n - 1 then IO.nwrite ctx.ch ", ";
			done;
			IO.nwrite ctx.ch ") {\n";
			IO.printf ctx.ch "\tvalue a = Alloc_array(%d);\n" n;
			for i = 0 to n - 1 do
				IO.printf ctx.ch "\tASet(a,%d,p%d);\n" i i;
			done;
			IO.printf ctx.ch "\treturn a;\n}\n\n";
		end;
	| EObjDecl fl ->
		List.iter (fun (_,v) -> generate_lambda_val ctx v) fl;
		try
			ignore(get_alloc ctx fl)
		with
			Not_found ->
				let id = add_alloc ctx fl in
				IO.printf ctx.ch "static value Obj_%d(" id;
				generate_list ", " (fun (s,v) -> write ctx "value "; write ctx s) ctx fl;
				write ctx ") {";
				open_block ctx;
				write ctx "value O = Alloc_object()";
				next ctx;
				List.iter (fun (s,_) ->
					write ctx (Printf.sprintf "Set(O,id_%s,%s)" s s);
					next ctx;
				) fl;
				write ctx "return O;";
				close_block ctx;
				IO.nwrite ctx.ch "}\n\n"

and generate_lambda_expr ctx (e,p) =
	match e with
	| EVars (_,vl) ->
		List.iter (fun (_,_,vo) -> match vo with None -> ()  | Some v -> generate_lambda_val ctx v) vl
	| EBlock el ->
		List.iter (generate_lambda_expr ctx) el
	| EFor (inits,conds,incrs,e) ->
		List.iter (generate_lambda_expr ctx) inits;
		List.iter (generate_lambda_val ctx) conds;
		List.iter (generate_lambda_val ctx) incrs;
		generate_lambda_expr ctx e
	| EIf (v,e,eo) ->
		generate_lambda_val ctx v;
		generate_lambda_expr ctx e;
		(match eo with
		| None -> ()
		| Some e -> generate_lambda_expr ctx e)
	| EWhile (v,e,_) ->
		generate_lambda_val ctx v;
		generate_lambda_expr ctx e;
	| ESwitch (v,cases,def) ->
		generate_lambda_val ctx v;
		List.iter (fun (v,e) ->
			generate_lambda_val ctx v;
			generate_lambda_expr ctx e;
		) cases;
		(match def with
		| None -> ()
		| Some e -> generate_lambda_expr ctx e)
	| EReturn (Some v)
	| EVal v ->
		generate_lambda_val ctx v
	| EReturn None
	| EBreak
	| EContinue ->
		()
	| EFunction _ 
	| EImport _
	| EClass _
	| EInterface _ ->
		assert false

let generate_class ctx clctx =
	IO.nwrite ctx.ch "/* ******************************************************** */\n";
	IO.printf ctx.ch "// CODE OF %s\n\n" (Class.filename clctx);
	ctx.current <- clctx;
	List.iter (fun f ->
		match f.fexpr with
		| None -> ()
		| Some e -> 
			generate_lambda_expr ctx e;
			let sf = new_stack_frame ctx in
			List.iter (fun (n,_) -> add_local ctx n) f.fargs;
			generate_method_head ctx clctx f;
			IO.write ctx.ch ' ';
			generate_expr ctx e;
			List.iter (fun (n,_) -> Hashtbl.remove ctx.locals n) f.fargs;
			clean_stack_frame ctx sf;
			IO.nwrite ctx.ch "\n\n";
	) (Class.methods clctx);
	IO.nwrite ctx.ch "\n\n"

let generate_class_decls ctx clctx =
	ctx.classes <- clctx :: ctx.classes;
	ctx.current <- clctx;
	List.iter (fun (s,_) ->
		IO.printf ctx.ch "value %s;\n" (static_name (Class.path clctx) s);
	) (Class.statics clctx);
	let gen_method f =
		generate_method_head ctx clctx f;
		IO.nwrite ctx.ch ";\n";
	in
	List.iter (fun f ->
		if f.fstatic = IsStatic && f.fexpr <> None then gen_method f;		
	) (Class.methods clctx);
	match Class.constructor clctx with
	| Some f -> gen_method f
	| None ->
		if not (Class.interface clctx) then
			gen_method { fname = "new"; fargs = []; ftype = None; fstatic = IsMember; fexpr = None }

let generate_class_statics ctx clctx =	
	ctx.current <- clctx;
	List.iter (fun (s,v) ->		
		IO.printf ctx.ch "\t%s = " (static_name (Class.path clctx) s);
		generate_val ctx v;
		IO.nwrite ctx.ch ";\n";
	) (Class.statics clctx)

let generate file exprs =
	let ctx = {
		alloc_count = 0;
		lambda_count = 0;
		allocs = Hashtbl.create 0;
		arrays = Hashtbl.create 0;
		lambdas = Hashtbl.create 0;
		cur_frame = 0;		
		locals = Hashtbl.create 0;
		wrote = false;
		level = 0;
		classes = [];
		ch = IO.output_channel (open_out file);
		current = Class.empty;
	} in
	IO.nwrite ctx.ch "#include <mtypes.h>\n\n";
	Class.generate (fun clctx ->
		if not (Class.native clctx) then generate_class_decls ctx clctx
	) exprs;
	ctx.classes <- List.rev ctx.classes;
	IO.nwrite ctx.ch "\n\n";
	IO.nwrite ctx.ch "/* ******************************************************** */\n";
	IO.printf ctx.ch "// INIT CODE \n\n";
	List.iter (fun clctx ->
		List.iter (fun (_,v) -> generate_lambda_val ctx v) (Class.statics clctx)
	) ctx.classes;
	IO.printf ctx.ch "void init() {\n";	
	List.iter (generate_class_statics ctx) ctx.classes;
	IO.printf ctx.ch "}\n\n";
	List.iter (generate_class ctx) ctx.classes;
	IO.close_out ctx.ch;

;;
let output = ref None in
Plugin.add
	[
		("-c",Arg.String (fun f -> output := Some f),"<file> : generate C code into target file");
	]
	(fun t -> 
		match !output with
		| None -> ()
		| Some file -> generate file (Typer.exprs t)
	)