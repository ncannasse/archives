(* module PHand

 - CLIENT
 - affichage de la main d'un joueur

*)
open Message
open Mtlib
open ICard
open M3d

type t = {
	hhand : Hand.t;
	hcards : PCard.t MList.t;
	mutable henable : bool;
}

let empty() = 
	{
		hhand = Hand.empty();
		hcards = MList.empty();
		henable = false;
	}

let count h = Hand.count h.hhand

let calc_size h =
	match count h with
	| l when l <= 7 -> 1.0
	| l -> 7. /. (itof l)

let add h cards =
	Hand.add h.hhand cards;
	let n = ref (MList.length h.hcards) in
	let new_size =
		match !n + (List.length cards) with
		| l when l <= 7 -> 1.0
		| l -> 7. /. (itof l)
	in
	let p = ref 0 in
	MList.iter (fun c -> PCard.set_pos c !p new_size; incr p) h.hcards;
	List.iter (fun c -> MList.add h.hcards (PCard.create_hand c !n new_size); incr n) cards

let close h =
	MList.iter PCard.close h.hcards

let filter h pred =
	MList.iter (fun c -> PCard.filter c (pred (PCard.icard c))) h.hcards

let cfilter h pred =
	MList.iter (fun c -> PCard.filter c (pred (PCard.card c))) h.hcards		

let unfilter h =
	MList.iter (fun c -> PCard.filter c false) h.hcards

let enable h e =
	h.henable <- e

let play h card =	
	Hand.play h.hhand card;
	try	
		let c = MList.find (fun p -> PCard.icard p = card) h.hcards in
		MList.remove h.hcards c;
		PCard.play c (fun () ->
			let p = ref (-1) in
			let size = calc_size h in
			MList.iter (fun c -> incr p; PCard.set_pos c !p size) h.hcards;
		);
	with
		Not_found -> raise (Hand.No_such_card card.cid)

let show h b =
	let size = calc_size h in
	h.henable <- b;
	match b with 
	| true ->
		let pos = ref (-1) in
		MList.iter (fun c ->
			incr pos;
			PCard.set_pos c !pos size;
			PCard.show c b
		) h.hcards
	| false ->
		MList.iter (fun c ->
			PCard.set_pos c 0 size;
			PCard.show c b
		) h.hcards

let selected h =
	try
		let c = MList.find PCard.selected h.hcards in
		Some c
	with
		Not_found -> None

let clear h =
	Hand.clear h.hhand;
	MList.iter PCard.close h.hcards;
	MList.clear h.hcards
	
