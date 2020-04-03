
type 'a tree
type 'a t

exception Recursive_insert

val create : 'a -> 'a t
val of_root : 'a tree -> 'a t
val item : 'a -> 'a tree list -> 'a tree

val root : 'a t -> 'a tree
val pos : 'a tree -> int
val at : 'a t -> int -> 'a tree option
val sort : 'a t -> ('a -> 'a -> int) -> unit
val map : ('a -> 'b) -> 'a t -> 'b t

val insert : item:('a tree) -> parent:('a tree) -> unit
val remove : 'a tree -> unit
val parent : 'a tree -> 'a tree option
val childs : 'a tree -> 'a tree list
val data : 'a tree -> 'a
