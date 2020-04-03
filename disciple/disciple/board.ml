(* module Board

 - COMMON
 - gestion du terrain de jeu

*)
open Mtlib
open ICard
open Card

type t = {
	pwidth : float;
	pheight : float;
	pcreatures : Creature.t MList.t;
}

type boardpos =
	| POutBoard
	| PCollide of Creature.t list

exception Invalid_position
exception No_such_creature of icid

let empty w h =
	{
		pwidth = w;
		pheight = h;
		pcreatures = MList.empty();
	}

let is_valid_pos b (x,y) =
	x >= b.pwidth *. -1. && y >= b.pheight *. -1. && x <= b.pwidth && y <= b.pheight

let on_board b p size =
	match is_valid_pos b p with
	| false -> POutBoard
	| true ->
		PCollide (List.filter (fun c -> Creature.collide c p size) (MList.to_list b.pcreatures))

let ray_intersect b p p' size =
	let l = List.filter (fun c ->
			let pos = Creature.pos c in
			match pos <> p with
			| false -> false
			| true ->
				let cdist = Maths.pldist p p' pos in
				cdist >= 0. && cdist +. 10. < itof ((Creature.size c) + size))
				(MList.to_list b.pcreatures) in
	PCollide l

let invoke b icard p =
	let card = Card.find icard.ccard in
	match card.ctype with
	| Creature c ->
		match on_board b p c.csize with
		| PCollide [] ->
			let c = Creature.invoke icard p in
			MList.add b.pcreatures c;
			c
		| _ ->
			raise Invalid_position

let rec is_valid_path b from poly size cid =
	let p = Maths.polygon_start poly in
	let next() =
		try
			is_valid_path b p (Maths.polygon_next poly) size cid
		with
			Maths.Invalid_polygon -> true
	in
	let test() =
		match ray_intersect b from p size with
		| PCollide [] -> next()
		| PCollide [c] when Creature.id c = cid -> next()
		| PCollide _ | POutBoard ->
			M3d.Render.print "here@1";
			false
	in
	match on_board b p size with
	| PCollide [] -> test()
	| PCollide [c] when Creature.id c = cid -> test()
	| PCollide _ | POutBoard ->
		M3d.Render.print "here@2";
		false
		


let creature b cid =
	try
		MList.find (fun c -> Creature.id c = cid) b.pcreatures
	with
		Not_found -> raise (No_such_creature cid)

let command_calc b =
	let cur = ref 0 and max = ref 0 in
	MList.iter (fun c ->
		cur += Creature.cmd_consume c;
		max += Creature.cmd_add c;
	) b.pcreatures;
	(!cur,!max)
