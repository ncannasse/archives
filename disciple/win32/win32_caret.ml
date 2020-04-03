(* OCaml-Win32
 * win32_caret.ml
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
(* User Interface Services: Resources: Carets *)

type caret_description =
    Cd_solid of size
  | Cd_gray of size
  | Cd_bitmap of hbitmap
external create_caret : wnd:hwnd -> description:caret_description -> unit = "create_caret"

external destroy_caret : unit -> unit = "destroy_caret"

external get_caret_blink_time : unit -> int = "get_caret_blink_time"

external get_caret_pos : unit -> point = "get_caret_pos"

external hide_caret : wnd:hwnd -> unit = "hide_caret"

external set_caret_blink_time : time:int -> unit = "set_caret_blink_time"

external set_caret_pos : pos:point -> unit = "set_caret_pos"

external show_caret : wnd:hwnd -> unit = "show_caret"
