(* module Server

 - SERVER
 - Application Serveur

*)
open Mtlib
open Message
open Printf
open Run

type debug_level =
	| DBG_NONE
	| DBG_SOME
	| DBG_FULL

let debug = ref DBG_FULL

let print x =
	Printf.printf "S>     ";
	let r = Printf.printf x in
	flush stdout;
	r

let connect func =
	let pid = Players.create func in
	if !debug = DBG_FULL then print "Player #%s connected\n" (pid_str pid);
	Players.send pid (CConnected pid);
	Players.send pid (CGames (Games.make_list()))

let debug_some pid m = ()

let debug_full pid m =
	print "Player #%s is " (pid_str pid);
	match m with
	| SCreateGame data ->
		print_endline ("creating game \""^data.gcd_name^"\"")
	| SListGames ->
		print_endline "listing games"
	| SRunGame gid ->
		print_endline ("running game #"^(gid_str gid))
	| SViewGameInfos gid ->
		print_endline ("requesting game infos #"^(gid_str gid))
	| SSelectDeck cards ->
		print_endline ("selecting his deck ("^(itos (List.length cards))^" cards)")
	| SMulligan b ->
		print_endline ((if b then "" else "not ")^"taking a mulligan")
	| SInvoke idata ->
		print_endline ("invoking creature #"^(icid_str idata.id_card))
	| SMoveCreature (cid,pos) ->
		print_endline ("moving creature #"^(icid_str cid))

let send_msg pid m =
	(match !debug with
	| DBG_NONE -> ()
	| DBG_SOME -> debug_some pid m
	| DBG_FULL -> debug_full pid m
	);
	try
		match m with
		| SCreateGame data ->
			let g = Players.create_game pid data in
			if !debug <> DBG_NONE then print "Game #%s \"%s\" created by player #%s\n" (gid_str (Games.gid g)) (Games.name g) (pid_str pid);
			Players.send_all (CGameCreated (Games.make_game_client g));
		| SListGames ->
			Players.send pid (CGames (Games.make_list()))
		| SRunGame gid ->
			Games.run pid gid;
			Players.send_all (CGameStarted gid);
		| SViewGameInfos gid ->
			Players.send pid (CGameInfos (Games.infos gid))
		| SSelectDeck cards ->
			Players.load_deck pid cards
		| SMulligan b ->
			let g = Players.game pid in
			Game.mulligan g pid b
		| SInvoke data ->
			let g = Players.game pid in
			Game.invoke g pid data
		| SMoveCreature (cid,pos) ->
			let g = Players.game pid in
			Game.move g pid cid pos
	with
		| SError e -> Players.send pid (CError e)
		| e -> Run.process_exc "Server" e

let init() =
	Random.self_init();
	Card.load_dbase();
	ICard.load_dbase()

;;
Run.register {
	m_name = "Server";
	m_init = Some init;
	m_main = None;
	m_close = None;
}