(* OCaml-Win32
 * win32_richedit.ml
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
(* User Interface Services: Controls: RichEdit Controls *)

(* Styles *)

type richedit_style_option =
    ES_DISABLENOSCROLL
  | ES_EX_NOCALLOLEINIT
  | ES_NOIME
  | ES_SAVESEL
  | ES_SELFIME
  | ES_VERTICAL

type findtext_style =
	FT_MATCHCASE
  | FT_WHOLEWORD

type findword_style =
	WB_ISDELIMITER
  | WB_LEFT
  | WB_LEFTBREAK
  | WB_MOVEWORDLEFT
  | WB_MOVEWORDRIGHT
  | WB_RIGHT
  | WB_RIGHTBREAK
  | WB_CLASSIFY (* bool return value is set to int *)

type charformat_style =
	SCF_ALL
  |	SCF_SELECTION
  | SCF_WORD

type charformat_effect =
	CFE_AUTOCOLOR
  | CFE_BOLD
  | CFE_ITALIC
  | CFE_STRIKEOUT
  | CFE_UNDERLINE
  | CFE_PROTECTED

type selection_type =
	SEL_EMPTY
  | SEL_TEXT
  | SEL_OBJECT
  | SEL_MULTICHAR
  | SEL_MULTIOBJECT

type rich_operation =
	ECOOP_SET
  | ECOOP_OR
  | ECOOP_AND
  | ECOOP_XOR

type richoption =
	ECO_AUTOWORDSELECTION
  | ECO_AUTOVSCROLL
  | ECO_AUTOHSCROLL
  | ECO_NOHIDESEL
  | ECO_READONLY
  | ECO_WANTRETURN
  | ECO_SAVESEL
  | ECO_SELECTIONBAR
  | ECO_VERTICAL

type charformat = {
	cf_effects : (charformat_effect list);
	cf_height : int option;
	cf_offset : int option;
	cf_textcolor : color option;
	cf_charset : char_set;
    cf_pitch : font_pitch;
    cf_family : font_family;
	cf_facename : string option;
}

type charrange = {
	cpMin : int;
	cpMax : int;
}

type findtext = {
	ft_chrg : charrange;
	ft_text : string;
}

type paraformat_number =
	PFN_ZERO
  | PFN_BULLET

type paraformat_align =
	PFA_LEFT
  | PFA_RIGHT
  | PFA_CENTER	

type paraformat = {
	pf_numbering : paraformat_number option;	
	pf_startindent : (int * bool) option;
	pf_rightindent : int option;
	pf_offset : int option;
	pf_alignment : paraformat_align option;
	pf_tabs : (int list) option;
}

type undonameid = 
    UID_UNKNOWN
  | UID_TYPING
  | UID_DELETE
  | UID_DRAGDROP
  | UID_CUT
  | UID_PASTE

type textmode =
	TM_PLAINTEXT
  | TM_RICHTEXT
  | TM_SINGLELEVELUNDO
  | TM_MULTILEVELUNDO

external richedit_style_of_option : richedit_style_option -> style = "richedit_style_of_option"

let richedit_style_of_options ~options:(std,ed,red) =
	Int64.logor
		(special_style_of_options ~special_func:Win32_editcontrol.edit_style_of_option ~options:(std,ed))
		(special_style_of_options ~special_func:richedit_style_of_option ~options:([],red))

(* Special pack / unpack *)

open Win32_pack

external unpack_selection_type : message_result -> selection_type = "unpack_selection_type"
external unpack_charformat : message_result -> charformat = "unpack_charformat"
external unpack_charrange : message_result -> charrange = "unpack_charrange"
external unpack_richoptions : message_result -> richoption list = "unpack_richoptions"
external unpack_undonameid : message_result -> undonameid = "unpack_undonameid"
external unpack_textmodes : message_result -> textmode list = "unpack_textmodes"

external unpack_special_findtextex_chrgText : message_result -> charrange = "unpack_special_findtextex_chrgText"

external pack_wfindtext_styles : msg:int -> value:(findtext_style list) -> message_contents = "pack_wfindtext_styles"
external pack_wfindword_style : msg:int -> value:findword_style -> message_contents = "pack_wfindword_style"
external pack_wcharformat_style : msg:int -> value:charformat_style -> message_contents = "pack_wcharformat_style"
external pack_wrich_operation : msg:int -> value:rich_operation -> message_contents = "pack_wrich_operation"
external pack_wtextmodes : msg:int -> value:textmode list -> message_contents = "pack_wtextmodes"

external pack_lcharrange : msg:int -> value:charrange -> message_contents = "pack_lcharrange"
external pack_lcharformat : msg:int -> value:charformat -> message_contents = "pack_lcharformat"
external pack_lfindtext : msg:int -> value:findtext -> message_contents = "pack_lfindtext"
external pack_lfindtextex : msg:int -> value:findtext -> message_contents = "pack_lfindtextex"

external pack_lpointlh : msg:int -> value:point -> message_contents = "pack_lpointlh"
external pack_lparaformat : msg:int -> value:paraformat -> message_contents = "pack_lparaformat"
external pack_lrichoptions : msg:int -> value:richoption list -> message_contents = "pack_lrichoptions"

(* Messages *)

let em_getlimittext			= 0x0425
let issue_em_getlimittext ~sender =
	unpack_int (sender ~msg:(
		pack_simple_message ~id:em_getlimittext))

let em_posfromchar			= 0x0426
let issue_em_posfromchar ~sender ~pos =
	unpack_pointlh (sender ~msg:(
		pack_wint ~msg:em_posfromchar ~value:pos))

let em_charfrompos			= 0x0427
let issue_em_charfrompos ~sender ~pos =
	unpack_int (sender ~msg:(
		pack_lpointlh ~msg:em_charfrompos ~value:pos))

let em_scrollcaret			= 0x0431
let issue_em_scrollcaret ~sender =
	unpack_bool (sender ~msg:(
		pack_simple_message ~id:em_scrollcaret))

let em_canpaste				= 0x0432
(* ... needs clipboard support *)

let em_displayband			= 0x0433
let issue_em_displayband ~sender ~rect =
	unpack_bool (sender ~msg:(
		pack_lrect ~msg:em_displayband ~value:rect))

let em_exgetsel				= 0x0434
let defcharrange = {cpMin=0;cpMax=0}
let issue_em_exgetsel ~sender =
	unpack_lparam ~sender ~msg:(
		pack_lcharrange ~msg:em_exgetsel ~value:defcharrange)
		~unpack:unpack_charrange

let em_exlimittext			= 0x0435
let issue_em_exlimittext ~sender ~limit =
	unpack_unit (sender ~msg:(
		pack_lint32 ~msg:em_exlimittext ~value:limit))

let em_exlinefromchar		= 0x0436
let issue_em_exlinefromchar ~sender ~pos =
	unpack_int (sender ~msg:(
		pack_lint ~msg:em_exlinefromchar ~value:pos))

let em_exsetsel				= 0x0437
let issue_em_exsetsel ~sender ~range =
	unpack_int (sender ~msg:(
		pack_lcharrange ~msg:em_exsetsel ~value:range))

let em_findtext				= 0x0438
let issue_em_findtext ~sender ~find ~flags =
	unpack_int (sender ~msg:(pack2 ~msg:em_findtext
		~wparam:(pack_wfindtext_styles ~value:flags)
		~lparam:(pack_lfindtext ~value:find)))

let em_formatrange			= 0x0439
(* ... printers *)

let em_getcharformat		= 0x043A
let defcharformat = {
	cf_effects = [];
	cf_height = None;
	cf_offset = None;
	cf_textcolor = None;
	cf_charset = Default_charset;
    cf_pitch = DEFAULT_PITCH;
    cf_family = FF_DONTCARE;
	cf_facename = None; }

let issue_em_getcharformat ~sender ~selection =
	unpack_lparam ~sender ~msg:(pack2 ~msg:em_getcharformat
		~wparam:(pack_wbool ~value:selection)
		~lparam:(pack_lcharformat ~value:defcharformat))
		~unpack:unpack_charformat

let em_geteventmask			= 0x043B
let issue_em_geteventmask ~sender =
	unpack_int32 (sender ~msg:(
		pack_simple_message ~id:em_geteventmask))

let em_getoleinterface		= 0x043C
(* ... OLE *)

let em_getparaformat		= 0x043D
let defparaformat = {
	pf_numbering = None;
	pf_startindent = None;
	pf_rightindent = None;
	pf_offset = None;
	pf_alignment = None;
	pf_tabs = None; }
let issue_em_getparaformat ~sender =
	unpack_lparam ~sender ~msg:(
		pack_lparaformat ~msg:em_getparaformat ~value:defparaformat)

let em_getseltext			= 0x043E
let issue_em_getseltext ~sender =
	unpack_lparam ~sender ~msg:(
		pack_lemptystring ~msg:em_getseltext)
		~unpack:unpack_string

let em_hideselection		= 0x043F
let issue_em_hideselection ~sender ~hide ~change =
	unpack_unit (sender ~msg:(pack2 ~msg:em_hideselection
		~wparam:(pack_wbool ~value:hide)
		~lparam:(pack_lbool ~value:change)))

let em_pastespecial			= 0x0440
(* ... clipboard *)

let em_requestresize		= 0x0441
let issue_em_requestresize ~sender =
	unpack_unit (sender ~msg:(
		pack_simple_message ~id:em_requestresize))

let em_selectiontype		= 0x0442
let issue_em_selectiontype ~sender =
	unpack_selection_type (sender ~msg:(
		pack_simple_message ~id:em_selectiontype))		

let em_setbkgndcolor		= 0x0443
let issue_em_setbkgndcolor ~sender ~usesys ~color =
	unpack_color (sender ~msg:(pack2 ~msg:em_setbkgndcolor
		~wparam:(pack_wbool ~value:usesys)
		~lparam:(pack_lcolor ~value:color)))

let em_setcharformat		= 0x0444
let issue_em_setcharformat ~sender ~style ~format =
	unpack_bool (sender ~msg:(pack2 ~msg:em_setcharformat
				~wparam:(pack_wcharformat_style ~value:style)
				~lparam:(pack_lcharformat ~value:format)))

let em_seteventmask			= 0x0445
let issue_em_seteventmask ~sender ~mask =
	unpack_int32 (sender ~msg:(
		pack_lint32 ~msg:em_seteventmask ~value:mask))

let em_setolecallback		= 0x0446
(* ... OLE *)

let em_setparaformat		= 0x0447
let issue_em_setparaformat ~sender ~pformat =
	unpack_bool (sender ~msg:(
		pack_lparaformat ~msg:em_setparaformat ~value:pformat))

let em_settargetdevice		= 0x0448
let issue_em_settargetdevice ~sender ~target ~linewidth =
	unpack_bool (sender ~msg:(pack2 ~msg:em_settargetdevice
		~wparam:(pack_whandle ~value:target)
		~lparam:(pack_lint ~value:linewidth)))


let em_streamin				= 0x0449
(* ... *)
let em_streamout			= 0x044A
(* ... *)

let em_gettextrange			= 0x044B
external issue_em_gettextrange : sender:message_sender -> range:charrange -> string = "issue_em_gettextrange"

let em_findwordbreak		= 0x044C
let issue_em_findwordbreak ~sender ~style ~pos =
	unpack_int (sender ~msg:(pack2 ~msg:em_findwordbreak
		~wparam:(pack_wfindword_style ~value:style)
		~lparam:(pack_lint ~value:pos)))

let em_setoptions			= 0x044D
let issue_em_setoptions ~sender ~operation ~options =
	unpack_richoptions (sender ~msg:(pack2 ~msg:em_setoptions
		~wparam:(pack_wrich_operation ~value:operation)
		~lparam:(pack_lrichoptions ~value:options)))

let em_getoptions			= 0x044E
let issue_em_getoptions ~sender =
	unpack_richoptions (sender ~msg:(
		pack_simple_message ~id:em_getoptions))

let em_findtextex			= 0x044F
let issue_em_findtextex ~sender ~find ~flags =
	unpack_special_findtextex_chrgText
	(sender ~msg:(pack2 ~msg:em_findtextex
		~wparam:(pack_wfindtext_styles ~value:flags)
		~lparam:(pack_lfindtextex ~value:find)))

let em_getwordbreakprocex	= 0x0450
(* ... need callbacks *)
let em_setwordbreakprocex	= 0x0451
(* ... need callbacks *)

(* richedit v2.0 messages *)

let	em_setundolimit			= 0x0452
let issue_em_setundolimit ~sender ~limit =
	unpack_int (sender ~msg:(
		pack_wint ~msg:em_setundolimit ~value:limit))

let em_redo					= 0x0454
let issue_em_redo ~sender =
	unpack_bool (sender ~msg:(
		pack_simple_message ~id:em_redo))

let em_canredo				= 0x0455
let issue_em_canredo ~sender =
	unpack_bool (sender ~msg:(
		pack_simple_message ~id:em_canredo))

let em_getundoname			= 0x0456
let issue_em_getundoname ~sender =
	unpack_undonameid (sender ~msg:(
		pack_simple_message ~id:em_getundoname))

let em_getredoname			= 0x0457
let issue_em_getredodoname ~sender =
	unpack_undonameid (sender ~msg:(
		pack_simple_message ~id:em_getredoname))

let em_stopgrouptyping		= 0x0458
let issue_em_stopgrouptyping ~sender =
	unpack_unit (sender ~msg:(
		pack_simple_message ~id:em_stopgrouptyping))

let em_settextmode			= 0x0459
let issue_em_settextmode ~sender ~modes =
	unpack_bool (sender ~msg:(
		pack_wtextmodes ~msg:em_settextmode ~value:modes))

let em_gettextmode			= 0x045A
let issue_em_gettextmode ~sender =
	unpack_textmodes (sender ~msg:(
		pack_simple_message ~id:em_gettextmode))
					
let em_autourldetect		= 0x045B
let issue_em_autourldetect ~sender ~enable =
	unpack_bool (sender ~msg:(
		pack_wbool ~msg:em_autourldetect ~value:enable))	

let em_getautourldetect		= 0x045C
let issue_em_getautourldetect ~sender =
	unpack_bool (sender ~msg:(
		pack_simple_message ~id:em_getautourldetect))

(* .. undocumented richedit 2.0 *)
let em_setpalette			= 0x045D
let em_gettextex			= 0x045E
let em_gettextlengthex		= 0x045F

(* ... far east specific messages *)
let em_setpunctuation		= 0x0464
let em_getpunctuation		= 0x0465
let em_setwordwrapmode		= 0x0466
let em_getwordwrapmode		= 0x0467
let em_setimecolor			= 0x0468
let em_getimecolor			= 0x0469
let em_setimeoptions		= 0x046A
let em_getimeoptions		= 0x046B
let em_convposition 		= 0x046C
let em_setlangoptions		= 0x0478
let em_getlangoptions		= 0x0479
let em_getimecompmode		= 0x047A
let em_findtextw			= 0x047B
let em_findtextexw			= 0x047C

(* bidi specific messages *)
let em_setbidioptions		= 0x04C8
let em_getbidioptions		= 0x04C9

(* WM messages *)

let wm_contextmenu			= 0x007b
let wm_printclient			= 0x0318

let wm_setredraw = 0x000B
let issue_wm_setredraw ~sender ~redraw =
	unpack_bool (sender ~msg:(
		pack_wbool ~msg:wm_setredraw ~value:redraw))

(* Notifications *)

let en_correcttext = 0x0705
let en_dropfiles = 0x0703
let en_imechange = 0x0707
let en_msgfilter = 0x0700
let en_oleopfailed = 0x0709
let en_protected = 0x0704
let en_requestresize = 0x0701
let en_saveclipboard = 0x0708
let en_selchange = 0x0702
let en_stopnoundo = 0x0706
let en_objectpositions = 0x070a
let en_link	= 0x070b
let en_dragdropdone	= 0x070c

(* Special *)

external richedit_colorize : func:('a -> string -> int -> color option) -> wnd:hwnd -> range:charrange -> unit = "richedit_colorize"