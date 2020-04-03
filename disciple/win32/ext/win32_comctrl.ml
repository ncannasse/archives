(* OCaml-Win32
 * win32_comctrl.ml
 * Copyright (c) 2002 by Harry Chomsky & Nicolas Cannasse
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
open Win32_pack

(********************************************************************)
(* Common Controls Library *)

external init_common_controls : unit -> unit = "init_common_controls"

exception ComCtrl_error of string
let _ = Callback.register_exception "win32 common control error" (ComCtrl_error "")

type comctrl_style_option =
	CCS_ADJUSTABLE
  | CCS_BOTTOM
  | CCS_NODIVIDER
  | CCS_NOHILITE
  | CCS_NOMOVEY
  | CCS_NOPARENTALIGN
  | CCS_NORESIZE
  | CCS_TOP

external comctrl_style_of_option : comctrl_style_option -> style = "comctrl_style_of_option"

let comctrl_style_of_options ~options:(std,com) =
	special_style_of_options ~special_func:comctrl_style_of_option ~options:(std,com)

(* Notifications *)

let nm_outofmemory = -1
let nm_click = -2
let nm_dblclk = -3
let nm_return = -4
let nm_rclick = -5
let nm_rdblclk = -6
let nm_setfocus = -7
let nm_killfocus = -8
let nm_customdraw = -12
let nm_hover = -13
let nm_nchittest = -14
let nm_keydown = -15
let nm_releasedcapture = -16
let nm_setcursor = -17
let nm_char = -18

let ttn_show = -521
let ttn_pop = -522

(* WM_NOTIFY add-on *)

let wm_notify = 0x004E
type wm_notify_handler =
    wnd:hwnd ->
    msg:message_contents ->
    notify_code:int ->
    id:int ->
    ctrl:hwnd ->
    unit message_handler_result
external deliver_wm_notify :
    handler:wm_notify_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_notify"

let on_wm_notify = on_message ~id:wm_notify ~deliver:deliver_wm_notify ~return:return_simple_message

(********************************************************************)
(*** IMAGE LIST ***)

type himglist

type img_color_flag =
	ILC_COLOR
  | ILC_COLOR4
  | ILC_COLOR8
  | ILC_COLOR16
  | ILC_COLOR24
  | ILC_COLOR32
  | ILC_COLORDDB

type img_draw_flag =
	ILD_BLEND25
  | ILD_FOCUS
  | ILD_BLEND50
  | ILD_SELECTED
  | ILD_BLEND
  | ILD_MASK
  | ILD_NORMAL
  | ILD_TRANSPARENT

type img_init = {
	imi_size : size;
	imi_color : img_color_flag;
	imi_masked : bool;
	imi_initial : int;
	imi_grow : int;
}

external imagelist_create : init:img_init -> himglist = "imagelist_create"
external imagelist_destroy : imglist:himglist -> unit = "imagelist_destroy"

external imagelist_add : imglist:himglist -> image:hbitmap -> mask:(hbitmap option) -> int = "imagelist_add"
external imagelist_addmasked : imglist:himglist -> image:hbitmap -> maskcolor:color -> int = "imagelist_addmasked"
external imagelist_replace : imglist:himglist -> index:int -> image:hbitmap -> mask:(hbitmap option) -> unit = "imagelist_replace"
external imagelist_replaceicon : imglist:himglist -> index:int -> icon:hicon -> int = "imagelist_replaceicon"
external imagelist_remove : imglist:himglist -> index:int -> unit = "imagelist_remove"

external imagelist_geticon : imglist:himglist -> index:int -> flags:(img_draw_flag list) -> hicon = "imagelist_geticon"
external imagelist_setbkcolor : imglist:himglist -> color:(color option) -> unit = "imagelist_setbkcolor"
external imagelist_getbkcolor : imglist:himglist -> color option = "imagelist_getbkcolor"
external imagelist_setoverlayimage : imglist:himglist -> image:int -> overlay:int -> unit = "imagelist_setoverlayimage"
external imagelist_getimagecount : imglist:himglist -> int = "imagelist_getimagecount"
external imagelist_seticonsize : imglist:himglist -> size:size -> unit = "imagelist_seticonsize"

external imagelist_dragshownolock : show:bool -> unit = "imagelist_dragshownolock"
external imagelist_setdragcursorimage : imglist:himglist -> index:int -> pos:point -> unit = "imagelist_setdragcursorimage"
external imagelist_getdragimage : unit -> (himglist * point * point) = "imagelist_getdragimage"
external imagelist_begindrag : imglist:himglist -> index:int -> pos:point -> unit = "imagelist_begindrag"
external imagelist_enddrag : unit -> unit = "imagelist_enddrag"
external imagelist_dragmove : pos:point -> unit = "imagelist_dragmove"
external imagelist_dragenter : wndlock:hwnd -> pos:point -> unit = "imagelist_dragenter"
external imagelist_dragleave : wndlock:hwnd -> unit = "imagelist_dragleave"

external get_null_imglist : unit -> himglist = "get_null_handle"
let null_imglist = get_null_imglist()

let imagelist_addicon = imagelist_replaceicon ~index:(-1)
let imagelist_removeall = imagelist_remove ~index:(-1)

let indextooverlaymask i = i lsl 8

(* imagelist_merge ... *)
(* imagelist_draw ... *)
(* imagelist_drawex ... *)
(* imagelist_getimageinfo ... *)
(* imagelist_geticonsize ... *)
(* imagelist_loadimage ... *)

(* imagelist_read ... *)
(* imagelist_write ... *)

(********************************************************************)
(*** TREE VIEW ***)

(* Types *)

let wc_treeview = "SysTreeView32"

type treeview_style_option =
	TVS_HASBUTTONS
  | TVS_HASLINES
  | TVS_LINESATROOT
  | TVS_EDITLABELS
  | TVS_DISABLEDRAGDROP
  | TVS_SHOWSELALWAYS
  | TVS_RTLREADING
  | TVS_NOTOOLTIPS
  | TVS_CHECKBOXES
  | TVS_TRACKSELECT
  | TVS_SINGLEEXPAND
  | TVS_INFOTIP
  | TVS_FULLROWSELECT
  | TVS_NOSCROLL
  | TVS_NONEVENHEIGHT

type htreeitem

type tv_is_insertafter =
	TVI_FIRST
  | TVI_LAST
  | TVI_SORT
  | TVI_AFTER of htreeitem


type tv_expand =
	TVE_COLLAPSE
  | TVE_COLLAPSERESET
  | TVE_EXPAND
  | TVE_TOGGLE

type tvi_children =
	TVI_NOCHILD
  | TVI_SOMECHILD

type tvi_state = {
	tvis_bold : bool option;
	tvis_cut : bool option;
	tvis_drophilited : bool option;
	tvis_expanded : bool option;
	tvis_expandedonce : bool option;
	tvis_selected : bool option;
}

type tv_item = {
	tvi_children : tvi_children option;
	tvi_handle : htreeitem option;
	tvi_image : int option;
	tvi_selectedimg : int option;
	tvi_state : tvi_state option;
	tvi_text : string option;
}

type tv_insertstruct = {
	tvis_parent : htreeitem option;
	tvis_insertafter : tv_is_insertafter;	
	tvis_item : tv_item;
}

type nm_treeview = {
	nmt_itemnew : tv_item;
	nmt_itemold : tv_item;
	nmt_drag : point;
}

type tv_selstyle = 
	TVGN_CARET
  | TVGN_DROPHILITE
  | TVGN_FIRSTVISIBLE

type tv_sil =
	TVSIL_NORMAL
  | TVSIL_STATE

(* Functions *)

external get_tvi_root : unit -> htreeitem = "get_tvi_root"
external treeview_style_of_option : treeview_style_option -> style = "treeview_style_of_option"
let treeview_style_of_options = special_style_of_options ~special_func:treeview_style_of_option

external unpack_himglist : message_result -> himglist = "unpack_handle"
external unpack_nm_treeview : message_result -> nm_treeview = "unpack_nm_treeview"

external pack_wtvselstyle : msg:int -> value:tv_selstyle -> message_contents = "pack_wtvselstyle"
external pack_wtvsil : msg:int -> value:tv_sil -> message_contents = "pack_wtvsil"
external pack_wtvexpand : msg:int -> value:tv_expand -> message_contents = "pack_wtvexpand"

external pack_lhimglist : msg:int -> value:himglist -> message_contents = "pack_lhandle"
external pack_lhtreeitem : msg:int -> value:htreeitem -> message_contents = "pack_lhandle"
external pack_ltvitem : msg:int -> value:tv_item -> message_contents = "pack_ltvitem"

external eq_htreeitem : htreeitem -> htreeitem -> bool = "eq_handle"
external get_null_htreeitem : unit -> htreeitem = "get_null_handle"
let null_htreeitem = get_null_htreeitem()
let tvi_root = get_tvi_root()

(* Messages *)

let tv_first = 0x1100
let tvn_first = -400

(*-- label edit --*)

let tvm_editlabel = tv_first+14
let issue_tvm_editlabel ~sender ~item =
	unpack_hwnd (sender ~msg:(
		pack_lhtreeitem ~msg:tvm_editlabel ~value:item))

let tvm_endeditlabelnow = tv_first+22
let issue_tvm_endeditlabelnow ~sender ~cancel =
	unpack_bool (sender ~msg:(
		pack_wbool ~msg:tvm_endeditlabelnow ~value:cancel))

let tvm_geteditcontrol = tv_first+15
let issue_tvm_geteditcontrol ~sender =
	unpack_hwnd (sender ~msg:(
		pack_simple_message ~id:tvm_geteditcontrol))

(*-- item states --*)

let tvm_setitem = tv_first+13
let issue_tvm_setitem ~sender ~item =
	unpack_int (sender ~msg:(
		pack_ltvitem ~msg:tvm_setitem ~value:item))

let tvm_getitem = tv_first+12
external issue_tvm_getitem : sender:message_sender -> item:htreeitem -> tv_item = "issue_tvm_getitem"

let tvm_selectitem = tv_first+11
let issue_tvm_selectitem ~sender ~style ~item =
	unpack_bool (sender ~msg:(pack2 ~msg:tvm_selectitem
		~wparam:(pack_wtvselstyle ~value:style)
		~lparam:(pack_lhtreeitem ~value:item)))

(*-- images list --*)

let tvm_setimagelist = tv_first+9
let issue_tvm_setimagelist ~sender ~state ~list =
	unpack_himglist (sender ~msg:(pack2 ~msg:tvm_setimagelist
		~wparam:(pack_wtvsil ~value:state)
		~lparam:(pack_lhimglist ~value:list)))

let tvm_getimagelist = tv_first+8
let issue_tvm_getimagelist ~sender ~state =
	unpack_himglist (sender ~msg:(
		pack_wtvsil ~msg:tvm_getimagelist ~value:state))

(*-- item position --*)

let tvm_insertitem = tv_first
external issue_tvm_insertitem : sender:message_sender -> item:tv_insertstruct -> htreeitem = "issue_tvm_insertitem"

let tvm_sortchildren = tv_first+19
let issue_tvm_sortchildren ~sender ~item =
	unpack_bool (sender ~msg:(
		pack_ltvitem ~msg:tvm_sortchildren ~value:item))

let tvm_sortchildrencb = tv_first+21
	(* ... *)

(*-- parent & childs --*)

let tvm_expand = tv_first+2
let issue_tvm_expand ~sender ~flag ~item =
	unpack_bool (sender ~msg:(pack2 ~msg:tvm_expand
		~wparam:(pack_wtvexpand ~value:flag)
		~lparam:(pack_ltvitem ~value:item)))

let tvm_setident = tv_first+7
let issue_tvm_setident ~sender ~width =
	unpack_unit (sender ~msg:(
		pack_wint ~msg:tvm_setident ~value:width))

let tvm_getident = tv_first+6
let issue_tvm_getident ~sender =
	unpack_int (sender ~msg:(
		pack_simple_message ~id:tvm_getident))

(*-- item informations --*)

let tvm_ensurevisible = tv_first+20
let issue_tvm_ensurevisible ~sender ~item =
	unpack_bool (sender ~msg:(
		pack_ltvitem ~msg:tvm_ensurevisible ~value:item))

let tvm_getnexitem = tv_first+10
	(* ... *)

let tvm_getcount = tv_first+5
let issue_tvm_getcount ~sender =
	unpack_int (sender ~msg:(
		pack_simple_message ~id:tvm_getcount))

let tvm_getvisiblecount = tv_first+16
let issue_tvm_getvisiblecount ~sender =
	unpack_int (sender ~msg:(
		pack_simple_message ~id:tvm_getvisiblecount))

(*-- drag'n drop operations --*)

let tvm_createdragimage = tv_first+18
let issue_tvm_createdragimage ~sender ~item =
	unpack_himglist (sender ~msg:(
		pack_lhtreeitem ~msg:tvm_createdragimage ~value:item))


let tvm_hittest = tv_first+17
external issue_tvm_hittest : sender:message_sender -> pos:point -> htreeitem option = "issue_tvm_hittest"

(*-- misc --*)

let tvm_deleteitem = tv_first+1
let issue_tvm_deleteitem ~sender ~item =
	unpack_bool (sender ~msg:(
		pack_lhtreeitem ~msg:tvm_deleteitem ~value:item))

let tvm_getisearchstring = tv_first+23
	(* ... *)
	

let tvm_getitemrect = tv_first+4
	(* ... *)

let tvm_settooltips = tv_first+24
let tvm_gettooltips = tv_first+25
let tvm_setinsertmark = tv_first+26
let tvm_setitemheight = tv_first+27
let tvm_getitemheight = tv_first+28
let tvm_setbkcolor = tv_first+29
let tvm_settextcolor = tv_first+30
let tvm_getbkcolor = tv_first+31
let tvm_gettextcolor = tv_first+32
let tvm_setscrolltime = tv_first+33
let tvm_getscrolltime = tv_first+34
let tvm_setinsertmarkcolor = tv_first+37
let tvm_getinsertmarkcolor = tv_first+38
	(* ... undocumented *)

(* Notifications *)

let tvn_itemexpanding = tvn_first-5
let tvn_itemexpanded = tvn_first-6
let tvn_beginlabeledit = tvn_first-10
let tvn_endlabeledit = tvn_first-11
let tvn_selchanging = tvn_first-1
let tvn_selchanged = tvn_first-2
let tvn_begindrag = tvn_first-7
let tvn_beginrdrag = tvn_first-8
let tvn_deleteitem = tvn_first-9
let tvn_getdispinfo = tvn_first-3
let tvn_setdispinfo = tvn_first-4
let tvn_keydown = tvn_first-12

let tvn_getinfotip = tvn_first-13
let tvn_singleexpand = tvn_first-15

(********************************************************************)
(* Header Control *)

(*-- Types --*)

let wc_header = "SysHeader32"

type header_style_option =
	| HDS_BUTTONS
	| HDS_HIDDEN
	| HDS_HORZ	

type hi_format =
	| HDF_CENTER
	| HDF_LEFT
	| HDF_RIGHT	

type hd_item = {
	hi_width : int option;
	hi_height : int option;
	hi_text : string option;
	hi_bitmap : hbitmap option;
	hi_format : hi_format option;	
}

type hd_hitwhere =
	| HHT_NOWHERE
	| HHT_ONHEADER
	| HHT_ONDIVIDER
	| HHT_ONDIVOPEN
	| HHT_ABOVE
	| HHT_BELOW
	| HHT_TORIGHT
	| HHT_TOLEFT

(*-- Externals *)

external header_style_of_option : header_style_option -> style = "header_style_of_option"
let header_style_of_options = special_style_of_options ~special_func:header_style_of_option

external pack_lhditem : msg:int -> value:hd_item -> message_contents = "pack_lhditem"

(*-- Messages *)

let hdm_first = 0x1200
let hdn_first = -300

let hdm_getitemcount = hdm_first + 0
let issue_hdm_getitemcount ~sender =
	unpack_int (sender ~msg:(
		pack_simple_message ~id:hdm_getitemcount))

let hdm_insertitem = hdm_first + 1
let issue_hdm_insertitem ~sender ~index ~item =
	unpack_int (sender ~msg:(pack2 ~msg:hdm_insertitem
		~wparam:(pack_wint ~value:index)
		~lparam:(pack_lhditem ~value:item)))

let hdm_deleteitem = hdm_first + 2
let issue_hdm_deleteitem ~sender ~index =
	unpack_bool (sender ~msg:(
		pack_wint ~msg:hdm_deleteitem ~value:index))

let hdm_getitem =  hdm_first + 3
external issue_hdm_getitem : sender:message_sender -> index:int -> hd_item = "issue_hdm_getitem"

let hdm_setitem = hdm_first + 4
let issue_hdm_setitem ~sender ~index ~data =
	unpack_bool (sender ~msg:(pack2 ~msg:hdm_setitem
		~wparam:(pack_wint ~value:index)
		~lparam:(pack_lhditem ~value:data)))

let hdm_layout = hdm_first + 5
external issue_hdm_layout : sender:message_sender -> rect:rect -> windowpos = "issue_hdm_layou"

let hdm_hittest	= hdm_first + 6
external issue_hdm_hittest : sender:message_sender -> pt:point -> (hd_hitwhere * int option) = "issue_hdm_hittest"

let hdm_getitemrect = hdm_first + 7

let hdm_setimagelist = hdm_first + 8

let hdm_getimagelist = hdm_first + 9

let hdm_ordertoindex = hdm_first + 15

let hdm_createdragimage = hdm_first + 16

let hdm_getorderarray = hdm_first + 17

let hdm_setorderarray = hdm_first + 18

let hdm_sethotdivider = hdm_first + 19


(*-- Notifications Messages --*)

let hdn_itemchanging = hdn_first-0
let hdn_itemchanged = hdn_first-1
let hdn_itemclick = hdn_first-2
let hdn_itemdblclick = hdn_first-3
let hdn_dividerdblclick = hdn_first-5
let hdn_begintrack = hdn_first-6
let hdn_endtrack = hdn_first-7
let hdn_track = hdn_first-8
let hdn_getdispinfo = hdn_first-9
let hdn_begindrag = hdn_first-10
let hdn_enddrag = hdn_first-11

(***EOF***)