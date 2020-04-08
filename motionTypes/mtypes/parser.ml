open Type
open Expr

type error_msg =
	| Unexpected of token
	| Unclosed_parenthesis
	| Duplicate_default

exception Error of error_msg * pos

let error_msg = function
	| Unexpected t -> "Unexpected "^(s_token t)
	| Unclosed_parenthesis -> "Unclosed parenthesis"
	| Duplicate_default -> "Duplicate default"

let error m p = raise (Error (m,p))

let priority = function
	| OpAssign | OpAssignOp _ -> -3
	| OpBoolAnd | OpBoolOr -> -2
	| OpEq | OpNotEq | OpGt | OpLt | OpGte | OpLte -> -1
	| OpAdd | OpSub -> 0
	| OpMult | OpDiv -> 1
	| OpOr | OpAnd | OpXor -> 2
	| OpShl | OpShr | OpMod | OpUShr -> 3

let rec make_binop op e ((v,p2) as e2) =
	match v with
	| EBinop (_op,_e,_e2) when priority _op <= priority op ->
		let _e = make_binop op e _e in
		EBinop (_op,_e,_e2) , punion (pos _e) (pos _e2)
	| _ ->
		EBinop (op,e,e2) , punion (pos e) (pos e2)

let rec make_unop op ((v,p2) as e) p1 = 
	match v with
	| EBinop (bop,e,e2) -> EBinop (bop, make_unop op e p1 , e2) , (punion p1 p2)
	| _ ->
		EUnop (op,Prefix,e), punion p1 p2

let rec make_path e = 
	let rec loop acc (e,_) =
		match e with
		| EConst (Ident s) -> s :: acc
		| EField (e,f) -> loop (f :: acc) e
		| _ -> raise Stream.Failure
	in
	loop [] e

let rec parse_sign = parser
	| [< '(Kwd Interface,p1); path = parse_class_path; params = parse_class_params; '(BrOpen,p); el , p2 = parse_interface >] -> EInterface (path,params,(EBlock el,punion p p2)) , punion p1 p2
	| [< '(Kwd Native,p); '(Kwd Class,p); path = parse_class_path; params = parse_class_params; herits = parse_herits; '(BrOpen,p); el, p2 = parse_interface >] -> EClass (path,params,HNative::herits,(EBlock el,punion p p2)) , punion p p2
	| [< '(Kwd Class,p); path = parse_class_path; params = parse_class_params; herits = parse_herits; e = parse_sign >] -> EClass (path,params,herits,e) , punion p (pos e)
	| [< '(BrOpen,p1); el , p2 = parse_block parse_sign p1 >] -> EBlock el , punion p1 p2
	| [< e = parse_decl IsMember >] -> e
	| [< e = parse_expr >] -> e

and parse_expr = parser
	| [< '(BrOpen,p1); el , p2 = parse_block parse_expr p1 >] -> EBlock el , punion p1 p2
	| [< '(Kwd For,p); '(POpen,_); e = parse_for p >] -> e
	| [< '(Kwd If,p); cond = parse_eval; e = parse_expr; e2 , p2 = parse_else (pos e) >] -> EIf (cond,e,e2), punion p p2
	| [< '(Kwd Return,p); v , p2 = parse_eval_option p; >] -> EReturn v , punion p p2
	| [< '(Kwd Break,p); >] -> EBreak , p
	| [< '(Kwd Continue,p); >] -> EContinue , p
	| [< '(Kwd While,p1); v = parse_eval; e = parse_expr >] -> EWhile (v,e,NormalWhile) , punion p1 (pos e)
	| [< '(Kwd Do,p1); e = parse_expr; '(Kwd While,_); v = parse_eval; >] -> EWhile (v,e,DoWhile) , punion p1 (pos v)
	| [< '(Kwd Switch,p1); v = parse_eval; '(BrOpen,_); el , eo, p2 = parse_switch >] -> ESwitch (v,el,eo) , punion p1 p2
	| [< '(Kwd Import,p1); p = parse_class_path >] -> EImport p , p1
	| [< '(Kwd Var,p1); vl, p2 = parse_vars p1 >] -> EVars (IsMember,vl), punion p1 p2
	| [< '(Kwd Try,p1); e = parse_expr; '(Kwd Catch,_); '(POpen,_); '(Const (Ident i),_); t = parse_exception_type; '(PClose,_); c = parse_expr >] -> ETry (e,i,t,c) , punion p1 (pos c)
	| [< e = parse_eval >] -> EVal e , pos e

and parse_eval = parser
	| [< '(Const (Ident "fun"),p1); '(POpen,_); args, _ = parse_args; t = parse_type_decl_option; e = parse_expr;
		v = parse_eval_next (ELambda {
			fname = "";
			fargs = args;
			ftype = t;
			fstatic = IsStatic;
			fexpr = Some e;
		} , punion p1 (pos e)) >] -> v
	| [< '(Const (Ident "new"),p1); p = parse_class_path; e = parse_new p p1 >] -> e
	| [< '(Const c,p); e = parse_eval_next (EConst c,p)  >] -> e
	| [< '(POpen,p1); e = parse_eval; '(PClose,p2); e = parse_eval_next (EParenthesis e , punion p1 p2) >] -> e
	| [< '(BrOpen,p1); el, p2 = parse_field_list; e = parse_eval_next (EObjDecl el, punion p1 p2) >] -> e
	| [< '(BkOpen,p1); el, p2 = parse_array; e = parse_eval_next (EArrayDecl el,punion p1 p2) >] -> e
	| [< '(Unop op,p1) when is_prefix op; e = parse_eval >] -> make_unop op e p1
	| [< '(Binop OpSub,p1); e = parse_eval >] -> make_unop Neg e p1

and parse_eval_next e = parser
	| [< '(BkOpen,_); e2 = parse_eval; '(BkClose,p2); e = parse_eval_next (EArray (e,e2) , punion (pos e) p2) >] -> e
	| [< op = parse_binop; e2 = parse_eval; >] -> make_binop op e e2
	| [< '(Dot,_); e = parse_field_access e >] -> e
	| [< '(POpen,_); args = parse_eval_list; '(PClose,p2); e = parse_eval_next (ECall (e,args), punion (pos e) p2) >] -> e
	| [< '(Unop op,p2) when is_postfix op; e = parse_eval_next (EUnop (op,Postfix,e), punion (pos e) p2) >] -> e
	| [< '(Question,_); v1 = parse_eval; '(DblDot,_); v2 = parse_eval; e = parse_eval_next (EQuestion (e,v1,v2), punion (pos e) (pos v2)) >] -> e
	| [< >] -> e

and parse_field_access e = parser
	| [< '(Const (Ident field),p2); e = parse_eval_next (EField (e,field), punion (pos e) p2) >] -> e
	| [< '(Const (Name cl),p2); e = parse_eval_next (EStatic (make_path e,cl), punion (pos e) p2) >] -> e

and parse_binop = parser
	| [< '(Binop OpGt,_); op = parse_binop_gt >] -> op
	| [< '(Binop op,_) >] -> op

and parse_binop_gt = parser
	| [< '(Binop OpGt,_); op = parse_binop_gt2 >] -> op
	| [< >] -> OpGt

and parse_binop_gt2 = parser
	| [< '(Binop OpGt,_) >] -> OpUShr
	| [< >] -> OpShr

and parse_decl stat = parser
	| [< '(Const (Ident "public"),_); e = parse_decl stat >] -> e
	| [< '(Const (Ident "private"),_); e = parse_decl stat >] -> e
	| [< '(Const (Ident "volatile"),_); e = parse_decl IsVolatile >] -> e
	| [< '(Kwd Static,_); e = parse_decl IsStatic >] -> e
	| [< '(Kwd Var,p1); vl, p2 = parse_vars p1 >] -> EVars (stat,vl), punion p1 p2
	| [< '(Kwd Function,p1); '(Const (Ident fname),_); '(POpen,_); args , p2 = parse_args; t = parse_type_decl_option; e = parse_expr >] -> 
		EFunction {
			fname = fname;
			fargs = args;
			ftype = t;
			fstatic = stat;
			fexpr = Some e;
		} , punion p1 (pos e)

and parse_interface = parser
	| [< '(Next,_); el , p = parse_interface >] -> el , p
	| [< '(BrClose,p) >] -> [] , p
	| [< e = parse_interface_decl IsMember; el, ep = parse_interface >] -> e :: el , ep

and parse_interface_decl stat = parser
	| [< '(Const (Ident "public"),_); e = parse_interface_decl stat >] -> e
	| [< '(Const (Ident "private"),_); e = parse_interface_decl stat >] -> e
	| [< '(Kwd Static,_); e = parse_interface_decl_static >] -> e 
	| [< '(Kwd Var,p1); vl, p2 = parse_vars p1;  >] -> EVars (stat,vl), punion p1 p2
	| [< '(Kwd Function,p1); '(Const (Ident fname),_); '(POpen,_); args , p2 = parse_args; t = parse_type_decl_option >] ->
		EFunction {	
			fname = fname;
			fargs = args;
			ftype = t;
			fstatic = stat;
			fexpr = None;
		} , punion p1 p2

and parse_interface_decl_static = parser
	| [< '(Const (Ident "public"),_); e = parse_interface_decl_static >] -> e
	| [< '(Const (Ident "private"),_); e = parse_interface_decl_static >] -> e
	| [< '(Kwd Var,p1); vl, p2 = parse_vars p1;  >] -> EVars (IsStatic,vl), punion p1 p2
	| [< '(Kwd Function,p1); '(Const (Ident fname),_); '(POpen,_); args , p2 = parse_args; t = parse_type_decl_option; e, p2 = parse_expr_option p2 >] ->
		EFunction {	
			fname = fname;
			fargs = args;
			ftype = t;
			fstatic = IsStatic;
			fexpr = e;
		} , punion p1 p2

and parse_herits = parser
	| [< '(Kwd Extends,_); p = parse_class_path; params = parse_type_params; l = parse_herits >] -> HExtends (p,params) :: l
	| [< '(Kwd Implements,_); p = parse_class_path; params = parse_type_params; l = parse_herits >] -> HImplements (p,params) :: l
	| [< >] -> []

and parse_new e p1 = parser
	| [< params = parse_type_params; '(POpen,_); args = parse_eval_list; '(PClose,p2); e = parse_eval_next (ENew (e,params,args), punion p1 p2) >] -> e	

and parse_expr_option p = parser
	| [< e = parse_expr >] -> Some e , pos e
	| [< >] -> None, p

and parse_eval_option p = parser
	| [< v = parse_eval >] -> Some v , pos v
	| [< >] -> None, p

and parse_eval_list = parser
	| [< v = parse_eval; vl = parse_eval_list >] -> v :: vl
	| [< '(Sep,_); vl = parse_eval_list >] -> vl
	| [< '(Next,_) >] -> []
	| [< >] -> []

and parse_field_list = parser
	| [< '(Const (Ident fname),_); '(DblDot,_); e = parse_eval; el , p = parse_field_list >] -> (fname,e) :: el , p
	| [< '(Sep,_); el = parse_field_list >] -> el
	| [< '(BrClose,p) >] -> [] , p

and parse_array = parser
	| [< e = parse_eval; el , p = parse_array >] -> e :: el , p
	| [< '(Sep,_); e = parse_array >] -> e
	| [< '(BkClose,p) >] -> [] , p

and parse_else p = parser
	| [< '(Next,_); e = parse_else p >] -> e
	| [< '(Kwd Else,_); e = parse_expr >] -> Some e, pos e
	| [< >] -> None , p

and parse_for p = parser
	| [< cl = parse_for_conds; l1 = parse_eval_list; l2 = parse_eval_list; '(PClose,p2); e = parse_expr >] -> EFor(cl,l1,l2,e) , punion p p2

and parse_for_conds = parser
	| [< e = parse_expr; l = parse_for_conds_next >] -> e :: l

and parse_for_conds_next = parser
	| [< '(Sep,_); l = parse_for_conds >] -> l
	| [< '(Next,_) >] -> [] 
	| [< >] -> []

and parse_args = parser
	| [< '(Const (Ident name),_); t = parse_type_decl_option; al , p = parse_args >] -> (name , t) :: al , p
	| [< '(Sep,_); al= parse_args >] -> al
	| [< '(PClose,p) >] -> [] , p

and parse_vars p = parser
	| [< '(Const (Ident name),p); t = parse_type_decl_option; v = parse_var_init; vl , p = parse_vars_next p >] -> (name , t, v) :: vl , p
	| [< >] -> [] , p

and parse_vars_next p = parser
	| [< '(Sep,_); vl , p = parse_vars p >] -> vl , p
	| [< >] -> [] , p

and parse_var_init = parser
	| [< '(Binop OpAssign,_); v = parse_eval >] -> Some v
	| [< >] -> None

and parse_switch = parser
	| [< '(BrClose,p) >] -> [] , None , p
	| [< '(Kwd Case,p); v = parse_eval; '(DblDot,_); c = parse_switch_clause; el, eo, p2 = parse_switch >] -> (v,(EBlock c,p)) :: el , eo , p2
	| [< '(Kwd Default,p); '(DblDot,_); c = parse_switch_clause; el, eo, p2 = parse_switch >] -> 
		if eo <> None then error Duplicate_default p;
		el , Some (EBlock c,p) , p2

and parse_switch_clause = parser
	| [< e = parse_expr; el = parse_switch_clause >] -> e :: el
	| [< '(Next,_); el = parse_switch_clause >] -> el
	| [< >] -> []

and parse_block callb sp = parser
	| [< e = callb; el,p = parse_block callb sp >] -> e :: el , p
	| [< '(Next,_); el = parse_block callb sp >] -> el
	| [< '(BrClose,p); >] -> [] , p
	| [< '(Eof,_) >] -> error Unclosed_parenthesis sp

and parse_expr_list p = parser
	| [< e = parse_expr; el, p = parse_expr_list (pos e) >] -> e :: el , p
	| [< '(Next,_); el = parse_expr_list p >] -> el
	| [< >] -> [] , p

and parse_class_params = parser
	| [< '(Binop OpLt,_); tl = parse_class_param_list; >] -> tl
	| [< >] -> []

and parse_class_param_list = parser
	| [< '(Binop OpGt,_) >] -> []
	| [< '(Sep,_); l = parse_class_param_list >] -> l
	| [< '(Quote s,_); l = parse_class_param_list >] -> s :: l

and parse_type_decl_option = parser
	| [< '(DblDot,_); t = parse_type_decl >] -> Some t
	| [< >] -> None

and parse_exception_type = parser
	| [< '(DblDot,_); t = parse_class_path >] -> Some t
	| [< >] -> None

and parse_type_decl = parser
	| [< tstd = parse_type_std; t = parse_type_decl_next (TypeStd tstd) >] -> t
	| [< p = parse_class_path; params = parse_type_params; t = parse_type_decl_next (TypePath (p,params)) >] -> t
	| [< '(POpen,_); t = parse_type_decl; '(PClose,_); t = parse_type_decl_next t >] -> t
	| [< '(Quote s,_); t = parse_type_decl_next (TypePoly s) >] -> t
	| [< '(BrOpen,_); ext = parse_type_ext; fields = parse_type_fields; t = parse_type_decl_next (TypeFields (fields,ext)) >] -> t

and parse_type_ext = parser
	| [< '(Binop OpGt,_); t = parse_type_decl >] -> Some t
	| [< >] -> None

and parse_type_fields = parser
	| [< '(Const (Ident name),_); '(DblDot,_); t = parse_type_decl; l = parse_type_fields >] -> (name,t) :: l
	| [< '(Sep,_); l = parse_type_fields >] -> l
	| [< '(BrClose,_) >] -> []

and parse_type_params = parser
	| [< '(Binop OpLt,_); tl = parse_type_decl_list; '(Binop OpGt,_); >] -> tl
	| [< >] -> []

and parse_type_decl_next t = parser
	| [< '(Arrow,_); t2 = parse_type_decl >] -> TypeFun (t,t2)
	| [< >] -> t

and parse_type_std = parser
	| [< '(Const (Ident "int"),_); >] -> TypeInt
	| [< '(Const (Ident "float"),_); >] -> TypeFloat
	| [< '(Const (Ident "bool"),_); >] -> TypeBool
	| [< '(Const (Ident "void"),_); >] -> TypeVoid

and parse_type_decl_list = parser
	| [< t = parse_type_decl; tl = parse_type_decl_list >] -> t :: tl
	| [< '(Sep,_); tl = parse_type_decl_list >] -> tl
	| [< >] -> []

and parse_class_path = parser
	| [< '(Const (Ident name),_); '(Dot,_); path , cname = parse_class_path  >] -> name :: path , cname
	| [< '(Const (Name name),_) >] -> [] , name

and	parse_code = parser
	| [< '(Eof,_) >] -> []
	| [< '(Next,_); el = parse_code >] -> el
	| [< e = parse_sign; el = parse_code >] -> e :: el

let parse code file =
	let old = Lexer.save() in
	Lexer.init file;
	let last = ref (Eof,null_pos) in
	let comments = ref [] in
	let rec next_token x =
		let t, p = Lexer.token code in
		match t with 
		| Comment s | CommentLine s -> 
			comments := (s,p) :: !comments;
			next_token x
		| _ ->
			last := (t , p);
			Some (t , p)
	in
	try
		let l = parse_code (Stream.from next_token) in
		Lexer.restore old;
		l , List.rev !comments
	with
		| Stream.Error _
		| Stream.Failure -> 
			Lexer.restore old;
			error (Unexpected (fst !last)) (pos !last)
		| e ->
			Lexer.restore old;
			raise e
