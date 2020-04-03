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

(********************************************************************)
(* Shell API *)


type shell_op =
	| Shell_default
	| Shell_open
	| Shell_print
	| Shell_explore

external shell_execute :
	wnd:hwnd ->
	operation:shell_op ->
	file:string ->
	params:string option ->
	dir:string option->
	show:show_window_option -> 
	hinstance = "shell_execute_bytecode" "shell_execute_native"

type browseflag =
	BIF_BROWSEFORCOMPUTER
	| BIF_BROWSEFORPRINTER
	| BIF_DONTGOBELOWDOMAIN
	| BIF_RETURNFSANCESTORS
	| BIF_RETURNONLYFSDIRS
	| BIF_STATUSTEXT
	| BIF_NEWDIALOGSTYLE
	| BIF_EDITBOX
	| BIF_VALIDATE
	| BIF_BROWSEINCLUDEURLS
	| BIF_UAHINT
	| BIF_NONEWFOLDERBUTTON
	| BIF_NOTRANSLATETARGETS
	| BIF_BROWSEINCLUDEFILES
	| BIF_SHAREABLE

type browseinfos =  {
	bri_owner : hwnd;
	bri_title : string;
	bri_flags : browseflag list;
}

external sh_browse_for_folder :	browseinfos -> (string * int) option = "sh_browse_for_folder"

(* ... *)

(********************************************************************)
