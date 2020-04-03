(* module Card

 - COMMON
 - Gestion des informations des cartes

*)
open Mtlib
open Message

type cid = Message.cid

type essence = 
	| EIce
	| EFire
	| EVoid
	| EVegetal
	| ELight

type cattack = {
	acoef : int;
	apower : int;
	adist : int;
	amindist : int;
	aanim : string;
}

type ccapacity =
	| XAttack of cattack
	| XCommand of int
	| XCaptain

type ccreature = {
	clevel : int;
	cessence : essence;
	clife : int;
	cmovement : int;
	cspeed : int;
	csize : int;
	cmodel : Vfs.fid;
	canimbcl : string;
	canimmove : string;
	cmovespeed : int;
	ccapacities : ccapacity list;
}

type ctype =
	| Creature of ccreature

type t = {
	cname : string;
	ctype : ctype;
}

exception No_such_card of cid

let dbase = MBTree.empty()

let ctype_strings = ["Creature"]	
let essence_strings = ["Ice";"Fire";"Limb";"Vegetal";"Light"]
let ftype = Vfs.get_ftype "DCA"

let save id (script,card) =	
	Vfs.write id (script,(match card with
		| Creature data -> Creature { data with cmodel = safe_cast (Vfs.add_dep id data.cmodel) }
	))

let load id =
	let (script,c) = Vfs.read id in
	(script,
		match c with
		| Creature data -> Creature { data with cmodel = Vfs.get_dep id (safe_cast data.cmodel) }
	)

let get id =
	try
		snd(MBTree.find dbase id)
	with
		Not_found -> raise (No_such_card id)

let find id =
	let c = get id in
	{
		cname = Vfs.name_of_file id;
		ctype = c;
	}

let art c =
	Vfs.find_hlink_local (Vfs.get_root()) (c.cname^"_art.png")

let load_dbase() =
	MBTree.clear dbase;
	List.iter (fun cid -> MBTree.add dbase cid (load cid)) (Vfs.get_group ftype);
	MBTree.optimize dbase

let is_captain t = 
	match t.ctype with
	| Creature c -> List.exists (fun c -> c = XCaptain) c.ccapacities
