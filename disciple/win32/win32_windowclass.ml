(* OCaml-Win32
 * win32_windowclass.ml
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
(* User Interface Services: Windowing: Window Classes *)

type class_style_option =
    CS_BYTEALIGNCLIENT
  | CS_BYTEALIGNWINDOW
  | CS_CLASSDC
  | CS_DBLCLKS
  | CS_GLOBALCLASS
  | CS_HREDRAW
  | CS_NOCLOSE
  | CS_OWNDC
  | CS_PARENTDC
  | CS_SAVEBITS
  | CS_VREDRAW
type class_background =
    Cb_none
  | Cb_sys_color of sys_color
  | Cb_brush of hbrush
type wndclass = {
    wc_style : class_style_option list;
    wc_wnd_proc : window_proc;
    wc_cls_extra : int;
    wc_wnd_extra : int;
    wc_instance : hinstance;
    wc_icon : hicon;
    wc_cursor : hcursor;
    wc_background : class_background;
    wc_menu_name : resource_name option;
    wc_class_name : atom_name;
    wc_icon_sm : hicon }

external get_class_info : inst:hinstance -> name:atom_name -> wndclass = "get_class_info"

external get_class_long : wnd:hwnd -> index:int -> int32 = "get_class_long"

external get_class_name : wnd:hwnd -> string = "get_class_name"

external get_class_word : wnd:hwnd -> index:int -> int = "get_class_word"

external get_window_long : wnd:hwnd -> index:int -> int32 = "get_window_long"

external register_class : wc:wndclass -> int = "register_class"

external set_class_long : wnd:hwnd -> index:int -> value:int32 -> int32 = "set_class_long"

external set_class_word : wnd:hwnd -> index:int -> value:int -> int = "set_class_word"

external set_window_long : wnd:hwnd -> index:int -> value:int32 -> int32 = "set_window_long"

external unregister_class : name:atom_name -> inst:hinstance -> unit = "unregister_class"

(* Specialized functions to get/set class/window info *)

external get_class_atom : wnd:hwnd -> int = "get_class_atom"

external get_class_cls_extra : wnd:hwnd -> int = "get_class_cls_extra"

external get_class_wnd_extra : wnd:hwnd -> int = "get_class_wnd_extra"

external get_class_background : wnd:hwnd -> class_background = "get_class_background"

external get_class_cursor : wnd:hwnd -> hcursor = "get_class_cursor"

external get_class_icon : wnd:hwnd -> hicon = "get_class_icon"

external get_class_icon_sm : wnd:hwnd -> hicon = "get_class_icon_sm"

external get_class_module : wnd:hwnd -> hinstance = "get_class_module"

external get_class_menu_name : wnd:hwnd -> resource_name option = "get_class_menu_name"

external get_class_style : wnd:hwnd -> class_style_option list = "get_class_style"

external get_class_wndproc : wnd:hwnd -> window_proc = "get_class_wndproc"

external get_window_style : wnd:hwnd -> style = "get_window_style"

external get_window_proc : wnd:hwnd -> window_proc = "get_window_proc"

external get_window_instance : wnd:hwnd -> hinstance = "get_window_instance"

external get_window_parent : wnd:hwnd -> hwnd = "get_window_parent"

external get_window_id : wnd:hwnd -> int = "get_window_id"

external get_window_user_data : wnd:hwnd -> int32 = "get_window_user_data"

external get_dialog_proc : wnd:hwnd -> window_proc = "get_dialog_proc"

external get_dialog_msg_result : wnd:hwnd -> message_result = "get_dialog_msg_result"

external get_dialog_user_data : wnd:hwnd -> int32 = "get_dialog_user_data"

external set_class_cls_extra : wnd:hwnd -> extra:int -> int = "set_class_cls_extra"

external set_class_wnd_extra : wnd:hwnd -> extra:int -> int = "set_class_wnd_extra"

external set_class_background : wnd:hwnd -> bg:class_background -> class_background = "set_class_background"

external set_class_cursor : wnd:hwnd -> cursor:hcursor -> hcursor = "set_class_cursor"

external set_class_icon : wnd:hwnd -> icon:hicon -> hicon = "set_class_icon"

external set_class_icon_sm : wnd:hwnd -> icon:hicon -> hicon = "set_class_icon_sm"

external set_class_module : wnd:hwnd -> inst:hinstance -> hinstance = "set_class_module"

external set_class_menu_name : wnd:hwnd -> name:resource_name option -> resource_name option = "set_class_menu_name"

external set_class_style : wnd:hwnd -> style:class_style_option list -> class_style_option list = "set_class_style"

external set_class_wndproc : wnd:hwnd -> proc:window_proc -> window_proc = "set_class_wndproc"

external set_window_style : wnd:hwnd -> style:style -> style = "set_window_style"

external set_window_proc : wnd:hwnd -> proc:window_proc -> window_proc = "set_window_proc"

external set_window_instance : wnd:hwnd -> inst:hinstance -> hinstance = "set_window_instance"

external set_window_id : wnd:hwnd -> id:int -> int = "set_window_id"

external set_window_user_data : wnd:hwnd -> data:int32 -> int32 = "set_window_user_data"

external set_dialog_proc : wnd:hwnd -> proc:window_proc -> window_proc = "set_dialog_proc"

external set_dialog_msg_result : wnd:hwnd -> result:message_result -> message_result = "set_dialog_msg_result"

external set_dialog_user_data : wnd:hwnd -> data:int32 -> int32 = "set_dialog_user_data"
