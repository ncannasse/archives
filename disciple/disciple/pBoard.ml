(* module PBoard

 - CLIENT
 - affichage et gestion du terrain de jeu

*)
open Message
open Mtlib
open M3d

type t = {
	pcreatures : PCreature.t MList.t;
	pmodel : Inst.t;
	pboard : Board.t;
}


exception Select_creature of PCreature.t
exception No_such_creature of icid

let empty w h =
	{
		pcreatures = MList.empty();
		pmodel = Inst.attach Constfiles.fBoardModel;
		pboard = Board.empty w h;
	}

let board b = b.pboard

let filter_creatures b f =
	MList.iter (fun c -> 
		match f c with
		| None -> PCreature.unfilter c 
		| Some color -> PCreature.filter c color
	) b.pcreatures

let unfilter_creatures b =
	MList.iter PCreature.unfilter b.pcreatures

let invoke b card pos =
	let c = Board.invoke b.pboard card pos in
	MList.add b.pcreatures (PCreature.invoke c)

let creature b id =
	try
		MList.find (fun c -> PCreature.id c = id) b.pcreatures
	with
		Not_found -> raise (No_such_creature id)

let process b =
	if Mouse.click() then
		match Render.pick (Mouse.pos()) with
		| [] -> ()
		| elt::_ ->
			match PCreature.from_model elt with
			| None -> ()
			| Some c -> raise (Select_creature c)
				
let close b =
	MList.iter PCreature.close b.pcreatures;
	Inst.detach b.pmodel