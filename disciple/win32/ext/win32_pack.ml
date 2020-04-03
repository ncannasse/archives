(* OCaml-Win32
 * win32_pack.ml
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
(* Standard Pack & Unpack functions for core types *)

type 'a pack = msg:int -> value:'a -> message_contents
type 'a unpack = message_result -> 'a

let unpack_unit = ignore
external unpack_int : message_result -> int = "unpack_int"
external unpack_int32 : message_result -> int32 = "unpack_int32"
external unpack_bool : message_result -> bool = "unpack_bool"
external unpack_pointlh : message_result -> point = "unpack_pointlh"
external unpack_string : message_result -> string = "unpack_string"
external unpack_color : message_result -> color = "unpack_color"
external unpack_handle : message_result -> handle = "unpack_handle"
external unpack_hwnd : message_result -> hwnd = "unpack_handle"

external pack_wint : msg:int -> value:int -> message_contents = "pack_wint"
external pack_wbool : msg:int -> value:bool -> message_contents = "pack_wbool"
external pack_whandle : msg:int -> value:handle -> message_contents = "pack_whandle"

external pack_lrect : msg:int -> value:rect -> message_contents = "pack_lrect"
external pack_lint32 : msg:int -> value:int32 -> message_contents = "pack_lint32"
external pack_lint : msg:int -> value:int -> message_contents = "pack_lint"
external pack_lbool : msg:int -> value:bool -> message_contents = "pack_lbool"
external pack_lcolor : msg:int -> value:color -> message_contents = "pack_lcolor"
external pack_lhandle : msg:int -> value:handle -> message_contents = "pack_lhandle"

external pack_lemptystring : msg:int -> message_contents = "pack_lemptystring"
external pack_loption : msg:int -> value:'a option -> pack:('a pack) -> message_contents = "pack_loption"
external unpack_lparam : sender:message_sender -> msg:message_contents -> unpack:('a unpack) -> 'a = "unpack_lparam"
external pack2 : msg:int -> wparam:(msg:int -> message_contents) -> lparam:(msg:int -> message_contents) -> message_contents = "pack2"

(********************************************************************)
