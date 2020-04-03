open Xgui
open Xml

val create : xparent option -> string -> xobj
val create_in : obj:xobj -> parent:string -> name:string -> unit
val close : xobj -> unit
val reload : unit -> unit
val get : string -> xml