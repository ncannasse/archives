open Types

type error_msg =
	| Unclosed_block
	| Unclosed_parenthesis
	| Do_without_while
	| Unexpected of token_val
	| Include_not_found of string
	| Duplicate_default
	| Invalid_expression

exception Error of error_msg * pos

let error_msg = function
	| Unclosed_block -> "Unclosed block"
	| Unclosed_parenthesis -> "Unclosed parenthesis"
	| Do_without_while -> "do without matching while"
	| Unexpected t -> "Unexpected "^(s_token t)
	| Include_not_found file -> "Included file not found " ^ file
	| Invalid_expression -> "Invalid expression"
	| Duplicate_default -> "Duplicate default"

let error m p = raise (Error (m,p))

let op_priority = function
	| Assign | AssignOp _ | AssignRef -> -3
	| BAnd | BOr -> -2
	| Eq | Neq | Gt | Lt | Gte | Lte -> -1
	| Add | Sub -> 0
	| Mult | Div -> 1
	| Or | And | Xor -> 2
	| Shl | Shr | Mod -> 3

let rec unop p uop e =
	match e with
	| EOp (op,a,b) , pe -> EOp (op,unop p uop a,b) , punion p pe
	| _ , pe -> EUnop (e,uop,Prefix) , punion p pe

let blk = function
	| (EVars _ as v) , p -> EBlock [v,p] , p
	| e -> e

let rec binop op e1 e2 =
	match e2 with
	| EOp (op2,a,b) , pe when op_priority op >= op_priority op2 ->
		EOp (op2,binop op e1 a,b) , punion (pos e1) pe
	| _ , _ ->
		EOp (op,e1,e2) , punion (pos e1) (pos e2)

let rec parse_eval = parser
	| [< '(Const (Ident "fun"),p1); '(POpen,sp); args , _ = parse_arglist sp; e = parse_expr; s >] -> parse_eval_next (ELambda (args,e),punion p1 (pos e)) s
	| [< '(Const (Ident name),p1) when name.[0] >= 'A' && name.[0] <= 'Z'; '(Dot,_); '(Const (Ident vname),p2);
		  args , p2 = parse_eval_import p2; e = parse_eval_next ((EImported (name,vname,args) , punion p1 p2)) >] -> e
	| [< '(Const c,p) ; e = parse_eval_next (EConst c,p) >] -> e
	| [< '(BkOpen,p1); l = parse_eval_list; '(BkClose,p2); e = parse_eval_next (EArray l,punion p1 p2) >] -> e
	| [< '(BOpen,p1); l = parse_field_list; '(BClose,p2); e = parse_eval_next (EObject l,punion p1 p2) >] -> e
	| [< '(POpen,p) ; e = parse_eval; e = parse_eval_p p (EParenthesis e) >] -> e
	| [< '(Op Sub,p) ; e = parse_eval; >] -> unop p Minus e
	| [< '(Unop uop,p) ; e = parse_eval; >] -> unop p uop e
	| [< '(Kwd New,p1) ; mname, name, args, p2 = parse_new; e = parse_eval_next (ENew (mname,name,args) , punion p1 p2) >] -> e
	| [< '(Op And,p1) ; '(Const (Ident name),p2); e = parse_eval_next (ERef name, punion p1 p2) >] -> e
	| [< '(Res name,p); e = parse_eval_next (ERes name, p) >] -> e
and
	parse_eval_next c = parser
	| [< '(Question,_); e1 = parse_eval; '(DblDot,_); e2 = parse_eval >] -> EQuestion (c,e1,e2), punion (pos c) (pos e2)
	| [< '(Dot,_); i , p = parse_ident_path; e = parse_eval_next (EDot (c,(i,p)), punion (pos c) p) >] -> e
	| [< '(POpen,sp) ; el , p = parse_params sp; e = parse_eval_next (ECall (c,el),punion p (pos c)) >] -> e
	| [< '(Unop uop,p); e = parse_eval_next (EUnop (c,uop,Postfix),punion p (pos c)) >] -> e
	| [< '(BkOpen,_); e = parse_eval; '(BkClose,p); e = parse_eval_next (EAccess (c,e),punion (pos c) p) >] -> e
	| [< '(Op op,p); e = parse_eval >] -> binop op c e
	| [< >] -> c
and
	parse_eval_list = parser
	| [< '(Sep,_); l = parse_eval_list >] -> l
	| [< p = parse_eval; l = parse_eval_list >] -> p :: l
	| [< >] -> []
and
	parse_field_list = parser
	| [< '(Const (Ident name),_); '(DblDot,_); e = parse_eval; l = parse_field_list >] -> (name,e) :: l
	| [< '(Sep,_); l = parse_field_list >] -> l
	| [< >] -> []
and
	parse_new = parser
	| [< '(Const (Ident mname),_) when mname.[0] >= 'A' && mname.[0] <= 'Z'; mname, name, args, p = parse_new_next mname >] -> mname , name, args, p
	| [< '(Const (Ident name),_); '(POpen,sp) ; args , p2 = parse_params sp; >] -> None , name , args, p2
and
	parse_new_next mname = parser
	| [< '(Dot,_); '(Const (Ident name),_); '(POpen,sp); args , p2 = parse_params sp; >] -> (Some mname) , name, args, p2
	| [< '(POpen,sp); args , p2 = parse_params sp; >] -> None, mname, args, p2
and
	parse_eval_import p = parser
	| [< '(POpen,sp) ; el , p = parse_params sp; >] -> Some el , p
	| [< >] -> None , p
and
	parse_ident_path = parser
	| [< '(Const (Ident name),p); id = parse_ident_next name p >] -> id
and
	parse_ident_next name p = parser
	| [< '(Dot,_); id = parse_ident_path >] -> IDot (name,id) , p
	| [< >] -> Id name , p
and
	parse_expr = parser
	| [< '(BOpen,p1) ; el, p2 = parse_block p1 >] -> EBlock el , punion p1 p2
	| [< '(Kwd If,p1); v = parse_eval; e = parse_expr; ep, p2 = parse_else (pos e) >] -> EIfThenElse (v,blk e,ep) , punion p1 p2
	| [< '(Kwd While,p); v = parse_eval; e = parse_expr >] -> EWhile (v,blk e,Normal) , punion p (pos e)
	| [< '(Kwd Do,p); e = parse_expr; v = parse_dowhile p >] -> EWhile(v,blk e,DoWhile) , punion p (pos v)
	| [< '(Kwd Switch,p1); v = parse_eval; '(BOpen,_); el , eo, p2 = parse_switch >] -> ESwitch (v,el,eo) , punion p1 p2
	| [< '(Kwd For,p); '(POpen,_); l1 = parse_for; l2 = parse_for; l3 , _ = parse_last_for p; e = parse_expr >] -> EFor(l1,l2,l3,blk e) , punion p (pos e)
	| [< '(Kwd Function,p); '(Const (Ident name),_); '(POpen,sp); args , _ = parse_arglist sp; e = parse_expr >] -> EFunction(name,args,e) , punion p (pos e)
	| [< '(Kwd Primitive,p1); prim , p2 = parse_prim_decl >] -> prim , punion p1 p2
	| [< '(Kwd Return,p1); r , p2 = parse_return p1; >] -> EReturn r , punion p1 p2
	| [< '(Kwd Break,p) >] -> EBreak , p
	| [< '(Kwd Var,sp); '(Const (Ident name),p); v , p = parse_vars name p >] -> EVars v , punion sp p
	| [< '(Kwd Import,p1); name , opt , p2 = parse_import >] -> 
		name.[0] <- Char.uppercase name.[0];
		EImport (name,opt), punion p1 p2
	| [< '(Kwd Class,p1); '(Const (Ident name),_); ext , sp = parse_class_header; ms, p2 = parse_class sp >] -> EClass (name,ext,ms) , punion p1 p2
	| [< '(Kwd Include,_); '(Const (String file),p) >] ->
		let ch = (try open_in file with _ -> error (Include_not_found file) p) in
		let state = Lexer.save() in
		let e = parse file (Lexing.from_channel ch) in
		Lexer.restore state;
		close_in ch;
		e
	| [< (e,p) = parse_eval >] -> EVal e , p
and
	parse_class_header = parser
	| [< '(BOpen,sp); >] -> None , sp
	| [< '(Const (Ident "extends"),_); '(Const (Ident name),p); '(BOpen,sp) >] -> Some (name,p) , sp
and
	parse_class sp = parser
	| [< e = parse_class_item; l , p = parse_class sp >] -> e :: l , p
	| [< '(Next,_); l = parse_class sp >] -> l
	| [< '(BClose,p) >] -> [] , p
	| [< '(Eof,_) >] -> error Unclosed_block sp
and
	parse_class_item = parser
	| [< '(Kwd Function,p); '(Const (Ident name),_); '(POpen,sp); args , _ = parse_arglist sp; e = parse_expr >] -> EFunction(name,args,e) , punion p (pos e)
and
	parse_return p = parser
	| [< e = parse_eval >] -> Some e , pos e
	| [< >] -> None , p
and 
	parse_vars name p = parser
	| [< '(Op Assign,_); v = parse_eval; vl, p = parse_vars_next (pos v) >] -> (name,Some v) :: vl , p
	| [< vl , p = parse_vars_next p >] -> (name,None) :: vl , p
and
	parse_vars_next p = parser
	| [< '(Sep,_); '(Const (Ident name),p); vl, p = parse_vars name p >] -> vl , p
	| [< >] -> [] , p
and
	parse_eval_p sp e = parser
	| [< '(PClose,p); e = parse_eval_next (e,punion sp p) >] -> e
	| [< '(Eof,_) >] -> error Unclosed_parenthesis sp
and
	parse_params sp = parser
	| [< '(PClose,p) >] -> [] , p
	| [< '(Sep,_); el = parse_params sp >] -> el
	| [< e = parse_eval; el , p = parse_params sp >] -> e :: el , p
	| [< '(Eof,_) >] -> error Unclosed_parenthesis sp
and
	parse_import = parser
	| [< '(Kwd Class,_); '(Const (Ident name),_); '(Dot,_); '(Const (Ident cname),p2) >] -> name , Some("#"^cname,-1) , p2
	| [< '(Const (Ident name),p); opt, p2 = parse_import_function p >] -> name, opt, p2
and
	parse_import_function p = parser
	| [< '(Dot,_); '(Const (Ident name),p); nargs , p = parse_import_args p >] -> Some (name,nargs) , p
	| [< >] -> None , p
and
	parse_import_args p = parser
	| [< '(POpen,sp); args , p = parse_arglist sp; >] -> List.length args , p
	| [< >] -> -1 , p
and
	parse_prim_decl = parser
	| [< '(Const (Ident "static"),_); '(Const (Ident name),_); '(Op Assign,_); '(Const (Ident addr),p2) >] -> EPrimitive (name,0,addr,PrimStatic) , p2
	| [< '(Const (Ident name),_); nargs = parse_primitive ; '(Const (Ident addr),p2) >] -> EPrimitive (name,nargs,addr,PrimFunction) , p2
	| [< '(Kwd Class,_); '(Const (Ident name),_); '(Op Assign,_) ; '(Const (Ident addr),p2) >] -> EPrimitive (name,0,addr,PrimClass) , p2
and
	parse_primitive = parser
	| [< '(POpen,sp); args , _ = parse_arglist sp; '(Op Assign,_) >] -> List.length args
	| [< '(Op Assign,_) >] -> -1
and
	parse_arglist sp = parser
	| [< '(PClose,p) >] -> [] , p
	| [< '(Sep,_) ; l , p = parse_arglist sp >] -> l , p
	| [< '(Const (Ident name),_); l , p = parse_arglist sp >] -> (name,ArgNormal) :: l , p
	| [< '(Op And,_); '(Const (Ident name),_); l , p = parse_arglist sp >] -> (name,ArgReference) :: l , p
	| [< '(Eof,_) >] -> error Unclosed_parenthesis sp
and
	parse_for = parser
	| [< '(Next,_) >] -> []
	| [< '(Sep,_) ; l = parse_for>] -> l
	| [< e = parse_eval; l = parse_for>] -> e :: l
and
	parse_last_for sp = parse_params sp
and 
	parse_switch = parser
	| [< '(BClose,p) >] -> [] , None , p
	| [< '(Kwd Case,p); v = parse_eval; '(DblDot,_); c = parse_switch_clause; el, eo, p2 = parse_switch >] -> (v,(EBlock c,p)) :: el , eo , p2
	| [< '(Kwd Default,p); '(DblDot,_); c = parse_switch_clause; el, eo, p2 = parse_switch >] -> 
		if eo <> None then error Duplicate_default p;
		el , Some (EBlock c,p) , p2
and 
	parse_switch_clause = parser
	| [< e = parse_expr; el = parse_switch_clause >] -> e :: el
	| [< '(Next,_); el = parse_switch_clause >] -> el
	| [< >] -> []
and
	parse_else sp = parser
	| [< '(Kwd Else,_); e = parse_expr >] -> Some (blk e) , pos e
	| [< '(Next,_); e = parse_else sp >] -> e
	| [< >] -> None , sp
and
	parse_dowhile sp = parser
	| [< '(Kwd While,_); e = parse_eval >] -> e
	| [< >] -> error Do_without_while sp
and
	parse_block sp = parser
	| [< '(BClose,p) >] -> [] , p
	| [< '(Next,_); el = parse_block sp >] -> el
	| [< e = parse_expr ; el, p = parse_block sp >] -> e :: el , p
	| [< '(Eof,_) >] -> error Unclosed_block sp
and
	parse_code = parser
	| [< '(Eof,_) >] -> []
	| [< '(Next,_); el = parse_code >] -> el
	| [< e = parse_expr; el = parse_code >] -> e :: el
and
  parse filename code =
	Lexer.init filename;
	let last = ref (Eof,null_pos) in
	let rec next_token x =
		let t, p = Lexer.token code in
		match t with 
		| Comment _ | CommentLine _ -> next_token x
		| _ ->
			last := (t , p);
			Some (t , p)
	in
	try
		let l = parse_code (Stream.from next_token) in
		EBlock l , { pfile = filename; pmin = 0; pmax = (snd !last).pmax }
	with
		| Stream.Error _
		| Stream.Failure -> error (Unexpected (fst !last)) (pos !last)

let rec check (e,p) =
	let rec check_val (v,p) =
		match v with
		| EOp (Assign,_,_)
		| EOp (AssignRef,_,_)
		| EOp (AssignOp _,_,_) ->
			()
		| EParenthesis v ->
			check_val v
		| EConst _ 
		| EOp _
		| EUnop (_,Minus,_)
		| EUnop (_,Not,_)
		| ENew _
		| EAccess _ 
		| EArray _
		| EObject _
		| EDot _
		| EImported (_,_,None)
		| ERef _ 
		| ERes _ ->
			error Invalid_expression p
		| ELambda (_,p) ->
			check p
		| EUnop (_,UIncr,_)
		| EUnop (_,UDecr,_)
		| EQuestion _
		| EImported (_,_,Some _)
		| ECall _ ->
			()
	in
	match e with
	| EBlock l -> List.iter check l
	| ENext (a,b) -> check a; check b;
	| EVal v -> check_val (v,p)
	| EIfThenElse (_,p,None) -> check p
	| EIfThenElse (_,p,Some p2) -> check p; check p2
	| EWhile (_,p,_) -> check p
	| EFor (vl,_,v2l,p) -> List.iter check_val vl; List.iter check_val v2l; check p
	| ESwitch (v,cases,def) ->
		List.iter (fun (_,e) -> check e) cases;
		(match def with None -> () | Some e -> check e)
	| EFunction (_,_,p) -> check p
	| EBreak -> ()
	| EReturn _ -> ()
	| EVars _ -> ()
	| EPrimitive _ -> ()
	| EImport _ -> ()
	| EClass (_,_,pl) -> List.iter check pl
