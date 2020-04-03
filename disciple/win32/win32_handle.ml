(* OCaml-Win32
 * win32_handle.ml
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
(* Windows Base Services: General Library: Handles and Objects *)

external close_handle : handle:handle -> unit = "close_handle"

external duplicate_handle :
    source_process:handle ->
    source:handle ->
    target_process:handle ->
    access:access_mask option ->
    inherit_handle:bool ->
    close_source:bool ->
    handle = "duplicate_handle_bytecode" "duplicate_handle_native"

external get_handle_information : handle:handle -> handle_flag list = "get_handle_information"

external set_handle_information : handle:handle -> mask:handle_flag list -> flags:handle_flag list -> unit = "set_handle_information"
