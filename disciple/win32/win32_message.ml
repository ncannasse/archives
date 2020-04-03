(* OCaml-Win32
 * win32_message.ml
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
(* User Interface Services: Windowing: Messages and Message Queues *)

external broadcast_system_message :
    options:broadcast_system_message_option list ->
    recipients:broadcast_system_message_recipient list ->
    msg:message_contents ->
    (bool * broadcast_system_message_recipient list) = "broadcast_system_message"

external dispatch_message : msg:msg -> message_result = "dispatch_message"

external get_input_state : unit -> bool = "get_input_state"

external get_message : wnd:hwnd option -> msg_filter:(int * int) option -> (bool * msg) = "get_message"

external get_message_extra_info : unit -> int32 = "get_message_extra_info"

external get_message_pos : unit -> point = "get_message_pos"

external get_message_time : unit -> int32 = "get_message_time"

external get_queue_status :
    options:queue_status_option list ->
    (queue_status_option list * queue_status_option list) = "get_queue_status"

external in_send_message : unit -> bool = "in_send_message"

external peek_message : wnd:hwnd option -> msg_filter:(int * int) option -> options:peek_message_option list -> msg option = "peek_message"

external post_message : wnd:hwnd -> msg:message_contents -> unit = "post_message"

external post_quit_message : exit_code:int32 -> unit = "post_quit_message"

external post_thread_message : thread:int32 -> msg:message_contents -> unit = "post_thread_message"

external register_window_message : name:string -> int = "register_window_message"

external reply_message : result:message_result -> bool = "reply_message"

external send_message : wnd:hwnd -> msg:message_contents -> message_result = "send_message"
let send ~wnd ~issuer = issuer ~sender:(send_message ~wnd)

external send_message_callback :
    wnd:hwnd ->
    msg:message_contents ->
    func:(wnd:hwnd -> id:message_id -> result:message_result -> unit) ->
    unit = "send_message_callback"

external send_message_timeout :
    wnd:hwnd ->
    msg:message_contents ->
    options:send_message_timeout_option list ->
    timeout:int ->
    message_result option = "send_message_timeout"

external send_notify_message : wnd:hwnd -> msg:message_contents -> unit = "send_notify_message"

external set_message_extra_info : info:int32 -> int32 = "set_message_extra_info"

external translate_message : msg:msg -> bool = "translate_message"

external wait_message : unit -> unit = "wait_message"
