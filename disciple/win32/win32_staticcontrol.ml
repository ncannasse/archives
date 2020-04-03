(* OCaml-Win32
 * win32_staticcontrol.ml
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
(* User Interface Services: Controls: Static Controls *)

(* Styles *)

type static_style_option =
    SS_BITMAP
  | SS_BLACKFRAME
  | SS_BLACKRECT
  | SS_CENTER
  | SS_CENTERIMAGE
  | SS_ENDELLIPSIS
  | SS_ENHMETAFILE
  | SS_ETCHEDFRAME
  | SS_ETCHEDHORZ
  | SS_ETCHEDVERT
  | SS_GRAYFRAME
  | SS_GRAYRECT
  | SS_ICON
  | SS_LEFT
  | SS_LEFTNOWORDWRAP
  | SS_NOPREFIX
  | SS_NOTIFY
  | SS_OWNERDRAW
  | SS_PATHELLIPSIS
  | SS_REALSIZEIMAGE
  | SS_RIGHT
  | SS_RIGHTJUST
  | SS_SIMPLE
  | SS_SUNKEN
  | SS_WHITEFRAME
  | SS_WHITERECT
  | SS_WORDELLIPSIS

external static_style_of_option : static_style_option -> style = "static_style_of_option"

let static_style_of_options = special_style_of_options ~special_func:static_style_of_option

(* Messages *)

let stm_geticon = 0x0171
let pack_stm_geticon = pack_simple_message ~id:stm_geticon
external unpack_stm_geticon : message_result -> hicon = "unpack_stm_geticon"
let issue_stm_geticon ~sender = unpack_stm_geticon (sender ~msg:pack_stm_geticon)

let stm_getimage = 0x0173
external pack_stm_getimage : typ:image_type -> message_contents = "pack_stm_getimage"
external unpack_stm_getimage : message_result -> handle = "unpack_stm_getimage"
let issue_stm_getimage ~sender ~typ = unpack_stm_getimage (sender ~msg:(pack_stm_getimage ~typ))

let stm_seticon = 0x0170
external pack_stm_seticon : icon:hicon -> message_contents = "pack_stm_seticon"
external unpack_stm_seticon : message_result -> hicon = "unpack_stm_seticon"
let issue_stm_seticon ~sender ~icon = unpack_stm_seticon (sender ~msg:(pack_stm_seticon ~icon))

let stm_setimage = 0x0172
external pack_stm_setimage : typ:image_type -> image:handle -> message_contents = "pack_stm_setimage"
external unpack_stm_setimage : message_result -> handle = "unpack_stm_setimage"
let issue_stm_setimage ~sender ~typ ~image = unpack_stm_setimage (sender ~msg:(pack_stm_setimage ~typ ~image))

let wm_ctlcolorstatic = 0x0138
let on_wm_ctlcolorstatic = on_ctlcolor_message ~id:wm_ctlcolorstatic
let _ = add_explicit_dialog_return wm_ctlcolorstatic

(* Notifications *)

let stn_clicked = 0
let stn_dblclk = 1
let stn_disable = 3
let stn_enable = 2
