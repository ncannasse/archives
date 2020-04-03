(* OCaml-Win32
 * win32_resource.ml
 * Copyright (c) 2002 by Harry Chomsky
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
(* User Interface Services: Resources: Resources *)

external begin_update_resource : filename:string -> delete_existing_resources:bool -> handle = "begin_update_resource"

external copy_image : image:handle -> typ:image_type -> size:size -> options:copy_image_option list -> handle = "copy_image"

external end_update_resource : rsrc:handle -> discard:bool -> unit = "end_update_resource"

(* ... enum_resource_languages *)

external enum_resource_names : inst:hinstance -> typ:resource_type -> (name:resource_name -> bool) -> unit = "enum_resource_names"

external enum_resource_types : inst:hinstance -> (typ:resource_type -> bool) -> unit = "enum_resource_types"

external find_resource : inst:hinstance -> name:resource_name -> typ:resource_type -> hrsrc = "find_resource"

(* ... find_resource_ex *)

external load_image : inst:hinstance -> name:resource_name -> typ:image_type -> size:size -> options:load_image_option list -> handle = "load_image"

external load_image_from_file : filename:string -> typ:image_type -> size:size -> options:load_image_option list -> handle = "load_image_from_file"

external load_resource : inst:hinstance -> rsrc:hrsrc -> handle = "load_resource"

external lock_resource : rsrc:handle -> pointer = "lock_resource"

external sizeof_resource : inst:hinstance -> rsrc:handle -> int64 = "sizeof_resource"

(* ... update_resource *)
