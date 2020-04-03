(* OCaml-Win32
 * win32_editcontrol.ml
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
(* User Interface Services: Controls: Edit Controls *)

(* Styles *)

type edit_style_option =
    ES_AUTOHSCROLL
  | ES_AUTOVSCROLL
  | ES_CENTER
  | ES_LEFT
  | ES_LOWERCASE
  | ES_MULTILINE
  | ES_NOHIDESEL
  | ES_NUMBER
  | ES_OEMCONVERT
  | ES_PASSWORD
  | ES_READONLY
  | ES_RIGHT
  | ES_UPPERCASE
  | ES_WANTRETURN

external edit_style_of_option : edit_style_option -> style = "edit_style_of_option"

let edit_style_of_options = special_style_of_options ~special_func:edit_style_of_option

(* Messages *)

let em_canundo = 0x00C6
let pack_em_canundo = pack_simple_message ~id:em_canundo
external unpack_em_canundo : message_result -> bool = "unpack_em_canundo"
let issue_em_canundo ~sender = unpack_em_canundo (sender ~msg:pack_em_canundo)

let em_charfrompos = 0x00D7
external pack_em_charfrompos : pos:point -> message_contents = "pack_em_charfrompos"
external unpack_em_charfrompos : message_result -> (int * int) = "unpack_em_charfrompos"
let issue_em_charfrompos ~sender ~pos = unpack_em_charfrompos (sender ~msg:(pack_em_charfrompos ~pos))

let em_emptyundobuffer = 0x00CD
let pack_em_emptyundobuffer = pack_simple_message ~id:em_emptyundobuffer
let unpack_em_emptyundobuffer = ignore
let issue_em_emptyundobuffer ~sender = unpack_em_emptyundobuffer (sender ~msg:pack_em_emptyundobuffer)

let em_fmtlines = 0x00C8
external pack_em_fmtlines : add_eol:bool -> message_contents = "pack_em_fmtlines"
let unpack_em_fmtlines = ignore
let issue_em_fmtlines ~sender ~add_eol = unpack_em_fmtlines (sender ~msg:(pack_em_fmtlines ~add_eol))

let em_getfirstvisibleline = 0x00CE
let pack_em_getfirstvisibleline = pack_simple_message ~id:em_getfirstvisibleline
external unpack_em_getfirstvisibleline : message_result -> int = "unpack_em_getfirstvisibleline"
let issue_em_getfirstvisibleline ~sender = unpack_em_getfirstvisibleline (sender ~msg:pack_em_getfirstvisibleline)

let em_gethandle = 0x00BD
let pack_em_gethandle = pack_simple_message ~id:em_gethandle
external unpack_em_gethandle : message_result -> handle = "unpack_em_gethandle"
let issue_em_gethandle ~sender = unpack_em_gethandle (sender ~msg:pack_em_gethandle)

let em_getlimittext = 0x00D5
let pack_em_getlimittext = pack_simple_message ~id:em_getlimittext
external unpack_em_getlimittext : message_result -> int = "unpack_em_getlimittext"
let issue_em_getlimittext ~sender = unpack_em_getlimittext (sender ~msg:pack_em_getlimittext)

let em_getline = 0x00C4
external issue_em_getline : sender:message_sender -> line:int -> max_length:int -> string = "issue_em_getline"

let em_getlinecount = 0x00BA
let pack_em_getlinecount = pack_simple_message ~id:em_getlinecount
external unpack_em_getlinecount : message_result -> int = "unpack_em_getlinecount"
let issue_em_getlinecount ~sender = unpack_em_getlinecount (sender ~msg:pack_em_getlinecount)

let em_getmargins = 0x00D4
let pack_em_getmargins = pack_simple_message ~id:em_getmargins
external unpack_em_getmargins : message_result -> (int * int) = "unpack_em_getmargins"
let issue_em_getmargins ~sender = unpack_em_getmargins (sender ~msg:pack_em_getmargins)

let em_getmodify = 0x00B8
let pack_em_getmodify = pack_simple_message ~id:em_getmodify
external unpack_em_getmodify : message_result -> bool = "unpack_em_getmodify"
let issue_em_getmodify ~sender = unpack_em_getmodify (sender ~msg:pack_em_getmodify)

let em_getpasswordchar = 0x00D2
let pack_em_getpasswordchar = pack_simple_message ~id:em_getpasswordchar
external unpack_em_getpasswordchar : message_result -> char option = "unpack_em_getpasswordchar"
let issue_em_getpasswordchar ~sender = unpack_em_getpasswordchar (sender ~msg:pack_em_getpasswordchar)

let em_getrect = 0x00B2
external issue_em_getrect : sender:message_sender -> rect = "issue_em_getrect"

let em_getsel = 0x00B0
external issue_em_getsel : sender:message_sender -> (int * int) = "issue_em_getsel"

let em_getthumb = 0x00BE
let pack_em_getthumb = pack_simple_message ~id:em_getthumb
external unpack_em_getthumb : message_result -> int = "unpack_em_getthumb"
let issue_em_getthumb ~sender = unpack_em_getthumb (sender ~msg:pack_em_getthumb)

let em_getwordbreakproc = 0x00D1
(* ... *)

let em_limittext = 0x00C5
external pack_em_limittext : limit:int -> message_contents = "pack_em_limittext"
let unpack_em_limittext = ignore
let issue_em_limittext ~sender ~limit = unpack_em_limittext (sender ~msg:(pack_em_limittext ~limit))

let em_linefromchar = 0x00C9
external pack_em_linefromchar : index:int -> message_contents = "pack_em_linefromchar"
external unpack_em_linefromchar : message_result -> int = "unpack_em_linefromchar"
let issue_em_linefromchar ~sender ~index = unpack_em_linefromchar (sender ~msg:(pack_em_linefromchar ~index))

let em_lineindex = 0x00BB
external pack_em_lineindex : line:int -> message_contents = "pack_em_lineindex"
external unpack_em_lineindex : message_result -> int = "unpack_em_lineindex"
let issue_em_lineindex ~sender ~line = unpack_em_lineindex (sender ~msg:(pack_em_lineindex ~line))

let em_linelength = 0x00C1
external pack_em_linelength : line:int -> message_contents = "pack_em_linelength"
external unpack_em_linelength : message_result -> int = "unpack_em_linelength"
let issue_em_linelength ~sender ~line = unpack_em_linelength (sender ~msg:(pack_em_linelength ~line))

let em_linescroll = 0x00B6
external pack_em_linescroll : x:int -> y:int -> message_contents = "pack_em_linescroll"
let unpack_em_linescroll = ignore
let issue_em_linescroll ~sender ~x ~y = unpack_em_linescroll (sender ~msg:(pack_em_linescroll ~x ~y))

let em_posfromchar = 0x00D6
external pack_em_posfromchar : index:int -> message_contents = "pack_em_posfromchar"
external unpack_em_posfromchar : message_result -> point = "unpack_em_posfromchar"
let issue_em_posfromchar ~sender ~index = unpack_em_posfromchar (sender ~msg:(pack_em_posfromchar ~index))

let em_replacesel = 0x00C2
external issue_em_replacesel : sender:message_sender -> can_undo:bool -> str:string -> unit = "issue_em_replacesel"

let em_scroll = 0x00B5
external pack_em_scroll : action:scroll_action -> message_contents = "pack_em_scroll"
external unpack_em_scroll : message_result -> int option = "unpack_em_scroll"
let issue_em_scroll ~sender ~action = unpack_em_scroll (sender ~msg:(pack_em_scroll ~action))

let em_scrollcaret = 0x00B7
let pack_em_scrollcaret = pack_simple_message ~id:em_scrollcaret
let unpack_em_scrollcaret = ignore
let issue_em_scrollcaret ~sender = unpack_em_scrollcaret (sender ~msg:pack_em_scrollcaret)

let em_sethandle = 0x00BC
external pack_em_sethandle : handle:handle -> message_contents = "pack_em_sethandle"
let unpack_em_sethandle = ignore
let issue_em_sethandle ~sender ~handle = unpack_em_sethandle (sender ~msg:(pack_em_sethandle ~handle))

type margin_spec =
    Ms_usefontinfo
  | Ms_explicit of int option * int option
let em_setmargins = 0x00D3
external pack_em_setmargins : spec:margin_spec -> message_contents = "pack_em_setmargins"
let unpack_em_setmargins = ignore
let issue_em_setmargins ~sender ~spec = unpack_em_setmargins (sender ~msg:(pack_em_setmargins ~spec))

let em_setmodify = 0x00B9
external pack_em_setmodify : modified:bool -> message_contents = "pack_em_setmodify"
let unpack_em_setmodify = ignore
let issue_em_setmodify ~sender ~modified = unpack_em_setmodify (sender ~msg:(pack_em_setmodify ~modified))

let em_setpasswordchar = 0x00CC
external pack_em_setpasswordchar : char:char -> message_contents = "pack_em_setpasswordchar"
let unpack_em_setpasswordchar = ignore
let issue_em_setpasswordchar ~sender ~char = unpack_em_setpasswordchar (sender ~msg:(pack_em_setpasswordchar ~char))

let em_setreadonly = 0x00CF
external pack_em_setreadonly : readonly:bool -> message_contents = "pack_em_setreadonly"
external unpack_em_setreadonly : message_result -> bool = "unpack_em_setreadonly"
let issue_em_setreadonly ~sender ~readonly = unpack_em_setreadonly (sender ~msg:(pack_em_setreadonly ~readonly))

let em_setrect = 0x00B3
external issue_em_setrect : sender:message_sender -> rect:rect -> unit = "issue_em_setrect"

let em_setrectnp = 0x00B4
external issue_em_setrectnp : semder:message_sender -> rect:rect -> unit = "issue_em_setrectnp"

let em_setsel = 0x00B1
external pack_em_setsel : start_sel:int -> end_sel:int -> message_contents = "pack_em_setsel"
let unpack_em_setsel = ignore
let issue_em_setsel ~sender ~start_sel ~end_sel = unpack_em_setsel (sender ~msg:(pack_em_setsel ~start_sel ~end_sel))

let em_settabstops = 0x00CB
external issue_em_settabstops : sender:message_sender -> tab_stops:int list -> bool = "issue_em_settabstops"

let em_setwordbreakproc = 0x00D0
(* ... *)

let em_undo = 0x00C7
let pack_em_undo = pack_simple_message ~id:em_undo
external unpack_em_undo : message_result -> bool = "unpack_em_undo"
let issue_em_undo ~sender = unpack_em_undo (sender ~msg:pack_em_undo)

let wm_ctlcoloredit = 0x0133
let on_wm_ctlcoloredit = on_ctlcolor_message ~id:wm_ctlcoloredit
let _ = add_explicit_dialog_return wm_ctlcoloredit

let wm_undo = 0x0304
let pack_wm_undo = pack_simple_message ~id:wm_undo
external unpack_wm_undo : message_result -> bool = "unpack_wm_undo"
let issue_wm_undo ~sender = unpack_wm_undo (sender ~msg:pack_wm_undo)

(* Notifications *)

let en_change = 0x0300
let en_errspace = 0x0500
let en_hscroll = 0x0601
let en_killfocus = 0x0200
let en_maxtext = 0x0501
let en_setfocus = 0x0100
let en_update = 0x0400
let en_vscroll = 0x0602
