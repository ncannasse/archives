(* OCaml-Win32
 * win32_listbox.ml
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
(* User Interface Services: Controls: List Boxes *)

external dlg_dir_list : wnd:hwnd -> path:string -> list_id:int -> static_id:int -> options:dlg_dir_list_option list -> string = "dlg_dir_list"

external dlg_dir_select : wnd:hwnd -> list_id:int -> (string * bool) = "dlg_dir_select"

(* Styles *)

type listbox_style_option =
    LBS_DISABLENOSCROLL
  | LBS_EXTENDEDSEL
  | LBS_HASSTRINGS
  | LBS_MULTICOLUMN
  | LBS_MULTIPLESEL
  | LBS_NODATA
  | LBS_NOINTEGRALHEIGHT
  | LBS_NOREDRAW
  | LBS_NOSEL
  | LBS_NOTIFY
  | LBS_OWNERDRAWFIXED
  | LBS_OWNERDRAWVARIABLE
  | LBS_SORT
  | LBS_STANDARD
  | LBS_USETABSTOPS
  | LBS_WANTKEYBOARDINPUT

external listbox_style_of_option : listbox_style_option -> style = "listbox_style_of_option"

let listbox_style_of_options = special_style_of_options ~special_func:listbox_style_of_option

(* Messages *)

let lb_addfile = 0x0196
external issue_lb_addfile : sender:message_sender -> filename:string -> int = "issue_lb_addfile"

let lb_addstring = 0x0180
external issue_lb_addstring : sender:message_sender -> str:string -> int = "issue_lb_addstring"
external pack_lb_addstring_data : data:int32 -> message_contents = "pack_lb_addstring_data"
external unpack_lb_addstring_data : message_result -> int = "unpack_lb_addstring_data"
let issue_lb_addstring_data ~sender ~data = unpack_lb_addstring_data (sender ~msg:(pack_lb_addstring_data ~data))

let lb_deletestring = 0x0182
external pack_lb_deletestring : index:int -> message_contents = "pack_lb_deletestring"
external unpack_lb_deletestring : message_result -> int = "unpack_lb_deletestring"
let issue_lb_deletestring ~sender ~index = unpack_lb_deletestring (sender ~msg:(pack_lb_deletestring ~index))

let lb_dir = 0x018D
external issue_lb_dir : sender:message_sender -> filename:string -> options:dlg_dir_list_option list -> int = "issue_lb_dir"

let lb_findstring = 0x018F
external issue_lb_findstring : sender:message_sender -> start:int -> str:string -> int option = "issue_lb_findstring"
external pack_lb_findstring_data : start:int -> data:int32 -> message_contents = "pack_lb_findstring_data"
external unpack_lb_findstring_data : message_result -> int option = "unpack_lb_findstring_data"
let issue_lb_findstring_data ~sender ~start ~data = unpack_lb_findstring_data (sender ~msg:(pack_lb_findstring_data ~start ~data))

let lb_findstringexact = 0x01A2
external issue_lb_findstringexact : sender:message_sender -> start:int -> str:string -> int option = "issue_lb_findstringexact"
external pack_lb_findstringexact_data : start:int -> data:int32 -> message_contents = "pack_lb_findstringexact_data"
external unpack_lb_findstringexact_data : message_result -> int option = "unpack_lb_findstringexact_data"
let issue_lb_findstringexact_data ~sender ~start ~data = unpack_lb_findstringexact_data (sender ~msg:(pack_lb_findstringexact_data ~start ~data))

let lb_getanchorindex = 0x019D
let pack_lb_getanchorindex = pack_simple_message ~id:lb_getanchorindex
external unpack_lb_getanchorindex : message_result -> int = "unpack_lb_getanchorindex"
let issue_lb_getanchorindex ~sender = unpack_lb_getanchorindex (sender ~msg:pack_lb_getanchorindex)

let lb_getcaretindex = 0x019F
let pack_lb_getcaretindex = pack_simple_message ~id:lb_getcaretindex
external unpack_lb_getcaretindex : message_result -> int = "unpack_lb_getcaretindex"
let issue_lb_getcaretindex ~sender = unpack_lb_getcaretindex (sender ~msg:pack_lb_getcaretindex)

let lb_getcount = 0x018B
let pack_lb_getcount = pack_simple_message ~id:lb_getcount
external unpack_lb_getcount : message_result -> int = "unpack_lb_getcount"
let issue_lb_getcount ~sender = unpack_lb_getcount (sender ~msg:pack_lb_getcount)

let lb_getcursel = 0x0188
let pack_lb_getcursel = pack_simple_message ~id:lb_getcursel
external unpack_lb_getcursel : message_result -> int option = "unpack_lb_getcursel"
let issue_lb_getcursel ~sender = unpack_lb_getcursel (sender ~msg:pack_lb_getcursel)

let lb_gethorizontalextent = 0x0193
let pack_lb_gethorizontalextent = pack_simple_message ~id:lb_gethorizontalextent
external unpack_lb_gethorizontalextent : message_result -> int = "unpack_lb_gethorizontalextent"
let issue_lb_gethorizontalextent ~sender = unpack_lb_gethorizontalextent (sender ~msg:pack_lb_gethorizontalextent)

let lb_getitemdata = 0x0199
external pack_lb_getitemdata : index:int -> message_contents = "pack_lb_getitemdata"
external unpack_lb_getitemdata : message_result -> int32 = "unpack_lb_getitemdata"
let issue_lb_getitemdata ~sender ~index = unpack_lb_getitemdata (sender ~msg:(pack_lb_getitemdata ~index))

let lb_getitemheight = 0x01A1
external pack_lb_getitemheight : index:int -> message_contents = "pack_lb_getitemheight"
external unpack_lb_getitemheight : message_result -> int = "unpack_lb_getitemheight"
let issue_lb_getitemheight ~sender ~index = unpack_lb_getitemheight (sender ~msg:(pack_lb_getitemheight ~index))

let lb_getitemrect = 0x0198
external issue_lb_getitemrect : sender:message_sender -> index:int -> rect = "issue_lb_getitemrect"

let lb_getlocale = 0x01A6
(* ... *)

let lb_getsel = 0x0187
external pack_lb_getsel : index:int -> message_contents = "pack_lb_getsel"
external unpack_lb_getsel : message_result -> bool = "unpack_lb_getsel"
let issue_lb_getsel ~sender ~index = unpack_lb_getsel (sender ~msg:(pack_lb_getsel ~index))

let lb_getselcount = 0x0190
let pack_lb_getselcount = pack_simple_message ~id:lb_getselcount
external unpack_lb_getselcount : message_result -> int = "unpack_lb_getselcount"
let issue_lb_getselcount ~sender = unpack_lb_getselcount (sender ~msg:pack_lb_getselcount)

let lb_getselitems = 0x0191
external issue_lb_getselitems : sender:message_sender -> max_count:int -> int array = "issue_lb_getselitems"

let lb_gettext = 0x0189
external issue_lb_gettext : sender:message_sender -> index:int -> max_length:int -> string = "issue_lb_gettext"

let lb_gettextlen = 0x018A
external pack_lb_gettextlen : index:int -> message_contents = "pack_lb_gettextlen"
external unpack_lb_gettextlen : message_result -> int = "unpack_lb_gettextlen"
let issue_lb_gettextlen ~sender ~index = unpack_lb_gettextlen (sender ~msg:(pack_lb_gettextlen ~index))

let lb_gettopindex = 0x018E
let pack_lb_gettopindex = pack_simple_message ~id:lb_gettopindex
external unpack_lb_gettopindex : message_result -> int = "unpack_lb_gettopindex"
let issue_lb_gettopindex ~sender = unpack_lb_gettopindex (sender ~msg:pack_lb_gettopindex)

let lb_initstorage = 0x01A8
external pack_lb_initstorage : count:int -> size:int -> message_contents = "pack_lb_initstorage"
external unpack_lb_initstorage : message_result -> int = "unpack_lb_initstorage"
let issue_lb_initstorage ~sender ~count ~size = unpack_lb_initstorage (sender ~msg:(pack_lb_initstorage ~count ~size))

let lb_insertstring = 0x0181
external issue_lb_insertstring : sender:message_sender -> index:int -> str:string -> int = "issue_lb_insertstring"
external pack_lb_insertstring_data : index:int -> data:int32 -> message_contents = "pack_lb_insertstring_data"
external unpack_lb_insertstring_data : message_result -> int = "unpack_lb_insertstring_data"
let issue_lb_insertstring_data ~sender ~index ~data = unpack_lb_insertstring_data (sender ~msg:(pack_lb_insertstring_data ~index ~data))

let lb_itemfrompoint = 0x01A9
external pack_lb_itemfrompoint : pt:point -> message_contents = "pack_lb_itemfrompoint"
external unpack_lb_itemfrompoint : message_result -> (int * bool) = "unpack_lb_itemfrompoint"
let issue_lb_itemfrompoint ~sender ~pt = unpack_lb_itemfrompoint (sender ~msg:(pack_lb_itemfrompoint ~pt))

let lb_resetcontent = 0x0184
let pack_lb_resetcontent = pack_simple_message ~id:lb_resetcontent
let unpack_lb_resetcontent = ignore
let issue_lb_resetcontent ~sender = unpack_lb_resetcontent (sender ~msg:pack_lb_resetcontent)

let lb_selectstring = 0x018C
external issue_lb_selectstring : sender:message_sender -> start:int -> str:string -> int option = "issue_lb_selectstring"
external pack_lb_selectstring_data : start:int -> data:int32 -> message_contents = "pack_lb_selectstring_data"
external unpack_lb_selectstring_data : message_result -> int option = "unpack_lb_selectstring_data"
let issue_lb_selectstring_data ~sender ~start ~data = unpack_lb_selectstring_data (sender ~msg:(pack_lb_selectstring_data ~start ~data))

let lb_selitemrange = 0x019B
external pack_lb_selitemrange : first:int -> last:int -> select:bool -> message_contents = "pack_lb_selitemrange"
external unpack_lb_selitemrange : message_result -> unit = "unpack_lb_selitemrange"
let issue_lb_selitemrange ~sender ~first ~last ~select = unpack_lb_selitemrange (sender ~msg:(pack_lb_selitemrange ~first ~last ~select))

let lb_selitemrangeex = 0x0183
external pack_lb_selitemrangeex : first:int -> last:int -> message_contents = "pack_lb_selitemrangeex"
external unpack_lb_selitemrangeex : message_result -> unit = "unpack_lb_selitemrangeex"
let issue_lb_selitemrangeex ~sender ~first ~last = unpack_lb_selitemrangeex (sender ~msg:(pack_lb_selitemrangeex ~first ~last))

let lb_setanchorindex = 0x019C
external pack_lb_setanchorindex : index:int -> message_contents = "pack_lb_setanchorindex"
external unpack_lb_setanchorindex : message_result -> unit = "unpack_lb_setanchorindex"
let issue_lb_setanchorindex ~sender ~index = unpack_lb_setanchorindex (sender ~msg:(pack_lb_setanchorindex ~index))

let lb_setcaretindex = 0x019E
external pack_lb_setcaretindex : index:int -> partial_scroll:bool -> message_contents = "pack_lb_setcaretindex"
external unpack_lb_setcaretindex : message_result -> unit = "unpack_lb_setcaretindex"
let issue_lb_setcaretindex ~sender ~index ~partial_scroll = unpack_lb_setcaretindex (sender ~msg:(pack_lb_setcaretindex ~index ~partial_scroll))

let lb_setcolumnwidth = 0x0195
external pack_lb_setcolumnwidth : width:int -> message_contents = "pack_lb_setcolumnwidth"
let unpack_lb_setcolumnwidth = ignore
let issue_lb_setcolumnwidth ~sender ~width = unpack_lb_setcolumnwidth (sender ~msg:(pack_lb_setcolumnwidth ~width))

let lb_setcount = 0x01A7
external pack_lb_setcount : count:int -> message_contents = "pack_lb_setcount"
external unpack_lb_setcount : message_result -> unit = "unpack_lb_setcount"
let issue_lb_setcount ~sender ~count = unpack_lb_setcount (sender ~msg:(pack_lb_setcount ~count))

let lb_setcursel = 0x0186
external pack_lb_setcursel : index:int -> message_contents = "pack_lb_setcursel"
let unpack_lb_setcursel = ignore
let issue_lb_setcursel ~sender ~index = unpack_lb_setcursel (sender ~msg:(pack_lb_setcursel ~index))

let lb_sethorizontalextent = 0x0194
external pack_lb_sethorizontalextent : extent:int -> message_contents = "pack_lb_sethorizontalextent"
let unpack_lb_sethorizontalextent = ignore
let issue_lb_sethorizontalextent ~sender ~extent = unpack_lb_sethorizontalextent (sender ~msg:(pack_lb_sethorizontalextent ~extent))

let lb_setitemdata = 0x019A
external pack_lb_setitemdata : index:int -> data:int32 -> message_contents = "pack_lb_setitemdata"
external unpack_lb_setitemdata : message_result -> unit = "unpack_lb_setitemdata"
let issue_lb_setitemdata ~sender ~index ~data = unpack_lb_setitemdata (sender ~msg:(pack_lb_setitemdata ~index ~data))

let lb_setitemheight = 0x01A0
external pack_lb_setitemheight : index:int -> height:int -> message_contents = "pack_lb_setitemheight"
external unpack_lb_setitemheight : message_result -> unit = "unpack_lb_setitemheight"
let issue_lb_setitemheight ~sender ~index ~height = unpack_lb_setitemheight (sender ~msg:(pack_lb_setitemheight ~index ~height))

let lb_setlocale = 0x01A5
(* ... *)

let lb_setsel = 0x0185
external pack_lb_setsel : index:int -> select:bool -> message_contents = "pack_lb_setsel"
external unpack_lb_setsel : message_result -> unit = "unpack_lb_setsel"
let issue_lb_setsel ~sender ~index ~select = unpack_lb_setsel (sender ~msg:(pack_lb_setsel ~index ~select))

let lb_settabstops = 0x0192
external issue_lb_settabstops : sender:message_sender -> tab_stops:int list -> bool = "issue_lb_settabstops"

let lb_settopindex = 0x0197
external pack_lb_settopindex : index:int -> message_contents = "pack_lb_settopindex"
external unpack_lb_settopindex : message_result -> unit = "unpack_lb_settopindex"
let issue_lb_settopindex ~sender ~index = unpack_lb_settopindex (sender ~msg:(pack_lb_settopindex ~index))

let wm_chartoitem = 0x002F
(* ... explicit dialog return *)

let wm_ctlcolorlistbox = 0x0134
let on_wm_ctlcolorlistbox = on_ctlcolor_message ~id:wm_ctlcolorlistbox
let _ = add_explicit_dialog_return ~id:wm_ctlcolorlistbox

type deleteitemstruct = {
    delis_control_type : control_type;
    delis_control_id : int;
    delis_item_id : int;
    delis_control : hwnd;
    delis_item_data : int32 }
let wm_deleteitem = 0x002D
type wm_deleteitem_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dis:deleteitemstruct ->
    unit message_handler_result
external deliver_wm_deleteitem :
    handler:wm_deleteitem_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_deleteitem"
let on_wm_deleteitem = on_message ~id:wm_deleteitem ~deliver:deliver_wm_deleteitem ~return:return_simple_message

let wm_vkeytoitem = 0x002E
(* ... explicit dialog return *)

(* Notifications *)

let lbn_dblclk = 2
let lbn_errspace = -2
let lbn_killfocus = 5
let lbn_selcancel = 3
let lbn_selchange = 1
let lbn_setfocus = 4
