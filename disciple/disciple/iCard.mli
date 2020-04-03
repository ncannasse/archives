(* module ICard

 - COMMON
 - Instance de cartes (une carte avec une ID unique et un Propriétaire)

*)
open Message

type icid = Message.icid

type t = {
	cid : icid;
	ccard : cid;
}

exception No_such_icard of icid

val load_dbase : unit -> unit
val load_deck : Vfs.fid -> pid -> icid list
val create : cid -> pid -> icid
val load : pid -> icid -> t
val id : t -> icid
val find : icid -> t
val owner : icid -> pid
