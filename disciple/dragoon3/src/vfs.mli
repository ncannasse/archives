(********************************************************************************)
(* Types *)

type fid
type fh_r
type fh_w
type ftype
type fdep
type fver = int

type fileinfos = {
	fi_id : fid;
	fi_name : string;
	fi_parent : fid;
	fi_ver : fver;
	fi_t : ftype;
}

type filehard = {
	fh_ext : string list;
	fh_import : fid -> string -> unit;
}

type fevent = fid -> unit

type filetype = {
	ft_name : string;
	ft_t : ftype;	
	ft_icon : Osiris.hres;
	ft_new : fevent option;
	ft_select : fevent option;
	ft_unselect : fevent option;
	ft_open : fevent option;
	ft_show : fevent option;
	ft_hide : fevent option;
	ft_close : fevent option;
	ft_context : (string * fevent) list;
	ft_delete : fevent option;
	ft_hardlinked : filehard option;
	ft_haschilds : bool;
	ft_export : ((fid -> Xml.xml) * (fid -> Xml.xml -> unit)) option;
}

exception No_such_file_id of fid
exception No_such_hlink of fid
exception No_such_file of string
exception File_already_exist of string
exception Invalid_parent of fid
exception Invalid_file_name of string
exception Type_already_registred
exception Unknown_ftype of string
exception Cannot_delete of fid
exception Cannot_rename of fid * string
exception Operation_not_supported
exception Hlink_no_data of fid

(********************************************************************************)
(* File System *)

val get_root : unit -> fid
val get_all_files : unit -> fid list
val enum_childs : (fid -> 'a) -> fid -> 'a list

val create_file : name:string -> parent:fid -> ftype:ftype -> fid
val create_hlink : name:string -> file:string -> parent:fid -> ftype:ftype -> fid
val create_tmp : name:string -> ftype:ftype -> fid

val read : fid -> 'a
val write : fid -> 'a -> unit

val name_of_file : fid -> string
val get_file_infos : fid -> fileinfos
val get_file_path : fid -> string
val has_childs : fid -> bool
val get_fid : string -> fid

val rename_file : fid -> string -> unit
val delete_file : fid -> unit
val move_file : file:fid -> parent:fid -> unit

(* hlink *)

val open_in_hlink : fid -> in_channel
val name_of_hlink : fid -> string
val size_of_hlink : fid -> int
val find_hlink_local : fid -> string -> fid

(* misc *)

val refresh_callback : (unit -> unit) -> unit
val sync : (string -> unit) -> unit
val checkout_hlink : (string -> unit) -> fid -> unit
val export_tree : string -> fid -> unit

val islocal : bool
val isdebug : bool
val path : string

(* import / export *)

val export : unit -> Xml.xml
val import : Xml.xml -> unit
val export_fdep : fdep -> string
val import_fdep : string -> fdep

(********************************************************************************)
(* File Deps *)

val add_dep : fid -> fid -> fdep
val get_dep : fid -> fdep -> fid
val del_dep : fid -> fdep -> unit
val enum_deps : (fid -> unit) -> fid -> unit
val clear_deps : fid -> unit

(********************************************************************************)
(* File Types *)

val get_ftype : string -> ftype
val get_group : ftype -> fid list
val register : filetype -> unit
val get_ftype_infos : ftype -> filetype
val enum_ftype : (filetype -> unit) -> unit

(********************************************************************************)