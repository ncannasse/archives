(* module Games

 - SERVER
 - Données sur l'ensemble des parties crées

*)
open Message
open Mtlib

type t = {
	gid : gid;
	gname : string;
	gcreator : pid;
	gplayers : pid MList.t;
	mutable gdata : Game.t option;
	mutable grunning : bool;
}

let games = MList.empty()

let make_game_client g =
	{
		gi_id = g.gid;
		gi_name = g.gname;
		gi_creator = g.gcreator;			
		gi_running = g.grunning;
	}

let get gid =
	try
		MList.find (fun g -> g.gid = gid) games;
	with
		Not_found -> raise (SError (EUnkGame gid))

let make_list() =
	MList.to_list (MList.map make_game_client games)

let create pid data =
	let gdata = {
		gid = alloc_gid();
		gname = data.gcd_name;
		gcreator = pid;
		gplayers = MList.from_list [pid];
		gdata = None;
		grunning = false;
	} in
	MList.add games gdata;
	gdata

let run pid gid =
	let data = get gid in
	if data.gcreator <> pid then raise (SError (ENoGameRights gid));
	if data.grunning then raise (SError (EWarning "Game is already running"));
	data.grunning <- true;
	let g = Game.create data.gid (MList.to_list data.gplayers) in
	data.gdata <- Some g;
	Game.start g

let infos gid =
	let data = get gid in	
	{
		gai_id = gid;
		gai_players = MList.to_list data.gplayers;
	}

let gid g = g.gid
let name g = g.gname

let data g =
	match g.gdata with
	| None -> raise (SError (EGameIsNotRunning g.gid))
	| Some data -> data