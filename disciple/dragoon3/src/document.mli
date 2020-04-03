open Vfs

exception No_such_document of fid

val dopen : fid -> unit
val close : fid -> unit
val close_all : unit -> unit
val kill : fid -> unit
val get : fid -> Xgui.xobj
val create : fid -> Xml.xml -> (Xgui.xobj -> unit) -> unit
val current : unit -> fid option