(* OCaml-Win32
 * win32_mouse.ml
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
(* User Interface Services: User Input: Mouse Input *)

external drag_detect : wnd:hwnd -> pt:point -> bool = "drag_detect"

external get_capture : unit -> hwnd = "get_capture"

external get_double_click_time : unit -> int = "get_double_click_time"

external release_capture : unit -> unit = "release_capture"

external set_capture : wnd:hwnd -> hwnd = "set_capture"

external set_double_click_time : time:int -> unit = "set_double_click_time"

external swap_mouse_button : swap:bool -> bool = "swap_mouse_button"

(* ... track_mouse_event *)

(* Messages *)

let wm_capturechanged = 0x0215
type wm_capturechanged_handler =
    wnd:hwnd ->
    msg:message_contents ->
    capturing_wnd:hwnd ->
    unit message_handler_result
external deliver_wm_capturechanged :
    handler:wm_capturechanged_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_capturechanged"
let on_wm_capturechanged = on_message ~id:wm_capturechanged ~deliver:deliver_wm_capturechanged ~return:return_simple_message

let wm_lbuttondblclk = 0x0203
let on_wm_lbuttondblclk = on_mouse_message ~id:wm_lbuttondblclk

let wm_lbuttondown = 0x0201
let on_wm_lbuttondown = on_mouse_message ~id:wm_lbuttondown

let wm_lbuttonup = 0x0202
let on_wm_lbuttonup = on_mouse_message ~id:wm_lbuttonup

let wm_mbuttondblclk = 0x0209
let on_wm_mbuttondblclk = on_mouse_message ~id:wm_mbuttondblclk

let wm_mbuttondown = 0x0207
let on_wm_mbuttondown = on_mouse_message ~id:wm_mbuttondown

let wm_mbuttonup = 0x0208
let on_wm_mbuttonup = on_mouse_message ~id:wm_mbuttonup

let wm_mouseactivate = 0x0021
type wm_mouseactivate_handler =
    wnd:hwnd ->
    msg:message_contents ->
    top_parent:hwnd ->
    hit:hit_test_result ->
    id:message_id ->
    mouse_activate_option message_handler_result
external deliver_wm_mouseactivate :
    handler:wm_mouseactivate_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    mouse_activate_option message_handler_result = "deliver_wm_mouseactivate"
external return_wm_mouseactivate : result:mouse_activate_option -> message_result = "return_wm_mouseactivate"
let on_wm_mouseactivate = on_message ~id:wm_mouseactivate ~deliver:deliver_wm_mouseactivate ~return:return_wm_mouseactivate

let wm_mousemove = 0x0200
let on_wm_mousemove = on_mouse_message ~id:wm_mousemove

let wm_mousewheel = 0x020A
type wm_mousewheel_handler =
    wnd:hwnd ->
    msg:message_contents ->
    modifiers:mouse_modifier list ->
    delta:int ->
    pt:point ->
    unit message_handler_result
external deliver_wm_mousewheel :
    handler:wm_mousewheel_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_mousewheel"
let on_wm_mousewheel = on_message ~id:wm_mousewheel ~deliver:deliver_wm_mousewheel ~return:return_simple_message

let wm_nchittest = 0x0084
type wm_nchittest_handler =
    wnd:hwnd ->
    msg:message_contents ->
    pt:point ->
    hit_test_result message_handler_result
external deliver_wm_nchittest :
    handler:wm_nchittest_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    hit_test_result message_handler_result = "deliver_wm_nchittest"
external return_wm_nchittest : result:hit_test_result -> message_result = "return_wm_nchittest"
let on_wm_nchittest = on_message ~id:wm_nchittest ~deliver:deliver_wm_nchittest ~return:return_wm_nchittest

let wm_nclbuttondblclk = 0x00A3
let on_wm_nclbuttondblclk = on_mouse_nc_message ~id:wm_nclbuttondblclk

let wm_nclbuttondown = 0x00A1
let on_wm_nclbuttondown = on_mouse_nc_message ~id:wm_nclbuttondown

let wm_nclbuttonup = 0x00A2
let on_wm_nclbuttonup = on_mouse_nc_message ~id:wm_nclbuttonup

let wm_ncmbuttondblclk = 0x00A9
let on_wm_ncmbuttondblclk = on_mouse_nc_message ~id:wm_ncmbuttondblclk

let wm_ncmbuttondown = 0x00A7
let on_wm_ncmbuttondown = on_mouse_nc_message ~id:wm_ncmbuttondown

let wm_ncmbuttonup = 0x00A8
let on_wm_ncmbuttonup = on_mouse_nc_message ~id:wm_ncmbuttonup

let wm_ncmousemove = 0x00A0
let on_wm_ncmousemove = on_mouse_nc_message ~id:wm_ncmousemove

let wm_ncrbuttondblclk = 0x00A6
let on_wm_ncrbuttondblclk = on_mouse_nc_message ~id:wm_ncrbuttondblclk

let wm_ncrbuttondown = 0x00A4
let on_wm_ncrbuttondown = on_mouse_nc_message ~id:wm_ncrbuttondown

let wm_ncrbuttonup = 0x00A5
let on_wm_ncrbuttonup = on_mouse_nc_message ~id:wm_ncrbuttonup

let wm_rbuttondblclk = 0x0206
let on_wm_rbuttondblclk = on_mouse_message ~id:wm_rbuttondblclk

let wm_rbuttondown = 0x0204
let on_wm_rbuttondown = on_mouse_message ~id:wm_rbuttondown

let wm_rbuttonup = 0x0205
let on_wm_rbuttonup = on_mouse_message ~id:wm_rbuttonup
