(* module PCreature

 - COMMON
 - gestion des creatures

*)
open Mtlib
open ICard
open Card
open Message

type t = {
	picard : ICard.t;
	pcard : Card.t;
	pcreature : Card.ccreature;
	mutable pcontroler : pid;
	mutable ppos : (float * float);
}

let id c =
	c.picard.cid

let zone c =
	c.pcreature.csize

let collide c p size =
	let d = Maths.pdist c.ppos p in
	d < itof (c.pcreature.csize + size)

let invoke ic (x,y) = 
	let card = Card.find ic.ccard in
	match card.ctype with
	| Creature c ->
		let size = (itof c.csize) /. 50. in
		let c = {
			picard = ic;
			pcard = card;
			pcreature = c;
			pcontroler = ICard.owner ic.cid;
			ppos = (x,y);
		} in
		c

let move_to c p =
	c.ppos <- p

let controler c = c.pcontroler

let card c = c.pcard

let infos c = c.pcreature

let pos c = c.ppos

let size c = c.pcreature.csize

let cmd_consume c =
	c.pcreature.clevel

let cmd_add c =
	let rec aux = function
		| [] -> 0
		| (XCommand x)::_ -> x
		| _::l -> aux l
	in
	aux c.pcreature.ccapacities
