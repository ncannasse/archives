(* module Card

 - COMMON
 - Gestion des informations des cartes

*)

type cid = Message.cid

type essence = 
	| EIce
	| EFire
	| EVoid
	| EVegetal
	| ELight

type cattack = {
	acoef : int;
	apower : int;
	adist : int;
	amindist : int;
	aanim : string;
}

type ccapacity =
	| XAttack of cattack
	| XCommand of int
	| XCaptain

type ccreature = {
	clevel : int;
	cessence : essence;
	clife : int;
	cmovement : int;
	cspeed : int;
	csize : int;
	cmodel : Vfs.fid;
	canimbcl : string;
	canimmove : string;
	cmovespeed : int;
	ccapacities : ccapacity list;
}

type ctype =
	| Creature of ccreature

type t = {
	cname : string;
	ctype : ctype;
}

exception No_such_card of cid

val ctype_strings : string list
val essence_strings : string list
val ftype : Vfs.ftype

val find : cid -> t
val art : t -> Vfs.fid
val load_dbase : unit -> unit

(* Predicates *)

val is_captain : t -> bool

(* Only For Dragoon3 *)

val save : cid -> (string * ctype) -> unit
val load : cid -> (string * ctype)
