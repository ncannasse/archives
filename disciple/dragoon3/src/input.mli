type state =
	| Up
	| Down
	| Pressed of int
	| Released

val is_down : state -> bool

module Mouse :
  sig
	
	val pos : unit -> (float * float)
	val set_pos : (float * float) -> unit

	val left : unit -> state
	val right : unit -> state
	val middle : unit -> state	

	val reset_left : unit -> unit

  end

exception Invalid_keyname of string

module Keyb :
  sig

	type key

	val key : string -> key
	val name : key -> string
	
	val state : key -> state
	val current : unit -> key list

	val bind : (unit -> unit) -> key list -> unit
	val unbind : key list -> unit

	val left : key
	val right : key
	val up : key
	val down : key

  end

val init : Osiris.handle -> bool
val process : unit -> unit
val close : unit -> unit