(* OCaml-Win32
 * win32_button.ml
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
(* User Interface Services: Controls: Buttons *)

external check_dlg_button : dlg:hwnd -> id:int -> state:button_state -> unit = "check_dlg_button"

external check_radio_button : dlg:hwnd -> first_id:int -> last_id:int -> id:int -> unit = "check_radio_button"

external is_dlg_button_checked : dlg:hwnd -> id:int -> button_state = "is_dlg_button_checked"

(* Styles *)

type button_style_option =
    BS_3STATE
  | BS_AUTO3STATE
  | BS_AUTOCHECKBOX
  | BS_AUTORADIOBUTTON
  | BS_BITMAP
  | BS_BOTTOM
  | BS_CENTER
  | BS_CHECKBOX
  | BS_DEFPUSHBUTTON
  | BS_FLAT
  | BS_GROUPBOX
  | BS_ICON
  | BS_LEFT
  | BS_LEFTTEXT
  | BS_MULTILINE
  | BS_NOTIFY
  | BS_OWNERDRAW
  | BS_PUSHBUTTON
  | BS_PUSHLIKE
  | BS_RADIOBUTTON
  | BS_RIGHT
  | BS_RIGHTBUTTON
  | BS_TEXT
  | BS_TOP
  | BS_VCENTER

external button_style_of_option : button_style_option -> style = "button_style_of_option"

let button_style_of_options = special_style_of_options ~special_func:button_style_of_option

(* Messages *)

let bm_click = 0x00F5
let pack_bm_click = pack_simple_message ~id:bm_click
let unpack_bm_click = ignore
let issue_bm_click ~sender = unpack_bm_click (sender ~msg:pack_bm_click)

let bm_getcheck = 0x00F0
let pack_bm_getcheck = pack_simple_message ~id:bm_getcheck
external unpack_bm_getcheck : message_result -> button_state = "unpack_bm_getcheck"
let issue_bm_getcheck ~sender = unpack_bm_getcheck (sender ~msg:pack_bm_getcheck)

let bm_getimage = 0x00F6
external pack_bm_getimage : typ:image_type -> message_contents = "pack_bm_getimage"
external unpack_bm_getimage : message_result -> handle = "unpack_bm_getimage"
let issue_bm_getimage ~sender ~typ = unpack_bm_getimage (sender ~msg:(pack_bm_getimage ~typ))

let bm_getstate = 0x00F2
let pack_bm_getstate = pack_simple_message ~id:bm_getstate
external unpack_bm_getstate : message_result -> (button_state * button_state_ex list) = "unpack_bm_getstate"
let issue_bm_getstate ~sender = unpack_bm_getstate (sender ~msg:pack_bm_getstate)

let bm_setcheck = 0x00F1
external pack_bm_setcheck : state:button_state -> message_contents = "pack_bm_setcheck"
let unpack_bm_setcheck = ignore
let issue_bm_setcheck ~sender ~state = unpack_bm_setcheck (sender ~msg:(pack_bm_setcheck ~state))

let bm_setimage = 0x00F7
external pack_bm_setimage : typ:image_type -> image:handle -> message_contents = "pack_bm_setimage"
external unpack_bm_setimage : message_result -> handle = "unpack_bm_setimage"
let issue_bm_setimage ~sender ~typ ~image = unpack_bm_setimage (sender ~msg:(pack_bm_setimage ~typ ~image))

let bm_setstate = 0x00F3
external pack_bm_setstate : state:bool -> message_contents = "pack_bm_setstate"
let unpack_bm_setstate = ignore
let issue_bm_setstate ~sender ~state = unpack_bm_setstate (sender ~msg:(pack_bm_setstate ~state))

let bm_setstyle = 0x00F4
external pack_bm_setstyle : style:button_style_option list -> redraw:bool -> message_contents = "pack_bm_setstyle"
let unpack_bm_setstyle = ignore
let issue_bm_setstyle ~sender ~style ~redraw = unpack_bm_setstyle (sender ~msg:(pack_bm_setstyle ~style ~redraw))

let wm_ctlcolorbtn = 0x0135
let on_wm_ctlcolorbtn = on_ctlcolor_message ~id:wm_ctlcolorbtn
let _ = add_explicit_dialog_return wm_ctlcolorbtn

(* Notifications *)

let bn_clicked = 0
let bn_dblclk = 5
let bn_doubleclicked = 5
let bn_killfocus = 7
let bn_setfocus = 6
