{
open Dparser

let get = Lexing.lexeme

let unescape s = 
	let l = String.length s in
	String.sub s 1 (l-2)

}

let space = [' ' '\t' '\r' '\n']

let any = ['a'-'z' 'A'-'Z' '0'-'9' '.' '-' '_']

let ident = ['a'-'z'] any*

let construct = ['A'-'Z'] any*

let tint = ['0'-'9']+

let tfloat = ['0'-'9']+ '.' ['0'-'9']*

let tstring = '"' [^ '"']* '"'
let tstring2 = '\'' [^ '\'']* '\''

rule tags = parse
	| eof		{ EOF }
	| space+	{ tags lexbuf }
	| construct { CONS (get lexbuf) }
	| ident		{ IDENT (get lexbuf) }
	| tfloat	{ FLOAT (float_of_string (get lexbuf)) }
	| tint		{ INT (int_of_string (get lexbuf)) }
	| tstring	{ STRING (unescape (get lexbuf)) }
	| tstring2	{ STRING (unescape (get lexbuf)) }
	| '{'		{ BOPEN }
	| '}'		{ BCLOSE }
	| '('		{ POPEN }
	| ')'		{ PCLOSE }
	| ','		{ NEXT }
	| ';'		{ END }	
	| '='		{ EQUAL }
	| _			{ raise Parsing.Parse_error }