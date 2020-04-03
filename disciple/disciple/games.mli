(* module Games

 - SERVER
 - Données sur l'ensemble des parties crées

*)
open Message

type t

val make_list : unit -> game_infos list
val make_game_client : t -> game_infos

val create : pid -> game_create_data -> t
val run : pid -> gid -> unit

val gid : t -> gid
val name : t -> string
val infos : gid -> game_all_infos

val data : t -> Game.t