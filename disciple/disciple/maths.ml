(* module Maths

 - COMMON
 - Formule mathématiques ( 2D / 3D )

*)

(***** POINTS / LINES ********************************************************)

let plen (x,y) =
	sqrt( x *. x +. y *. y)

let pdist (ax,ay) (bx,by) =
	plen (ax -. bx , ay -. by)

let plproj (ax,ay) (bx,by) (px,py) =
	let dx,dy = bx -. ax , by -. ay and
	ux , uy = px -. ax , py -. ay in
	let lmag = plen (dx,dy) in	
	let umag = (ux *. dx +. uy*. dy) /. (lmag *. lmag) in		
	(ax +. umag *. dx, ay +. umag *. dy)

let pldist (ax,ay) (bx,by) (px,py) =
	let dx,dy = bx -. ax , by -. ay and
	ux , uy = px -. ax , py -. ay in
	let lmag = plen (dx,dy) in
	let umag = (ux *. dx +. uy*. dy) /. (lmag *. lmag) in
	let ix,iy = ax +. umag *. dx -. px , ay +. umag *. dy -. py in
	(plen (ix,iy)) *. (if umag < 0. || umag > 1. then -1. else 1.)

(***** POLYGONS **************************************************************)

exception Invalid_polygon

type polygon = (float * float) list

let calc_polygon points delta =
	let rec aux x = function
		| a::b::c::l ->
			let c' = plproj a b c in
			if pdist c c' < 10. then
				aux c (a::c'::l)
			else
				a::(aux c (x::c::l))
		| [a;b] -> [a;x]
		| _ -> assert false
	in
	match points with
	| [] | [_] -> raise Invalid_polygon
	| a::b::l -> aux b points

let polygon_length p =
	let rec aux = function
		| a::b::l ->
			let len = pdist a b in
			len +. (aux (b::l))
		| _ -> 
			0.
	in
	match p with
	| [] | [_] -> raise Invalid_polygon
	| _ -> aux p

let polygon_start p = List.hd p

let polygon_next = function
	| [] | [_] -> raise Invalid_polygon
	| _::l -> l

let rec polygon_end = function
	| [] -> raise Invalid_polygon
	| [x] -> x
	| _::l -> polygon_end l

(***** ANGLES *****************************************************************)

let pi = 3.14159265359 (* ~> pi *)

let ang_norm a =
	let n = (float_of_int (int_of_float (a /. pi)))  in
	a -. n *. 2. *. pi

let ang_sub a b =
	match ang_norm (a-.b) with
	| x when x < 0. -> x *. -1.
	| x -> x
