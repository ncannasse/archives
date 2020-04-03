
type pos = {
	pfile : string;
	pmin : int;
	pmax : int;
}

type keyword =
	| Function
	| Class
	| Private
	| Public
	| Static
	| Var
	| New
	| If
	| Else
	| While
	| Do
	| For
	| Break
	| Continue
	| Return
	| Interface
	| Extends
	| Implements
	| Import
	| Switch
	| Case
	| Default
	| SharpType
	| NoType

type binop =
	| OpAdd
	| OpMult
	| OpDiv
	| OpSub
	| OpAssign
	| OpEq
	| OpNotEq
	| OpGt
	| OpGte
	| OpLt
	| OpLte
	| OpAnd
	| OpOr
	| OpXor
	| OpBoolAnd
	| OpBoolOr
	| OpShl
	| OpShr
	| OpMod
	| OpAssignOp of binop

type unop =
	| Increment
	| Decrement
	| Not
	| Neg

type constant =
	| Int of string
	| Float of string
	| String of string
	| Ident of string

type token =
	| Eof
	| Const of constant
	| Kwd of keyword
	| Comment of string
	| CommentLine of string
	| Binop of binop
	| Unop of unop
	| Next
	| Sep
	| BrOpen
	| BrClose
	| BkOpen
	| BkClose
	| POpen
	| PClose
	| Dot
	| DblDot
	| Question

type public_flag =
	| IsPublic
	| IsPrivate

type static_flag =
	| IsMember
	| IsStatic

type unop_flag =
	| Prefix
	| Postfix

type while_flag =
	| NormalWhile
	| DoWhile

type type_path = string list * string

type type_decl = type_path

type func = {
	fname : string;
	fargs : (string * type_decl option) list;
	ftype : type_decl option;
	fpublic : public_flag;
	fstatic : static_flag;
	fexpr : expr option;
}

and herit = 
	| HExtends of type_path
	| HImplements of type_path
	| HIntrinsic

and eval_def =
	| EConst of constant
	| EArray of eval * eval
	| EBinop of binop * eval * eval
	| EField of eval * string
	| EParenthesis of eval
	| EObjDecl of (string * eval) list
	| EArrayDecl of eval list
	| ECall of eval * eval list
	| ENew of type_path * eval list
	| EUnop of unop * unop_flag * eval
	| EQuestion of eval * eval * eval
	| EConstraint of eval * type_decl

and eval = eval_def * pos

and expr_def =
	| EClass of type_path * herit list * expr
	| EInterface of type_path * expr
	| EVars of public_flag * static_flag * (string * type_decl option * eval option) list
	| EFunction of func
	| EBlock of expr list
	| EFor of expr list * eval list * eval list * expr
	| EForIn of expr * eval * expr
	| EIf of eval * expr * expr option
	| EWhile of eval * expr * while_flag
	| ESwitch of eval * (eval * expr) list * expr option
	| EReturn of eval option
	| EBreak
	| EContinue
	| EVal of eval
	| EImport of type_path
	| EType of eval
	| ENoType of expr

and expr = expr_def * pos

let pos = snd

let is_postfix = function
	| Increment | Decrement -> true
	| Not | Neg -> false

let is_prefix = function
	| Increment | Decrement -> true
	| Not | Neg -> true

let rec base_class_name = snd

let null_pos = { pfile = "<null>"; pmin = -1; pmax = -1 }

let punion p p2 =
	{
		pfile = p.pfile;
		pmin = min p.pmin p2.pmin;
		pmax = max p.pmax p2.pmax;
	}

let s_type_path (p,s) = match p with [] -> s | _ -> String.concat "." p ^ "." ^ s

let s_type_decl = s_type_path

let s_escape s =
	let b = Buffer.create (String.length s) in
	for i = 0 to (String.length s) - 1 do
		match s.[i] with
		| '\n' -> Buffer.add_string b "\\n"
		| '\t' -> Buffer.add_string b "\\t"
		| '\r' -> Buffer.add_string b "\\r"
		| c -> Buffer.add_char b c
	done;
	Buffer.contents b

let s_constant = function
	| Int s -> s
	| Float s -> s
	| String s -> "\"" ^ s_escape s ^ "\""
	| Ident s -> s
	
let s_keyword = function
	| Function -> "function"
	| Class -> "class"
	| Private -> "private"
	| Public -> "public"
	| Static -> "static"
	| Var -> "var"
	| New -> "new"
	| If -> "if"
	| Else -> "else"
	| While -> "while"
	| Do -> "do"
	| For -> "for"
	| Break -> "break"
	| Return -> "return"
	| Continue -> "continue"
	| Interface -> "interface"
	| Extends -> "extends"
	| Implements -> "implements"
	| Import -> "import"
	| Switch -> "switch"
	| Case -> "case"
	| Default -> "default"
	| SharpType -> "#type"
	| NoType -> "//#NOTYPE"

let rec s_binop = function
	| OpAdd -> "+"
	| OpMult -> "*"
	| OpDiv -> "/"
	| OpSub -> "-"
	| OpAssign -> "="
	| OpEq -> "=="
	| OpNotEq -> "!="
	| OpGte -> ">="
	| OpLte -> "<="
	| OpGt -> ">"
	| OpLt -> "<"
	| OpAnd -> "&"
	| OpOr -> "|"
	| OpXor -> "^"
	| OpBoolAnd -> "&&"
	| OpBoolOr -> "||"
	| OpShr -> ">>"
	| OpShl -> "<<"
	| OpMod -> "%"
	| OpAssignOp op -> s_binop op ^ "="

let s_unop = function
	| Increment -> "++"
	| Decrement -> "--"
	| Not -> "!"
	| Neg -> "-"

let s_token = function
	| Eof -> "<end of file>"
	| Const c -> s_constant c
	| Kwd k -> s_keyword k
	| Comment s -> "/*"^s^"*/"
	| CommentLine s -> "//"^s
	| Binop o -> s_binop o
	| Unop o -> s_unop o
	| Next -> ";"
	| Sep -> ","
	| BkOpen -> "["
	| BkClose -> "]"
	| BrOpen -> "{"
	| BrClose -> "}"
	| POpen -> "("
	| PClose -> ")"
	| Dot -> "."
	| DblDot -> ":"
	| Question -> "?"
