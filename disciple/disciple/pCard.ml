(* module PCard

 - CLIENT
 - affichage des cartes pendant le jeu

*)

open M3d
open Mtlib
open ICard
open Card

type ctype =
	| CInHand of int

type sprite_id =
	| Background
	| Artwork

type t = {
	pcard : ICard.t;
	pmodel : Card.t;
	ptype : ctype;	
	psprites : (sprite_id * Sprite.t) MList.t;
	pname : Text.t;
	mutable pfilter : bool;
}

exception No_such_pcard_sprite of sprite_id

let alloc_card_z =
	let z_base = ref 0.01 in
	(fun () ->
		z_base := !z_base /. 1.01;
		(!z_base /. 1.001,!z_base,!z_base /. 1.002)
	)

let icard c =
	c.pcard

let card c =
	c.pmodel

let get_sprite c s =
	try
		let (_,sprite) = MList.find (fun (id,_) -> id = s) c.psprites in sprite
	with
		Not_found -> raise (No_such_pcard_sprite s)

let smult (x,y) k =  (x*.k,y*.k)

let set_pos ?(linear=false) c pos size =
	let time = (if linear then 0.2 +. (itof pos) *. 0.1 else 0.2) in
	let height = (snd Constants.card_size) /. 2. in
	let vdest = (-0.72 +. 0.12*.size +. (itof pos)*.0.24*.size,0.7+.height-.height*.size) in
	let bg = get_sprite c Background in
	let art = get_sprite c Artwork in
	Sprite.size bg (smult Constants.card_size size);
	Sprite.size art (smult Constants.art_size size);
	Sprite.dest bg vdest time;
	Sprite.dest art ((fst vdest) +. Constants.art_delta_x *. size , (snd vdest) +. Constants.art_delta_y *. size ) time;
	Text.dest c.pname ((fst vdest) +. Constants.text_delta_x *. size , (snd vdest) +. Constants.text_delta_y *. size ) time

let create_hand ic pos size =
	let cmodel = Card.find ic.ccard in
	let bg = Sprite.attach Constfiles.fCardBackGround Constants.card_bg_zone in
	let art = Sprite.attach (Card.art cmodel) Sprite.all in
	Sprite.size bg (smult Constants.card_size size);
	Sprite.size art (smult Constants.art_size size);
	let card_z,art_z,text_z = alloc_card_z() in
	let vpos = { x = -1.; y = 1.+.(Random.float 0.5); z = 0. } in
	Sprite.pos art {
		x = vpos.x +. Constants.art_delta_x *. size; 
		y = vpos.y +. Constants.art_delta_y *. size; 
		z = art_z;
	};
	Sprite.pos bg { vpos with z = card_z };
	let sprites = [(Background,bg);(Artwork,art)] in
	let tname = Text.create (Data.font()) in
	Text.center tname true;
	Text.string tname (String.capitalize cmodel.cname);
	Text.pos tname {
		x = vpos.x +. Constants.text_delta_x *. size;
		y = vpos.y +. Constants.text_delta_y *. size;
		z = text_z;
	};
	Text.color tname (Color.rgb 0x000000);
	let c = {
		pcard = ic;
		pmodel = cmodel;
		ptype = CInHand pos;
		psprites = MList.from_list sprites;
		pname = tname;
		pfilter = false;
	} in
	set_pos ~linear:true c pos size;
	c

let show c b =
	match b with
	| true ->
		Sprite.fade (get_sprite c Background) Color.full 0.2;
		Sprite.fade (get_sprite c Artwork) Color.full 0.2
	| false ->
		Sprite.fade (get_sprite c Background) Color.zero 0.2;
		Sprite.fade (get_sprite c Artwork) Color.zero 0.2

let filter c b =
	c.pfilter <- b;	
	MList.iter (fun (_,s) -> Sprite.fade s (match b with false -> Color.full | true -> Color.rgb 0x808080) 0.2) c.psprites

let filtered c =
	c.pfilter

let close c =
	MList.iter (fun (_,s) -> Sprite.detach s) c.psprites	

let play c endfunc =
	MList.iter (fun (_,s) -> Sprite.color s (Color.rgb 0xFF0000); Sprite.fade s Color.zero 0.2) c.psprites;
	Text.remove c.pname;
	Events.add_delayed (fun () -> close c; endfunc()) 0.2

let selected c =
	let mx,my = Mouse.pos() in
	let s = get_sprite c Background in
	let px,py = (let v = Sprite.get_pos s in v.x,v.y) in
	let sx,sy = Sprite.get_size s in
	let px2 = px +. sx/.2. in
	let py2 = py +. sy/.2. in
	let px1 = px -. sx/.2. in
	let py1 = py -. sy/.2. in
	(mx >= px1 && my >= py1 && mx <= px2 && my <= py2)