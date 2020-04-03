(* module PEvent

 - client
 - gestion & affichage des Evenements

*)
open M3d
open Message
open Mtlib
open ICard
open Card

type t = {
	eevent : event;
	esprite : Sprite.t;
}

let pos t =
	let rel_t = (itof t) /. 500. in
	let abs_t = -0.9+.(rel_t*.1.8) in
	abs_t

let emit e time =
	let s = match e with
		| EInvokeCaptains ->
			Sprite.attach Constfiles.fEventArt_Invoke Sprite.all
		| EDrawPhase ->
			Sprite.attach Constfiles.fEventArt_Draw Sprite.all
		| EPlayCreature id ->
			let cmodel = Card.find (ICard.find id).ccard in
			Sprite.attach (Card.art cmodel) Sprite.all
	in
	Sprite.size s Constants.event_size;
	Sprite.color s Color.zero;
	Sprite.fade s Color.full 0.2;
	Sprite.pos s { Constants.events_pos with x = pos time;  };
	{
		eevent = e;
		esprite = s;
	}

let evt e = e.eevent

let update e t =
	Sprite.dest e.esprite ((pos t),Constants.events_pos.y) 0.3

let select e =
	Sprite.dest e.esprite Constants.first_event_pos 0.3;
	Events.add_delayed (fun () -> Sprite.dest e.esprite Constants.current_event_pos 0.2) 0.3

let close e =
	Sprite.detach e.esprite

let destroy e =
	Sprite.fade e.esprite Color.zero 0.3;
	Events.add_delayed  (fun () -> close e) 0.3

