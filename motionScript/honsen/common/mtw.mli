
type t =
	| Data of string * string
	| Group of string * t list
	| Ident of string
	| Number of string * int32

type file = t list

exception Invalid_tag of string

val write : 'a IO.output -> t -> unit
val read : IO.input -> t
val tag : t -> string
val size : t -> int

val get_data : string -> t -> string (* raise Not_found *)
val get_data_rec : string -> t -> string (* raise Not_found *)
