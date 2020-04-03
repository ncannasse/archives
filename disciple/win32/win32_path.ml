(* OCaml-Win32
 * win32_path.ml
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
(* Graphics and Multimedia Services: GDI: Paths *)

external abort_path : dc:hdc -> unit = "abort_path"

external begin_path : dc:hdc -> unit = "begin_path"

external close_figure : dc:hdc -> unit = "close_figure"

external end_path : dc:hdc -> unit = "end_path"

external fill_path : dc:hdc -> unit = "fill_path"

external flatten_path : dc:hdc -> unit = "flatten_path"

external get_miter_limit : dc:hdc -> float = "get_miter_limit"

(* ... get_path *)

external path_to_region : dc:hdc -> hrgn = "path_to_region"

external set_miter_limit : dc:hdc -> limit:float -> float = "set_miter_limit"

external stroke_and_fill_path : dc:hdc -> unit = "stroke_and_fill_path"

external stroke_path : dc:hdc -> unit = "stroke_path"

external widen_path : dc:hdc -> unit = "widen_path"
