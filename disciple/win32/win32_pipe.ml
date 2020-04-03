(* OCaml-Win32
 * win32_pipe.ml
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
(* Windows Base Services: Interprocess Communication: Pipes *)

(* ... call_named_pipe, connect_named_pipe, create_named_pipe *)

external create_pipe : security:security_attributes option -> size:int option -> (handle * handle) = "create_pipe"

(* ... disconnect_named_pipe, get_named_pipe_handle_state, get_named_pipe_info, peek_named_pipe, set_named_pipe_handle_state, transact_named_pipe, wait_named_pipe *)
