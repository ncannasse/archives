(* module CGames

 - CLIENT
 - Listes des parties visibles du joueur (avec qqs informations)

*)

open Message

type t

exception No_such_game of gid

val create : game_infos -> unit
val refresh : game_infos list -> unit
val count : unit -> int

val set_infos : game_all_infos -> unit
val start : gid -> unit
