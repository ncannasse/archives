
(* -------------- common definitions ----------- *)

type pos = {
	pfile : string;
	pmin : int;
	pmax : int;
}

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
	| OpUShr
	| OpMod
	| OpAssignOp of binop

type unop =
	| Increment
	| Decrement
	| Not
	| Neg
	| NegBits

type unop_flag =
	| Prefix
	| Postfix

type while_flag =
	| NormalWhile
	| DoWhile

type static_flag = 
	| IsMember
	| IsStatic
	| IsVolatile

type type_path = string list * string

(* -------------- type definition ------------- *)

type variance =
	| Constraint
	| Upper
	| Lower

type t_field = {
	f_name : string;
	f_type : t;
}

and t_class = {
	mutable c_fields : t_field list;
	c_name : type_path option;
	c_id : int;
}

and t_object = {	
	o_inf : t_class;
	o_sup : t_class option;
}

and t_function = {
	f_argnames : string list;
	f_args : t list;
	f_ret : t;
}

and type_def =
	| TLink of t
	| TLazy of (unit -> unit)
	| TObject of t_object
	| TFunction of t_function
	| TPoly
	| TMono
	| TTemplate of string
	| TAbbrev of type_path * t list * variance
	| TInt
	| TFloat
	| TNumber
	| TBool
	| TVoid

and t = {
	mutable t : type_def;
	mutable id : int
}

and ttype = t

(* ---------------- typed tree ---------------- *)

type tconstant =
	| TConstInt of string
	| TConstFloat of string
	| TConstString of string
	| TConstBool of bool
	| TConstNull
	| TConstThis

type class_property = 
	| PNative
	| PInterface
	| PEnum

type class_field = {
	cf_name : string;
	cf_type : t;
	cf_static : bool;
	cf_volatile : bool;
	mutable cf_expr : texpr option;
}

and class_context = {
	c_path : type_path;
	c_params : (string * t) list;
	mutable c_super : (class_context * t list) option;
	mutable c_interfaces : (class_context * t list) list;
	mutable c_class_fields : class_field list;
	c_style : class_property list;
	c_inst : t_class;
	c_class : t_class;
	c_pos : pos;
}

and builtin =
	| Cast of texpr
	| Upcast of texpr
	| Downcast of texpr
	| Trace of texpr * string * int
	| ToInt of texpr
	| ToString of texpr
	| PrintType of texpr
	| Callback of texpr * string * texpr list
	| SuperConstructor

and texpr_def =
	| TVars of (string * t * texpr option) list
	| TBlock of texpr list
	| TFor of texpr list * texpr list * texpr list * texpr
	| TIf of texpr * texpr * texpr option
	| TWhile of texpr * texpr * while_flag
	| TSwitch of texpr * (texpr * texpr) list * texpr option
	| TTry of texpr * string * type_path option * texpr
	| TReturn of texpr option
	| TBreak
	| TContinue
	| TConst of tconstant
	| TArray of texpr * texpr
	| TBinop of binop * texpr * texpr
	| TField of texpr * string
	| TLocal of string
	| TMember of string * class_context
	| TClass of class_context
	| TParenthesis of texpr
	| TObjDecl of (string * texpr) list
	| TArrayDecl of texpr list
	| TCall of texpr * texpr list
	| TNew of class_context * t list * texpr list
	| TUnop of unop * unop_flag * texpr
	| TLambda of (string * t) list * texpr * t
	| TBuiltin of builtin
	| TDiscard of texpr

and texpr = {
	texpr : texpr_def;
	ttype : t;
	tpos : pos;
}

