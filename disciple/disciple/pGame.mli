(* module PGame

 - CLIENT
 - Partie en Cours

*)
open Message

type game_message =
	| GDraw of pid * int
	| GYouDraw of icid list
	| GInvoke of invoke_data
	| GPlay of icid
	| GEvent of (event * int)
	| GMulliganQuery
	| GPlayerMulligan of pid
	| GPlayEvent of event
	| GMoveCreature of (icid * Maths.polygon)

exception No_current_pgame
exception No_such_player of pid

val start_create : unit -> unit
val create : game_infos -> unit
val init : game_init_data -> unit
val run : unit -> unit
val close : unit -> unit
val gid : unit -> gid
val process : unit -> unit

val message : game_message -> unit
