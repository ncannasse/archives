(* OCaml-Win32
 * win32_combobox.ml
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
(* User Interface Services: Controls: Combo Boxes *)

external dlg_dir_list_combo_box : wnd:hwnd -> path:string -> combo_id:int -> static_id:int -> options:dlg_dir_list_option list -> string = "dlg_dir_list_combo_box"

external dlg_dir_select_combo_box : wnd:hwnd -> combo_id:int -> (string * bool) = "dlg_dir_select_combo_box"

(* Styles *)

type combobox_style_option =
    CBS_AUTOHSCROLL
  | CBS_DISABLENOSCROLL
  | CBS_DROPDOWN
  | CBS_DROPDOWNLIST
  | CBS_HASSTRINGS
  | CBS_LOWERCASE
  | CBS_NOINTEGRALHEIGHT
  | CBS_OEMCONVERT
  | CBS_OWNERDRAWFIXED
  | CBS_OWNERDRAWVARIABLE
  | CBS_SIMPLE
  | CBS_SORT
  | CBS_UPPERCASE

external combobox_style_of_option : combobox_style_option -> style = "combobox_style_of_option"

let combobox_style_of_options = special_style_of_options ~special_func:combobox_style_of_option

(* Messages *)

let cb_addstring = 0x0143
external issue_cb_addstring : sender:message_sender -> str:string -> int = "issue_cb_addstring"
external pack_cb_addstring_data : data:int32 -> message_contents = "pack_cb_addstring_data"
external unpack_cb_addstring_data : message_result -> int = "unpack_cb_addstring_data"
let issue_cb_addstring_data ~sender ~data = unpack_cb_addstring_data (sender ~msg:(pack_cb_addstring_data ~data))

let cb_deletestring = 0x0144
external pack_cb_deletestring : index:int -> message_contents = "pack_cb_deletestring"
external unpack_cb_deletestring : message_result -> int = "unpack_cb_deletestring"
let issue_cb_deletestring ~sender ~index = unpack_cb_deletestring (sender ~msg:(pack_cb_deletestring ~index))

let cb_dir = 0x0145
external issue_cb_dir : sender:message_sender -> filename:string -> options:dlg_dir_list_option list -> int = "issue_cb_dir"

let cb_findstring = 0x014C
external issue_cb_findstring : sender:message_sender -> start:int -> str:string -> int option = "issue_cb_findstring"
external pack_cb_findstring_data : start:int -> data:int32 -> message_contents = "pack_cb_findstring_data"
external unpack_cb_findstring_data : message_result -> int option = "unpack_cb_findstring_data"
let issue_cb_findstring_data ~sender ~start ~data = unpack_cb_findstring_data (sender ~msg:(pack_cb_findstring_data ~start ~data))

let cb_findstringexact = 0x0158
external issue_cb_findstringexact : sender:message_sender -> start:int -> str:string -> int option = "issue_cb_findstringexact"
external pack_cb_findstringexact_data : start:int -> data:int32 -> message_contents = "pack_cb_findstringexact_data"
external unpack_cb_findstringexact_data : message_result -> int option = "unpack_cb_findstringexact_data"
let issue_cb_findstringexact_data ~sender ~start ~data = unpack_cb_findstringexact_data (sender ~msg:(pack_cb_findstringexact_data ~start ~data))

let cb_getcount = 0x0146
let pack_cb_getcount = pack_simple_message ~id:cb_getcount
external unpack_cb_getcount : message_result -> int = "unpack_cb_getcount"
let issue_cb_getcount ~sender = unpack_cb_getcount (sender ~msg:pack_cb_getcount)

let cb_getcursel = 0x0147
let pack_cb_getcursel = pack_simple_message ~id:cb_getcursel
external unpack_cb_getcursel : message_result -> int option = "unpack_cb_getcursel"
let issue_cb_getcursel ~sender = unpack_cb_getcursel (sender ~msg:pack_cb_getcursel)

let cb_getdroppedcontrolrect = 0x0152
external issue_cb_getdroppedcontrolrect : sender:message_sender -> rect = "issue_cb_getdroppedcontrolrect"

let cb_getdroppedstate = 0x0157
let pack_cb_getdroppedstate = pack_simple_message ~id:cb_getdroppedstate
external unpack_cb_getdroppedstate : message_result -> bool = "unpack_cb_getdroppedstate"
let issue_cb_getdroppedstate ~sender = unpack_cb_getdroppedstate (sender ~msg:pack_cb_getdroppedstate)

let cb_getdroppedwidth = 0x015f
let pack_cb_getdroppedwidth = pack_simple_message ~id:cb_getdroppedwidth
external unpack_cb_getdroppedwidth : message_result -> int = "unpack_cb_getdroppedwidth"
let issue_cb_getdroppedwidth ~sender = unpack_cb_getdroppedwidth (sender ~msg:pack_cb_getdroppedwidth)

let cb_geteditsel = 0x0140
external issue_cb_geteditsel : sender:message_sender -> (int * int) = "issue_cb_geteditsel"

let cb_getextendedui = 0x0156
let pack_cb_getextendedui = pack_simple_message ~id:cb_getextendedui
external unpack_cb_getextendedui : message_result -> bool = "unpack_cb_getextendedui"
let issue_cb_getextendedui ~sender = unpack_cb_getextendedui (sender ~msg:pack_cb_getextendedui)

let cb_gethorizontalextent = 0x015d
let pack_cb_gethorizontalextent = pack_simple_message ~id:cb_gethorizontalextent
external unpack_cb_gethorizontalextent : message_result -> int = "unpack_cb_gethorizontalextent"
let issue_cb_gethorizontalextent ~sender = unpack_cb_gethorizontalextent (sender ~msg:pack_cb_gethorizontalextent)

let cb_getitemdata = 0x0150
external pack_cb_getitemdata : index:int -> message_contents = "pack_cb_getitemdata"
external unpack_cb_getitemdata : message_result -> int32 = "unpack_cb_getitemdata"
let issue_cb_getitemdata ~sender ~index = unpack_cb_getitemdata (sender ~msg:(pack_cb_getitemdata ~index))

let cb_getitemheight = 0x0154
external pack_cb_getitemheight : index:int -> message_contents = "pack_cb_getitemheight"
external unpack_cb_getitemheight : message_result -> int = "unpack_cb_getitemheight"
let issue_cb_getitemheight ~sender ~index = unpack_cb_getitemheight (sender ~msg:(pack_cb_getitemheight ~index))

let cb_getlbtext = 0x0148
external issue_cb_getlbtext : sender:message_sender -> index:int -> max_length:int -> string = "issue_cb_getlbtext"

let cb_getlbtextlen = 0x0149
external pack_cb_getlbtextlen : index:int -> message_contents = "pack_cb_getlbtextlen"
external unpack_cb_getlbtextlen : message_result -> int = "unpack_cb_getlbtextlen"
let issue_cb_getlbtextlen ~sender ~index = unpack_cb_getlbtextlen (sender ~msg:(pack_cb_getlbtextlen ~index))

let cb_getlocale = 0x015A
(* ... *)

let cb_gettopindex = 0x015b
let pack_cb_gettopindex = pack_simple_message ~id:cb_gettopindex
external unpack_cb_gettopindex : message_result -> int = "unpack_cb_gettopindex"
let issue_cb_gettopindex ~sender = unpack_cb_gettopindex (sender ~msg:pack_cb_gettopindex)

let cb_initstorage = 0x0161
external pack_cb_initstorage : count:int -> size:int -> message_contents = "pack_cb_initstorage"
external unpack_cb_initstorage : message_result -> int = "unpack_cb_initstorage"
let issue_cb_initstorage ~sender ~count ~size = unpack_cb_initstorage (sender ~msg:(pack_cb_initstorage ~count ~size))

let cb_insertstring = 0x014A
external issue_cb_insertstring : sender:message_sender -> index:int -> str:string -> int = "issue_cb_insertstring"
external pack_cb_insertstring_data : index:int -> data:int32 -> message_contents = "pack_cb_insertstring_data"
external unpack_cb_insertstring_data : message_result -> int = "unpack_cb_insertstring_data"
let issue_cb_insertstring_data ~sender ~index ~data = unpack_cb_insertstring_data (sender ~msg:(pack_cb_insertstring_data ~index ~data))

let cb_limittext = 0x0141
external pack_cb_limittext : limit:int -> message_contents = "pack_cb_limittext"
let unpack_cb_limittext = ignore
let issue_cb_limittext ~sender ~limit = unpack_cb_limittext (sender ~msg:(pack_cb_limittext ~limit))

let cb_resetcontent = 0x014B
let pack_cb_resetcontent = pack_simple_message ~id:cb_resetcontent
let unpack_cb_resetcontent = ignore
let issue_cb_resetcontent ~sender = unpack_cb_resetcontent (sender ~msg:pack_cb_resetcontent)

let cb_selectstring = 0x014D
external issue_cb_selectstring : sender:message_sender -> start:int -> str:string -> int option = "issue_cb_selectstring"
external pack_cb_selectstring_data : start:int -> data:int32 -> message_contents = "pack_cb_selectstring_data"
external unpack_cb_selectstring_data : message_result -> int option = "unpack_cb_selectstring_data"
let issue_cb_selectstring_data ~sender ~start ~data = unpack_cb_selectstring_data (sender ~msg:(pack_cb_selectstring_data ~start ~data))

let cb_setcursel = 0x014E
external pack_cb_setcursel : index:int -> message_contents = "pack_cb_setcursel"
let unpack_cb_setcursel = ignore
let issue_cb_setcursel ~sender ~index = unpack_cb_setcursel (sender ~msg:(pack_cb_setcursel ~index))

let cb_setdroppedwidth = 0x0160
external pack_cb_setdroppedwidth : width:int -> message_contents = "pack_cb_setdroppedwidth"
external unpack_cb_setdroppedwidth : message_result -> unit = "unpack_cb_setdroppedwidth"
let issue_cb_setdroppedwidth ~sender ~width = unpack_cb_setdroppedwidth (sender ~msg:(pack_cb_setdroppedwidth ~width))

let cb_seteditsel = 0x0142
external pack_cb_seteditsel : start_sel:int -> end_sel:int -> message_contents = "pack_cb_seteditsel"
external unpack_cb_seteditsel : message_result -> unit = "unpack_cb_seteditsel"
let issue_cb_seteditsel ~sender ~start_sel ~end_sel = unpack_cb_seteditsel (sender ~msg:(pack_cb_seteditsel ~start_sel ~end_sel))

let cb_setextendedui = 0x0155
external pack_cb_setextendedui : extended:bool -> message_contents = "pack_cb_setextendedui"
external unpack_cb_setextendedui : message_result -> unit = "unpack_cb_setextendedui"
let issue_cb_setextendedui ~sender ~extended = unpack_cb_setextendedui (sender ~msg:(pack_cb_setextendedui ~extended))

let cb_sethorizontalextent = 0x015e
external pack_cb_sethorizontalextent : extent:int -> message_contents = "pack_cb_sethorizontalextent"
let unpack_cb_sethorizontalextent = ignore
let issue_cb_sethorizontalextent ~sender ~extent = unpack_cb_sethorizontalextent (sender ~msg:(pack_cb_sethorizontalextent ~extent))

let cb_setitemdata = 0x0151
external pack_cb_setitemdata : index:int -> data:int32 -> message_contents = "pack_cb_setitemdata"
external unpack_cb_setitemdata : message_result -> unit = "unpack_cb_setitemdata"
let issue_cb_setitemdata ~sender ~index ~data = unpack_cb_setitemdata (sender ~msg:(pack_cb_setitemdata ~index ~data))

let cb_setitemheight = 0x0153
external pack_cb_setitemheight : index:int -> height:int -> message_contents = "pack_cb_setitemheight"
external unpack_cb_setitemheight : message_result -> unit = "unpack_cb_setitemheight"
let issue_cb_setitemheight ~sender ~index ~height = unpack_cb_setitemheight (sender ~msg:(pack_cb_setitemheight ~index ~height))

let cb_setlocale = 0x0159
(* ... *)

let cb_settopindex = 0x015c
external pack_cb_settopindex : index:int -> message_contents = "pack_cb_settopindex"
external unpack_cb_settopindex : message_result -> unit = "unpack_cb_settopindex"
let issue_cb_settopindex ~sender ~index = unpack_cb_settopindex (sender ~msg:(pack_cb_settopindex ~index))

let cb_showdropdown = 0x014F
external pack_cb_showdropdown : show:bool -> message_contents = "pack_cb_showdropdown"
let unpack_cb_showdropdown = ignore
let issue_cb_showdropdown ~sender ~show = unpack_cb_showdropdown (sender ~msg:(pack_cb_showdropdown ~show))

type compareitemstruct = {
    cis_control_type : control_type;
    cis_control_id : int;
    cis_control : hwnd;
    cis_item_1_id : int;
    cis_item_1_data : int32;
    cis_item_2_id : int;
    cis_item_2_data : int32 }
let wm_compareitem = 0x0039
type wm_compareitem_handler =
    wnd:hwnd ->
    msg:message_contents ->
    cis:compareitemstruct ->
    int message_handler_result
external deliver_wm_compareitem :
    handler:wm_compareitem_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    int message_handler_result = "deliver_wm_compareitem"
external return_wm_compareitem : result:int -> message_result = "return_wm_compareitem"
let on_wm_compareitem = on_message ~id:wm_compareitem ~deliver:deliver_wm_compareitem ~return:return_wm_compareitem
let _ = add_explicit_dialog_return ~id:wm_compareitem

type owner_draw_action =
    ODA_DRAWENTIRE
  | ODA_FOCUS
  | ODA_SELECT
type owner_draw_state =
    ODS_CHECKED
  | ODS_COMBOBOXEDIT
  | ODS_DEFAULT
  | ODS_DISABLED
  | ODS_FOCUS
  | ODS_GRAYED
  | ODS_SELECTED
type drawitemstruct = {
    dis_control_type : control_type;
    dis_control_id : int;
    dis_item_id : int;
    dis_item_action : owner_draw_action list;
    dis_item_state : owner_draw_state list;
    dis_control : hwnd;
    dis_hdc : hdc;
    dis_item_rect : rect;
    dis_item_data : int32 }
let wm_drawitem = 0x002B
type wm_drawitem_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dis:drawitemstruct ->
    unit message_handler_result
external deliver_wm_drawitem :
    handler:wm_drawitem_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_drawitem"
external return_wm_drawitem : result:unit -> message_result = "return_wm_drawitem"
let on_wm_drawitem = on_message ~id:wm_drawitem ~deliver:deliver_wm_drawitem ~return:return_wm_drawitem

type measureitemstruct = {
    mis_control_type : control_type;
    mis_control_id : int;
    mis_item_id : int;
    mis_item_data : int32 }
let wm_measureitem = 0x002C
type wm_measureitem_handler =
    wnd:hwnd ->
    msg:message_contents ->
    mis:measureitemstruct ->
    size message_handler_result
external on_wm_measureitem :
    wm_measureitem_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    message_result option = "on_wm_measureitem"

(* Notifications *)

let cbn_closeup = 8
let cbn_dblclk = 2
let cbn_dropdown = 7
let cbn_editchange = 5
let cbn_editupdate = 6
let cbn_killfocus = 4
let cbn_selchange = 1
let cbn_selendcancel = 10
let cbn_selendok = 9
let cbn_setfocus = 3
