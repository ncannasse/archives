(* module Message

 - COMMON
 - Messages & Structures Client/Serveur

*)
type gid
type pid
type cid = Vfs.fid
type icid

type event =
	| EInvokeCaptains
	| EDrawPhase
	| EPlayCreature of icid

type game_infos = {
	gi_id : gid;
	gi_name : string;
	gi_creator : pid;
	gi_running : bool;
}

type game_all_infos = {
	gai_id : gid;
	gai_players : pid list;
}

type game_create_data = {
	gcd_name : string;
}

type game_init_player = {
	gip_pid : pid;
	gip_deck_count : int;
}

type game_init_data = {
	gid_id : gid;
	gid_players : game_init_player list;
}

type invoke_data = {
	id_card : icid;
	id_pos : (float * float);
}

type client_error =
	| EWarning of string
	| EUnkGame of gid
	| EUnkPlayer of pid
	| ENoGameRights of gid
	| EInvalidDeck of string
	| ECardNotOwned of icid
	| ECardNotInHand of icid
	| ENoDeckSelected
	| ENoGameJoined
	| EGameIsNotRunning of gid
	| EActionNotAvailable of string	
	| EInvalidPosition
	| EInvalidCreature of icid
	| ECreatureNotControled of icid

type client_message =
	| CError of client_error
	| CConnected of pid
	| CGames of game_infos list
	| CGameCreated of game_infos
	| CGameStarted of gid
	| CGameInit of game_init_data
	| CGameInfos of game_all_infos
	| CPlayerDraw of (pid * int)
	| CDrawCards of icid list
	| CInvoke of invoke_data
	| CPlayCard of icid
	| CMulliganQuery
	| CPlayerTookMulligan of pid
	| CEmitEvent of (event * int)
	| CPlayEvent of event
	| CMoveCreature of (icid * Maths.polygon)

type server_message = 
	| SListGames
	| SSelectDeck of icid list
	| SCreateGame of game_create_data	
	| SViewGameInfos of gid
	| SRunGame of gid
	| SMulligan of bool
	| SInvoke of invoke_data
	| SMoveCreature of (icid * Maths.polygon)

exception SError of client_error
exception SMessage of server_message

val alloc_pid : unit -> pid
val alloc_gid : unit -> gid
val alloc_icid : unit -> icid

val pid_str : pid -> string
val gid_str : gid -> string
val icid_str : icid -> string

val safe_cast : 'a -> 'b