(* OCaml-Win32
 * win32_comdlg.ml
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

(********************************************************************)

type comdlg_error_type =
	Cderr_dialogfailure
  | Cderr_findresfailure
  | Cderr_initialization
  | Cderr_loadresfailure
  | Cderr_loadstrfailure
  | Cderr_lockresfailure
  | Cderr_memallocfailure
  | Cderr_memlockfailure
  | Cderr_nohinstance
  | Cderr_nohook
  | Cderr_notemplate
  | Cderr_registermsgfaile
  | Cderr_structsize
  | Pderr_createicfailure
  | Pderr_defaultdifferent
  | Pderr_dndmmismatch
  | Pderr_getdevmodefail
  | Pderr_initfailure
  | Pderr_loaddrvfailure
  | Pderr_nodefaultprn
  | Pderr_nodevices
  | Pderr_parsefailure
  | Pderr_printernotfound
  | Pderr_retdeffailure
  | Pderr_setupfailure
  | Cferr_maxlessthanmin
  | Cferr_nofonts
  | Fnerr_buffertoosmall
  | Fnerr_invalidfilename
  | Fnerr_subclassfailure
  | Frerr_bufferlengthzero

exception Comdlg_error of comdlg_error_type
let _ = Callback.register_exception "win32 common dialog error" (Comdlg_error Cderr_dialogfailure)

(********************************************************************)
(* Common Dialog Box Library *)

type colorflag =
	CC_FULLOPEN
  | CC_PREVENTFULLOPEN
  | CC_SHOWHELP
(*	CC_RGBINIT : use Nome/Some as cc_init
	CC_ENABLEHOOK
	CC_ENABLETEMPLATE
	CC_ENABLETEMPLATEHANDLE
*)

type choosecolor = {
	cc_owner : hwnd;
	cc_init : color option;
	cc_flags : colorflag list;
	(* ... hooks & templates & defcolors *)
}

type openfileflag =
	OFN_ALLOWMULTISELECT
  | OFN_CREATEPROMPT
  | OFN_EXPLORER
  | OFN_FILEMUSTEXIST
  | OFN_HIDEREADONLY
  | OFN_LONGNAMES
  | OFN_NOCHANGEDIR
  | OFN_NODEREFERENCELINKS
  | OFN_NOLONGNAMES
  | OFN_NONETWORKBUTTON
  | OFN_NOREADONLYRETURN
  | OFN_NOTESTFILECREATE
  | OFN_NOVALIDATE
  | OFN_OVERWRITEPROMPT
  | OFN_PATHMUSTEXIST
  | OFN_READONLY
  | OFN_SHAREAWARE
  | OFN_SHOWHELP
(*
	OFN_ENABLEHOOK	
	OFN_ENABLETEMPLATE	
	OFN_ENABLETEMPLATEHANDLE
*)

type openfilename = {
	ofn_owner : hwnd;
	ofn_filters : (string * string) list;
	ofn_filterindex : int;
	ofn_default_filename : string option;
	ofn_initialdir : string option;
	ofn_title : string option;
	ofn_flags : openfileflag list;
	ofn_default_extension : string option;
	(* ... hooks & templates *)
}

external choose_color : choosecolor -> color option = "choose_color"

external get_open_file_name : openfilename -> string option = "get_open_file_name"
external get_save_file_name : openfilename -> string option = "get_save_file_name"

(* ... other dialogs *)

(********************************************************************)
