(* module CGames

 - CLIENT
 - Listes des parties visibles du joueur (avec qqs informations)

*)
open Message
open Mtlib

type t = {
	gid : gid;
	gname : string;
	gcreator : pid;
	gplayers : pid MList.t;
	mutable grunning : bool;
}

let games = MList.empty()

exception No_such_game of gid

let make_game_server g = {
	gid = g.gi_id;
	gname = g.gi_name;
	gcreator = g.gi_creator;
	gplayers = MList.from_list [g.gi_creator];
	grunning = g.gi_running;
}

let get gid =
	try
		MList.find (fun g -> g.gid = gid) games;
	with
		Not_found -> raise (No_such_game gid)

let refresh glist =
	MList.copy_list games (List.map make_game_server glist)

let count() =
	MList.length games

let create gdata = 
	MList.add games (make_game_server gdata)

let set_infos ginf =
	let g = get ginf.gai_id in
	MList.copy_list g.gplayers ginf.gai_players

let start gid =
	let g = get gid in
	g.grunning <- true
