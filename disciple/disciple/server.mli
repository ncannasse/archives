(* module Server

 - SERVER
 - Application Serveur

*)
open Message

val connect : (client_message -> unit) -> unit
val send_msg : pid -> server_message -> unit
