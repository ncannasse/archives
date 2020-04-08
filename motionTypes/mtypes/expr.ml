open Type

type keyword =
	| Function
	| Class
	| Var
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
	| Static
	| Native
	| Try
	| Catch

type constant =
	| Int of string
	| Float of string
	| String of string
	| Ident of string
	| Name of string

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
	| Arrow
	| Quote of string

type type_std =
	| TypeVoid
	| TypeInt
	| TypeFloat
	| TypeBool

type type_decl = 
	| TypePath of type_path * type_decl list
	| TypeStd of type_std
	| TypeFun of type_decl * type_decl	
	| TypePoly of string
	| TypeFields of (string * type_decl) list * type_decl option

type func = {
	fname : string;
	fargs : (string * type_decl option) list;
	ftype : type_decl option;
	fstatic : static_flag;
	fexpr : expr option;
}

and herit = 
	| HExtends of type_path * type_decl list
	| HImplements of type_path * type_decl list
	| HNative

and eval_def =
	| EConst of constant
	| EArray of eval * eval
	| EBinop of binop * eval * eval
	| EField of eval * string
	| EStatic of type_path
	| EParenthesis of eval
	| EObjDecl of (string * eval) list
	| EArrayDecl of eval list
	| ECall of eval * eval list
	| ENew of type_path * type_decl list * eval list
	| EUnop of unop * unop_flag * eval
	| EQuestion of eval * eval * eval
	| ELambda of func

and eval = eval_def * pos

and expr_def =
	| EClass of type_path * string list * herit list * expr
	| EInterface of type_path * string list * expr
	| EVars of static_flag * (string * type_decl option * eval option) list
	| EFunction of func
	| EBlock of expr list
	| EFor of expr list * eval list * eval list * expr
	| EIf of eval * expr * expr option
	| EWhile of eval * expr * while_flag
	| ESwitch of eval * (eval * expr) list * expr option
	| ETry of expr * string * type_path option * expr
	| EReturn of eval option
	| EBreak
	| EContinue
	| EVal of eval
	| EImport of type_path

and expr = expr_def * pos

let pos = snd

let is_postfix = function
	| Increment | Decrement -> true
	| Not | Neg | NegBits -> false

let is_prefix = function
	| Increment | Decrement -> true
	| Not | Neg | NegBits -> true

let base_class_name = snd

let null_pos = { pfile = "<null>"; pmin = -1; pmax = -1 }

let punion p p2 =
	{
		pfile = p.pfile;
		pmin = min p.pmin p2.pmin;
		pmax = max p.pmax p2.pmax;
	}

let s_type_path (p,s) = match p with [] -> s | _ -> String.concat "." p ^ "." ^ s

let rec s_type_decl = function 
	| TypePath (t,[]) -> s_type_path t
	| TypePath (t,p) -> s_type_path t ^ "<" ^ String.concat "," (List.map s_type_decl p) ^ ">"
	| TypeFun (t1,t2) -> s_type_decl t1 ^ " -> " ^ s_type_decl t2
	| TypePoly s -> s
	| TypeFields (fields,Some t) ->
		let s = String.concat ", " (List.map (fun (f,t) -> f ^ " : " ^ s_type_decl t) fields) in
		"{> " ^ s_type_decl t ^ " " ^ s ^ " }"
	| TypeFields (fields,None) ->
		"{ " ^ (String.concat ", " (List.map (fun (f,t) -> f ^ " : " ^ s_type_decl t) fields)) ^ " }"
	| TypeStd t ->
		match t with
		| TypeVoid -> "void"
		| TypeInt -> "int"
		| TypeFloat -> "float"
		| TypeBool -> "bool"

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
	| Name s -> s
	
let s_keyword = function
	| Function -> "function"
	| Class -> "class"
	| Static -> "static"
	| Var -> "var"
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
	| Native -> "native"
	| Try -> "try"
	| Catch -> "catch"

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
	| OpUShr -> ">>>"
	| OpShl -> "<<"
	| OpMod -> "%"
	| OpAssignOp op -> s_binop op ^ "="

let s_unop = function
	| Increment -> "++"
	| Decrement -> "--"
	| Not -> "!"
	| Neg -> "-"
	| NegBits -> "~"

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
	| Arrow -> "->"
	| Quote s -> s 
