open Printf

(* ------------------------------------------------------------------------ *)
(* TYPES - Common *)

type pos = {
	pmin : int;
	pmax : int;
	pfile : string;
}

type op =
	| Add
	| Sub
	| Mult
	| Div
	| Mod
	| Eq
	| Neq
	| Shl
	| Shr
	| Or
	| And
	| Xor
	| BAnd
	| BOr
	| Assign
	| AssignRef
	| AssignOp of op
	| Lt
	| Lte
	| Gt
	| Gte

type unop =
	| UIncr
	| UDecr
	| Minus
	| Not
	
type ident = string

type const =
	| Null
	| Int of int
	| Float of string
	| Ident of ident
	| String of string

(* ------------------------------------------------------------------------ *)
(* TYPES - Lexer *)

type kwd =
	| If
	| Else
	| While
	| Do
	| For
	| Break
	| Function
	| Return
	| Var
	| Primitive
	| Import
	| Class
	| New
	| Include
	| Switch
	| Case
	| Default

type token_val =
	| Eof
	| Const of const
	| Comment of string
	| CommentLine of string
	| Op of op
	| Unop of unop
	| Kwd of kwd
	| Res of string
	| POpen
	| PClose
	| BOpen
	| BClose
	| Sep
	| Next
	| Dot
	| DblDot
	| BkOpen
	| BkClose
	| Question

type token = token_val * pos

(* ------------------------------------------------------------------------ *)
(* TYPES - Parser *)

type prim_flag =
	| PrimFunction
	| PrimClass
	| PrimStatic

type arg_flag =
	| ArgNormal
	| ArgReference

type while_mode =
	| Normal
	| DoWhile

type way =
	| Prefix
	| Postfix

type ident_path =
	| Id of ident
	| IDot of ident * pident_path
and
	pident_path = ident_path * pos

type eval =
	| EConst of const
	| EParenthesis of pval
	| EOp of op * pval * pval
	| EUnop of pval * unop * way
	| ECall of pval * pval list
	| EAccess of pval * pval
	| EArray of pval list
	| EObject of (string * pval) list
	| ENew of string option * string * pval list
	| EDot of pval * pident_path
	| EImported of string * string * pval list option
	| ELambda of (string * arg_flag) list * pexpr
	| EQuestion of pval * pval * pval
	| ERef of string
	| ERes of string
and
	pval = eval * pos

and expr =
	| EBlock of pexpr list
	| ENext of pexpr * pexpr
	| EVal of eval
	| EIfThenElse of pval * pexpr * pexpr option
	| EWhile of pval * pexpr * while_mode
	| EFor of pval list * pval list * pval list * pexpr
	| ESwitch of pval * (pval * pexpr) list * pexpr option
	| EFunction of string * (string * arg_flag) list * pexpr
	| EBreak
	| EReturn of pval option
	| EVars of (string * pval option) list
	| EPrimitive of string * int * string * prim_flag
	| EImport of string * (string * int) option
	| EClass of string * (string * pos) option * pexpr list
and
	pexpr = expr * pos

(* ------------------------------------------------------------------------ *)
(* TOOLS *)

let pos = snd

let null_pos = { pfile = ""; pmin = 0; pmax = 0; }

let punion p p2 =
	{
		pfile = p.pfile;
		pmin = min p.pmin p2.pmin;
		pmax = max p.pmax p2.pmax;
	}

let rec expr_rec f (expr,_) =
	f expr;
	match expr with
	| ESwitch (_,el,eo) -> List.iter (fun (_,e) -> expr_rec f e) el; (match eo with None -> () | Some e -> expr_rec f e)
	| EBlock l -> List.iter (expr_rec f) l
	| ENext (a,b) -> expr_rec f a; expr_rec f b
	| EIfThenElse (_,e,None) -> expr_rec f e;
	| EIfThenElse (_,a,Some b) -> expr_rec f a; expr_rec f b
	| EWhile (_,e,_) -> expr_rec f e
	| EFor (_,_,_,e) -> expr_rec f e
	| EFunction (_,_,e) -> expr_rec f e
	| EClass (_,_,l) -> List.iter (expr_rec f) l
	| EImport (_,_) 
	| EPrimitive (_,_,_,_)
	| EVars _
	| EReturn _
	| EVal _
	| EBreak ->
		()

(* ------------------------------------------------------------------------ *)
(* PRINTERS - Common *)

let rec s_op = function
	| Add -> "+"
	| Sub -> "-"
	| Mult -> "*"
	| Div -> "/"
	| Eq -> "=="
	| Neq -> "!="
	| Shl -> "<<"
	| Shr -> ">>"
	| Or -> "|"
	| And -> "&"
	| Xor -> "^"
	| BAnd -> "&&"
	| BOr -> "||"
	| Assign -> "="
	| AssignRef -> ":="
	| Lt -> "<"
	| Lte -> "<="
	| Gt -> ">"
	| Gte -> ">="
	| Mod -> "%"
	| AssignOp op -> sprintf "%s=" (s_op op)

let s_unop = function
	| UIncr -> "++"
	| UDecr -> "--"
	| Minus -> "-"
	| Not -> "!"

let escape s =
	let b = Buffer.create 0 in
	for i = 0 to (String.length s) - 1 do
		match s.[i] with
		| '\n' -> Buffer.add_string b "\\n"
		| '\t' -> Buffer.add_string b "\\t"
		| '\r' -> Buffer.add_string b "\\r"
		| c -> Buffer.add_char b c
	done;
	Buffer.contents b

let s_const = function
	| Int i -> sprintf "%d" i
	| Float s -> s
	| Ident i -> i
	| String s -> sprintf "\"%s\"" (escape s)
	| Null -> "null"

(* ------------------------------------------------------------------------ *)
(* PRINTERS - Lexer *)

let s_kwd = function
	| If -> "if"
	| Else -> "else"
	| For -> "for"
	| Do -> "do"
	| While -> "while"
	| Break -> "break"
	| Function -> "function"
	| Return -> "return"
	| Var -> "var"
	| Primitive -> "primitive"
	| Import -> "import"
	| Class -> "class"
	| New -> "new"
	| Include -> "#include"
	| Switch -> "switch"
	| Case -> "case"
	| Default -> "default"

let s_token = function
	| Eof -> "end of file"
	| Const c -> s_const c
	| Comment s -> sprintf "//%s" s
	| CommentLine s -> sprintf "/*%s*/" s
	| Op op -> s_op op
	| Unop uop -> s_unop uop
	| Res s -> sprintf "'%s" s
	| POpen -> "("
	| PClose -> ")"
	| BOpen -> "{"
	| BClose -> "}"
	| Sep -> ","
	| Next -> ";"
	| Dot -> "."
	| DblDot -> ":"
	| Kwd k -> s_kwd k
	| BkOpen -> "["
	| BkClose -> "]"
	| Question -> "?"
