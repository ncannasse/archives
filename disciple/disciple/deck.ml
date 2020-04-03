(* module Deck

 - SERVER
 - Deck d'un joueur (pendant et en dehors de la partie)

*)
open Message
open Mtlib 

type t = ICard.t MList.t

exception No_more_cards

let empty() = MList.empty()

let is_empty deck =
	match MList.to_list deck with
	| [] -> true
	| _ -> false

let count deck = MList.length deck

let fill deck cards pid =
	let len = List.length cards in
	if len < Constants.min_deck_count || len > Constants.max_deck_count then raise (SError (EInvalidDeck "Invalid Card Number"));	
	MList.copy_list deck (List.map (ICard.load pid) cards)

let shuffle deck =
	MList.shuffle deck	

let copy deck =
	MList.from_list (MList.to_list deck)

let copy_to from dto =
	MList.copy_list dto (MList.to_list from)

let pop deck n =
	try
		MList.npop deck n
	with
		Invalid_operation -> raise No_more_cards
