(* OCaml-Win32
 * win32_dynlink.ml
 * Copyright (c) 2002 by Harry Chomsky & Nicolas Cannasse
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *)

open Win32_core

(********************************************************************)
(* Dynamic Linking *)

type dynamic_flag =
	| DONT_RESOLVE_DLL_REFERENCES 
	| LOAD_LIBRARY_AS_DATAFILE
	| LOAD_WITH_ALTERED_SEARCH_PATH

type hmodule = handle

external disable_thread_library_calls : lib:hmodule -> unit = "disable_thread_library_calls"
external free_library : lib:hmodule -> unit = "free_library"
external free_library_and_exit_thread : lib:hmodule -> exit_code:int -> unit = "free_library_and_exit_thread"
external get_module_file_name : lib:(hmodule option) -> string = "get_module_file_name"
external get_module_handle : filename:(string option) -> hmodule = "get_module_handle"
external get_proc_address : lib:hmodule -> procname:string -> pointer = "get_proc_address"
external load_library : filename:string -> hmodule = "load_library"
external load_library_ex : filename:string -> flag:dynamic_flag -> hmodule = "load_library_ex"

(* TO USE WITH CAUTION *)
external caml_get_proc_address : lib:hmodule -> procname:string -> ('a -> unit) = "get_proc_address"

(********************************************************************)
