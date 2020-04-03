(* module Players

 - SERVER
 - Données sur l'ensemble des joueurs connectés

*)
open Message

type t

val create : (client_message -> unit) -> pid
val load_deck : pid -> ICard.icid list -> unit
val get_deck : pid -> Deck.t
val send : pid -> client_message -> unit
val send_all : client_message -> unit

val create_game : pid -> game_create_data -> Games.t
val join_game : pid -> Games.t -> unit
val leave_game : pid -> unit

val game : pid -> Game.t