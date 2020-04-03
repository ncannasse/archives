(* module PCreature

 - CLIENT
 - affichage et gestion des creatures

*)
open Mtlib
open M3d
open ICard
open Card

type canim =
	| AWait
	| AWalk

type t = {
	pcreature : Creature.t;
	pmodel : Inst.t;
	pfilter : Inst.t;
	mutable panim : canim;
	mutable pangle : float;
}

(* for custom call to get Creature with an Inst *)
let current_creature = Global.empty "Current Creature"

let get_anim c = function
	| AWait -> (Creature.infos c.pcreature).canimbcl
	| AWalk -> (Creature.infos c.pcreature).canimmove

let play_anim c a =
	if c.panim <> a then begin
		c.panim <- a;
		Inst.anim c.pmodel (Some (get_anim c a));
	end

let filter c color =
	Inst.color c.pfilter Color.zero;
	Inst.fade c.pfilter color 0.2;
	Inst.visible c.pfilter true

let unfilter c =
	Inst.fade c.pfilter Color.zero 0.2;
	Events.add_delayed (fun () -> Inst.visible c.pfilter false) 0.2

let id c = 
	Creature.id c.pcreature

let angle c =
	c.pangle

let update_pos c pos ang =
	c.pangle <- ang;
	Inst.pos c.pmodel (Matrix.trs
		{ x = fst pos; y = snd pos; z = 0. }
		{ x = 0.; y = 0.; z = ang }
		{ x = 1.; y = 1.; z = 1. }
	)

let from_model inst =
	Inst.do_action inst;
	let o = Global.opt current_creature in
	Global.undef current_creature;
	o

let invoke c = 
	let (x,y) = Creature.pos c in
	let infos = Creature.infos c in
	let filter = Inst.attach Constfiles.fSelectionModel in
	FX.color_op (Inst filter) Top_Modulate;
	FX.alpha_op (Inst filter) Top_Modulate;
	FX.src_blend (Inst filter) Blend_SrcAlpha;
	FX.dst_blend (Inst filter) Blend_InvSrcAlpha;
	Inst.visible filter false;
	let model = Inst.attach infos.cmodel in
	Inst.color model Color.zero;
	let pos = { x =  x ; y = y; z = 0. } in
	let size = (itof infos.csize) /. 50. in
	Render.update_time();
	Inst.pos model (Matrix.trans pos);
	Inst.pos filter (Matrix.trs { pos with z = 0.15 } { x = 0.; y = 0.; z = 0. } { x = size; y = size; z = size;} );
	Inst.fade model Color.full 0.3;
	let c = {
		pcreature = c;
		pmodel = model;
		pfilter = filter;
		panim = AWalk;
		pangle = 0.;
	} in
	Inst.action model (fun () -> Global.set current_creature c);
	play_anim c AWait;
	c

let rec move_to ?(play=true) c path callback =
	if not play then begin
		Creature.move_to c.pcreature (Maths.polygon_end path);
		callback();
	end else
		let speed = 1.0 in
		let (x,y) = Maths.polygon_start path in
		let pi = 3.1416 in
		let infos = Creature.infos c.pcreature in
		let cx,cy = Creature.pos c.pcreature in
		let dist = Maths.pdist (cx,cy) (x,y) in
		let ang = pi -. (atan2 (x-.cx) (y-.cy)) in
		let time = (dist /. (itof infos.cmovespeed)) /. speed in
		let rtime = (match Maths.ang_sub c.pangle ang with x when x < pi /. 2. -> 0. | x -> x /. 7. ) in
		let size = (itof infos.csize) /. 50. in
		Inst.speed c.pmodel 1.0;
		let move() =
			play_anim c AWalk;
			c.pangle <- ang;
			Inst.speed c.pmodel speed;
			Inst.dest c.pmodel (Matrix.trs { x = x; y = y; z = 0. } { x = 0.; y = 0.; z = ang } { x = 1.; y = 1.; z = 1.;} ) time
		in
		if rtime > 0. then begin
			play_anim c AWait;
			Inst.pos c.pfilter (Matrix.trs { x = x; y = y; z = 0.15 } { x = 0.; y = 0.; z = 0. } { x = size; y = size; z = size;} );
			Inst.dest c.pmodel (Matrix.trs { x = cx; y = cy; z = 0. } { x = 0.; y = 0.; z = ang } { x = 1.; y = 1.; z = 1.;} ) rtime;
			Events.add_delayed move rtime;
		end else
			move();
		Events.add_delayed (fun () ->
			Creature.move_to c.pcreature (x,y);
			try				
				move_to ~play:true c (Maths.polygon_next path) callback
			with
				Maths.Invalid_polygon ->
					play_anim c AWait;
					callback()
		) (time +. rtime)

let close c =
	Inst.detach c.pmodel;
	Inst.detach c.pfilter

let data c =
	c.pcreature

let active_actions c =
	(** TODO : "display actions icons" **)
	()
