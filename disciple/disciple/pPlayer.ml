(* module PPlayer

 - CLIENT
 - Données des joueurs pendant la partie

*)
open Message
open Mtlib

type t = {
	pid : pid;
	mutable pdeck_count : int;
	mutable ptotal_count : int;
	mutable phand_count : int;
	mutable pgrave : ICard.t MList.t;
}

let create infos =
	{
		pid = infos.gip_pid;
		ptotal_count = infos.gip_deck_count;
		pdeck_count = infos.gip_deck_count;
		phand_count = 0;
		pgrave = MList.empty();
	}

let id p = p.pid

let draw p n =
	p.pdeck_count <- p.pdeck_count-n;
	p.phand_count <- p.phand_count+n

let close p =
	()

let play p card =
	p.phand_count <- p.phand_count - 1;
	MList.add p.pgrave card
	
let mulligan p =	
	p.phand_count <- p.phand_count - 1;
	p.pdeck_count <- p.ptotal_count - p.phand_count