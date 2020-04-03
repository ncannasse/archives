(* OCaml-Win32
 * win32_console.ml
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
(* Windows Base Services: Files and I/O: Consoles and Character-Mode Support *)

(* ... lots of functions to add here *)

external alloc_console : unit -> unit = "alloc_console"

external free_console : unit -> unit = "free_console"

external generate_console_ctrl_event :
    event:ctrl_event ->
    process_group_id:int32 ->
    unit = "generate_console_ctrl_event"

external get_console_title : max_length:int -> string = "get_console_title"

external get_std_handle : std_handle:std_handle -> handle = "get_std_handle"

external set_console_title : title:string -> unit = "set_console_title"

external set_std_handle : std_handle:std_handle -> handle:handle -> unit = "set_std_handle"
