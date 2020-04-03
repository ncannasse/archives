
type t = string list

let windows = (Sys.os_type = "Win32")
let def_sep = (if windows then "\\" else "/")

exception Cannot_up
exception Invalid_path of string

let to_string = function
	| [] -> ""
	| [x] when x == def_sep -> def_sep
	| p ->
		String.concat def_sep (List.rev p) ^ def_sep

let rec normalize = function
	| [] -> []
	| "." :: l -> normalize l
	| ".." :: l as p ->
		(match normalize l with
		| [] -> [".."]
		| [""] -> raise (Invalid_path (to_string p))
		| _ :: l -> l)
	| x :: l ->
		x :: normalize l

let make s =
	let p = ref [] in
	let pos = ref 0 in
	let len = String.length s - 1 in
	for i = 0 to len do
		match String.unsafe_get s i with
		| '/' | '\\' ->
			if i = 0 then
				p := "" :: !p
			else if i == !pos then
				raise (Invalid_path s)
			else
				p := String.sub s !pos (i - !pos) :: !p;
			pos := i+1;
		| _ ->
			()
	done;
	if !pos <> len+1 then p:= String.sub s !pos (len - !pos + 1) :: !p;
	normalize !p

let rec absolute = function
	| [] -> make (Sys.getcwd())
	| [""] when windows ->
		[String.sub (Sys.getcwd()) 0 2] (* get drive letter *)
	| [x] when windows && String.length x = 2 && String.unsafe_get x 1 = ':' ->
		[x]
	| ".." :: l ->
		(match absolute l with
		| [] -> assert false
		| [_] -> raise (Invalid_path (to_string (".."::l))) (* we can't go up if we're at root *)
		| _ :: l -> l)
	| x :: l ->
		x :: absolute l

let relative p1 p2 =
	let rp1 = List.rev (absolute p1) in
	let rp2 = List.rev (absolute p2) in
	match rp1 , rp2 with
	| root1 :: _ , root2 :: _ when root1 <> root2 -> p1
	| p1 , p2 ->
		let rec loop p1 p2 =
			match p1 , p2 with
			| [] , [] -> []
			| x1 :: l1 , x2 :: l2 when x1 = x2 -> loop l1 l2
			| _ , _ ->
				List.fold_left (fun acc _ -> ".." :: acc) (List.rev p1) p2
		in
		loop p1 p2

let up = function
	| [] -> assert false
	| [x] -> raise Cannot_up
	| _ :: l -> l
