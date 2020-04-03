(* OCaml-Win32
 * win32_windowproc.ml
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
(* User Interface Services: Windowing: Window Procedures *)

external call_window_proc : proc:pointer -> wnd:hwnd -> msg:message_contents -> message_result = "call_window_proc"

external def_window_proc : wnd:hwnd -> msg:message_contents -> message_result = "def_window_proc"

(* Standard window procedures *)

let apply_window_proc ~proc ~wnd ~msg =
    match proc with
        Wp_function f -> f ~wnd ~msg
      | Wp_external p -> call_window_proc ~proc:p ~wnd ~msg

let standard_window_proc ~processors =
    let p = combine_message_processors processors in
    let f ~wnd ~msg =
        match p ~wnd ~msg with
            Some result -> result
          | None -> def_window_proc ~wnd ~msg
    in
    Wp_function f

let subclassing_window_proc ~base ~processors =
    let p = combine_message_processors processors in
    let f ~wnd ~msg =
        match p ~wnd ~msg with
            Some result -> result
          | None -> apply_window_proc ~proc:base ~wnd ~msg
    in
    Wp_function f
