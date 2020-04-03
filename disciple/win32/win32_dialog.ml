(* OCaml-Win32
 * win32_dialog.ml
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
(* User Interface Services: Windowing: Dialog Boxes *)

(* Dialog templates *)

type dialog_style_option =
    DS_3DLOOK
  | DS_ABSALIGN
  | DS_CENTER
  | DS_CENTERMOUSE
  | DS_CONTEXTHELP
  | DS_CONTROL
  | DS_FIXEDSYS
  | DS_MODALFRAME
  | DS_NOFAILCREATE
  | DS_NOIDLEMSG
  | DS_SETFOREGROUND
  | DS_SYSMODAL

external dialog_style_of_option : dialog_style_option -> style = "dialog_style_of_option"

let dialog_style_of_options = special_style_of_options ~special_func:dialog_style_of_option

type dialog_font_info = {
    dfi_point_size : int;
    dfi_weight : int;
    dfi_italic : bool;
    dfi_char_set : char_set;
    dfi_face_name : string }

let default_dialog_font_info = {
    dfi_point_size = 0;
    dfi_weight = fw_dontcare;
    dfi_italic = false;
    dfi_char_set = Default_charset;
    dfi_face_name = "" }

type dlgitemtemplate = {
    dit_help_id : int;
    dit_style : style;
    dit_position : point;
    dit_size : size;
    dit_id : int;
    dit_class : atom_name;
    dit_title : resource_name;
    dit_creation_data : buffer }

let default_dlgitemtemplate = {
    dit_help_id = 0;
    dit_style = Int64.zero;
    dit_position = { x = 0; y = 0 };
    dit_size = { cx = 0; cy = 0 };
    dit_id = 0;
    dit_class = An_string "";
    dit_title = Rn_string "";
    dit_creation_data = "" }

type dlgtemplate = {
    dt_version : int;
    dt_help_id : int;
    dt_style : style;
    dt_position : point;
    dt_size : size;
    dt_menu : resource_name option;
    dt_class : atom_name option;
    dt_title : string;
    dt_font : dialog_font_info option;
    dt_items : dlgitemtemplate list }

let default_dlgtemplate = {
    dt_version = 1;
    dt_help_id = 0;
    dt_style = Int64.zero;
    dt_position = { x = 0; y = 0 };
    dt_size = { cx = 0; cy = 0 };
    dt_menu = None;
    dt_class = None;
    dt_title = "";
    dt_font = None;
    dt_items = [] }

external pack_dlgtemplate : template:dlgtemplate -> buffer = "pack_dlgtemplate"
external unpack_dlgtemplate : buf:raw -> dlgtemplate = "unpack_dlgtemplate"

(* Functions *)

external create_dialog : inst:hinstance -> name:resource_name -> parent:hwnd -> proc:window_proc -> hwnd = "create_dialog"

external create_dialog_indirect : inst:hinstance -> template:dlgtemplate -> parent:hwnd -> proc:window_proc -> hwnd = "create_dialog_indirect"

external create_dialog_indirect_param : inst:hinstance -> template:dlgtemplate -> parent:hwnd -> proc:pointer -> param:int32 -> hwnd = "create_dialog_indirect_param"

external create_dialog_param : inst:hinstance -> name:resource_name -> parent:hwnd -> proc:pointer -> param:int32 -> hwnd = "create_dialog_param"

external def_dlg_proc : dlg:hwnd -> msg:message_contents -> message_result = "def_dlg_proc"

external dialog_box : inst:hinstance -> name:resource_name -> parent:hwnd -> proc:window_proc -> int = "dialog_box"

external dialog_box_indirect : inst:hinstance -> template:dlgtemplate -> parent:hwnd -> proc:window_proc -> int = "dialog_box_indirect"

external dialog_box_indirect_param : inst:hinstance -> template:dlgtemplate -> parent:hwnd -> proc:pointer -> param:int32 -> int = "dialog_box_indirect_param"

external dialog_box_param : inst:hinstance -> name:resource_name -> parent:hwnd -> proc:pointer -> param:int32 -> int = "dialog_box_param"

external end_dialog : dlg:hwnd -> result:int -> unit = "end_dialog"

external get_dialog_base_units : unit -> size = "get_dialog_base_units"

external get_dlg_ctrl_id : ctrl:hwnd -> int = "get_dlg_ctrl_id"

external get_dlg_item : dlg:hwnd -> id:int -> hwnd = "get_dlg_item"

external get_dlg_item_int : dlg:hwnd -> id:int -> is_signed:bool -> int64 = "get_dlg_item_int"

external get_dlg_item_text : dlg:hwnd -> id:int -> max_length:int -> string = "get_dlg_item_text"

external get_next_dlg_group_item : dlg:hwnd -> ctrl:hwnd -> previous:bool -> hwnd = "get_next_dlg_group_item"

external get_next_dlg_tab_item : dlg:hwnd -> ctrl:hwnd -> previous:bool -> hwnd = "get_next_dlg_tab_item"

external is_dialog_message : dlg:hwnd -> msg:msg -> bool = "is_dialog_message"

external map_dialog_rect : dlg:hwnd -> rect:rect -> rect = "map_dialog_rect"

external message_box :
    wnd:hwnd ->
    text:string ->
    caption:string ->
    options:message_box_option list ->
    standard_control_id = "message_box"

(* ... message_box_ex, message_box_indirect *)

external send_dlg_item_message : dlg:hwnd -> id:int -> msg:message_contents -> message_result = "send_dlg_item_message"
let send_dlg_item ~dlg ~id ~issuer = issuer ~sender:(send_dlg_item_message ~dlg ~id)

external set_dlg_item_int : dlg:hwnd -> id:int -> value:int64 -> unit = "set_dlg_item_int"

external set_dlg_item_text : dlg:hwnd -> id:int -> text:string -> unit = "set_dlg_item_text"

(* Messages *)

let dm_getdefid = wm_user + 0
type dm_getdefid_handler = int option plain_message_handler
external return_dm_getdefid : result:int option -> message_result = "return_dm_getdefid"
let on_dm_getdefid = on_message ~id:dm_getdefid ~deliver:deliver_plain_message ~return:return_dm_getdefid
let pack_dm_getdefid = pack_simple_message ~id:dm_getdefid
external unpack_dm_getdefid : message_result -> int option = "unpack_dm_getdefid"
let issue_dm_getdefid ~sender = unpack_dm_getdefid (sender ~msg:pack_dm_getdefid)

let dm_reposition = wm_user + 2
type dm_reposition_handler = simple_message_handler
let on_dm_reposition = on_simple_message ~id:dm_reposition
let pack_dm_reposition = pack_simple_message ~id:dm_reposition
let unpack_dm_reposition = ignore
let issue_dm_reposition ~sender = unpack_dm_reposition (sender ~msg:pack_dm_reposition)

let dm_setdefid = wm_user + 1
type dm_setdefid_handler =
    wnd:hwnd ->
    msg:message_contents ->
    id:int ->
    unit message_handler_result
external deliver_dm_setdefid :
    handler:dm_setdefid_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_dm_setdefid"
let on_dm_setdefid = on_message ~id:dm_setdefid ~deliver:deliver_dm_setdefid ~return:return_simple_message
external pack_dm_setdefid : id:int -> message_contents = "pack_dm_setdefid"
let unpack_dm_setdefid = ignore
let issue_dm_setdefid ~sender ~id = unpack_dm_setdefid (sender ~msg:(pack_dm_setdefid ~id))

let wm_ctlcolordlg = 0x0136
let on_ctlcolordlg = on_ctlcolor_message ~id:wm_ctlcolordlg
let _ = add_explicit_dialog_return wm_ctlcolordlg

let wm_enteridle = 0x0121
type wm_enteridle_handler =
    wnd:hwnd ->
    msg:message_contents ->
    source:input_event_type ->
    hwnd message_handler_result
external deliver_wm_enteridle :
    handler:wm_enteridle_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_enteridle"
let on_wm_enteridle = on_message ~id:wm_enteridle ~deliver:deliver_wm_enteridle ~return:return_simple_message

let wm_getdlgcode = 0x0087
type wm_getdlgcode_handler =
    wnd:hwnd ->
    msg:message_contents ->
    full_msg:msg option ->
    dialog_code list message_handler_result
external deliver_wm_getdlgcode :
    handler:wm_getdlgcode_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    dialog_code list message_handler_result = "deliver_wm_getdlgcode"
external return_wm_getdlgcode : result:dialog_code list -> message_result = "return_wm_getdlgcode"
let on_wm_getdlgcode = on_message ~id:wm_getdlgcode ~deliver:deliver_wm_getdlgcode ~return:return_wm_getdlgcode

let wm_initdialog = 0x0110
type wm_initdialog_handler =
    wnd:hwnd -> msg:message_contents -> focus:hwnd -> bool message_handler_result
external deliver_wm_initdialog :
    handler:wm_initdialog_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_initdialog"
external return_wm_initdialog : result:bool -> message_result = "return_wm_initdialog"
let on_wm_initdialog = on_message ~id:wm_initdialog ~deliver:deliver_wm_initdialog ~return:return_wm_initdialog
let _ = add_explicit_dialog_return wm_initdialog

type nextdlgctl_param =
    Ndcp_control of hwnd
  | Ndcp_previous
  | Ndcp_next
let wm_nextdlgctl = 0x0028
type wm_nextdlgctl_handler =
    wnd:hwnd ->
    msg:message_contents ->
    param:nextdlgctl_param ->
    unit message_handler_result
external deliver_wm_nextdlgctl :
    handler:wm_nextdlgctl_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_nextdlgctl"
let on_wm_nextdlgctl = on_message ~id:wm_nextdlgctl ~deliver:deliver_wm_nextdlgctl ~return:return_simple_message
external pack_wm_nextdlgctl : param:nextdlgctl_param -> message_contents = "pack_wm_nextdlgctl"
let unpack_wm_nextdlgctl = ignore
let issue_wm_nextdlgctl ~sender ~param = unpack_wm_nextdlgctl (sender ~msg:(pack_wm_nextdlgctl ~param))

(* Standard dialog procedure *)

let standard_dialog_proc ~processors =
    let p = combine_message_processors processors in
    let f ~wnd ~msg =
        let translate result =
            if List.mem msg.mc_id !explicit_dialog_returns then
                result
            else
              begin
                set_dialog_result ~wnd ~result;
                Int32.one
              end
        in
        match p ~wnd ~msg with
            Some result -> translate result
          | None -> Int32.zero
    in
    Wp_function f
