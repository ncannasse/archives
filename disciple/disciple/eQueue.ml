(* module EQueue

 - COMMON
 - Queue d'Evenements

*)
open Mtlib

type 'a t = ('a * int) MList.t

let compare (_,n) (_,n') = compare n' n

let empty = MList.empty

let insert q x t =
	MList.add_sort compare q (x,t)

let move q x t =	
	let rec aux = function
		| [] -> failwith "EQueue.move"
		| elt::l when fst elt = x -> l
		| elt::l -> elt::(aux l)
	in
	MList.copy_list q (aux (MList.to_list q));
	insert q x t
	
let time q t =
	match MList.to_list q with
	 | [] -> 0
	 | (_,t)::_ -> t

let pop q =
	try
		let x,dt = MList.pop q in
		MList.copy q (MList.map (fun (x,t) -> (x,t-dt)) q);
		x
	with
		Invalid_operation -> failwith "EQueue.pop"

let max q =
	let rec aux m = function
		| [] -> m
		| (_,t)::l when m >= t -> aux m l
		| (_,t)::l -> aux t l
    in
	aux 0 (MList.to_list q)

let iter = MList.iter
	