(* module Hand

 - COMMON
 - gestion de la main d'un joueur

*)
open Message
open Mtlib
open ICard

type t = ICard.t MList.t

exception No_such_card of icid

let empty() = MList.empty()

let count h = MList.length h

let add h cards =
	List.iter (MList.add h) cards

let play h c =
	try
		MList.remove h c;
	with
		Not_found -> raise (No_such_card c.cid)

let has_card h c =
	MList.exists ((=) c) h

let clear h =
	MList.clear h