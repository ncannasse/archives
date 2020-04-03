(* OCaml-Win32
 * win32_windowprop.ml
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
(* User Interface Services: Windowing: Window Properties *)

external enum_props : wnd:hwnd -> func:(name:atom_name -> data:int32 -> bool) -> unit = "enum_props"

external get_prop : wnd:hwnd -> name:atom_name -> int32 = "get_prop"

external remove_prop : wnd:hwnd -> name:atom_name -> unit = "remove_prop"

external set_prop : wnd:hwnd -> name:atom_name -> data:int32 -> unit = "set_prop"
