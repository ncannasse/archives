{
open Lexing
open Types

type error_msg =
	| Invalid_character of char
	| Unterminated_string
	| Unclosed_comment

exception Error of error_msg * int

let error_msg = function
	| Invalid_character c -> Printf.sprintf "Invalid character 0x%.2X" (int_of_char c)
	| Unterminated_string -> "Unterminated string"
	| Unclosed_comment -> "Unclosed comment"

let keywords =
	let h = Hashtbl.create 3 in
	List.iter (fun k -> Hashtbl.add h (s_kwd k) k)
		[If;Else;For;While;Do;Break;Function;Return;Var;Primitive;Import;Class;New;Switch;Case;Default];
	h

let buf = Buffer.create 100
let current_file = ref ""
let lines = Hashtbl.create 0

let init file =
	current_file := file;
	Hashtbl.add lines file []

let save () =
	!current_file

let restore file =
	current_file := file

let get_current_file() =
	!current_file

let newline lexbuf =
	Hashtbl.replace lines !current_file ((lexeme_end lexbuf) :: (Hashtbl.find lines !current_file))

let get_lines file = 
	try
		List.rev (Hashtbl.find lines file)
	with
		Not_found -> []

let find_line p lines =
	let rec loop n delta = function
		| [] -> n , p - delta
		| lp :: l when lp > p -> n , p - delta
		| lp :: l -> loop (n+1) lp l
	in
	loop 1 0 lines

let get_error_pos p =
	let ll = get_lines p.pfile in
	let l1, p1 = find_line p.pmin ll in
	let l2, p2 = find_line p.pmax ll in
	if l1 = l2 then begin
		let s = (if p1 = p2 then Printf.sprintf " %d" p1 else Printf.sprintf "s %d-%d" p1 p2) in
		Printf.sprintf "line %d character%s" l1 s
	end else
		Printf.sprintf "lines %d-%d" l1 l2

let reset() = Buffer.reset buf
let contents() = Buffer.contents buf
let store lexbuf = Buffer.add_string buf (lexeme lexbuf)
let add c = Buffer.add_char buf c

let mk_tok t pmin pmax =
	t , { pfile = !current_file; pmin = pmin; pmax = pmax }

let mk lexbuf t = 
	mk_tok t (lexeme_start lexbuf) (lexeme_end lexbuf)

let mk_ident lexbuf =
	match lexeme lexbuf with
	| "null" -> mk lexbuf (Const Null)
	| "true" -> mk lexbuf (Const (Int 1))
	| "false" -> mk lexbuf (Const (Int 0))
	| s ->
		mk lexbuf (try Kwd (Hashtbl.find keywords s) with Not_found -> Const (Ident s))

let hex_convert lexbuf =
	let s = lexeme lexbuf in
	let number = ref 0 in
	for i = 2 to String.length s - 1 do
		let n = match s.[i] with
				| '0'..'9' -> int_of_char s.[i] - int_of_char '0'
				| 'a'..'f' -> int_of_char s.[i] - int_of_char 'a' + 10
				| 'A'..'F' -> int_of_char s.[i] - int_of_char 'A' + 10
				| _ -> assert false
		in
		number := (!number lsl 4) lor n;
	done;
	!number
}

let ident = ['_' 'a'-'z' 'A'-'Z' '$'] ['_' 'a'-'z' 'A'-'Z' '0'-'9']*

rule token = parse
	| eof { mk lexbuf Eof }
	| [' ' '\r' '\t']+ { token lexbuf } 
	| '\n' { newline lexbuf; token lexbuf }
	| "0x" ['0'-'9' 'a'-'f' 'A'-'F']+ { mk lexbuf (Const (Int (hex_convert lexbuf))) }
	| ['0'-'9']+ { mk lexbuf (Const (Int (int_of_string (lexeme lexbuf)))) }
	| ['0'-'9']+ '.' ['0'-'9']* { mk lexbuf (Const (Float (lexeme lexbuf))) }
	| "#include" { mk lexbuf (Kwd Include) }
	| ident { mk_ident lexbuf }
	| "//" [^'\n']*  {
			let s = lexeme lexbuf in
			let n = (if s.[String.length s - 1] = '\r' then 3 else 2) in
			mk lexbuf (CommentLine (String.sub s 2 ((String.length s)-n)))
		}
	| "/*" {
			reset();
			let pmin = lexeme_start lexbuf in
			let pmax = (try comment lexbuf with Exit -> raise (Error (Unclosed_comment,pmin))) in
			mk_tok (Comment (contents())) pmin pmax;
		}
	| '"' {
			reset();
			let pmin = lexeme_start lexbuf in
			let pmax = (try string lexbuf with Exit -> raise (Error (Unterminated_string,pmin))) in
			mk_tok (Const (String (contents()))) pmin pmax;
		}
	| '\'' ['a'-'z' 'A'-'Z' '0'-'9' '_' '-']+ {
			let s = lexeme lexbuf in
			mk lexbuf (Res (String.sub s 1 (String.length s - 1)))
		}
	| "!=" { mk lexbuf (Op Neq) }
	| "==" { mk lexbuf (Op Eq) }
	| ">" { mk lexbuf (Op Gt) }
	| "<" { mk lexbuf (Op Lt) }
	| ">=" { mk lexbuf (Op Gte) }
	| "<=" { mk lexbuf (Op Lte) }

	| "++" { mk lexbuf (Unop UIncr) }
	| "--" { mk lexbuf (Unop UDecr) }
	| "!" { mk lexbuf (Unop Not) }

	| "<<" { mk lexbuf (Op Shl) }
	| ">>" { mk lexbuf (Op Shr) }
	| "|" { mk lexbuf (Op Or) }
	| "&" { mk lexbuf (Op And) }
	| "^" { mk lexbuf (Op Xor) }
	| "&&" { mk lexbuf (Op BAnd) }
	| "||" { mk lexbuf (Op BOr) }
	| "+" { mk lexbuf (Op Add) }
	| "-" { mk lexbuf (Op Sub) }
	| "*" { mk lexbuf (Op Mult) }
	| "/" { mk lexbuf (Op Div) }
	| "%" { mk lexbuf (Op Mod) }

	| "=" { mk lexbuf (Op Assign) }
	| ":=" { mk lexbuf (Op AssignRef) }
	| "+=" { mk lexbuf (Op (AssignOp Add)) }
	| "-=" { mk lexbuf (Op (AssignOp Sub)) }
	| "*=" { mk lexbuf (Op (AssignOp Mult)) }
	| "/=" { mk lexbuf (Op (AssignOp Div)) }
	| "|=" { mk lexbuf (Op (AssignOp Or)) }
	| "&=" { mk lexbuf (Op (AssignOp And)) }
	| "^=" { mk lexbuf (Op (AssignOp Xor)) }
	| "%=" { mk lexbuf (Op (AssignOp Mod)) }
	| "<<=" { mk lexbuf (Op (AssignOp Shl)) }
	| ">>=" { mk lexbuf (Op (AssignOp Shr)) }

	| "(" { mk lexbuf POpen }
	| ")" { mk lexbuf PClose }
	| "," { mk lexbuf Sep }
	| ";" { mk lexbuf Next }
	| "{" { mk lexbuf BOpen }
	| "}" { mk lexbuf BClose }
	| "[" { mk lexbuf BkOpen }
	| "]" { mk lexbuf BkClose }
	| "." { mk lexbuf Dot }
	| ":" { mk lexbuf DblDot }
	| "?" { mk lexbuf Question }
	| _ { raise (Error (Invalid_character (lexeme_char lexbuf 0),lexeme_start lexbuf)) }

and comment = parse
	| eof { raise Exit }
	| '\r' { comment lexbuf }
	| '\n' { newline lexbuf; store lexbuf; comment lexbuf }
	| "*/" { lexeme_end lexbuf }
	| '*' { comment lexbuf }
	| [^'*' '\n' '\r']+ { store lexbuf; comment lexbuf }

and string = parse
	| eof { raise Exit }
	| '\r' { string lexbuf }
	| '\n' { newline lexbuf; store lexbuf; string lexbuf }
	| "\\n" { add '\n'; string lexbuf }
	| "\\t" { add '\t'; string lexbuf }
	| "\\r" { add '\r'; string lexbuf }
	| "\\\\" { add '\\'; string lexbuf }
	| "\\\"" { add '"'; string lexbuf }
	| "\\" [^'\\'] { add (lexeme_char lexbuf 1); string lexbuf }
	| '"' { lexeme_end lexbuf }
	| [^'"' '\\' '\r' '\n']+ { store lexbuf; string lexbuf }
