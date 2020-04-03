(* module Game

 - SERVER
 - Données sur une partie en cours

*)
open Message

type t

val create : gid -> pid list -> t
val start : t -> unit

val mulligan : t -> pid -> bool -> unit
val invoke : t -> pid -> invoke_data -> unit
val move : t -> pid -> icid -> Maths.polygon -> unit
