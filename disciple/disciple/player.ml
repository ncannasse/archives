(* module Player

 - SERVER
 - Données d'un joueur pendant la partie

*)
open Message
open Mtlib
open ICard
open Card

type t = {
	pid : pid;
	pfulldeck : Deck.t;
	pdeck : Deck.t;
	phand : Hand.t;
	pfun : client_message -> unit;
	pcreatures : Creature.t MList.t;
}

let create id = 
	let deck = SData.get_deck id in
	let p = {
		pid = id;
		pfulldeck = deck;
		pdeck = Deck.copy deck;
		phand = Hand.empty();
		pcreatures = MList.empty();
		pfun = SData.get_fun id;
	} in
	Deck.shuffle p.pdeck;
	p

let id p = p.pid

let deck_count p = Deck.count p.pdeck

let hand_count p = Hand.count p.phand

let send p msg =
	try
		p.pfun msg
	with
		e -> Run.process_exc "Client" e

let draw p n =
	let cards = Deck.pop p.pdeck n in
	send p (CDrawCards (List.map ICard.id cards));
	Hand.add p.phand cards

let play p c =
	Hand.play p.phand c

let has_card p c =
	Hand.has_card p.phand c

let mulligan p =
	let n = Hand.count p.phand in
	Deck.copy_to p.pfulldeck p.pdeck;
	Deck.shuffle p.pdeck;
	Hand.clear p.phand;
	draw p (n-1);
	n > 1

let add_creature p c =
	MList.add p.pcreatures c

let has_captain p =
	MList.exists (fun c -> is_captain (Creature.card c)) p.pcreatures

