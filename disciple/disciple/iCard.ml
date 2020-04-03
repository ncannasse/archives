(* module ICard

 - COMMON
 - Instance de cartes (une carte avec une ID unique et un Propriétaire)

*)
open Mtlib
open Message

type icid = Message.icid

type t = {
	cid : icid;
	ccard : Vfs.fid;
}

type tdb = {
	dcard : Vfs.fid;
	downer : pid;
}

exception No_such_icard of icid

let dbase = MBTree.empty()

let save_dbase() = 
	let ch = open_out_bin "icards.dat" in
	output_value ch dbase;
	close_out ch

let create cid pid =
	let id = Message.alloc_icid() in
	MBTree.add dbase id { dcard = cid; downer = pid };
	MBTree.optimize dbase;
	(*save_dbase();*)
	id

let get id =
	try
		MBTree.find dbase id
	with
		Not_found -> raise (No_such_icard id)

let load pid id =
	let card = get id in
	if card.downer <> pid then raise (SError (ECardNotOwned id));
	{
		cid = id;
		ccard = card.dcard;
	}

let id c = c.cid

let find id =
	let c = get id in
	{
		cid = id;
		ccard = c.dcard;
	}

	
let load_dbase() =
	try
		let ch = open_in_bin "icards.dat" in
		MBTree.copy dbase (input_value ch);
		close_in ch
	with
		Sys_error _ -> ()

let load_deck fid pid =
	let cards = Vfs.read fid in
	List.map (fun dep -> create (Vfs.get_dep fid dep) pid) cards

let owner id =
	let c = get id in
	c.downer
