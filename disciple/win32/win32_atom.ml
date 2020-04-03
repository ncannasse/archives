(* OCaml-Win32
 * win32_atom.ml
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
(* Windows Base Services: Interprocess Communication: Atoms *)

external add_atom : name:string -> int = "add_atom"

external delete_atom : atom:int -> unit = "delete_atom"

external find_atom : name:string -> int = "find_atom"

external get_atom_name : atom:int -> string = "get_atom_name"

external global_add_atom : name:string -> int = "global_add_atom"

external global_delete_atom : atom:int -> unit = "global_delete_atom"

external global_find_atom : name:string -> int = "global_find_atom"

external global_get_atom_name : atom:int -> string = "global_get_atom_name"

external init_atom_table : size:int -> unit = "init_atom_table"
