type phandle

exception Error of string
exception ID_out of int
exception Invalid_version of (int * int)
exception Invalid_link_version of (int * int)

type pinfos = {
	pname : string;
	pparent : int;
	ptype : int;
	pver : int;
}

type pdata = {
	pdata : string;
	pdeps : int list;
}

val connect : host:string -> project:string -> user:string -> passwd:string -> phandle
val get_root : phandle -> int
val get_infos : phandle -> int -> pinfos
val get_childs : phandle -> int -> int list
val get_deps : phandle -> int -> int list
val get_link : phandle -> int -> pinfos

val create : phandle:phandle -> name:string -> parent:int -> ftype:int -> int * string
val create_link : phandle -> int -> string -> unit
val rename : phandle -> int -> string -> bool
val move : phandle -> int -> int -> unit
val delete : phandle -> int -> unit
val undelete : phandle:phandle -> id:int -> parent:int -> unit
val update : phandle:phandle -> id:int -> version:int -> deps:(int list) -> data:string -> bool
val update_link : phandle:phandle -> id:int -> version:int -> data:string -> unit
val checkout : phandle -> int -> pdata option
val checkout_link : phandle -> int -> string option

val get_last_id : unit -> int

val close : phandle -> unit