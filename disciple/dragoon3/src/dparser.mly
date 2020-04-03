%{
open Card

type param =
	| Int of int
	| Float of float
	| String of string
	| Rule of ccapacity
	| Struct of (string * param) list

let rules = ["Attack";"Command";"Captain"]

let make_rule name params =
	let error() =
		failwith ("Invalid params for <"^name^">");
	in
	let get_val s fname =
		try
			snd (List.find (fun (n,_) -> n = fname) s)
		with
			Not_found -> failwith ("Field not found <"^fname^"> in <"^name^">")
	in
	let get_int s fname =
		match get_val s fname with
		| Int i -> i
		| _ -> failwith ("Invalid type for field <"^fname^"> in <"^name^">")
	in
	let get_string s fname =
		match get_val s fname with
		| String s -> s
		| _ -> failwith ("Invalid type for field <"^fname^"> in "^name^">")
	in
	match name, params with
	| "Attack", [Struct s] ->		
		XAttack {
			acoef = get_int s "coef";
			apower = get_int s "power";
			adist = get_int s "dist";
			amindist = get_int s "mindist";
			aanim = get_string s "anim";
		}
	| "Command", [Int n] -> XCommand n
	| "Captain", [] -> XCaptain
	| _ ->
		try
			ignore(List.find ((=) name) rules);
			error()
		with
			Not_found -> failwith ("Unknown rule <"^name^">")

%}

%token EOF
%token <string> CONS 
%token <string> IDENT
%token <float> FLOAT
%token <int> INT
%token <string> STRING
%token BOPEN
%token BCLOSE
%token POPEN
%token PCLOSE
%token NEXT
%token END
%token EQUAL

%start capacities
%type <'a> capacities
%%

capacities:
	| rules EOF	{ $1 }
;
rules:
	| rule END rules { $1::$3 }
	| rule { [$1] }
	| { [] }
;
rule:
	| CONS POPEN params PCLOSE { make_rule $1 $3 }
	| CONS params { make_rule $1 $2 }	
;
params:
	| param NEXT params { $1::$3 }
	| param { [$1] }
	| { [] }
;
param:
	| INT		{ Int $1 }
	| FLOAT		{ Float $1 }
	| STRING	{ String $1 }
	| rule		{ Rule $1 }
	| tstruct	{ Struct $1 }
;
tstruct:
	| BOPEN fields BCLOSE { $2 }
;
fields:
	| field END fields { $1::$3 }
	| field { [$1] }
	| { [] }
;
field:
	| IDENT EQUAL param { ($1,$3) }