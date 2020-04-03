(* **************************************************************************** *)
(*																				*)
(*		MTLib , Motion-Twin Ocaml Standard Library								*)
(*		(c)2002 Nicolas Cannasse												*)
(*		(c)2002 Motion Twin														*)
(*																				*)
(* **************************************************************************** *)
(* Globals *)

exception Global_not_initialized of string

module Global :
  sig

	type 'a t

	val empty : string -> 'a t (* returns an new named empty global *)
	val name : 'a t -> string (* retrieve the name of a global *)
	val set : 'a t -> 'a -> unit (* set the global value contents *)
	val get : 'a t -> 'a (* get the global value contents - raise Global_not_initialized if not defined *)
	val undef : 'a t -> unit (* reset the global value contents to undefined *)
	val isdef : 'a t -> bool (* tell if the global value has been set *)

	val opt : 'a t -> 'a option (* return None if the global is undefined, or Some v where v is the current global value contents *)

  end

(* **************************************************************************** *)
(* Mutable List *)

exception Invalid_index of int
exception Invalid_operation

module MList :
  sig
	type 'a t
	
	val empty : unit -> 'a t
	val isempty : 'a t -> bool

	val copy : 'a t -> 'a t -> unit
	val copy_list : 'a t -> 'a list -> unit 
	val from_list : 'a list -> 'a t
	val to_list : 'a t -> 'a list
	
	val add : 'a t -> 'a -> unit	
	val push : 'a t -> 'a -> unit
	val pop : 'a t -> 'a
	val npop : 'a t -> int -> 'a list
	val clear : 'a t -> unit
	val length : 'a t -> int
	val add_sort : ('a -> 'a -> int) -> 'a t -> 'a -> unit

	val hd : 'a t -> 'a
	val tl : 'a t -> 'a list
	val iter : ('a -> unit) -> 'a t -> unit
	val find : ('a -> bool) -> 'a t -> 'a
	val findex : ('a -> bool) -> 'a t -> exn -> 'a
	val exists : ('a -> bool) -> 'a t -> bool
	val map : ('a -> 'b) -> 'a t -> 'b t
	val sort : ('a -> 'a -> int) -> 'a t -> unit

	val last : 'a t -> 'a
	val index_of : 'a t -> 'a -> int
	val at_index : 'a t -> int -> 'a

	val index_of_with : ('a -> bool) -> 'a t -> int

	val set : 'a t -> int -> 'a -> unit
	val remove : 'a t -> 'a -> unit
	val remove_if : ('a -> bool) -> 'a t -> unit
	val remove_at_index : 'a t -> int -> unit
	val shuffle : 'a t -> unit
  end

(* **************************************************************************** *)
(* Extended String *)

module MString :
  sig

	val find_sub : string -> string -> int
	val split : string -> string -> string * string
	val split_char : string -> char -> string * string
	val rsplit_char : string -> char -> string * string
	val lchop : string -> string
	val rchop : string -> string
	val unconcat : string -> string -> string list

  end

val ( =& ) : string -> string -> bool

val itos : int -> string
val ftos : float -> string
val ftoi : float -> int
val itof : int -> float
val stoi : string -> int
val stof : string -> float
val print : string -> unit

(* **************************************************************************** *)
(* Binary Tree - using "compare" function for key comparison *)

exception Already_Exists

module MBTree :
  sig

	type ('a,'b) t
	
	val empty : unit -> ('a,'b) t
	val length : ('a,'b) t -> int
	val depth : ('a,'b) t -> int

	val add : ('a,'b) t -> 'a -> 'b -> unit
	val find : ('a,'b) t -> 'a -> 'b
	val remove : ('a,'b) t -> 'a -> unit

	val to_list : ('a,'b) t -> ('a * 'b) list
	val optimize : ('a,'b) t -> unit

	val iter : ('a -> 'b -> unit) -> ('a,'b) t -> unit
	val find_p : ('a -> 'b -> bool) -> ('a,'b) t -> ('a * 'b)

	val copy : ('a,'b) t -> ('a,'b) t -> unit
	val clear : ('a,'b) t -> unit

  end

(* **************************************************************************** *)
(* Misc *)

val read_all_file : in_channel -> string

val fnot : ('a -> bool) -> 'a -> bool
val fall : 'a -> bool

val fabs : float -> float

val ( += ) : int ref -> int -> unit
val ( -= ) : int ref -> int -> unit
val ( +=. ) : float ref -> float -> unit
val ( -=. ) : float ref -> float -> unit

(* **************************************************************************** *)
