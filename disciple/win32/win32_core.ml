(* OCaml-Win32
 * win32_core.ml
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

(********************************************************************)
(* Types *)

(* Constants *)

type activate_flag =
    WA_ACTIVE
  | WA_CLICKACTIVE
  | WA_INACTIVE

type animation_type =
    IDANI_CAPTION
  | IDANI_CLOSE
  | IDANI_OPEN

type arc_direction =
    AD_COUNTERCLOCKWISE
  | AD_CLOCKWISE

type background_mode =
    OPAQUE
  | TRANSPARENT

type binary_type =
    SCS_32BIT_BINARY
  | SCS_DOS_BINARY
  | SCS_OS216_BINARY
  | SCS_PIF_BINARY
  | SCS_POSIX_BINARY
  | SCS_WOW_BINARY

type border_option =
    BF_ADJUST
  | BF_BOTTOM
  | BF_BOTTOMLEFT
  | BF_BOTTOMRIGHT
  | BF_DIAGONAL
  | BF_DIAGONAL_ENDBOTTOMLEFT
  | BF_DIAGONAL_ENDBOTTOMRIGHT
  | BF_DIAGONAL_ENDTOPLEFT
  | BF_DIAGONAL_ENDTOPRIGHT
  | BF_FLAT
  | BF_LEFT
  | BF_MIDDLE
  | BF_MONO
  | BF_RECT
  | BF_RIGHT
  | BF_SOFT
  | BF_TOP
  | BF_TOPLEFT
  | BF_TOPRIGHT

type broadcast_system_message_option =
    BSF_FLUSHDISK
  | BSF_FORCEIFHUNG
  | BSF_IGNORECURRENTTASK
  | BSF_NOHANG
  | BSF_NOTIMEOUTIFNOTHUNG
  | BSF_POSTMESSAGE
  | BSF_QUERY

type broadcast_system_message_recipient =
    BSM_ALLDESKTOPS
  | BSM_APPLICATIONS
  | BSM_INSTALLABLEDRIVERS
  | BSM_NETDRIVER
  | BSM_VXDS

type button_state =
    BST_CHECKED
  | BST_INDETERMINATE
  | BST_UNCHECKED

type button_state_ex =
    BST_FOCUS
  | BST_PUSHED

type child_window_option =
    CWP_ALL
  | CWP_SKIPDISABLED
  | CWP_SKIPINVISIBLE
  | CWP_SKIPTRANSPARENT

type color_attribute =
    FOREGROUND_BLUE
  | FOREGROUND_GREEN
  | FOREGROUND_RED
  | FOREGROUND_INTENSITY
  | BACKGROUND_BLUE
  | BACKGROUND_GREEN
  | BACKGROUND_RED
  | BACKGROUND_INTENSITY

type control_type =
    ODT_BUTTON
  | ODT_COMBOBOX
  | ODT_LISTBOX
  | ODT_LISTVIEW
  | ODT_MENU
  | ODT_STATIC
  | ODT_TAB

type copy_image_option =
    LR_COPYDELETEORG
  | LR_COPYFROMRESOURCE
  | LR_COPYMONOCHROME   (* Win32 uses LR_MONOCHROME to indicate this *)
  | LR_COPYRETURNORG

type create_process_option =
    CREATE_DEFAULT_ERROR_MODE
  | CREATE_NEW_CONSOLE
  | CREATE_NEW_PROCESS_GROUP
  | CREATE_SEPARATE_WOW_VDM
  | CREATE_SHARED_WOW_VDM
  | CREATE_SUSPENDED
  | CREATE_UNICODE_ENVIRONMENT
  | DEBUG_PROCESS
  | DEBUG_ONLY_THIS_PROCESS
  | DETACHED_PROCESS

type ctrl_event =
    CTRL_BREAK_EVENT
  | CTRL_C_EVENT
  | CTRL_CLOSE_EVENT
  | CTRL_LOGOFF_EVENT
  | CTRL_SHUTDOWN_EVENT

type dialog_code =
    DLGC_BUTTON
  | DLGC_DEFPUSHBUTTON
  | DLGC_HASSETSEL
  | DLGC_RADIOBUTTON
  | DLGC_STATIC
  | DLGC_UNDEFPUSHBUTTON
  | DLGC_WANTALLKEYS
  | DLGC_WANTARROWS
  | DLGC_WANTCHARS
  | DLGC_WANTMESSAGE
  | DLGC_WANTTAB

type dib_color_type =
    DIB_PAL_COLORS
  | DIB_RGB_COLORS

type dlg_dir_list_option =
    DDL_ARCHIVE
  | DDL_DIRECTORY
  | DDL_DRIVES
  | DDL_EXCLUSIVE
  | DDL_HIDDEN
  | DDL_READONLY
  | DDL_READWRITE
  | DDL_SYSTEM
  | DDL_POSTMSGS

type draw_caption_option =
    DC_ACTIVE
  | DC_ICON
  | DC_INBUTTON
  | DC_SMALLCAP
  | DC_TEXT

type draw_frame_control_option =
    DFCS_ADJUSTRECT
  | DFCS_CHECKED
  | DFCS_FLAT
  | DFCS_INACTIVE
  | DFCS_MONO
  | DFCS_PUSHED

type draw_frame_control_type =
    DFC_BUTTON_3STATE
  | DFC_BUTTON_CHECK
  | DFC_BUTTON_PUSH
  | DFC_BUTTON_RADIO
  | DFC_BUTTON_RADIOIMAGE
  | DFC_BUTTON_RADIOMASK
  | DFC_CAPTION_CLOSE
  | DFC_CAPTION_HELP
  | DFC_CAPTION_MAX
  | DFC_CAPTION_MIN
  | DFC_CAPTION_RESTORE
  | DFC_MENU_ARROW
  | DFC_MENU_ARROWRIGHT
  | DFC_MENU_BULLET
  | DFC_MENU_CHECK
  | DFC_SCROLL_COMBOBOX
  | DFC_SCROLL_DOWN
  | DFC_SCROLL_LEFT
  | DFC_SCROLL_RIGHT
  | DFC_SCROLL_SIZEGRIP
  | DFC_SCROLL_SIZEGRIPRIGHT
  | DFC_SCROLL_UP

type draw_icon_option =
    DI_COMPAT
  | DI_DEFAULTSIZE
  | DI_IMAGE
  | DI_MASK
  | DI_NORMAL

type drive_type =
    DRIVE_UNKNOWN
  | DRIVE_NO_ROOT_DIR
  | DRIVE_REMOVABLE
  | DRIVE_FIXED
  | DRIVE_REMOTE
  | DRIVE_CDROM
  | DRIVE_RAMDISK

type enable_scroll_bar_option =
    ESB_DISABLE_BOTH
  | ESB_DISABLE_DOWN
  | ESB_DISABLE_LEFT
  | ESB_DISABLE_LTUP
  | ESB_DISABLE_RIGHT
  | ESB_DISABLE_RTDN
  | ESB_DISABLE_UP
  | ESB_ENABLE_BOTH

type error_mode_option =
    SEM_FAILCRITICALERRORS
  | SEM_NOALIGNMENTFAULTEXCEPT
  | SEM_NOGPFAULTERRORBOX
  | SEM_NOOPENFILEERRORBOX

type ext_text_out_option =
    ETO_CLIPPED
  | ETO_GLYPH_INDEX
  | ETO_OPAQUE
  | ETO_RTLREADING

type file_attribute =
    FILE_ATTRIBUTE_ARCHIVE
  | FILE_ATTRIBUTE_COMPRESSED
  | FILE_ATTRIBUTE_DIRECTORY
  | FILE_ATTRIBUTE_ENCRYPTED
  | FILE_ATTRIBUTE_HIDDEN
  | FILE_ATTRIBUTE_NORMAL
  | FILE_ATTRIBUTE_OFFLINE
  | FILE_ATTRIBUTE_READONLY
  | FILE_ATTRIBUTE_REPARSE_POINT
  | FILE_ATTRIBUTE_SPARSE_FILE
  | FILE_ATTRIBUTE_SYSTEM
  | FILE_ATTRIBUTE_TEMPORARY

type file_change_type =
    FILE_NOTIFY_CHANGE_ATTRIBUTES
  | FILE_NOTIFY_CHANGE_CREATION
  | FILE_NOTIFY_CHANGE_DIR_NAME
  | FILE_NOTIFY_CHANGE_FILE_NAME
  | FILE_NOTIFY_CHANGE_LAST_ACCESS
  | FILE_NOTIFY_CHANGE_LAST_WRITE
  | FILE_NOTIFY_CHANGE_SECURITY
  | FILE_NOTIFY_CHANGE_SIZE

type file_create_option =
    CREATE_ALWAYS
  | CREATE_NEW
  | OPEN_EXISTING
  | OPEN_ALWAYS
  | TRUNCATE_EXISTING

type file_flag =
    FILE_FLAG_BACKUP_SEMANTICS
  | FILE_FLAG_DELETE_ON_CLOSE
  | FILE_FLAG_NO_BUFFERING
  | FILE_FLAG_OPEN_NO_RECALL
  | FILE_FLAG_OPEN_REPARSE_POINT
  | FILE_FLAG_OVERLAPPED
  | FILE_FLAG_POSIX_SEMANTICS
  | FILE_FLAG_RANDOM_ACCESS
  | FILE_FLAG_SEQUENTIAL_SCAN
  | FILE_FLAG_WRITE_THROUGH

type file_share_option =
    FILE_SHARE_DELETE
  | FILE_SHARE_READ
  | FILE_SHARE_WRITE

type file_type =
    FILE_TYPE_CHAR
  | FILE_TYPE_DISK
  | FILE_TYPE_PIPE
  | FILE_TYPE_UNKNOWN

type flood_fill_type =
    FLOODFILLBORDER
  | FLOODFILLSURFACE

type font_clip_precision =
    CLIP_CHARACTER_PRECIS
  | CLIP_EMBEDDED
  | CLIP_LH_ANGLES
  | CLIP_STROKE_PRECIS
  | CLIP_TT_ALWAYS

type font_family =
    FF_DECORATIVE
  | FF_DONTCARE
  | FF_MODERN
  | FF_ROMAN
  | FF_SCRIPT
  | FF_SWISS

type font_output_precision =
    OUT_CHARACTER_PRECIS
  | OUT_DEFAULT_PRECIS
  | OUT_DEVICE_PRECIS
  | OUT_OUTLINE_PRECIS
  | OUT_RASTER_PRECIS
  | OUT_STRING_PRECIS
  | OUT_STROKE_PRECIS
  | OUT_TT_ONLY_PRECIS
  | OUT_TT_PRECIS

type font_pitch =
    DEFAULT_PITCH
  | FIXED_PITCH
  | VARIABLE_PITCH

type font_pitch_flag =
    TMPF_FIXED_PITCH
  | TMPF_VECTOR
  | TMPF_TRUETYPE
  | TMPF_DEVICE

type font_quality =
    ANTIALIASED_QUALITY
  | DEFAULT_QUALITY
  | DRAFT_QUALITY
  | NONANTIALIASED_QUALITY
  | PROOF_QUALITY

type font_type =
    DEVICE_FONTTYPE
  | RASTER_FONTTYPE
  | TRUETYPE_FONTTYPE

type get_dc_ex_option =
    DCX_WINDOW
  | DCX_CACHE
  | DCX_PARENTCLIP
  | DCX_CLIPSIBLINGS
  | DCX_CLIPCHILDREN
  | DCX_NORESETATTRS
  | DCX_LOCKWINDOWUPDATE
  | DCX_EXCLUDERGN
  | DCX_INTERSECTRGN
  | DCX_VALIDATE

type get_menu_default_item_option =
    GMDI_GOINTOPOPUPS
  | GMDI_USEDISABLED

type get_window_option =
    GW_CHILD
  | GW_HWNDFIRST
  | GW_HWNDLAST
  | GW_HWNDNEXT
  | GW_HWNDPREV
  | GW_OWNER

type graphics_mode =
    GM_COMPATIBLE
  | GM_ADVANCED

type handle_flag =
    HANDLE_FLAG_INHERIT
  | HANDLE_FLAG_PROTECT_FROM_CLOSE

type hatch_style =
    HS_BDIAGONAL
  | HS_CROSS
  | HS_DIAGCROSS
  | HS_FDIAGONAL
  | HS_HORIZONTAL
  | HS_VERTICAL

type hit_test_result =
    HTBORDER
  | HTBOTTOM
  | HTBOTTOMLEFT
  | HTBOTTOMRIGHT
  | HTCAPTION
  | HTCLIENT
  | HTCLOSE
  | HTERROR
  | HTGROWBOX
  | HTHELP
  | HTHSCROLL
  | HTLEFT
  | HTMENU
  | HTMAXBUTTON
  | HTMINBUTTON
  | HTNOWHERE
  | HTRIGHT
  | HTSYSMENU
  | HTTOP
  | HTTOPLEFT
  | HTTOPRIGHT
  | HTTRANSPARENT
  | HTVSCROLL

type hot_key_modifier =
    MOD_ALT
  | MOD_CONTROL
  | MOD_SHIFT
  | MOD_WIN

type icon_option =
    ICON_BIG
  | ICON_SMALL

type image_type =
    IMAGE_BITMAP
  | IMAGE_CURSOR
  | IMAGE_ENHMETAFILE
  | IMAGE_ICON

type input_event_type =
    MSGF_DDEMGR
  | MSGF_DIALOGBOX
  | MSGF_MENU
  | MSGF_NEXTWINDOW
  | MSGF_SCROLLBAR

type load_image_option =
    LR_CREATEDIBSECTION
  | LR_DEFAULTSIZE
  | LR_LOADMAP3DCOLORS
  | LR_LOADTRANSPARENT
  | LR_MONOCHROME
  | LR_SHARED
  | LR_VGACOLOR

type map_mode =
    MM_ANISOTROPIC
  | MM_HIENGLISH
  | MM_HIMETRIC
  | MM_ISOTROPIC
  | MM_LOENGLISH
  | MM_LOMETRIC
  | MM_TEXT
  | MM_TWIPS

type menu_item_enable_state =
    MF_DISABLED
  | MF_ENABLED
  | MF_GRAYED

type menu_item_option =
    MFT_MENUBARBREAK
  | MFT_MENUBREAK
  | MFT_OWNERDRAW
  | MFT_RADIOCHECK
  | MFT_RIGHTJUSTIFY
  | MFT_RIGHTORDER

type menu_item_specifier_type =
    MF_BYCOMMAND
  | MF_BYPOSITION

type message_box_option =
    MB_ABORTRETRYIGNORE
  | MB_APPLMODAL
  | MB_DEFAULT_DESKTOP_ONLY
  | MB_DEFBUTTON1
  | MB_DEFBUTTON2
  | MB_DEFBUTTON3
  | MB_DEFBUTTON4
  | MB_HELP
  | MB_ICONASTERISK
  | MB_ICONERROR
  | MB_ICONEXCLAMATION
  | MB_ICONHAND
  | MB_ICONINFORMATION
  | MB_ICONQUESTION
  | MB_ICONSTOP
  | MB_ICONWARNING
  | MB_OK
  | MB_OKCANCEL
  | MB_RETRYCANCEL
  | MB_RIGHT
  | MB_RTLREADING
  | MB_SETFOREGROUND
  | MB_SYSTEMMODAL
  | MB_TASKMODAL
  | MB_TOPMOST
  | MB_YESNO
  | MB_YESNOCANCEL

type mouse_activate_option =
    MA_ACTIVATE
  | MA_ACTIVATEANDEAT
  | MA_NOACTIVATE
  | MA_NOACTIVATEANDEAT

type mouse_modifier =
    MK_CONTROL
  | MK_LBUTTON
  | MK_MBUTTON
  | MK_RBUTTON
  | MK_SHIFT

type move_file_option =
    MOVEFILE_COPY_ALLOWED
  | MOVEFILE_DELAY_UNTIL_REBOOT
  | MOVEFILE_REPLACE_EXISTING
  | MOVEFILE_WRITE_THROUGH

type msg_wait_option =
    MWMO_ALERTABLE
  | MWMO_INPUTAVAILABLE
  | MWMO_WAITALL

type object_type =
    OBJ_BITMAP
  | OBJ_BRUSH
  | OBJ_FONT
  | OBJ_PAL
  | OBJ_PEN
  | OBJ_EXTPEN
  | OBJ_REGION
  | OBJ_DC
  | OBJ_MEMDC
  | OBJ_METAFILE
  | OBJ_METADC
  | OBJ_ENHMETAFILE
  | OBJ_ENHMETADC

type peek_message_option =
    PM_NOREMOVE
  | PM_NOYIELD
  | PM_REMOVE

type pen_endcap_style =
    PS_ENDCAP_ROUND
  | PS_ENDCAP_SQUARE
  | PS_ENDCAP_FLAT

type pen_join_style =
    PS_JOIN_BEVEL
  | PS_JOIN_MITER
  | PS_JOIN_ROUND

type pen_style =
    PS_ALTERNATE
  | PS_DASH
  | PS_DASHDOT
  | PS_DASHDOTDOT
  | PS_DOT
  | PS_INSIDEFRAME
  | PS_NULL
  | PS_SOLID
  | PS_USERSTYLE

type poly_fill_mode =
    ALTERNATE
  | WINDING

type print_option =
    PRF_CHECKVISIBLE
  | PRF_CHILDREN
  | PRF_CLIENT
  | PRF_ERASEBKGND
  | PRF_NONCLIENT
  | PRF_OWNED

type priority_class =
    HIGH_PRIORITY_CLASS
  | IDLE_PRIORITY_CLASS
  | NORMAL_PRIORITY_CLASS
  | REALTIME_PRIORITY_CLASS

type queue_status_option =
    QS_ALLEVENTS
  | QS_ALLINPUT
  | QS_ALLPOSTMESSAGE
  | QS_HOTKEY
  | QS_INPUT
  | QS_KEY
  | QS_MOUSE
  | QS_MOUSEBUTTON
  | QS_MOUSEMOVE
  | QS_PAINT
  | QS_POSTMESSAGE
  | QS_SENDMESSAGE
  | QS_TIMER

type redraw_window_option =
    RDW_ALLCHILDREN
  | RDW_ERASE
  | RDW_ERASENOW
  | RDW_FRAME
  | RDW_INTERNALPAINT
  | RDW_INVALIDATE
  | RDW_NOCHILDREN
  | RDW_NOERASE
  | RDW_NOFRAME
  | RDW_NOINTERNALPAINT
  | RDW_UPDATENOW
  | RDW_VALIDATE

type region_complexity =
    COMPLEXREGION
  | NULLREGION
  | SIMPLEREGION

type region_mode =
    RGN_AND
  | RGN_COPY
  | RGN_DIFF
  | RGN_OR
  | RGN_XOR

type scroll_bar_type =
    SB_BOTH
  | SB_CTL
  | SB_HORZ
  | SB_VERT

type scroll_window_option =
    SW_ERASE
  | SW_INVALIDATE
  | SW_SCROLLCHILDREN

type seek_method =
    FILE_BEGIN
  | FILE_CURRENT
  | FILE_END

type send_message_timeout_option =
    SMTO_ABORTIFHUNG
  | SMTO_BLOCK
  | SMTO_NORMAL

type set_window_placement_option =
    WPF_RESTORETOMAXIMIZED
  | WPF_SETMINPOSITION

type show_window_option =
    SW_FORCEMINIMIZE
  | SW_HIDE
  | SW_MAXIMIZE
  | SW_MINIMIZE
  | SW_RESTORE
  | SW_SHOW
  | SW_SHOWDEFAULT
  | SW_SHOWMAXIMIZED
  | SW_SHOWMINIMIZED
  | SW_SHOWMINNOACTIVE
  | SW_SHOWNA
  | SW_SHOWNOACTIVATE
  | SW_SHOWNORMAL

type show_window_reason =
    SW_SHOWWINDOW       (* Win32 uses 0 to indicate this *)
  | SW_OTHERUNZOOM
  | SW_OTHERZOOM
  | SW_PARENTCLOSING
  | SW_PARENTOPENING

type size_type =
    SIZE_MAXHIDE
  | SIZE_MAXIMIZED
  | SIZE_MAXSHOW
  | SIZE_MINIMIZED
  | SIZE_RESTORED

type sizing_side =
    WMSZ_BOTTOM
  | WMSZ_BOTTOMLEFT
  | WMSZ_BOTTOMRIGHT
  | WMSZ_LEFT
  | WMSZ_RIGHT
  | WMSZ_TOP
  | WMSZ_TOPLEFT
  | WMSZ_TOPRIGHT

type sqos_option =
    SECURITY_ANONYMOUS
  | SECURITY_CONTEXT_TRACKING
  | SECURITY_DELEGATION
  | SECURITY_EFFECTIVE_ONLY
  | SECURITY_IDENTIFICATION
  | SECURITY_IMPERSONATION
  | SECURITY_SQOS_PRESENT

type startupinfo_option =
    STARTF_FORCEONFEEDBACK
  | STARTF_FORCEOFFFEEDBACK

type std_handle =
    STD_INPUT_HANDLE
  | STD_OUTPUT_HANDLE
  | STD_ERROR_HANDLE

type stock_object =
    ANSI_FIXED_FONT
  | ANSI_VAR_FONT
  | BLACK_BRUSH
  | BLACK_PEN
  | DEFAULT_GUI_FONT
  | DEFAULT_PALETTE
  | DEVICE_DEFAULT_FONT
  | DKGRAY_BRUSH
  | GRAY_BRUSH
  | LTGRAY_BRUSH
  | NULL_BRUSH
  | OEM_FIXED_FONT
  | SYSTEM_FIXED_FONT
  | SYSTEM_FONT
  | WHITE_BRUSH
  | WHITE_PEN

type stretch_blt_mode =
    BLACKONWHITE
  | COLORONCOLOR
  | HALFTONE
  | WHITEONBLACK

type sys_color =
    COLOR_3DDKSHADOW
  | COLOR_3DFACE
  | COLOR_3DHIGHLIGHT
  | COLOR_3DHILIGHT
  | COLOR_3DLIGHT
  | COLOR_3DSHADOW
  | COLOR_ACTIVEBORDER
  | COLOR_ACTIVECAPTION
  | COLOR_APPWORKSPACE
  | COLOR_BACKGROUND
  | COLOR_BTNFACE
  | COLOR_BTNHIGHLIGHT
  | COLOR_BTNHILIGHT
  | COLOR_BTNSHADOW
  | COLOR_BTNTEXT
  | COLOR_CAPTIONTEXT
  | COLOR_DESKTOP
  | COLOR_GRAYTEXT
  | COLOR_HIGHLIGHT
  | COLOR_HIGHLIGHTTEXT
  | COLOR_INACTIVEBORDER
  | COLOR_INACTIVECAPTION
  | COLOR_INACTIVECAPTIONTEXT
  | COLOR_INFOBK
  | COLOR_INFOTEXT
  | COLOR_MENU
  | COLOR_MENUTEXT
  | COLOR_SCROLLBAR
  | COLOR_WINDOW
  | COLOR_WINDOWFRAME
  | COLOR_WINDOWTEXT

type system_palette_use =
    SYSPAL_NOSTATIC
  | SYSPAL_NOSTATIC256
  | SYSPAL_STATIC

type thread_priority =
    THREAD_PRIORITY_ABOVE_NORMAL
  | THREAD_PRIORITY_BELOW_NORMAL
  | THREAD_PRIORITY_HIGHEST
  | THREAD_PRIORITY_IDLE
  | THREAD_PRIORITY_LOWEST
  | THREAD_PRIORITY_NORMAL
  | THREAD_PRIORITY_TIME_CRITICAL

type tile_option =
    MDITILE_HORIZONTAL
  | MDITILE_SKIPDISABLED
  | MDITILE_VERTICAL

type track_popup_menu_option =
    TPM_BOTTOMALIGN
  | TPM_CENTERALIGN
  | TPM_HORIZONTAL
  | TPM_LEFTALIGN
  | TPM_LEFTBUTTON
  | TPM_NONOTIFY
  | TPM_RIGHTALIGN
  | TPM_RIGHTBUTTON
  | TPM_TOPALIGN
  | TPM_VCENTERALIGN
  | TPM_VERTICAL

(* Raw memory *)

type buffer = string

type pointer

external eq_pointer : pointer -> pointer -> bool = "eq_pointer"
external is_null_pointer : ptr:pointer -> bool = "is_null_pointer"
external get_null_pointer : unit -> pointer = "get_null_pointer"
let null_pointer = get_null_pointer ()
external int32_of_pointer : pointer -> int32 = "int32_of_pointer"
external pointer_of_int32 : int32 -> pointer = "pointer_of_int32"
external get_byte : ptr:pointer -> char = "get_byte"
external get_bytes : ptr:pointer -> num:int -> buffer = "get_bytes"
external set_byte : ptr:pointer -> byte:char -> unit = "set_byte"
external set_bytes : ptr:pointer -> bytes:buffer -> unit = "set_bytes"

type raw =
    Raw_buffer of buffer
  | Raw_subbuffer of buffer * int
  | Raw_pointer of pointer

type sized_raw =
    Sraw_buffer of buffer
  | Sraw_subbuffer of buffer * int * int
  | Sraw_pointer of pointer * int

(* Handles *)

type handle

external eq_handle : handle -> handle -> bool = "eq_handle"
external is_null_handle : handle:handle -> bool = "is_null_handle"
external get_null_handle : unit -> handle = "get_null_handle"
let null_handle = get_null_handle ()
external int32_of_handle : handle -> int32 = "int32_of_handle"
external handle_of_int32 : int32 -> handle = "handle_of_int32"

type haccel = handle
type hbitmap = handle
type hbrush = handle
type hcursor = handle
type hdc = handle
type hfont = handle
type hgdiobj = handle
type hicon = handle
type hinstance = handle
type hmenu = handle
type hpalette = handle
type hpen = handle
type hrgn = handle
type hrsrc = handle

type hwnd

external eq_hwnd : hwnd -> hwnd -> bool = "eq_hwnd"
external is_null_hwnd : hwnd:hwnd -> bool = "is_null_hwnd"
external get_null_hwnd : unit -> hwnd = "get_null_hwnd"
let null_hwnd = get_null_hwnd ()
external get_hwnd_broadcast : unit -> hwnd = "get_hwnd_broadcast"
let hwnd_broadcast = get_hwnd_broadcast ()
external get_hwnd_desktop : unit -> hwnd = "get_hwnd_desktop"
let hwnd_desktop = get_hwnd_desktop ()
external get_hwnd_topmost : unit -> hwnd = "get_hwnd_topmost"
let hwnd_topmost = get_hwnd_topmost ()
external int32_of_hwnd : hwnd -> int32 = "int32_of_hwnd"
external hwnd_of_int32 : int32 -> hwnd = "hwnd_of_int32"

(* Access control *)

type access_mask = int64

type access_right =
    ACCESS_SYSTEM_SECURITY
  | DELETE
  | GENERIC_ALL
  | GENERIC_EXECUTE
  | GENERIC_READ
  | GENERIC_WRITE
  | READ_CONTROL
  | SPECIFIC_RIGHTS_ALL
  | STANDARD_RIGHTS_ALL
  | STANDARD_RIGHTS_EXECUTE
  | STANDARD_RIGHTS_READ
  | STANDARD_RIGHTS_REQUIRED
  | STANDARD_RIGHTS_WRITE 
  | SYNCHRONIZE
  | WRITE_DAC
  | WRITE_OWNER

external access_mask_of_right : access_right -> access_mask = "access_mask_of_right"

let access_mask_of_rights rights =
    List.fold_left Int64.logor Int64.zero (List.map access_mask_of_right rights)

let access_mask_of_specific_rights ~specific_func ~rights:(rights, specific_rights) =
    Int64.logor
        (access_mask_of_rights rights)
        (List.fold_left Int64.logor Int64.zero
            (List.map specific_func specific_rights))

type security_descriptor = pointer

type security_attributes = {
    sa_descriptor : security_descriptor;
    sa_inherit_handle : bool }

let default_security_attributes = {
    sa_descriptor = null_pointer;
    sa_inherit_handle = false }

(* Time *)

type filetime = int64

type file_times = {
    ft_creation_time : filetime;
    ft_last_access_time : filetime;
    ft_last_write_time : filetime }

type systemtime = {
    st_year : int;
    st_month : int;
    st_day_of_week : int;
    st_day : int;
    st_hour : int;
    st_minute : int;
    st_second : int;
    st_milliseconds : int }

type delay =
    Delay_none
  | Delay_milliseconds of int
  | Delay_seconds of int
  | Delay_infinite

type wait_result =
    Wait_object of int
  | Wait_abandoned of int
  | Wait_io_completion
  | Wait_new_input
  | Wait_timeout

(* Files *)

type find_data = {
    fd_file_attributes : file_attribute list;
    fd_times : file_times;
    fd_file_size : int64;
    fd_file_name : string;
    fd_alternate_file_name : string }

type by_handle_file_information = {
    bhfi_file_attributes : file_attribute list;
    bhfi_times : file_times;
    bhfi_volume_serial_number : int32;
    bhfi_file_size : int64;
    bhfi_number_of_links : int;
    bhfi_file_index : int64 }

(* Overlapped I/O *)

type overlapped

type overlapped_spec = {
    ovl_internal : int32;
    ovl_internal_high : int32;
    ovl_offset : int64;
    ovl_event : handle }

external make_overlapped : unit -> overlapped = "make_overlapped"
external get_overlapped_spec : overlapped:overlapped -> overlapped_spec = "get_overlapped_spec"
external set_overlapped_spec : overlapped:overlapped -> spec:overlapped_spec -> unit = "set_overlapped_spec"

let make_overlapped_with_event ~event =
    let ovl = make_overlapped () in
    let spec = get_overlapped_spec ~overlapped:ovl in
    set_overlapped_spec ~overlapped:ovl ~spec:{ spec with ovl_event = event };
    ovl

(* Points and rectangles *)

type point = {
    x : int;
    y : int }

type rect = {
    left : int;
    top : int;
    right : int;
    bottom : int }

type size = {
    cx : int;
    cy : int }

type xform = {
    xf_m11 : float;
    xf_m12 : float;
    xf_m21 : float;
    xf_m22 : float;
    xf_dx : float;
    xf_dy : float }

(* Regions *)

type rgndata = {
    rd_count : int;
    rd_bound : rect;
    rd_buffer : buffer }

external pack_rects : rects:rect array -> buffer = "pack_rects"
external unpack_rect : buf:buffer -> index:int -> rect = "unpack_rect"
external unpack_rects : buf:buffer -> rect array = "unpack_rects"

(* Names encoded in integers *)

type atom_name =
    An_string of string
  | An_int of int

type resource_type =
    Rt_string of string
  | Rt_int of int

type standard_resource_type =
    RT_ACCELERATOR
  | RT_ANICURSOR
  | RT_ANIICON
  | RT_BITMAP
  | RT_CURSOR
  | RT_DIALOG
  | RT_FONT
  | RT_FONTDIR
  | RT_GROUP_CURSOR
  | RT_GROUP_ICON
  | RT_HTML
  | RT_ICON
  | RT_MENU
  | RT_MESSAGETABLE
  | RT_PLUGPLAY
  | RT_RCDATA
  | RT_STRING
  | RT_VERSION
  | RT_VXD

external resource_type_of_standard : standard_resource_type -> resource_type = "resource_type_of_standard"

type resource_name =
    Rn_string of string
  | Rn_int of int

type standard_resource_name =
    IDC_APPSTARTING
  | IDC_ARROW
  | IDC_CROSS
  | IDC_HELP
  | IDC_IBEAM
  | IDC_ICON
  | IDC_NO
  | IDC_SIZE
  | IDC_SIZEALL
  | IDC_SIZENESW
  | IDC_SIZENS
  | IDC_SIZENWSE
  | IDC_SIZEWE
  | IDC_UPARROW
  | IDC_WAIT
  | IDI_APPLICATION
  | IDI_ASTERISK
  | IDI_ERROR
  | IDI_EXCLAMATION
  | IDI_HAND
  | IDI_INFORMATION
  | IDI_QUESTION
  | IDI_WARNING
  | IDI_WINLOGO

external resource_name_of_standard : standard_resource_name -> resource_name = "resource_name_of_standard"

(* Control ids *)

type standard_control_id =
    IDABORT
  | IDCANCEL
  | IDCLOSE
  | IDHELP
  | IDIGNORE
  | IDNO
  | IDOK
  | IDRETRY
  | IDYES

external control_id_of_standard : standard_control_id -> int = "control_id_of_standard"

(* Window styles *)

type style = int64

type style_option =
    WS_BORDER
  | WS_CAPTION
  | WS_CHILD
  | WS_CHILDWINDOW
  | WS_CLIPCHILDREN
  | WS_CLIPSIBLINGS
  | WS_DISABLED
  | WS_DLGFRAME
  | WS_GROUP
  | WS_HSCROLL
  | WS_ICONIC
  | WS_MAXIMIZE
  | WS_MAXIMIZEBOX
  | WS_MINIMIZE
  | WS_MINIMIZEBOX
  | WS_OVERLAPPED
  | WS_OVERLAPPEDWINDOW
  | WS_POPUP
  | WS_POPUPWINDOW
  | WS_SIZEBOX
  | WS_SYSMENU
  | WS_TABSTOP
  | WS_THICKFRAME
  | WS_TILED
  | WS_TILEDWINDOW
  | WS_VISIBLE
  | WS_VSCROLL
  | WS_EX_ACCEPTFILES
  | WS_EX_APPWINDOW
  | WS_EX_CLIENTEDGE
  | WS_EX_CONTEXTHELP
  | WS_EX_CONTROLPARENT
  | WS_EX_DLGMODALFRAME
  | WS_EX_LEFT
  | WS_EX_LEFTSCROLLBAR
  | WS_EX_LTRREADING
  | WS_EX_MDICHILD
  | WS_EX_NOPARENTNOTIFY
  | WS_EX_OVERLAPPEDWINDOW
  | WS_EX_PALETTEWINDOW
  | WS_EX_RIGHT
  | WS_EX_RIGHTSCROLLBAR
  | WS_EX_RTLREADING
  | WS_EX_STATICEDGE
  | WS_EX_TOOLWINDOW
  | WS_EX_TOPMOST
  | WS_EX_TRANSPARENT
  | WS_EX_WINDOWEDGE

external style_of_option : style_option -> style = "style_of_option"

let style_of_options options =
    List.fold_left Int64.logor Int64.zero (List.map style_of_option options)

let special_style_of_options ~special_func ~options:(options, special_options) =
    Int64.logor
        (List.fold_left Int64.logor Int64.zero
            (List.map style_of_option options))
        (List.fold_left Int64.logor Int64.zero
            (List.map special_func special_options))

(* Window creation *)

type createstruct = {
    cs_class_name : atom_name;
    cs_window_name : string;
    cs_style : style;
    cs_position : point option;
    cs_size : size option;
    cs_parent : hwnd;
    cs_menu : hmenu;
    cs_child_id : int;
    cs_instance : hinstance;
    cs_param : pointer }

(* Window positioning *)

type z_order =
    Zo_topmost
  | Zo_notopmost
  | Zo_top
  | Zo_insert_after of hwnd
  | Zo_bottom

type set_window_pos_option =
    SWP_ASYNCWINDOWPOS
  | SWP_DEFERERASE
  | SWP_DRAWFRAME
  | SWP_FRAMECHANGED
  | SWP_HIDEWINDOW
  | SWP_NOACTIVATE
  | SWP_NOCOPYBITS
  | SWP_NOMOVE
  | SWP_NOOWNERZORDER
  | SWP_NOREDRAW
  | SWP_NOREPOSITION
  | SWP_NOSENDCHANGING
  | SWP_NOSIZE
  | SWP_NOZORDER
  | SWP_SHOWWINDOW

type windowpos = {
    wp_hwnd : hwnd;
    wp_z_order : z_order;
    wp_position : point;
    wp_size : size;
    wp_options : set_window_pos_option list }

type windowplacement = {
    wpl_show_cmd : show_window_option;
    wpl_min_position : point;
    wpl_max_position : point;
    wpl_normal_position : rect }

type minmaxinfo = {
    mmi_max_size : size;
    mmi_max_position : point;
    mmi_min_track_size : size;
    mmi_max_track_size : size }

(* Colors *)

type rgb = {
    red : int;
    green : int;
    blue : int }

type color =
    Clr_rgb of rgb
  | Clr_palettergb of rgb
  | Clr_paletteindex of int
  | Clr_none
  | Clr_default

(* Raster operations *)

type rop2 =
    R2_BLACK
  | R2_COPYPEN
  | R2_MASKNOTPEN
  | R2_MASKPEN
  | R2_MASKPENNOT
  | R2_MERGENOTPEN
  | R2_MERGEPEN
  | R2_MERGEPENNOT
  | R2_NOP
  | R2_NOT
  | R2_NOTCOPYPEN
  | R2_NOTMASKPEN
  | R2_NOTMERGEPEN
  | R2_NOTXORPEN
  | R2_WHITE
  | R2_XORPEN

type rop3 = int

type rop3_common =
    BLACKNESS
  | DSTINVERT
  | MERGECOPY
  | MERGEPAINT
  | NOTSRCCOPY
  | NOTSRCERASE
  | PATCOPY
  | PATINVERT
  | PATPAINT
  | SRCAND
  | SRCCOPY
  | SRCERASE
  | SRCINVERT
  | SRCPAINT
  | WHITENESS

external rop3_of_common : rop3_common -> rop3 = "rop3_of_common"

(* GDI objects *)

type bitmap = {
    bm_size : size;
    bm_width_bytes : int;
    bm_planes : int;
    bm_bits_pixel : int;
    bm_bits : raw }

type logbrush =
    Lb_null
  | Lb_solid of color
  | Lb_hatched of color * hatch_style
  | Lb_pattern of hbitmap
  | Lb_pattern8x8 of hbitmap
  | Lb_dibpattern of dib_color_type * handle
  | Lb_dibpattern8x8 of dib_color_type * handle
  | Lb_dibpatternpt of dib_color_type * raw

type logpen = {
    lp_style : pen_style;
    lp_width : size;
    lp_color : color }

type ext_pen_style =
    Eps_cosmetic of pen_style
  | Eps_geometric of pen_style * pen_endcap_style * pen_join_style

(* Fonts *)

type char_set =
    Ansi_charset
  | Arabic_charset
  | Baltic_charset
  | Chinesebig5_charset
  | Default_charset
  | Easteurope_charset
  | Gb2312_charset
  | Greek_charset
  | Hangul_charset
  | Hebrew_charset
  | Johab_charset
  | Mac_charset
  | Oem_charset
  | Russian_charset
  | Shiftjis_charset
  | Symbol_charset
  | Thai_charset
  | Turkish_charset
  | Unknown_charset of int

type logfont = {
    lf_height : int;
    lf_width : int;
    lf_escapement : int;
    lf_orientation : int;
    lf_weight : int;
    lf_italic : bool;
    lf_underline : bool;
    lf_strike_out : bool;
    lf_char_set : char_set;
    lf_out_precision : font_output_precision;
    lf_clip_precision : font_clip_precision list;
    lf_quality : font_quality;
    lf_pitch : font_pitch;
    lf_family : font_family;
    lf_face_name : string }

let fw_dontcare = 0
let fw_thin = 100
let fw_extralight = 200
let fw_light = 300
let fw_normal = 400
let fw_medium = 500
let fw_semibold = 600
let fw_bold = 700
let fw_extrabold = 800
let fw_heavy = 900

let default_logfont = {
    lf_height = 0;
    lf_width = 0;
    lf_escapement = 0;
    lf_orientation = 0;
    lf_weight = fw_dontcare;
    lf_italic = false;
    lf_underline = false;
    lf_strike_out = false;
    lf_char_set = Default_charset;
    lf_out_precision = OUT_DEFAULT_PRECIS;
    lf_clip_precision = [];
    lf_quality = DEFAULT_QUALITY;
    lf_pitch = DEFAULT_PITCH;
    lf_family = FF_DONTCARE;
    lf_face_name = "" }

type textmetric = {
    tm_height : int;
    tm_ascent : int;
    tm_descent : int;
    tm_internal_leading : int;
    tm_external_leading : int;
    tm_ave_char_width : int;
    tm_max_char_width : int;
    tm_weight : int;
    tm_overhang : int;
    tm_digitized_aspect_x : int;
    tm_digitized_aspect_y : int;
    tm_first_char : char;
    tm_last_char : char;
    tm_default_char : char;
    tm_break_char : char;
    tm_italic : bool;
    tm_underlined : bool;
    tm_struck_out : bool;
    tm_pitch : font_pitch_flag list;
    tm_family : font_family;
    tm_char_set : char_set }

type font_flag =
    Ffl_italic
  | Ffl_underscore
  | Ffl_negative
  | Ffl_outline
  | Ffl_strikeout
  | Ffl_bold

type newtextmetric = {
    ntm_tm : textmetric;
    ntm_flags : font_flag list;
    ntm_size_em : int;
    ntm_cell_height : int;
    ntm_avg_width : int }

type abc = {
    abc_a : int;
    abc_b : int;
    abc_c : int }

type abcfloat = {
    abcf_a : float;
    abcf_b : float;
    abcf_c : float }

type kerningpair = {
    kp_first : char;
    kp_second : char;
    kp_kern_amount : int }

(* Text *)

type text =
    Tx_string of string
  | Tx_substring of string * int * int

external pack_glyphs : glyphs:int32 array -> string = "pack_glyphs"
external unpack_glyph : buf:string -> index:int -> int32 = "unpack_glyph"
external unpack_glyphs : buf:string -> int32 array = "unpack_glyphs"

type text_horizontal_alignment =
    TA_LEFT
  | TA_RIGHT
  | TA_CENTER

type text_vertical_alignment =
    TA_TOP
  | TA_BOTTOM
  | TA_BASELINE

(* ... VTA_* *)

type text_alignment = {
    ta_horizontal : text_horizontal_alignment;
    ta_vertical : text_vertical_alignment;
    ta_updatecp : bool }

let default_text_alignment = {
    ta_horizontal = TA_LEFT;
    ta_vertical = TA_TOP;
    ta_updatecp = false }

(* Painting *)

type paintstruct = {
    ps_hdc : hdc;
    ps_erase : bool;
    ps_rect : rect;
    ps_reserved : buffer }

(* Edge types *)

type inner_edge_type =
    BDR_RAISEDINNER
  | BDR_SUNKENINNER

type outer_edge_type =
    BDR_RAISEDOUTER
  | BDR_SUNKENOUTER

type edge_type = {
    inner : inner_edge_type;
    outer : outer_edge_type }

let edge_type_bump = { inner = BDR_SUNKENINNER; outer = BDR_RAISEDOUTER }
let edge_type_etched = { inner = BDR_RAISEDINNER; outer = BDR_SUNKENOUTER }
let edge_type_raised = { inner = BDR_RAISEDINNER; outer = BDR_RAISEDOUTER }
let edge_type_sunken = { inner = BDR_SUNKENINNER; outer = BDR_SUNKENOUTER }

(* Icons *)

type iconinfo = {
    ii_is_icon : bool;
    ii_hotspot : point;
    ii_mask_bitmap : hbitmap;
    ii_color_bitmap : hbitmap }

(* Menus *)

type menu_item_contents =
    Mic_separator
  | Mic_string of string
  | Mic_bitmap of hbitmap

type menu_item = {
    mi_contents : menu_item_contents;
    mi_options : menu_item_option list }

type menu_item_state = {
    mis_checked : bool;
    mis_default : bool;
    mis_enable : menu_item_enable_state;
    mis_hilite : bool }

type menu_item_check_marks = {
    micm_unchecked : hbitmap;
    micm_checked : hbitmap }

type get_menu_item_info = {
    gmii_item : menu_item;
    gmii_state : menu_item_state;
    gmii_id : int;
    gmii_submenu : hmenu;
    gmii_check_marks : menu_item_check_marks;
    gmii_data : int32 }

type set_menu_item_info = {
    smii_item : menu_item option;
    smii_state : menu_item_state option;
    smii_id : int option;
    smii_submenu : hmenu option;
    smii_check_marks : menu_item_check_marks option;
    smii_data : int32 option }

let default_set_menu_item_info = {
    smii_item = None;
    smii_state = None;
    smii_id = None;
    smii_submenu = None;
    smii_check_marks = None;
    smii_data = None }

(* Keyboard input *)

type virtual_key_code =
    Vk_lbutton
  | Vk_rbutton
  | Vk_cancel
  | Vk_mbutton
  | Vk_back
  | Vk_tab
  | Vk_clear
  | Vk_return
  | Vk_shift
  | Vk_control
  | Vk_menu
  | Vk_pause
  | Vk_capital
  | Vk_escape
  | Vk_space
  | Vk_prior
  | Vk_next
  | Vk_end
  | Vk_home
  | Vk_left
  | Vk_up
  | Vk_right
  | Vk_down
  | Vk_select
  | Vk_execute
  | Vk_snapshot
  | Vk_insert
  | Vk_delete
  | Vk_help
  | Vk_digit of int
  | Vk_letter of char
  | Vk_lwin
  | Vk_rwin
  | Vk_apps
  | Vk_numpad of int
  | Vk_multiply
  | Vk_add
  | Vk_separator
  | Vk_subtract
  | Vk_decimal
  | Vk_divide
  | Vk_f of int
  | Vk_numlock
  | Vk_scroll
  | Vk_lshift
  | Vk_rshift
  | Vk_lcontrol
  | Vk_rcontrol
  | Vk_lmenu
  | Vk_rmenu
  | Vk_attn
  | Vk_crsel
  | Vk_exsel
  | Vk_ereof
  | Vk_play
  | Vk_zoom
  | Vk_noname
  | Vk_pa1
  | Vk_oem_clear
  | Vk_unassigned of int

type key_data = {
    kd_repeat_count : int;
    kd_scan_code : int;
    kd_extended : bool;
    kd_dlgmode : bool;
    kd_menumode : bool;
    kd_altdown : bool;
    kd_repeat : bool;
    kd_up : bool }

(* Scroll bars *)

type scroll_action =
    Sb_bottom
  | Sb_endscroll
  | Sb_left
  | Sb_linedown
  | Sb_lineleft
  | Sb_lineright
  | Sb_lineup
  | Sb_pagedown
  | Sb_pageleft
  | Sb_pageright
  | Sb_pageup
  | Sb_right
  | Sb_thumbposition of int
  | Sb_thumbtrack of int
  | Sb_top

type get_scroll_info = {
    gsi_range : int * int;
    gsi_page : int;
    gsi_pos : int;
    gsi_track_pos : int }

type set_scroll_info = {
    ssi_range : (int * int) option;
    ssi_page : int option;
    ssi_pos : int option;
    ssi_disablenoscroll : bool }

let default_set_scroll_info = {
    ssi_range = None;
    ssi_page = None;
    ssi_pos = None;
    ssi_disablenoscroll = false }

(* Processes *)

type std_handles = {
    input : handle;
    output : handle;
    error : handle }

type startupinfo = {
    si_desktop : string option;
    si_title : string option;
    si_position : point option;
    si_size : size option;
    si_count_chars : size option;
    si_fill_attribute : color_attribute list option;
    si_flags : startupinfo_option list;
    si_show_window : show_window_option option;
    si_std_handles : std_handles option }

let default_startupinfo = {
    si_desktop = None;
    si_title = None;
    si_position = None;
    si_size = None;
    si_count_chars = None;
    si_fill_attribute = None;
    si_flags = [];
    si_show_window = None;
    si_std_handles = None }

type create_process_params = {
    cpp_application_name : string option;
    cpp_command_line : string option;
    cpp_process_attributes : security_attributes option;
    cpp_thread_attributes : security_attributes option;
    cpp_inherit_handles : bool;
    cpp_options : create_process_option list;
    cpp_priority_class : priority_class option;
    cpp_environment : string array option;
    cpp_current_directory : string option;
    cpp_startupinfo : startupinfo }

let default_create_process_params = {
    cpp_application_name = None;
    cpp_command_line = None;
    cpp_process_attributes = None;
    cpp_thread_attributes = None;
    cpp_inherit_handles = false;
    cpp_options = [];
    cpp_priority_class = None;
    cpp_environment = None;
    cpp_current_directory = None;
    cpp_startupinfo = default_startupinfo }

type process_information = {
    pi_process : handle;
    pi_thread : handle;
    pi_process_id : int32;
    pi_thread_id : int32 }

type termination_status =
    Ts_still_active
  | Ts_terminated of int32

type process_times = {
    pt_creation_time : filetime;
    pt_exit_time : filetime;
    pt_kernel_time : filetime;
    pt_user_time : filetime }

(* Interprocess communication *)

type copydatastruct = {
    cds_short_data : int32;
    cds_long_data : sized_raw option }

(********************************************************************)
(* Message handling *)

(* Messages *)

type message_id = int

let wm_user = 0x0400
let wm_app = 0x8000

type message_contents = {
    mc_id : message_id;
    mc_wparam : int32;
    mc_lparam : int32 }

let pack_simple_message ~id = {
    mc_id = id;
    mc_wparam = Int32.zero;
    mc_lparam = Int32.zero }

type msg = {
    msg_hwnd : hwnd;
    msg_contents : message_contents;
    msg_time : int32;
    msg_pt : point }

type message_result = int32

type message_sender = msg:message_contents -> message_result

(* Message processors *)

type message_processor = wnd:hwnd -> msg:message_contents -> message_result option

let null_message_processor = fun ~wnd ~msg -> None

let sequence_message_processors p1 p2 =
    fun ~wnd ~msg ->
        match p1 ~wnd ~msg with
          | Some _ as r -> r
          | None -> p2 ~wnd ~msg

let combine_message_processors l =
    List.fold_right sequence_message_processors l null_message_processor

(* Message handlers *)

type 'a message_handler_result =
    Mhr_not_handled
  | Mhr_raw of message_result
  | Mhr_val of 'a

let message_not_handled = Mhr_not_handled
let message_handled = Mhr_val ()
let message_return x = Mhr_val x
let message_return_raw x = Mhr_raw x

type ('a, 'b) message_deliverer =
    handler:'a ->
    wnd:hwnd ->
    msg:message_contents ->
    'b message_handler_result

type 'b message_returner = result:'b -> message_result

let on_message ~id ~deliver ~return =
    fun handler ->
        fun ~wnd ~msg ->
            if msg.mc_id = id then
                match deliver ~handler ~wnd ~msg with
                    Mhr_not_handled -> None
                  | Mhr_raw result -> Some result
                  | Mhr_val result -> Some (return ~result)
            else
                None

(* Specialized message handlers *)

type 'a plain_message_handler =
    wnd:hwnd -> msg:message_contents -> 'a message_handler_result
let deliver_plain_message ~handler ~wnd ~msg = handler ~wnd ~msg

type simple_message_handler = unit plain_message_handler
let return_simple_message ~result = Int32.zero
let on_simple_message ~id =
    on_message ~id ~deliver:deliver_plain_message ~return:return_simple_message

type general_message_handler = message_result plain_message_handler
let return_general_message ~result = result
let on_general_message ~id =
    on_message ~id ~deliver:deliver_plain_message ~return:return_general_message

let on_user_message ~num = on_general_message ~id:(wm_user + num)
let on_app_message ~num = on_general_message ~id:(wm_app + num)

type mouse_message_handler =
    wnd:hwnd ->
    msg:message_contents ->
    modifiers:mouse_modifier list ->
    pt:point ->
    unit message_handler_result
external deliver_mouse_message :
    handler:mouse_message_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_mouse_message"
let on_mouse_message ~id =
    on_message ~id ~deliver:deliver_mouse_message ~return:return_simple_message

type mouse_nc_message_handler =
    wnd:hwnd ->
    msg:message_contents ->
    hit:hit_test_result ->
    pt:point ->
    unit message_handler_result
external deliver_mouse_nc_message :
    handler:mouse_nc_message_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_mouse_nc_message"
let on_mouse_nc_message ~id =
    on_message ~id ~deliver:deliver_mouse_nc_message ~return:return_simple_message

type char_message_handler =
    wnd:hwnd ->
    msg:message_contents ->
    char:char ->
    data:key_data ->
    unit message_handler_result
external deliver_char_message :
    handler:char_message_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_char_message"
let on_char_message ~id =
    on_message ~id ~deliver:deliver_char_message ~return:return_simple_message

type key_message_handler =
    wnd:hwnd ->
    msg:message_contents ->
    key:virtual_key_code ->
    data:key_data ->
    unit message_handler_result
external deliver_key_message :
    handler:key_message_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_key_message"
let on_key_message ~id =
    on_message ~id ~deliver:deliver_key_message ~return:return_simple_message

type ctlcolor_message_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dc:hdc ->
    target:hwnd ->
    hbrush message_handler_result
external deliver_ctlcolor_message :
    handler:ctlcolor_message_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    hbrush message_handler_result = "deliver_ctlcolor_message"
external return_ctlcolor_message : result:hbrush -> message_result = "return_ctlcolor_message"
let on_ctlcolor_message ~id =
    on_message ~id ~deliver:deliver_ctlcolor_message ~return:return_ctlcolor_message

(********************************************************************)
(* Window procedures *)

type window_function = wnd:hwnd -> msg:message_contents -> message_result
type window_proc =
    Wp_function of window_function
  | Wp_external of pointer

external set_dialog_result : wnd:hwnd -> result:int32 -> unit = "set_dialog_result"

let explicit_dialog_returns = ref []

let add_explicit_dialog_return ~id =
    explicit_dialog_returns := id :: !explicit_dialog_returns

(********************************************************************)
(* Window utilities *)

external set_window_action : wnd:hwnd -> action:(unit -> unit) -> unit = "set_window_action"
external do_window_action : wnd:hwnd -> unit = "do_window_action"

(********************************************************************)
(* Errors *)

type error_code = int
exception Error of error_code
let _ = Callback.register_exception "win32 error" (Error 0)

exception Region_error
let _ = Callback.register_exception "win32 region error" Region_error

type combo_box_error_type =
    Cbe_general
  | Cbe_space
exception Combo_box_error of combo_box_error_type
let _ = Callback.register_exception "win32 combo box error" (Combo_box_error Cbe_general)

type list_box_error_type =
    Lbe_general
  | Lbe_space
exception List_box_error of list_box_error_type
let _ = Callback.register_exception "win32 list box error" (List_box_error Lbe_general)

(********************************************************************)
(* Global variables *)

external get_globals : unit -> hinstance * string * show_window_option = "get_globals"

let the_instance, the_command_line, the_show_command = get_globals ()
