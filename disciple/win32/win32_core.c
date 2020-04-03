/* OCaml-Win32
 * win32_core.c
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
 */

#include "win32.h"

/********************************************************************/
/* Simple type conversions */

char* new_string(value v)
{
    return strdup(String_val(v));
}

void free_string(char* s)
{
    free(s);
}

value alloc_some(value v)
{
    CAMLparam1(v);
    CAMLlocal1(val);

    val = alloc(1, 0);
    Store_field(val, 0, v);
    CAMLreturn(val);
}

char* stringoption_val(value v)
{
    if (Is_some(v))
        return String_val(Value_of(v));
    else
        return NULL;
}

char* new_stringoption(value v)
{
    if (Is_some(v))
        return strdup(String_val(Value_of(v)));
    else
        return NULL;
}

void free_stringoption(char* s)
{
    if (s)
        free(s);
}

value alloc_cons(value hd, value tl)
{
    CAMLparam2(hd, tl);
    CAMLlocal1(val);

    val = alloc(2, 0);
    Store_field(val, 0, hd);
    Store_field(val, 1, tl);
    CAMLreturn(val);
}

int list_length(value list)
{
    int length;

    length = 0;
    for (; !Is_nil(list); list = Tail(list))
        ++length;
    return length;
}

value val_constant_with_count(DWORD constant, int num_constants, DWORD* constants)
{
    int i;

    for (i = 0; i < num_constants; ++i)
        if (constants[i] == constant)
            return Val_int(i);
    failwith("Invalid constant");
    return Val_int(0);
}

DWORD flags_val(value list, DWORD* constants)
{
    int result;

    result = 0;
    for (; !Is_nil(list); list = Tail(list))
        result |= Constant_val(Head(list), constants);
    return result;
}

value alloc_flags_with_count(DWORD constant, int num_constants, DWORD* constants)
{
    CAMLparam0();
    CAMLlocal1(result);
    int i;

    result = Val_nil;
    for (i = num_constants - 1; i >= 0; --i)
        if (constant & constants[i])
            result = alloc_cons(Val_int(i), result);
    CAMLreturn(result);
}

/********************************************************************/
/* Types */

/* Constants */

DWORD activate_flags[] = {
    WA_ACTIVE,
    WA_CLICKACTIVE,
    WA_INACTIVE
};
Define_constants(activate_flags);

#ifndef IDANI_CLOSE
#	define IDANI_CLOSE 2 
#endif

DWORD animation_types[] = {
    IDANI_CAPTION,
    IDANI_CLOSE,
    IDANI_OPEN
};
Define_constants(animation_types);

DWORD arc_directions[] = {
    AD_COUNTERCLOCKWISE,
    AD_CLOCKWISE
};
Define_constants(arc_directions);

DWORD background_modes[] = {
    OPAQUE,
    TRANSPARENT
};
Define_constants(background_modes);

DWORD binary_types[] = {
    SCS_32BIT_BINARY,
    SCS_DOS_BINARY,
    SCS_OS216_BINARY,
    SCS_PIF_BINARY,
    SCS_POSIX_BINARY,
    SCS_WOW_BINARY
};
Define_constants(binary_types);

DWORD border_options[] = {
    BF_ADJUST,
    BF_BOTTOM,
    BF_BOTTOMLEFT,
    BF_BOTTOMRIGHT,
    BF_DIAGONAL,
    BF_DIAGONAL_ENDBOTTOMLEFT,
    BF_DIAGONAL_ENDBOTTOMRIGHT,
    BF_DIAGONAL_ENDTOPLEFT,
    BF_DIAGONAL_ENDTOPRIGHT,
    BF_FLAT,
    BF_LEFT,
    BF_MIDDLE,
    BF_MONO,
    BF_RECT,
    BF_RIGHT,
    BF_SOFT,
    BF_TOP,
    BF_TOPLEFT,
    BF_TOPRIGHT
};
Define_constants(border_options);

DWORD broadcast_system_message_options[] = {
    BSF_FLUSHDISK,
    BSF_FORCEIFHUNG,
    BSF_IGNORECURRENTTASK,
    BSF_NOHANG,
    BSF_NOTIMEOUTIFNOTHUNG,
    BSF_POSTMESSAGE,
    BSF_QUERY
};
Define_constants(broadcast_system_message_options);

DWORD broadcast_system_message_recipients[] = {
    BSM_ALLCOMPONENTS,
    BSM_ALLDESKTOPS,
    BSM_APPLICATIONS,
    BSM_INSTALLABLEDRIVERS,
    BSM_NETDRIVER,
    BSM_VXDS
};
Define_constants(broadcast_system_message_recipients);

DWORD button_states[] = {
    BST_CHECKED,
    BST_INDETERMINATE,
    BST_UNCHECKED
};
Define_constants(button_states);

DWORD button_state_exs[] = {
    BST_FOCUS,
    BST_PUSHED
};
Define_constants(button_state_exs);

DWORD child_window_options[] = {
    CWP_ALL,
    CWP_SKIPDISABLED,
    CWP_SKIPINVISIBLE,
    CWP_SKIPTRANSPARENT
};
Define_constants(child_window_options);

DWORD color_attributes[] = {
    FOREGROUND_BLUE,
    FOREGROUND_GREEN,
    FOREGROUND_RED,
    FOREGROUND_INTENSITY,
    BACKGROUND_BLUE,
    BACKGROUND_GREEN,
    BACKGROUND_RED,
    BACKGROUND_INTENSITY
};
Define_constants(color_attributes);

DWORD control_types[] = {
    ODT_BUTTON,
    ODT_COMBOBOX,
    ODT_LISTBOX,
    ODT_LISTVIEW,
    ODT_MENU,
    ODT_STATIC,
    ODT_TAB
};
Define_constants(control_types);

DWORD copy_image_options[] = {
    LR_COPYDELETEORG,
    LR_COPYFROMRESOURCE,
    LR_MONOCHROME,
    LR_COPYRETURNORG
};
Define_constants(copy_image_options);

DWORD create_process_options[] = {
    CREATE_DEFAULT_ERROR_MODE,
    CREATE_NEW_CONSOLE,
    CREATE_NEW_PROCESS_GROUP,
    CREATE_SEPARATE_WOW_VDM,
    CREATE_SHARED_WOW_VDM,
    CREATE_SUSPENDED,
    CREATE_UNICODE_ENVIRONMENT,
    DEBUG_PROCESS,
    DEBUG_ONLY_THIS_PROCESS,
    DETACHED_PROCESS
};
Define_constants(create_process_options);

DWORD ctrl_events[] = {
    CTRL_BREAK_EVENT,
    CTRL_C_EVENT,
    CTRL_CLOSE_EVENT,
    CTRL_LOGOFF_EVENT,
    CTRL_SHUTDOWN_EVENT
};
Define_constants(ctrl_events);

DWORD dialog_codes[] = {
    DLGC_BUTTON,
    DLGC_DEFPUSHBUTTON,
    DLGC_HASSETSEL,
    DLGC_RADIOBUTTON,
    DLGC_STATIC,
    DLGC_UNDEFPUSHBUTTON,
    DLGC_WANTALLKEYS,
    DLGC_WANTARROWS,
    DLGC_WANTCHARS,
    DLGC_WANTMESSAGE,
    DLGC_WANTTAB
};
Define_constants(dialog_codes);

DWORD dib_color_types[] = {
    DIB_PAL_COLORS,
    DIB_RGB_COLORS
};
Define_constants(dib_color_types);

DWORD dlg_dir_list_options[] = {
    DDL_ARCHIVE,
    DDL_DIRECTORY,
    DDL_DRIVES,
    DDL_EXCLUSIVE,
    DDL_HIDDEN,
    DDL_READONLY,
    DDL_READWRITE,
    DDL_SYSTEM,
    DDL_POSTMSGS
};
Define_constants(dlg_dir_list_options);

DWORD draw_caption_options[] = {
    DC_ACTIVE,
    DC_ICON,
    DC_INBUTTON,
    DC_SMALLCAP,
    DC_TEXT
};
Define_constants(draw_caption_options);

DWORD draw_frame_control_options[] = {
    DFCS_ADJUSTRECT,
    DFCS_CHECKED,
    DFCS_FLAT,
    DFCS_INACTIVE,
    DFCS_MONO,
    DFCS_PUSHED
};
Define_constants(draw_frame_control_options);

DWORD draw_frame_control_types[] = {
    MAKELONG(DFC_BUTTON, DFCS_BUTTON3STATE),
    MAKELONG(DFC_BUTTON, DFCS_BUTTONCHECK),
    MAKELONG(DFC_BUTTON, DFCS_BUTTONPUSH),
    MAKELONG(DFC_BUTTON, DFCS_BUTTONRADIO),
    MAKELONG(DFC_BUTTON, DFCS_BUTTONRADIOIMAGE),
    MAKELONG(DFC_BUTTON, DFCS_BUTTONRADIOMASK),
    MAKELONG(DFC_CAPTION, DFCS_CAPTIONCLOSE),
    MAKELONG(DFC_CAPTION, DFCS_CAPTIONHELP),
    MAKELONG(DFC_CAPTION, DFCS_CAPTIONMAX),
    MAKELONG(DFC_CAPTION, DFCS_CAPTIONMIN),
    MAKELONG(DFC_CAPTION, DFCS_CAPTIONRESTORE),
    MAKELONG(DFC_MENU, DFCS_MENUARROW),
    MAKELONG(DFC_MENU, DFCS_MENUARROWRIGHT),
    MAKELONG(DFC_MENU, DFCS_MENUBULLET),
    MAKELONG(DFC_MENU, DFCS_MENUCHECK),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLCOMBOBOX),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLDOWN),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLLEFT),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLRIGHT),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLSIZEGRIP),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLSIZEGRIPRIGHT),
    MAKELONG(DFC_SCROLL, DFCS_SCROLLUP)
};
Define_constants(draw_frame_control_types);

DWORD draw_icon_options[] = {
    DI_COMPAT,
    DI_DEFAULTSIZE,
    DI_IMAGE,
    DI_MASK,
    DI_NORMAL
};
Define_constants(draw_icon_options);

DWORD drive_types[] = {
    DRIVE_UNKNOWN,
    DRIVE_NO_ROOT_DIR,
    DRIVE_REMOVABLE,
    DRIVE_FIXED,
    DRIVE_REMOTE,
    DRIVE_CDROM,
    DRIVE_RAMDISK
};
Define_constants(drive_types);

DWORD enable_scroll_bar_options[] = {
    ESB_DISABLE_BOTH,
    ESB_DISABLE_DOWN,
    ESB_DISABLE_LEFT,
    ESB_DISABLE_LTUP,
    ESB_DISABLE_RIGHT,
    ESB_DISABLE_RTDN,
    ESB_DISABLE_UP,
    ESB_ENABLE_BOTH
};
Define_constants(enable_scroll_bar_options);

DWORD error_mode_options[] = {
    SEM_FAILCRITICALERRORS,
    SEM_NOALIGNMENTFAULTEXCEPT,
    SEM_NOGPFAULTERRORBOX,
    SEM_NOOPENFILEERRORBOX
};
Define_constants(error_mode_options);

DWORD ext_text_out_options[] = {
    ETO_CLIPPED,
    ETO_GLYPH_INDEX,
    ETO_OPAQUE,
    ETO_RTLREADING
};
Define_constants(ext_text_out_options);

DWORD file_attributes[] = {
    FILE_ATTRIBUTE_ARCHIVE,
    FILE_ATTRIBUTE_COMPRESSED,
    FILE_ATTRIBUTE_DIRECTORY,
    FILE_ATTRIBUTE_ENCRYPTED,
    FILE_ATTRIBUTE_HIDDEN,
    FILE_ATTRIBUTE_NORMAL,
    FILE_ATTRIBUTE_OFFLINE,
    FILE_ATTRIBUTE_READONLY,
    FILE_ATTRIBUTE_REPARSE_POINT,
    FILE_ATTRIBUTE_SPARSE_FILE,
    FILE_ATTRIBUTE_SYSTEM,
    FILE_ATTRIBUTE_TEMPORARY
};
Define_constants(file_attributes);

DWORD file_change_types[] = {
    FILE_NOTIFY_CHANGE_ATTRIBUTES,
    FILE_NOTIFY_CHANGE_CREATION,
    FILE_NOTIFY_CHANGE_DIR_NAME,
    FILE_NOTIFY_CHANGE_FILE_NAME,
    FILE_NOTIFY_CHANGE_LAST_ACCESS,
    FILE_NOTIFY_CHANGE_LAST_WRITE,
    FILE_NOTIFY_CHANGE_SECURITY,
    FILE_NOTIFY_CHANGE_SIZE
};
Define_constants(file_change_types);

DWORD file_create_options[] = {
    CREATE_ALWAYS,
    CREATE_NEW,
    OPEN_EXISTING,
    OPEN_ALWAYS,
    TRUNCATE_EXISTING
};
Define_constants(file_create_options);

DWORD file_flags[] = {
    FILE_FLAG_BACKUP_SEMANTICS,
    FILE_FLAG_DELETE_ON_CLOSE,
    FILE_FLAG_NO_BUFFERING,
    FILE_FLAG_OPEN_NO_RECALL,
    FILE_FLAG_OPEN_REPARSE_POINT,
    FILE_FLAG_OVERLAPPED,
    FILE_FLAG_POSIX_SEMANTICS,
    FILE_FLAG_RANDOM_ACCESS,
    FILE_FLAG_SEQUENTIAL_SCAN,
    FILE_FLAG_WRITE_THROUGH
};
Define_constants(file_flags);

DWORD file_share_options[] = {
    FILE_SHARE_DELETE,
    FILE_SHARE_READ,
    FILE_SHARE_WRITE
};
Define_constants(file_share_options);

DWORD file_types[] = {
    FILE_TYPE_CHAR,
    FILE_TYPE_DISK,
    FILE_TYPE_PIPE,
    FILE_TYPE_UNKNOWN
};
Define_constants(file_types);

DWORD flood_fill_types[] = {
    FLOODFILLBORDER,
    FLOODFILLSURFACE
};
Define_constants(flood_fill_types);

DWORD font_clip_precisions[] = {
    CLIP_CHARACTER_PRECIS,
    CLIP_EMBEDDED,
    CLIP_LH_ANGLES,
    CLIP_STROKE_PRECIS,
    CLIP_TT_ALWAYS
};
Define_constants(font_clip_precisions);

DWORD font_families[] = {
    FF_DECORATIVE,
    FF_DONTCARE,
    FF_MODERN,
    FF_ROMAN,
    FF_SCRIPT,
    FF_SWISS
};
Define_constants(font_families);

DWORD font_output_precisions[] = {
    OUT_CHARACTER_PRECIS,
    OUT_DEFAULT_PRECIS,
    OUT_DEVICE_PRECIS,
    OUT_OUTLINE_PRECIS,
    OUT_RASTER_PRECIS,
    OUT_STRING_PRECIS,
    OUT_STROKE_PRECIS,
    OUT_TT_ONLY_PRECIS,
    OUT_TT_PRECIS
};
Define_constants(font_output_precisions);

DWORD font_pitches[] = {
    DEFAULT_PITCH,
    FIXED_PITCH,
    VARIABLE_PITCH
};
Define_constants(font_pitches);

DWORD font_pitch_flags[] = {
    TMPF_FIXED_PITCH,
    TMPF_VECTOR,
    TMPF_TRUETYPE,
    TMPF_DEVICE
};
Define_constants(font_pitch_flags);

DWORD font_qualities[] = {
    ANTIALIASED_QUALITY,
    DEFAULT_QUALITY,
    DRAFT_QUALITY,
    NONANTIALIASED_QUALITY,
    PROOF_QUALITY
};
Define_constants(font_qualities);

DWORD font_types[] = {
    DEVICE_FONTTYPE,
    RASTER_FONTTYPE,
    TRUETYPE_FONTTYPE
};
Define_constants(font_types);

DWORD get_dc_ex_options[] = {
    DCX_WINDOW,
    DCX_CACHE,
    DCX_PARENTCLIP,
    DCX_CLIPSIBLINGS,
    DCX_CLIPCHILDREN,
    DCX_NORESETATTRS,
    DCX_LOCKWINDOWUPDATE,
    DCX_EXCLUDERGN,
    DCX_INTERSECTRGN,
    DCX_VALIDATE
};
Define_constants(get_dc_ex_options);

DWORD get_menu_default_item_options[] = {
    GMDI_GOINTOPOPUPS,
    GMDI_USEDISABLED
};
Define_constants(get_menu_default_item_options);

DWORD get_window_options[] = {
    GW_CHILD,
    GW_HWNDFIRST,
    GW_HWNDLAST,
    GW_HWNDNEXT,
    GW_HWNDPREV,
    GW_OWNER
};
Define_constants(get_window_options);

DWORD graphics_modes[] = {
    GM_COMPATIBLE,
    GM_ADVANCED
};
Define_constants(graphics_modes);

DWORD handle_flags[] = {
    HANDLE_FLAG_INHERIT,
    HANDLE_FLAG_PROTECT_FROM_CLOSE
};
Define_constants(handle_flags);

DWORD hatch_styles[] = {
    HS_BDIAGONAL,
    HS_CROSS,
    HS_DIAGCROSS,
    HS_FDIAGONAL,
    HS_HORIZONTAL,
    HS_VERTICAL
};
Define_constants(hatch_styles);

DWORD hit_test_results[] = {
    HTBORDER,
    HTBOTTOM,
    HTBOTTOMLEFT,
    HTBOTTOMRIGHT,
    HTCAPTION,
    HTCLIENT,
    HTCLOSE,
    HTERROR,
    HTGROWBOX,
    HTHELP,
    HTHSCROLL,
    HTLEFT,
    HTMENU,
    HTMAXBUTTON,
    HTMINBUTTON,
    HTNOWHERE,
    HTRIGHT,
    HTSYSMENU,
    HTTOP,
    HTTOPLEFT,
    HTTOPRIGHT,
    HTTRANSPARENT,
    HTVSCROLL
};
Define_constants(hit_test_results);

DWORD hot_key_modifiers[] = {
    MOD_ALT,
    MOD_CONTROL,
    MOD_SHIFT,
    MOD_WIN
};
Define_constants(hot_key_modifiers);

DWORD icon_options[] = {
    ICON_BIG,
    ICON_SMALL
};
Define_constants(icon_options);

DWORD image_types[] = {
    IMAGE_BITMAP,
    IMAGE_CURSOR,
    IMAGE_ENHMETAFILE,
    IMAGE_ICON
};
Define_constants(image_types);

DWORD input_event_types[] = {
    MSGF_DDEMGR,
    MSGF_DIALOGBOX,
    MSGF_MENU,
    MSGF_NEXTWINDOW,
    MSGF_SCROLLBAR
};
Define_constants(input_event_types);

DWORD load_image_options[] = {
    LR_CREATEDIBSECTION,
    LR_DEFAULTSIZE,
    LR_LOADMAP3DCOLORS,
    LR_LOADTRANSPARENT,
    LR_MONOCHROME,
    LR_SHARED,
    LR_VGACOLOR
};
Define_constants(load_image_options);

DWORD map_modes[] = {
    MM_ANISOTROPIC,
    MM_HIENGLISH,
    MM_HIMETRIC,
    MM_ISOTROPIC,
    MM_LOENGLISH,
    MM_LOMETRIC,
    MM_TEXT,
    MM_TWIPS
};
Define_constants(map_modes);

DWORD menu_item_enable_states[] = {
    MF_DISABLED,
    MF_ENABLED,
    MF_GRAYED
};
Define_constants(menu_item_enable_states);

DWORD menu_item_enable_states_new[] = {
    MFS_DISABLED,
    MFS_ENABLED,
    MFS_GRAYED
};
Define_constants(menu_item_enable_states_new);

DWORD menu_item_options[] = {
    MFT_MENUBARBREAK,
    MFT_MENUBREAK,
    MFT_OWNERDRAW,
    MFT_RADIOCHECK,
    MFT_RIGHTJUSTIFY,
    MFT_RIGHTORDER
};
Define_constants(menu_item_options);

DWORD menu_item_specifier_types[] = {
    MF_BYCOMMAND,
    MF_BYPOSITION
};
Define_constants(menu_item_specifier_types);

DWORD message_box_options[] = {
    MB_ABORTRETRYIGNORE,
    MB_APPLMODAL,
    MB_DEFAULT_DESKTOP_ONLY,
    MB_DEFBUTTON1,
    MB_DEFBUTTON2,
    MB_DEFBUTTON3,
    MB_DEFBUTTON4,
    MB_HELP,
    MB_ICONASTERISK,
    MB_ICONERROR,
    MB_ICONEXCLAMATION,
    MB_ICONHAND,
    MB_ICONINFORMATION,
    MB_ICONQUESTION,
    MB_ICONSTOP,
    MB_ICONWARNING,
    MB_OK,
    MB_OKCANCEL,
    MB_RETRYCANCEL,
    MB_RIGHT,
    MB_RTLREADING,
    MB_SETFOREGROUND,
    MB_SYSTEMMODAL,
    MB_TASKMODAL,
    MB_TOPMOST,
    MB_YESNO,
    MB_YESNOCANCEL
};
Define_constants(message_box_options);

DWORD mouse_activate_options[] = {
    MA_ACTIVATE,
    MA_ACTIVATEANDEAT,
    MA_NOACTIVATE,
    MA_NOACTIVATEANDEAT
};
Define_constants(mouse_activate_options);

DWORD mouse_modifiers[] = {
    MK_CONTROL,
    MK_LBUTTON,
    MK_MBUTTON,
    MK_RBUTTON,
    MK_SHIFT
};
Define_constants(mouse_modifiers);

DWORD move_file_options[] = {
    MOVEFILE_COPY_ALLOWED,
    MOVEFILE_DELAY_UNTIL_REBOOT,
    MOVEFILE_REPLACE_EXISTING,
    MOVEFILE_WRITE_THROUGH
};
Define_constants(move_file_options);

DWORD msg_wait_options[] = {
    MWMO_ALERTABLE,
    MWMO_INPUTAVAILABLE,
    MWMO_WAITALL
};
Define_constants(msg_wait_options);

DWORD object_types[] = {
    OBJ_BITMAP,
    OBJ_BRUSH,
    OBJ_FONT,
    OBJ_PAL,
    OBJ_PEN,
    OBJ_EXTPEN,
    OBJ_REGION,
    OBJ_DC,
    OBJ_MEMDC,
    OBJ_METAFILE,
    OBJ_METADC,
    OBJ_ENHMETAFILE,
    OBJ_ENHMETADC
};
Define_constants(object_types);

DWORD peek_message_options[] = {
    PM_NOREMOVE,
    PM_NOYIELD,
    PM_REMOVE
};
Define_constants(peek_message_options);

DWORD pen_endcap_styles[] = {
    PS_ENDCAP_ROUND,
    PS_ENDCAP_SQUARE,
    PS_ENDCAP_FLAT
};
Define_constants(pen_endcap_styles);

DWORD pen_join_styles[] = {
    PS_JOIN_BEVEL,
    PS_JOIN_MITER,
    PS_JOIN_ROUND
};
Define_constants(pen_join_styles);

DWORD pen_styles[] = {
    PS_ALTERNATE,
    PS_DASH,
    PS_DASHDOT,
    PS_DASHDOTDOT,
    PS_DOT,
    PS_INSIDEFRAME,
    PS_NULL,
    PS_SOLID,
    PS_USERSTYLE
};
Define_constants(pen_styles);

DWORD poly_fill_modes[] = {
    ALTERNATE,
    WINDING
};
Define_constants(poly_fill_modes);

DWORD print_options[] = {
    PRF_CHECKVISIBLE,
    PRF_CHILDREN,
    PRF_CLIENT,
    PRF_ERASEBKGND,
    PRF_NONCLIENT,
    PRF_OWNED
};
Define_constants(print_options);

DWORD priority_classes[] = {
    HIGH_PRIORITY_CLASS,
    IDLE_PRIORITY_CLASS,
    NORMAL_PRIORITY_CLASS,
    REALTIME_PRIORITY_CLASS
};
Define_constants(priority_classes);

DWORD queue_status_options[] = {
    QS_ALLEVENTS,
    QS_ALLINPUT,
    QS_ALLPOSTMESSAGE,
    QS_HOTKEY,
    QS_INPUT,
    QS_KEY,
    QS_MOUSE,
    QS_MOUSEBUTTON,
    QS_MOUSEMOVE,
    QS_PAINT,
    QS_POSTMESSAGE,
    QS_SENDMESSAGE,
    QS_TIMER
};
Define_constants(queue_status_options);

DWORD redraw_window_options[] = {
    RDW_ALLCHILDREN,
    RDW_ERASE,
    RDW_ERASENOW,
    RDW_FRAME,
    RDW_INTERNALPAINT,
    RDW_INVALIDATE,
    RDW_NOCHILDREN,
    RDW_NOERASE,
    RDW_NOFRAME,
    RDW_NOINTERNALPAINT,
    RDW_UPDATENOW,
    RDW_VALIDATE
};
Define_constants(redraw_window_options);

DWORD region_complexities[] = {
    COMPLEXREGION,
    NULLREGION,
    SIMPLEREGION
};
Define_constants(region_complexities);

DWORD region_modes[] = {
    RGN_AND,
    RGN_COPY,
    RGN_DIFF,
    RGN_OR,
    RGN_XOR
};
Define_constants(region_modes);

DWORD scroll_bar_types[] = {
    SB_BOTH,
    SB_CTL,
    SB_HORZ,
    SB_VERT
};
Define_constants(scroll_bar_types);

DWORD scroll_window_options[] = {
    SW_ERASE,
    SW_INVALIDATE,
    SW_SCROLLCHILDREN
};
Define_constants(scroll_window_options);

DWORD seek_methods[] = {
    FILE_BEGIN,
    FILE_CURRENT,
    FILE_END
};
Define_constants(seek_methods);

DWORD send_message_timeout_options[] = {
    SMTO_ABORTIFHUNG,
    SMTO_BLOCK,
    SMTO_NORMAL
};
Define_constants(send_message_timeout_options);

DWORD set_window_placement_options[] = {
    WPF_RESTORETOMAXIMIZED,
    WPF_SETMINPOSITION
};
Define_constants(set_window_placement_options);

DWORD show_window_options[] = {
    SW_FORCEMINIMIZE,
    SW_HIDE,
    SW_MAXIMIZE,
    SW_MINIMIZE,
    SW_RESTORE,
    SW_SHOW,
    SW_SHOWDEFAULT,
    SW_SHOWMAXIMIZED,
    SW_SHOWMINIMIZED,
    SW_SHOWMINNOACTIVE,
    SW_SHOWNA,
    SW_SHOWNOACTIVATE,
    SW_SHOWNORMAL
};
Define_constants(show_window_options);

DWORD show_window_reasons[] = {
    0,
    SW_OTHERUNZOOM,
    SW_OTHERZOOM,
    SW_PARENTCLOSING,
    SW_PARENTOPENING
};
Define_constants(show_window_reasons);

DWORD size_types[] = {
    SIZE_MAXHIDE,
    SIZE_MAXIMIZED,
    SIZE_MAXSHOW,
    SIZE_MINIMIZED,
    SIZE_RESTORED
};
Define_constants(size_types);

DWORD sizing_sides[] = {
    WMSZ_BOTTOM,
    WMSZ_BOTTOMLEFT,
    WMSZ_BOTTOMRIGHT,
    WMSZ_LEFT,
    WMSZ_RIGHT,
    WMSZ_TOP,
    WMSZ_TOPLEFT,
    WMSZ_TOPRIGHT
};
Define_constants(sizing_sides);

DWORD sqos_options[] = {
    SECURITY_ANONYMOUS,
    SECURITY_CONTEXT_TRACKING,
    SECURITY_DELEGATION,
    SECURITY_EFFECTIVE_ONLY,
    SECURITY_IDENTIFICATION,
    SECURITY_IMPERSONATION,
    SECURITY_SQOS_PRESENT
};
Define_constants(sqos_options);

DWORD startupinfo_options[] = {
    STARTF_FORCEONFEEDBACK,
    STARTF_FORCEOFFFEEDBACK
};
Define_constants(startupinfo_options);

DWORD std_handles[] = {
    STD_INPUT_HANDLE,
    STD_OUTPUT_HANDLE,
    STD_ERROR_HANDLE
};
Define_constants(std_handles);

DWORD stock_objects[] = {
    ANSI_FIXED_FONT,
    ANSI_VAR_FONT,
    BLACK_BRUSH,
    BLACK_PEN,
    DEFAULT_GUI_FONT,
    DEFAULT_PALETTE,
    DEVICE_DEFAULT_FONT,
    DKGRAY_BRUSH,
    GRAY_BRUSH,
    LTGRAY_BRUSH,
    NULL_BRUSH,
    OEM_FIXED_FONT,
    SYSTEM_FIXED_FONT,
    SYSTEM_FONT,
    WHITE_BRUSH,
    WHITE_PEN
};
Define_constants(stock_objects);

DWORD stretch_blt_modes[] = {
    BLACKONWHITE,
    COLORONCOLOR,
    HALFTONE,
    WHITEONBLACK
};
Define_constants(stretch_blt_modes);

DWORD sys_colors[] = {
    COLOR_3DDKSHADOW,
    COLOR_3DFACE,
    COLOR_3DHIGHLIGHT,
    COLOR_3DHILIGHT,
    COLOR_3DLIGHT,
    COLOR_3DSHADOW,
    COLOR_ACTIVEBORDER,
    COLOR_ACTIVECAPTION,
    COLOR_APPWORKSPACE,
    COLOR_BACKGROUND,
    COLOR_BTNFACE,
    COLOR_BTNHIGHLIGHT,
    COLOR_BTNHILIGHT,
    COLOR_BTNSHADOW,
    COLOR_BTNTEXT,
    COLOR_CAPTIONTEXT,
    COLOR_DESKTOP,
    COLOR_GRAYTEXT,
    COLOR_HIGHLIGHT,
    COLOR_HIGHLIGHTTEXT,
    COLOR_INACTIVEBORDER,
    COLOR_INACTIVECAPTION,
    COLOR_INACTIVECAPTIONTEXT,
    COLOR_INFOBK,
    COLOR_INFOTEXT,
    COLOR_MENU,
    COLOR_MENUTEXT,
    COLOR_SCROLLBAR,
    COLOR_WINDOW,
    COLOR_WINDOWFRAME,
    COLOR_WINDOWTEXT
};
Define_constants(sys_colors);

DWORD system_palette_uses[] = {
    SYSPAL_NOSTATIC,
    SYSPAL_NOSTATIC256,
    SYSPAL_STATIC
};
Define_constants(system_palette_uses);

DWORD thread_priorities[] = {
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_TIME_CRITICAL
};
Define_constants(thread_priorities);

DWORD tile_options[] = {
    MDITILE_HORIZONTAL,
    MDITILE_SKIPDISABLED,
    MDITILE_VERTICAL
};
Define_constants(tile_options);

DWORD track_popup_menu_options[] = {
    TPM_BOTTOMALIGN,
    TPM_CENTERALIGN,
    TPM_HORIZONTAL,
    TPM_LEFTALIGN,
    TPM_LEFTBUTTON,
    TPM_NONOTIFY,
    TPM_RIGHTALIGN,
    TPM_RIGHTBUTTON,
    TPM_TOPALIGN,
    TPM_VCENTERALIGN,
    TPM_VERTICAL
};
Define_constants(track_popup_menu_options);

/* Raw memory */

value alloc_pointer(PVOID h)
{
    value val;

    val = alloc_small(sizeof(PVOID) / sizeof(value), Abstract_tag);
    Pointer_val(val) = h;
    return val;
}

CAMLprim value eq_pointer(value pointer1, value pointer2)
{
    return Val_bool(Pointer_val(pointer1) == Pointer_val(pointer2));
}

CAMLprim value is_null_pointer(value pointer)
{
    return Val_bool(Pointer_val(pointer) == NULL);
}

CAMLprim value get_null_pointer(value unit)
{
    return alloc_pointer(NULL);
}

CAMLprim value int32_of_pointer(value p)
{
    return copy_int32((int)Pointer_val(p));
}

CAMLprim value pointer_of_int32(value i)
{
    return alloc_pointer((PVOID)Int32_val(i));
}

CAMLprim value get_byte(value pointer)
{
    return Val_int(*(unsigned char*)Pointer_val(pointer));
}

CAMLprim value get_bytes(value pointer, value size)
{
    CAMLparam2(pointer, size);
    CAMLlocal1(val);

    val = alloc_string(Int_val(size));
    memcpy(String_val(val), Pointer_val(pointer), Int_val(size));
    CAMLreturn(val);
}

CAMLprim value set_byte(value pointer, value byte)
{
    *(unsigned char*)Pointer_val(pointer) = Int_val(byte);
    return Val_unit;
}

CAMLprim value set_bytes(value pointer, value buffer)
{
    memcpy(Pointer_val(pointer), String_val(buffer), string_length(buffer));
    return Val_unit;
}

PVOID raw_val(value val)
{
    if (Tag_val(val) == 0)
        return String_val(Field(val, 0));
    else if (Tag_val(val) == 1)
        return String_val(Field(val, 0)) + Int_val(Field(val, 1));
    else
        return Pointer_val(Field(val, 0));
}

value alloc_raw(PVOID addr)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc(1, 2);
    v = alloc_pointer(addr);
    Store_field(val, 0, v);
    CAMLreturn(val);
}

void sizedraw_val(value val, PVOID* addr, int* size)
{
    if (Tag_val(val) == 0) {
        *addr = String_val(Field(val, 0));
        *size = string_length(Field(val, 0));
    } else if (Tag_val(val) == 1) {
        *addr = String_val(Field(val, 0)) + Int_val(Field(val, 1));
        *size = Int_val(Field(val, 2));
    } else {
        *addr = Pointer_val(Field(val, 0));
        *size = Int_val(Field(val, 1));
    }
}

BOOL is_sizedraw_fixed(value val)
{
    return Tag_val(val) == 2;
}

value alloc_sizedraw(PVOID addr, int size)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc(2, 2);
    v = alloc_pointer(addr);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_int(size));
    CAMLreturn(val);
}

/* Handles */

value alloc_handle(HANDLE h)
{
    value val;

    val = alloc_small(sizeof(HANDLE) / sizeof(value), Abstract_tag);
    Handle_val(val) = h;
    return val;
}

CAMLprim value eq_handle(value handle1, value handle2)
{
    return Val_bool(Handle_val(handle1) == Handle_val(handle2));
}

CAMLprim value is_null_handle(value handle)
{
    return Val_bool(Handle_val(handle) == NULL);
}

CAMLprim value get_null_handle(value unit)
{
    return alloc_handle(NULL);
}

CAMLprim value int32_of_handle(value h)
{
    return copy_int32((int)Handle_val(h));
}

CAMLprim value handle_of_int32(value i)
{
    return alloc_handle((HANDLE)Int32_val(i));
}

CAMLprim value eq_hwnd(value hwnd1, value hwnd2)
{
    return Val_bool(Handle_val(hwnd1) == Handle_val(hwnd2));
}

CAMLprim value is_null_hwnd(value hwnd)
{
    return Val_bool(Handle_val(hwnd) == NULL);
}

CAMLprim value get_null_hwnd(value unit)
{
    return alloc_handle(NULL);
}

CAMLprim value int32_of_hwnd(value h)
{
    return copy_int32((int)Handle_val(h));
}

CAMLprim value hwnd_of_int32(value i)
{
    return alloc_handle((HANDLE)Int32_val(i));
}

CAMLprim value get_hwnd_broadcast(value unit)
{
    return alloc_handle(HWND_BROADCAST);
}

CAMLprim value get_hwnd_desktop(value unit)
{
    return alloc_handle(HWND_DESKTOP);
}

CAMLprim value get_hwnd_topmost(value unit)
{
    return alloc_handle(HWND_TOPMOST);
}

/* Access control */

DWORD access_rights[] = {
    ACCESS_SYSTEM_SECURITY,
    DELETE,
    GENERIC_ALL,
    GENERIC_EXECUTE,
    GENERIC_READ,
    GENERIC_WRITE,
    READ_CONTROL,
    SPECIFIC_RIGHTS_ALL,
    STANDARD_RIGHTS_ALL,
    STANDARD_RIGHTS_EXECUTE,
    STANDARD_RIGHTS_READ,
    STANDARD_RIGHTS_REQUIRED,
    STANDARD_RIGHTS_WRITE,
    SYNCHRONIZE,
    WRITE_DAC,
    WRITE_OWNER
};
Define_constants(access_rights);

CAMLprim value access_mask_of_right(value right)
{
    return copy_int64((int64)Constant_val(right, access_rights));
}

void securityattributes_val(value val, SECURITY_ATTRIBUTES* sa)
{
    sa->nLength = sizeof(SECURITY_ATTRIBUTES);
    sa->lpSecurityDescriptor = Pointer_val(Field(val, 0));
    sa->bInheritHandle = Bool_val(Field(val, 1));
}

value alloc_securityattributes(SECURITY_ATTRIBUTES* sa)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(2);
    v = alloc_pointer(sa->lpSecurityDescriptor);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(sa->bInheritHandle));
    CAMLreturn(val);
}

/* Time */

void filetime_val(value val, FILETIME* ft)
{
    LARGE_INTEGER li;

    li.QuadPart = Int64_val(val);
    ft->dwLowDateTime = li.LowPart;
    ft->dwHighDateTime = li.HighPart;
}

value alloc_filetime(FILETIME* ft)
{
    LARGE_INTEGER li;

    li.LowPart = ft->dwLowDateTime;
    li.HighPart = ft->dwHighDateTime;
    return copy_int64(li.QuadPart);
}

void systemtime_val(value val, SYSTEMTIME* st)
{
    st->wYear = Int_val(Field(val, 0));
    st->wMonth = Int_val(Field(val, 1));
    st->wDayOfWeek = Int_val(Field(val, 2));
    st->wDay = Int_val(Field(val, 3));
    st->wHour = Int_val(Field(val, 4));
    st->wMinute = Int_val(Field(val, 5));
    st->wSecond = Int_val(Field(val, 6));
    st->wMilliseconds = Int_val(Field(val, 7));
}

value alloc_systemtime(SYSTEMTIME* st)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(8);
    Store_field(val, 0, Val_int(st->wYear));
    Store_field(val, 1, Val_int(st->wMonth));
    Store_field(val, 2, Val_int(st->wDayOfWeek));
    Store_field(val, 3, Val_int(st->wDay));
    Store_field(val, 4, Val_int(st->wHour));
    Store_field(val, 5, Val_int(st->wMinute));
    Store_field(val, 6, Val_int(st->wSecond));
    Store_field(val, 7, Val_int(st->wMilliseconds));
    CAMLreturn(val);
}

DWORD delay_val(value val)
{
    if (Is_long(val)) {
        if (Int_val(val) == 0)
            return 0;
        else
            return INFINITE;
    } else {
        if (Tag_val(val) == 0)
            return Int_val(Field(val, 0));
        else
            return (DWORD)Int_val(Field(val, 0)) * 1000;
    }
}

value alloc_waitresult(DWORD result, int count)
{
    CAMLparam0();
    CAMLlocal1(val);

    if (result == WAIT_IO_COMPLETION)
        val = Val_int(0);
    else if (result == WAIT_OBJECT_0 + count)
        val = Val_int(1);
    else if (result == WAIT_TIMEOUT)
        val = Val_int(2);
    else if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + count) {
        val = alloc(1, 0);
        Store_field(val, 0, Val_int(result - WAIT_OBJECT_0));
    } else if (result >= WAIT_ABANDONED_0 && result < WAIT_ABANDONED_0 + count) {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int(result - WAIT_ABANDONED_0));
    } else
        failwith("Invalid constant");
    CAMLreturn(val);
}

/* Files */

value alloc_finddata(WIN32_FIND_DATA* fd)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);
    LARGE_INTEGER li;

    val = alloc_tuple(5);
    v = Alloc_flags(fd->dwFileAttributes, file_attributes);
    Store_field(val, 0, v);
    v = alloc_tuple(3);
    vv = alloc_filetime(&fd->ftCreationTime);
    Store_field(v, 0, vv);
    vv = alloc_filetime(&fd->ftLastAccessTime);
    Store_field(v, 1, vv);
    vv = alloc_filetime(&fd->ftLastWriteTime);
    Store_field(v, 2, vv);
    Store_field(val, 1, v);
    li.LowPart = fd->nFileSizeLow;
    li.HighPart = fd->nFileSizeHigh;
    v = copy_int64(li.QuadPart);
    Store_field(val, 2, v);
    v = copy_string(fd->cFileName);
    Store_field(val, 3, v);
    v = copy_string(fd->cAlternateFileName);
    Store_field(val, 4, v);
    CAMLreturn(val);
}

value alloc_byhandlefileinformation(BY_HANDLE_FILE_INFORMATION* bhfi)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);
    LARGE_INTEGER li;

    val = alloc_tuple(6);
    v = Alloc_flags(bhfi->dwFileAttributes, file_attributes);
    Store_field(val, 0, v);
    v = alloc_tuple(3);
    vv = alloc_filetime(&bhfi->ftCreationTime);
    Store_field(v, 0, vv);
    vv = alloc_filetime(&bhfi->ftLastAccessTime);
    Store_field(v, 1, vv);
    vv = alloc_filetime(&bhfi->ftLastWriteTime);
    Store_field(v, 2, vv);
    Store_field(val, 1, v);
    v = copy_int32(bhfi->dwVolumeSerialNumber);
    Store_field(val, 2, v);
    li.LowPart = bhfi->nFileSizeLow;
    li.HighPart = bhfi->nFileSizeHigh;
    v = copy_int64(li.QuadPart);
    Store_field(val, 3, v);
    Store_field(val, 4, Val_int(bhfi->nNumberOfLinks));
    li.LowPart = bhfi->nFileIndexLow;
    li.HighPart = bhfi->nFileIndexHigh;
    v = copy_int64(li.QuadPart);
    Store_field(val, 5, v);
    CAMLreturn(val);
}

/* Overlapped I/O */

static void finalize_overlapped(value val)
{
    free(Overlappedptr_val(val));
}

static struct custom_operations overlapped_ops = {
    "win32.overlapped.1.0",
    finalize_overlapped,
    custom_compare_default,
    custom_hash_default,
    custom_serialize_default,
    custom_deserialize_default
};

CAMLprim value make_overlapped(value unit)
{
    CAMLparam1(unit);
    OVERLAPPED* ovl;
    CAMLlocal1(val);

    ovl = malloc(sizeof(OVERLAPPED));
    memset(ovl, 0, sizeof(OVERLAPPED));
    val = alloc_custom(&overlapped_ops, sizeof(OVERLAPPED*), 1, 1000);
    *(OVERLAPPED**)Data_custom_val(val) = ovl;
    CAMLreturn(val);
}

CAMLprim value get_overlapped_spec(value overlapped)
{
    CAMLparam1(overlapped);
    OVERLAPPED* ovl;
    CAMLlocal2(val, v);
    LARGE_INTEGER li;

    ovl = Overlappedptr_val(overlapped);
    val = alloc_tuple(4);
    v = copy_int32(ovl->Internal);
    Store_field(val, 0, v);
    v = copy_int32(ovl->InternalHigh);
    Store_field(val, 1, v);
    li.LowPart = ovl->Offset;
    li.HighPart = ovl->OffsetHigh;
    v = copy_int64(li.QuadPart);
    Store_field(val, 2, v);
    v = alloc_handle(ovl->hEvent);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

CAMLprim value set_overlapped_spec(value overlapped, value spec)
{
    OVERLAPPED* ovl;
    LARGE_INTEGER li;

    ovl = Overlappedptr_val(overlapped);
    ovl->Internal = Int32_val(Field(spec, 0));
    ovl->InternalHigh = Int32_val(Field(spec, 1));
    li.QuadPart = Int64_val(Field(spec, 2));
    ovl->Offset = li.LowPart;
    ovl->OffsetHigh = li.HighPart;
    ovl->hEvent = Handle_val(Field(spec, 3));
    return Val_unit;
}

/* Points and rectangles */

POINT point_val(value val)
{
    POINT p;

    p.x = X(val);
    p.y = Y(val);
    return p;
}

value alloc_point(POINT point)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(point.x));
    Store_field(val, 1, Val_int(point.y));
    CAMLreturn(val);
}

void rect_val(value val, RECT* rect)
{
    rect->left = Left(val);
    rect->top = Top(val);
    rect->right = Right(val);
    rect->bottom = Bottom(val);
}

value alloc_rect(RECT* rect)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(4);
    Store_field(val, 0, Val_int(rect->left));
    Store_field(val, 1, Val_int(rect->top));
    Store_field(val, 2, Val_int(rect->right));
    Store_field(val, 3, Val_int(rect->bottom));
    CAMLreturn(val);
}

SIZE size_val(value val)
{
    SIZE s;

    s.cx = Cx(val);
    s.cy = Cy(val);
    return s;
}

value alloc_size(SIZE size)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(size.cx));
    Store_field(val, 1, Val_int(size.cy));
    CAMLreturn(val);
}

void xform_val(value val, XFORM* xform)
{
    xform->eM11 = (FLOAT)Double_field(xform, 0);
    xform->eM12 = (FLOAT)Double_field(xform, 1);
    xform->eM21 = (FLOAT)Double_field(xform, 2);
    xform->eM22 = (FLOAT)Double_field(xform, 3);
    xform->eDx = (FLOAT)Double_field(xform, 4);
    xform->eDy = (FLOAT)Double_field(xform, 5);
}

value alloc_xform(XFORM* xform)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc(6 * Double_wosize, Double_array_tag);
    Store_double_field(val, 0, xform->eM11);
    Store_double_field(val, 1, xform->eM12);
    Store_double_field(val, 2, xform->eM21);
    Store_double_field(val, 3, xform->eM22);
    Store_double_field(val, 4, xform->eDx);
    Store_double_field(val, 5, xform->eDy);
    CAMLreturn(val);
}

/* Regions */

int rgndata_size(value val)
{
    return sizeof(RGNDATAHEADER) + string_length(Field(val, 2));
}

void rgndata_val(value val, RGNDATA* data)
{
    int size;

    size = string_length(Field(val, 2));
    data->rdh.dwSize = sizeof(RGNDATAHEADER);
    data->rdh.iType = RDH_RECTANGLES;
    data->rdh.nCount = Int_val(Field(val, 0));
    data->rdh.nRgnSize = size;
    rect_val(Field(val, 1), &data->rdh.rcBound);
    memcpy(data->Buffer, String_val(Field(val, 2)), size);
}

value alloc_rgndata(RGNDATA* data)
{
    CAMLparam0();
    int size;
    CAMLlocal2(val, v);

    size = data->rdh.nRgnSize;
    val = alloc_tuple(3);
    Store_field(val, 0, Val_int(data->rdh.nCount));
    v = alloc_rect(&data->rdh.rcBound);
    Store_field(val, 1, v);
    v = alloc_string(size);
    memcpy(String_val(v), data->Buffer, size);
    Store_field(val, 2, v);
    CAMLreturn(val);
}

CAMLprim value pack_rects(value rect_array)
{
    CAMLparam1(rect_array);
    int len, i;
    RECT* rects;
    CAMLlocal1(val);

    len = Wosize_val(rect_array);
    val = alloc_string(len * sizeof(RECT));
    rects = (RECT*)String_val(val);
    for (i = 0; i < len; ++i)
        rect_val(Field(rect_array, i), &rects[i]);
    CAMLreturn(val);
}

CAMLprim value unpack_rect(value buffer, value index)
{
    RECT* rects;
    RECT rect;

    rects = (RECT*)String_val(buffer);
    rect = rects[Int_val(index)];
    return alloc_rect(&rect);
}

CAMLprim value unpack_rects(value buffer)
{
    CAMLparam1(buffer);
    RECT* rects;
    RECT rect;
    int len, i;
    CAMLlocal2(val, v);

    len = string_length(buffer) / sizeof(RECT);
    val = alloc_tuple(len);
    for (i = 0; i < len; ++i) {
        rects = (RECT*)String_val(buffer);
        rect = rects[i];
        v = alloc_rect(&rect);
        Store_field(val, i, v);
    }
    CAMLreturn(val);
}

/* Names encoded in integers */

LPCTSTR atomname_val(value val)
{
    if (Tag_val(val) == 0)
        return String_val(Field(val, 0));
    else
        return MAKEINTATOM(Int_val(Field(val, 0)));
}

LPTSTR new_atomname(value val)
{
    if (Tag_val(val) == 0)
        return strdup(String_val(Field(val, 0)));
    else
        return MAKEINTATOM(Int_val(Field(val, 0)));
}

void free_atomname(LPTSTR string)
{
    if (HIWORD((DWORD)string) != 0)
        free(string);
}

value alloc_atomname(LPCTSTR string)
{
    CAMLparam0();
    CAMLlocal2(val, s);

    if (HIWORD((DWORD)string) != 0) {
        val = alloc(1, 0);
        s = copy_string((char*)string);
        Store_field(val, 0, s);
    } else {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int((WORD)(DWORD)string));
    }
    CAMLreturn(val);
}

LPCTSTR restype_val(value val)
{
    if (Tag_val(val) == 0)
        return String_val(Field(val, 0));
    else
        return MAKEINTRESOURCE(Int_val(Field(val, 0)));
}

LPTSTR new_restype(value val)
{
    if (Tag_val(val) == 0)
        return strdup(String_val(Field(val, 0)));
    else
        return MAKEINTRESOURCE(Int_val(Field(val, 0)));
}

void free_restype(LPTSTR string)
{
    if (HIWORD((DWORD)string) != 0)
        free(string);
}

value alloc_restype(LPCTSTR string)
{
    CAMLparam0();
    CAMLlocal2(val, s);

    if (HIWORD((DWORD)string) != 0) {
        val = alloc(1, 0);
        s = copy_string((char*)string);
        Store_field(val, 0, s);
    } else {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int((WORD)(DWORD)string));
    }
    CAMLreturn(val);
}

static LPTSTR standard_resource_types[] = {
    RT_ACCELERATOR,
    RT_ANICURSOR,
    RT_ANIICON,
    RT_BITMAP,
    RT_CURSOR,
    RT_DIALOG,
    RT_FONT,
    RT_FONTDIR,
    RT_GROUP_CURSOR,
    RT_GROUP_ICON,
    RT_HTML,
    RT_ICON,
    RT_MENU,
    RT_MESSAGETABLE,
    RT_PLUGPLAY,
    RT_RCDATA,
    RT_STRING,
    RT_VERSION,
    RT_VXD
};

CAMLprim value resource_type_of_standard(value type)
{
    return alloc_restype(standard_resource_types[Int_val(type)]);
}

LPCTSTR resname_val(value val)
{
    if (Tag_val(val) == 0)
        return String_val(Field(val, 0));
    else
        return MAKEINTRESOURCE(Int_val(Field(val, 0)));
}

LPTSTR new_resname(value val)
{
    if (Tag_val(val) == 0)
        return strdup(String_val(Field(val, 0)));
    else
        return MAKEINTRESOURCE(Int_val(Field(val, 0)));
}

void free_resname(LPTSTR string)
{
    if (HIWORD((DWORD)string) != 0)
        free(string);
}

value alloc_resname(LPCTSTR string)
{
    CAMLparam0();
    CAMLlocal2(val, s);

    if (HIWORD((DWORD)string) != 0) {
        val = alloc(1, 0);
        s = copy_string((char*)string);
        Store_field(val, 0, s);
    } else {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int((WORD)(DWORD)string));
    }
    CAMLreturn(val);
}

static LPTSTR standard_resource_names[] = {
    IDC_APPSTARTING,
    IDC_ARROW,
    IDC_CROSS,
    IDC_HELP,
    IDC_IBEAM,
    IDC_ICON,
    IDC_NO,
    IDC_SIZE,
    IDC_SIZEALL,
    IDC_SIZENESW,
    IDC_SIZENS,
    IDC_SIZENWSE,
    IDC_SIZEWE,
    IDC_UPARROW,
    IDC_WAIT,
    IDI_APPLICATION,
    IDI_ASTERISK,
    IDI_ERROR,
    IDI_EXCLAMATION,
    IDI_HAND,
    IDI_INFORMATION,
    IDI_QUESTION,
    IDI_WARNING,
    IDI_WINLOGO
};

CAMLprim value resource_name_of_standard(value name)
{
    return alloc_resname(standard_resource_names[Int_val(name)]);
}

/* Control ids */

DWORD standard_control_ids[] = {
    IDABORT,
    IDCANCEL,
    IDCLOSE,
    IDHELP,
    IDIGNORE,
    IDNO,
    IDOK,
    IDRETRY,
    IDYES
};
Define_constants(standard_control_ids);

CAMLprim value control_id_of_standard(value id)
{
    return Val_int((int)Constant_val(id, standard_control_ids));
}

/* Window styles */

static DWORD base_style_options[] = {
    WS_BORDER,
    WS_CAPTION,
    WS_CHILD,
    WS_CHILDWINDOW,
    WS_CLIPCHILDREN,
    WS_CLIPSIBLINGS,
    WS_DISABLED,
    WS_DLGFRAME,
    WS_GROUP,
    WS_HSCROLL,
    WS_ICONIC,
    WS_MAXIMIZE,
    WS_MAXIMIZEBOX,
    WS_MINIMIZE,
    WS_MINIMIZEBOX,
    WS_OVERLAPPED,
    WS_OVERLAPPEDWINDOW,
    WS_POPUP,
    WS_POPUPWINDOW,
    WS_SIZEBOX,
    WS_SYSMENU,
    WS_TABSTOP,
    WS_THICKFRAME,
    WS_TILED,
    WS_TILEDWINDOW,
    WS_VISIBLE,
    WS_VSCROLL
};

static DWORD ex_style_options[] = {
    WS_EX_ACCEPTFILES,
    WS_EX_APPWINDOW,
    WS_EX_CLIENTEDGE,
    WS_EX_CONTEXTHELP,
    WS_EX_CONTROLPARENT,
    WS_EX_DLGMODALFRAME,
    WS_EX_LEFT,
    WS_EX_LEFTSCROLLBAR,
    WS_EX_LTRREADING,
    WS_EX_MDICHILD,
    WS_EX_NOPARENTNOTIFY,
    WS_EX_OVERLAPPEDWINDOW,
    WS_EX_PALETTEWINDOW,
    WS_EX_RIGHT,
    WS_EX_RIGHTSCROLLBAR,
    WS_EX_RTLREADING,
    WS_EX_STATICEDGE,
    WS_EX_TOOLWINDOW,
    WS_EX_TOPMOST,
    WS_EX_TRANSPARENT,
    WS_EX_WINDOWEDGE
};

CAMLprim value style_of_option(value option)
{
    int i, n;
    LARGE_INTEGER li;

    i = Int_val(option);
    n = sizeof(base_style_options) / sizeof(DWORD);
    if (i < n) {
        li.LowPart = base_style_options[i];
        li.HighPart = 0;
    } else {
        li.LowPart = 0;
        li.HighPart = ex_style_options[i-n];
    }
    return copy_int64(li.QuadPart);
}

/* Window creation */

CREATESTRUCT* new_createstruct(value val)
{
    CREATESTRUCT* cs;
    value v;
    LARGE_INTEGER li;

    cs = (CREATESTRUCT*)malloc(sizeof(CREATESTRUCT));
    cs->lpszClass = new_atomname(Field(val, 0));
    cs->lpszName = new_string(Field(val, 1));
    v = Field(val, 2);
    li.QuadPart = Int64_val(v);
    cs->style = li.LowPart;
    cs->dwExStyle = li.HighPart;
    v = Field(val, 3);
    if (Is_some(v)) {
        cs->x = X(Value_of(v));
        cs->y = Y(Value_of(v));
    } else
        cs->x = cs->y = CW_USEDEFAULT;
    v = Field(val, 4);
    if (Is_some(v)) {
        cs->cx = Cx(Value_of(v));
        cs->cy = Cy(Value_of(v));
    } else
        cs->cx = cs->cy = CW_USEDEFAULT;
    cs->hwndParent = Handle_val(Field(val, 5));
    if (cs->style & WS_CHILD)
        cs->hMenu = (HMENU)Int_val(Field(val, 7));
    else
        cs->hMenu = Handle_val(Field(val, 6));
    cs->hInstance = Handle_val(Field(val, 8));
    cs->lpCreateParams = Pointer_val(Field(val, 9));
    return cs;
}

void free_createstruct(CREATESTRUCT* cs)
{
    free_atomname((LPTSTR)cs->lpszClass);
    free_string((LPTSTR)cs->lpszName);
    free(cs);
}

value alloc_createstruct(CREATESTRUCT* cs)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);
    LARGE_INTEGER li;
    POINT p;
    SIZE s;

    val = alloc_tuple(10);
    v = alloc_atomname(cs->lpszClass);
    Store_field(val, 0, v);
    v = copy_string((char*)cs->lpszName);
    Store_field(val, 1, v);
    li.LowPart = cs->style;
    li.HighPart = cs->dwExStyle;
    v = copy_int64(li.QuadPart);
    Store_field(val, 2, v);
    if (cs->x == CW_USEDEFAULT)
        Store_field(val, 3, Val_none);
    else {
        p.x = cs->x;
        p.y = cs->y;
        v = alloc_some(alloc_point(p));
        Store_field(val, 3, v);
    }
    if (cs->cx == CW_USEDEFAULT)
        Store_field(val, 4, Val_none);
    else {
        s.cx = cs->cx;
        s.cy = cs->cy;
        v = alloc_some(alloc_size(s));
        Store_field(val, 4, v);
    }
    v = alloc_handle(cs->hwndParent);
    Store_field(val, 5, v);
    if (cs->style & WS_CHILD) {
        v = alloc_handle(NULL);
        Store_field(val, 6, v);
        Store_field(val, 7, Val_int((int)cs->hMenu));
    } else {
        v = alloc_handle(cs->hMenu);
        Store_field(val, 6, v);
        Store_field(val, 7, Val_int(0));
    }
    v = alloc_handle(cs->hInstance);
    Store_field(val, 8, v);
    v = alloc_pointer(cs->lpCreateParams);
    Store_field(val, 9, v);
    CAMLreturn(val);
}

/* Window positioning */

static HWND zorderhwnd_val(value val)
{
    if (Is_block(val))
        return Handle_val(Field(val, 0));
    switch (Int_val(val)) {
        case 0: return HWND_TOPMOST;
        case 1: return HWND_NOTOPMOST;
        case 2: return HWND_TOP;
        case 3: return HWND_BOTTOM;
    }
    return NULL;
}

static value alloc_zorderhwnd(HWND hwnd)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    if (hwnd == HWND_TOPMOST)
        CAMLreturn(Val_int(0));
    else if (hwnd == HWND_NOTOPMOST)
        CAMLreturn(Val_int(1));
    else if (hwnd == HWND_TOP)
        CAMLreturn(Val_int(2));
    else if (hwnd == HWND_BOTTOM)
        CAMLreturn(Val_int(3));
    else {
        val = alloc(1, 0);
        v = alloc_handle(hwnd);
        Store_field(val, 0, v);
        CAMLreturn(val);
    }
}

static DWORD set_window_pos_options[] = {
    SWP_ASYNCWINDOWPOS,
    SWP_DEFERERASE,
    SWP_DRAWFRAME,
    SWP_FRAMECHANGED,
    SWP_HIDEWINDOW,
    SWP_NOACTIVATE,
    SWP_NOCOPYBITS,
    SWP_NOMOVE,
    SWP_NOOWNERZORDER,
    SWP_NOREDRAW,
    SWP_NOREPOSITION,
    SWP_NOSENDCHANGING,
    SWP_NOSIZE,
    SWP_NOZORDER,
    SWP_SHOWWINDOW
};
Define_static_constants(set_window_pos_options);

void windowpos_val(value val, WINDOWPOS* wp)
{
    wp->hwnd = Handle_val(Field(val, 0));
    wp->hwndInsertAfter = zorderhwnd_val(Field(val, 1));
    wp->x = X(Field(val, 2));
    wp->y = Y(Field(val, 2));
    wp->cx = Cx(Field(val, 3));
    wp->cy = Cy(Field(val, 3));
    wp->flags = flags_val(Field(val, 4), set_window_pos_options);
}

value alloc_windowpos(WINDOWPOS* wp)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    POINT p;
    SIZE s;

    val = alloc_tuple(5);
    v = alloc_handle(wp->hwnd);
    Store_field(val, 0, v);
    v = alloc_zorderhwnd(wp->hwndInsertAfter);
    Store_field(val, 1, v);
    p.x = wp->x;
    p.y = wp->y;
    v = alloc_point(p);
    Store_field(val, 2, v);
    s.cx = wp->cx;
    s.cy = wp->cy;
    v = alloc_size(s);
    Store_field(val, 3, v);
    v = Alloc_flags(wp->flags, set_window_pos_options);
    Store_field(val, 4, v);
    CAMLreturn(val);
}

void windowplacement_val(value val, WINDOWPLACEMENT* wpl, value options)
{
    wpl->length = sizeof(*wpl);
    wpl->flags = flags_val(options, set_window_placement_options);
    wpl->showCmd = Constant_val(Field(val, 0), show_window_options);
    wpl->ptMinPosition = point_val(Field(val, 1));
    wpl->ptMaxPosition = point_val(Field(val, 2));
    rect_val(Field(val, 3), &wpl->rcNormalPosition);
}

value alloc_windowplacement(WINDOWPLACEMENT* wpl)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(4);
    Store_field(val, 0, Val_constant(wpl->showCmd, show_window_options));
    v = alloc_point(wpl->ptMinPosition);
    Store_field(val, 1, v);
    v = alloc_point(wpl->ptMaxPosition);
    Store_field(val, 2, v);
    v = alloc_rect(&wpl->rcNormalPosition);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

void minmaxinfo_val(value val, MINMAXINFO* mmi)
{
    mmi->ptReserved.x = mmi->ptReserved.y = 0;
    mmi->ptMaxSize.x = Cx(Field(val, 0));
    mmi->ptMaxSize.y = Cy(Field(val, 0));
    mmi->ptMaxPosition = point_val(Field(val, 1));
    mmi->ptMinTrackSize.x = Cx(Field(val, 2));
    mmi->ptMinTrackSize.y = Cy(Field(val, 2));
    mmi->ptMaxTrackSize.x = Cx(Field(val, 3));
    mmi->ptMaxTrackSize.y = Cy(Field(val, 3));
}

value alloc_minmaxinfo(MINMAXINFO* mmi)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    SIZE s;

    val = alloc_tuple(4);
    s.cx = mmi->ptMaxSize.x;
    s.cy = mmi->ptMaxSize.y;
    v = alloc_size(s);
    Store_field(val, 0, v);
    v = alloc_point(mmi->ptMaxPosition);
    Store_field(val, 1, v);
    s.cx = mmi->ptMinTrackSize.x;
    s.cy = mmi->ptMinTrackSize.y;
    v = alloc_size(s);
    Store_field(val, 2, v);
    s.cx = mmi->ptMaxTrackSize.x;
    s.cy = mmi->ptMaxTrackSize.y;
    v = alloc_size(s);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

/* Colors */

value alloc_rgb(COLORREF cr)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(3);
    Store_field(val, 0, Val_int(GetRValue(cr)));
    Store_field(val, 1, Val_int(GetGValue(cr)));
    Store_field(val, 2, Val_int(GetBValue(cr)));
    CAMLreturn(val);
}

COLORREF color_val(value val)
{
    value v;

    if (Is_block(val)) {
        v = Field(val, 0);
        switch (Tag_val(val)) {
            case 0:
                return Rgb_val(v);
            case 1:
                return PALETTERGB(
                    (BYTE)Int_val(Field(v, 0)),
                    (BYTE)Int_val(Field(v, 1)), 
                    (BYTE)Int_val(Field(v, 2)));
            case 2:
                return PALETTEINDEX(Int_val(v));
        }
    } else {
        switch (Int_val(val)) {
            case 0:
                return CLR_NONE;
            case 1:
                return CLR_DEFAULT;
        }
    }
    return 0;
}

value alloc_color(COLORREF cr)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    DWORD hi_byte;

    if (cr == CLR_NONE)
        val = Val_int(0);
    else if (cr == CLR_DEFAULT)
        val = Val_int(1);
    else {
        hi_byte = cr & 0xFF000000;
        cr &= 0x00FFFFFF;
        if (hi_byte == 0) {
            val = alloc(1, 0);
            v = alloc_rgb(cr);
            Store_field(val, 0, v);
        } else if (hi_byte == 2) {
            val = alloc(1, 1);
            v = alloc_rgb(cr);
            Store_field(val, 0, v);
        } else {
            val = alloc(1, 2);
            Store_field(val, 0, Val_int(cr));
        }
    }
    CAMLreturn(val);
}

/* Raster operations */

DWORD rop2s[] = {
    R2_BLACK,
    R2_COPYPEN,
    R2_MASKNOTPEN,
    R2_MASKPEN,
    R2_MASKPENNOT,
    R2_MERGENOTPEN,
    R2_MERGEPEN,
    R2_MERGEPENNOT,
    R2_NOP,
    R2_NOT,
    R2_NOTCOPYPEN,
    R2_NOTMASKPEN,
    R2_NOTMERGEPEN,
    R2_NOTXORPEN,
    R2_WHITE,
    R2_XORPEN
};
Define_constants(rop2s);

static DWORD rop3_commons[] = {
    BLACKNESS,
    DSTINVERT,
    MERGECOPY,
    MERGEPAINT,
    NOTSRCCOPY,
    NOTSRCERASE,
    PATCOPY,
    PATINVERT,
    PATPAINT,
    SRCAND,
    SRCCOPY,
    SRCERASE,
    SRCINVERT,
    SRCPAINT,
    WHITENESS
};
Define_static_constants(rop3_commons);

CAMLprim value rop3_of_common(value common)
{
    return Val_int((int)Constant_val(common, rop3_commons));
}

/* GDI objects */

void bitmap_val(value val, BITMAP* bm)
{
    bm->bmType = 0;
    bm->bmWidth = Cx(Field(val, 0));
    bm->bmHeight = Cy(Field(val, 0));
    bm->bmWidthBytes = Int_val(Field(val, 1));
    bm->bmPlanes = Int_val(Field(val, 2));
    bm->bmBitsPixel = Int_val(Field(val, 3));
    bm->bmBits = raw_val(Field(val, 4));
}

value alloc_bitmap(BITMAP* bm)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    SIZE s;

    val = alloc_tuple(5);
    s.cx = bm->bmWidth;
    s.cy = bm->bmHeight;
    v = alloc_size(s);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_int(bm->bmWidthBytes));
    Store_field(val, 2, Val_int(bm->bmPlanes));
    Store_field(val, 3, Val_int(bm->bmBitsPixel));
    v = alloc_raw(bm->bmBits);
    Store_field(val, 4, v);
    CAMLreturn(val);
}

void logbrush_val(value val, LOGBRUSH* lb)
{
    int tag;

    if (Is_long(val)) {
        lb->lbStyle = BS_NULL;
        lb->lbColor = 0;
        lb->lbHatch = 0;
    } else {
        tag = Tag_val(val);
        switch (tag) {
            case 0:
                lb->lbStyle = BS_SOLID;
                lb->lbColor = color_val(Field(val, 0));
                lb->lbHatch = 0;
                break;
            case 1:
                lb->lbStyle = BS_HATCHED;
                lb->lbColor = color_val(Field(val, 0));
                lb->lbHatch = Constant_val(Field(val, 1), hatch_styles);
                break;
            case 2:
                lb->lbStyle = BS_PATTERN;
                lb->lbColor = 0;
                lb->lbHatch = (LONG)Handle_val(Field(val, 0));
                break;
            case 3:
                lb->lbStyle = BS_PATTERN8X8;
                lb->lbColor = 0;
                lb->lbHatch = (LONG)Handle_val(Field(val, 0));
                break;
            case 4:
                lb->lbStyle = BS_DIBPATTERN;
                lb->lbColor = Constant_val(Field(val, 0), dib_color_types);
                lb->lbHatch = (LONG)Handle_val(Field(val, 1));
                break;
            case 5:
                lb->lbStyle = BS_DIBPATTERN8X8;
                lb->lbColor = Constant_val(Field(val, 0), dib_color_types);
                lb->lbHatch = (LONG)Handle_val(Field(val, 1));
                break;
            case 6:
                lb->lbStyle = BS_DIBPATTERNPT;
                lb->lbColor = Constant_val(Field(val, 0), dib_color_types);
                lb->lbHatch = (LONG)raw_val(Field(val, 1));
                break;
        }
    }
}

value alloc_logbrush(LOGBRUSH* lb)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    switch (lb->lbStyle) {
        case BS_NULL:
            val = Val_int(0);
            break;
        case BS_SOLID:
            val = alloc(1, 0);
            v = alloc_color(lb->lbColor);
            Store_field(val, 0, v);
            break;
        case BS_HATCHED:
            val = alloc(2, 1);
            v = alloc_color(lb->lbColor);
            Store_field(val, 0, v);
            Store_field(val, 1, Val_constant(lb->lbHatch, hatch_styles));
            break;
        case BS_PATTERN:
            val = alloc(1, 2);
            v = alloc_handle((HANDLE)lb->lbHatch);
            Store_field(val, 0, v);
            break;
        case BS_PATTERN8X8:
            val = alloc(1, 3);
            v = alloc_handle((HANDLE)lb->lbHatch);
            Store_field(val, 0, v);
            break;
        case BS_DIBPATTERN:
            val = alloc(2, 4);
            Store_field(val, 0, Val_constant(lb->lbColor, dib_color_types));
            v = alloc_handle((HANDLE)lb->lbHatch);
            Store_field(val, 1, v);
            break;
        case BS_DIBPATTERN8X8:
            val = alloc(2, 5);
            Store_field(val, 0, Val_constant(lb->lbColor, dib_color_types));
            v = alloc_handle((HANDLE)lb->lbHatch);
            Store_field(val, 1, v);
            break;
        case BS_DIBPATTERNPT:
            val = alloc(2, 6);
            Store_field(val, 0, Val_constant(lb->lbColor, dib_color_types));
            v = alloc_raw((PVOID)lb->lbHatch);
            Store_field(val, 1, v);
            break;
    }
    CAMLreturn(val);
}

void logpen_val(value val, LOGPEN* lp)
{
    lp->lopnStyle = Constant_val(Field(val, 0), pen_styles);
    lp->lopnWidth.x = Cx(Field(val, 1));
    lp->lopnWidth.y = Cy(Field(val, 1));
    lp->lopnColor = color_val(Field(val, 2));
}

value alloc_logpen(LOGPEN* lp)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    SIZE s;

    val = alloc_tuple(3);
    Store_field(val, 0, Val_constant(lp->lopnStyle, pen_styles));
    s.cx = lp->lopnWidth.x;
    s.cy = lp->lopnWidth.y;
    v = alloc_size(s);
    Store_field(val, 1, v);
    v = alloc_color(lp->lopnColor);
    Store_field(val, 2, v);
    CAMLreturn(val);
}

DWORD extpenstyle_val(value val)
{
    if (Tag_val(val) == 0)
        return PS_COSMETIC | Constant_val(Field(val, 0), pen_styles);
    else
        return PS_GEOMETRIC |
            Constant_val(Field(val, 0), pen_styles) |
            Constant_val(Field(val, 1), pen_endcap_styles) |
            Constant_val(Field(val, 2), pen_join_styles);
}

value alloc_extpenstyle(DWORD ps)
{
    CAMLparam0();
    CAMLlocal1(val);

    if (ps & PS_TYPE_MASK == PS_COSMETIC) {
        val = alloc(1, 0);
        Store_field(val, 0, Val_constant(ps & PS_STYLE_MASK, pen_styles));
    } else {
        val = alloc(3, 1);
        Store_field(val, 0, Val_constant(ps & PS_STYLE_MASK, pen_styles));
        Store_field(val, 1, Val_constant(ps & PS_ENDCAP_MASK, pen_endcap_styles));
        Store_field(val, 2, Val_constant(ps & PS_JOIN_MASK, pen_join_styles));
    }
    CAMLreturn(val);
}

/* Fonts */

static BYTE char_sets_noparam[] = {
    ANSI_CHARSET,
    ARABIC_CHARSET,
    BALTIC_CHARSET,
    CHINESEBIG5_CHARSET,
    DEFAULT_CHARSET,
    EASTEUROPE_CHARSET,
    GB2312_CHARSET,
    GREEK_CHARSET,
    HANGUL_CHARSET,
    HEBREW_CHARSET,
    JOHAB_CHARSET,
    MAC_CHARSET,
    OEM_CHARSET,
    RUSSIAN_CHARSET,
    SHIFTJIS_CHARSET,
    SYMBOL_CHARSET,
    THAI_CHARSET,
    TURKISH_CHARSET
};

BYTE charset_val(value val)
{
    if (Is_long(val))
        return char_sets_noparam[Int_val(val)];
    else
        return Int_val(Field(val, 0));
}

value alloc_charset(BYTE cs)
{
    CAMLparam0();
    CAMLlocal1(val);
    int i;

    for (i = 0; i < sizeof(char_sets_noparam) / sizeof(char_sets_noparam[0]); ++i)
        if (char_sets_noparam[i] == cs)
            CAMLreturn(Val_int(i));
    val = alloc(1, 0);
    Store_field(val, 0, Val_int(cs));
    CAMLreturn(val);
}

void logfont_val(value val, LOGFONT* lf)
{
    lf->lfHeight = Int_val(Field(val, 0));
    lf->lfWidth = Int_val(Field(val, 1));
    lf->lfEscapement = Int_val(Field(val, 2));
    lf->lfOrientation = Int_val(Field(val, 3));
    lf->lfWeight = Int_val(Field(val, 4));
    lf->lfItalic = Bool_val(Field(val, 5));
    lf->lfUnderline = Bool_val(Field(val, 6));
    lf->lfStrikeOut = Bool_val(Field(val, 7));
    lf->lfCharSet = charset_val(Field(val, 8));
    lf->lfOutPrecision = (BYTE)Constant_val(Field(val, 9), font_output_precisions);
    lf->lfClipPrecision = (BYTE)flags_val(Field(val, 10), font_clip_precisions);
    lf->lfQuality = (BYTE)Constant_val(Field(val, 11), font_qualities);
    lf->lfPitchAndFamily =
        (BYTE)Constant_val(Field(val, 12), font_pitches) |
        (BYTE)Constant_val(Field(val, 13), font_families);
    strncpy(lf->lfFaceName, String_val(Field(val, 14)), LF_FACESIZE - 1);
    lf->lfFaceName[LF_FACESIZE-1] = '\0';
}

value alloc_logfont(LOGFONT* lf)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(15);
    Store_field(val, 0, Val_int(lf->lfHeight));
    Store_field(val, 1, Val_int(lf->lfWidth));
    Store_field(val, 2, Val_int(lf->lfEscapement));
    Store_field(val, 3, Val_int(lf->lfOrientation));
    Store_field(val, 4, Val_int(lf->lfWeight));
    Store_field(val, 5, Val_bool(lf->lfItalic));
    Store_field(val, 6, Val_bool(lf->lfUnderline));
    Store_field(val, 7, Val_bool(lf->lfStrikeOut));
    v = alloc_charset(lf->lfCharSet);
    Store_field(val, 8, v);
    Store_field(val, 9, Val_constant(lf->lfOutPrecision, font_output_precisions));
    v = Alloc_flags(lf->lfClipPrecision, font_clip_precisions);
    Store_field(val, 10, v);
    Store_field(val, 11, Val_constant(lf->lfQuality, font_qualities));
    Store_field(val, 12, Val_constant(lf->lfPitchAndFamily & 0x03, font_pitches));
    Store_field(val, 13, Val_constant(lf->lfPitchAndFamily & 0xF0, font_families));
    v = copy_string(lf->lfFaceName);
    Store_field(val, 14, v);
    CAMLreturn(val);
}

value alloc_textmetric(TEXTMETRIC* tm)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(21);
    Store_field(val, 0, Val_int(tm->tmHeight));
    Store_field(val, 1, Val_int(tm->tmAscent));
    Store_field(val, 2, Val_int(tm->tmDescent));
    Store_field(val, 3, Val_int(tm->tmInternalLeading));
    Store_field(val, 4, Val_int(tm->tmExternalLeading));
    Store_field(val, 5, Val_int(tm->tmAveCharWidth));
    Store_field(val, 6, Val_int(tm->tmMaxCharWidth));
    Store_field(val, 7, Val_int(tm->tmWeight));
    Store_field(val, 8, Val_int(tm->tmOverhang));
    Store_field(val, 9, Val_int(tm->tmDigitizedAspectX));
    Store_field(val, 10, Val_int(tm->tmDigitizedAspectY));
    Store_field(val, 11, Val_int(tm->tmFirstChar));
    Store_field(val, 12, Val_int(tm->tmLastChar));
    Store_field(val, 13, Val_int(tm->tmDefaultChar));
    Store_field(val, 14, Val_int(tm->tmBreakChar));
    Store_field(val, 15, Val_bool(tm->tmItalic));
    Store_field(val, 16, Val_bool(tm->tmUnderlined));
    Store_field(val, 17, Val_bool(tm->tmStruckOut));
    v = Alloc_flags(tm->tmPitchAndFamily & 0x0F, font_pitch_flags);
    Store_field(val, 18, v);
    Store_field(val, 19, Val_constant(tm->tmPitchAndFamily & 0xF0, font_families));
    v = alloc_charset(tm->tmCharSet);
    Store_field(val, 20, v);
    CAMLreturn(val);
}

value alloc_newtextmetric(NEWTEXTMETRIC* ntm)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    int i;

    val = alloc_tuple(5);
    v = alloc_textmetric((TEXTMETRIC*)ntm);
    Store_field(val, 0, v);
    v = Val_nil;
    for (i = 5; i >= 0; --i)
        if (ntm->ntmFlags & (1 << i))
            v = alloc_cons(Val_int(i), v);
    Store_field(val, 1, v);
    Store_field(val, 2, Val_int(ntm->ntmSizeEM));
    Store_field(val, 3, Val_int(ntm->ntmCellHeight));
    Store_field(val, 4, Val_int(ntm->ntmAvgWidth));
    CAMLreturn(val);
}

value alloc_abc(ABC* abc)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(3);
    Store_field(val, 0, Val_int(abc->abcA));
    Store_field(val, 1, Val_int(abc->abcB));
    Store_field(val, 2, Val_int(abc->abcC));
    CAMLreturn(val);
}

value alloc_abcfloat(ABCFLOAT* abcf)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc(3 * Double_wosize, Double_array_tag);
    Store_double_field(val, 0, abcf->abcfA);
    Store_double_field(val, 1, abcf->abcfB);
    Store_double_field(val, 2, abcf->abcfC);
    CAMLreturn(val);
}

value alloc_kerningpair(KERNINGPAIR* kp)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(3);
    Store_field(val, 0, Val_int(kp->wFirst));
    Store_field(val, 1, Val_int(kp->wSecond));
    Store_field(val, 2, Val_int(kp->iKernAmount));
    CAMLreturn(val);
}

/* Text */

void text_val(value val, char** buf_ptr, int* len_ptr)
{
    if (Tag_val(val) == 0) {
        *buf_ptr = String_val(Field(val, 0));
        *len_ptr = string_length(Field(val, 0));
    } else {
        *buf_ptr = String_val(Field(val, 0)) + Int_val(Field(val, 1));
        *len_ptr = Int_val(Field(val, 2));
    }
}

CAMLprim value pack_glyphs(value glyph_array)
{
    CAMLparam1(glyph_array);
    int len, i;
    UINT* glyphs;
    CAMLlocal1(val);

    len = Wosize_val(glyph_array);
    val = alloc_string(len * sizeof(UINT));
    glyphs = (UINT*)String_val(val);
    for (i = 0; i < len; ++i)
        glyphs[i] = Int32_val(Field(glyph_array, i));
    CAMLreturn(val);
}

CAMLprim value unpack_glyph(value string, value index)
{
    UINT* glyphs;

    glyphs = (UINT*)String_val(string);
    return copy_int32(glyphs[Int_val(index)]);
}

CAMLprim value unpack_glyphs(value string)
{
    CAMLparam1(string);
    UINT* glyphs;
    int len, i;
    CAMLlocal2(val, v);

    len = string_length(string) / sizeof(UINT);
    val = alloc_tuple(len);
    for (i = 0; i < len; ++i) {
        glyphs = (UINT*)String_val(string);
        v = copy_int32(glyphs[i]);
        Store_field(val, i, v);
    }
    CAMLreturn(val);
}

static DWORD text_horizontal_alignments[] = {
    TA_LEFT,
    TA_RIGHT,
    TA_CENTER
};
Define_static_constants(text_horizontal_alignments);

static DWORD text_vertical_alignments[] = {
    TA_TOP,
    TA_BOTTOM,
    TA_BASELINE
};
Define_static_constants(text_vertical_alignments);

#define TEXT_HORIZONTAL_ALIGNMENT_MASK (TA_LEFT | TA_RIGHT | TA_CENTER)
#define TEXT_VERTICAL_ALIGNMENT_MASK (TA_TOP | TA_BOTTOM | TA_BASELINE)
#define TEXT_UPDATECP_MASK (TA_NOUPDATECP | TA_UPDATECP)

DWORD textalignment_val(value val)
{
    DWORD ta;

    ta = 0;
    ta |= flags_val(Field(val, 0), text_horizontal_alignments);
    ta |= flags_val(Field(val, 1), text_vertical_alignments);
    if (Bool_val(Field(val, 3)))
        ta |= TA_UPDATECP;
    else
        ta |= TA_NOUPDATECP;
    return ta;
}

value alloc_textalignment(DWORD ta)
{
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(3);
    Store_field(val, 0, Val_constant(ta & TEXT_HORIZONTAL_ALIGNMENT_MASK, text_horizontal_alignments));
    Store_field(val, 1, Val_constant(ta & TEXT_VERTICAL_ALIGNMENT_MASK, text_vertical_alignments));
    Store_field(val, 2, Val_bool((ta & TEXT_UPDATECP_MASK) == TA_UPDATECP));
    CAMLreturn(val);
}

/* Painting */

#define PAINTSTRUCT_RESERVED_OFFSET (offsetof(PAINTSTRUCT, fRestore))
#define PAINTSTRUCT_RESERVED_START(ps) ((unsigned char*)(ps) + PAINTSTRUCT_RESERVED_OFFSET)
#define PAINTSTRUCT_RESERVED_SIZE (sizeof(PAINTSTRUCT) - PAINTSTRUCT_RESERVED_OFFSET)

void paintstruct_val(value val, PAINTSTRUCT* ps)
{
    ps->hdc = Handle_val(Field(val, 0));
    ps->fErase = Bool_val(Field(val, 1));
    rect_val(Field(val, 2), &ps->rcPaint);
    memcpy(PAINTSTRUCT_RESERVED_START(ps), String_val(Field(val, 3)),
        PAINTSTRUCT_RESERVED_SIZE);
}

value alloc_paintstruct(PAINTSTRUCT* ps)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(4);
    v = alloc_handle(ps->hdc);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(ps->fErase));
    v = alloc_rect(&ps->rcPaint);
    Store_field(val, 2, v);
    v = alloc_string(PAINTSTRUCT_RESERVED_SIZE);
    memcpy(String_val(v), PAINTSTRUCT_RESERVED_START(ps), PAINTSTRUCT_RESERVED_SIZE);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

/* Edge types */

static DWORD inner_edge_types[] = {
    BDR_RAISEDINNER,
    BDR_SUNKENINNER
};
Define_static_constants(inner_edge_types);

static DWORD outer_edge_types[] = {
    BDR_RAISEDOUTER,
    BDR_SUNKENOUTER
};
Define_static_constants(outer_edge_types);

DWORD edgetype_val(value val)
{
    return Constant_val(Field(val, 0), inner_edge_types) |
        Constant_val(Field(val, 1), outer_edge_types);
}

/* Icons */

void iconinfo_val(value val, ICONINFO* ii)
{
    ii->fIcon = Bool_val(Field(val, 0));
    ii->xHotspot = X(Field(val, 1));
    ii->yHotspot = Y(Field(val, 1));
    ii->hbmMask = Handle_val(Field(val, 2));
    ii->hbmColor = Handle_val(Field(val, 3));
}

value alloc_iconinfo(ICONINFO* ii)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    POINT pt;

    val = alloc_tuple(4);
    Store_field(val, 0, Val_bool(ii->fIcon));
    pt.x = ii->xHotspot;
    pt.y = ii->yHotspot;
    v = alloc_point(pt);
    Store_field(val, 1, v);
    v = alloc_handle(ii->hbmMask);
    Store_field(val, 2, v);
    v = alloc_handle(ii->hbmColor);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

/* Menus */

value alloc_getmenuiteminfo(MENUITEMINFO* mii)
{
    CAMLparam0();
    CAMLlocal4(val, v, v1, v2);

    val = alloc_tuple(6);

    v = alloc_tuple(2);
    if (mii->fType & MFT_SEPARATOR)
        v1 = Val_int(0);
    else if (mii->fType & MFT_BITMAP) {
        v1 = alloc(1, 1);
        v2 = alloc_handle((HBITMAP)mii->dwTypeData);
        Store_field(v1, 0, v2);
    } else {
        v1 = alloc(1, 0);
        v2 = copy_string(mii->dwTypeData);
        Store_field(v1, 0, v2);
    }
    Store_field(v, 0, v1);
    v1 = Alloc_flags(mii->fType, menu_item_options);
    Store_field(v, 1, v1);
    Store_field(val, 0, v);

    v = alloc_tuple(4);
    Store_field(v, 0, Val_bool((mii->fState & MFS_CHECKED) != 0));
    Store_field(v, 1, Val_bool((mii->fState & MFS_DEFAULT) != 0));
    Store_field(v, 2, Val_constant(mii->fState & (MFS_ENABLED | MFS_GRAYED | MFS_DISABLED), menu_item_enable_states_new));
    Store_field(v, 3, Val_bool((mii->fState & MFS_HILITE) != 0));
    Store_field(val, 1, v);

    Store_field(val, 2, Val_int(mii->wID));

    v = alloc_handle(mii->hSubMenu);
    Store_field(val, 3, v);

    v = alloc_tuple(2);
    v1 = alloc_handle(mii->hbmpUnchecked);
    Store_field(v, 0, v1);
    v1 = alloc_handle(mii->hbmpChecked);
    Store_field(v, 1, v1);
    Store_field(val, 4, v);

    v = copy_int32(mii->dwItemData);
    Store_field(val, 5, v);

    CAMLreturn(val);
}

void setmenuiteminfo_val(value val, MENUITEMINFO* mii)
{
    value v;

    memset(mii, 0, sizeof(MENUITEMINFO));
    mii->cbSize = sizeof(MENUITEMINFO);

    if (Is_some(Field(val, 0))) {
        v = Value_of(Field(val, 0));
        mii->fMask |= MIIM_TYPE;
        if (Is_long(Field(v, 0)))
            mii->fType |= MFT_SEPARATOR;
        else if (Tag_val(Field(v, 0)) == 1) {
            mii->fType |= MFT_BITMAP;
            mii->dwTypeData = (LPTSTR)Handle_val(Field(Field(v, 0), 0));
        } else {
            mii->fType |= MFT_STRING;
            mii->dwTypeData = String_val(Field(Field(v, 0), 0));
        }
        mii->fMask |= flags_val(Field(v, 1), menu_item_options);
    }

    if (Is_some(Field(val, 1))) {
        v = Value_of(Field(val, 1));
        mii->fMask |= MIIM_STATE;
        if (Bool_val(Field(v, 0)))
            mii->fState |= MFS_CHECKED;
        if (Bool_val(Field(v, 1)))
            mii->fState |= MFS_DEFAULT;
        mii->fState |= Constant_val(Field(v, 2), menu_item_enable_states_new);
        if (Bool_val(Field(v, 3)))
            mii->fState |= MFS_HILITE;
    }

    if (Is_some(Field(val, 2))) {
        v = Value_of(Field(val, 2));
        mii->fMask |= MIIM_ID;
        mii->wID = Int_val(v);
    }

    if (Is_some(Field(val, 3))) {
        v = Value_of(Field(val, 3));
        mii->fMask |= MIIM_SUBMENU;
        mii->hSubMenu = Handle_val(v);
    }

    if (Is_some(Field(val, 4))) {
        v = Value_of(Field(val, 4));
        mii->fMask |= MIIM_CHECKMARKS;
        mii->hbmpUnchecked = Handle_val(Field(v, 0));
        mii->hbmpChecked = Handle_val(Field(v, 1));
    }

    if (Is_some(Field(val, 5))) {
        v = Value_of(Field(val, 5));
        mii->fMask |= MIIM_DATA;
        mii->dwItemData = Int32_val(v);
    }
}

/* Keyboard input */

static BYTE virtual_key_codes_noparam[] = {
    VK_LBUTTON,
    VK_RBUTTON,
    VK_CANCEL,
    VK_MBUTTON,
    VK_BACK,
    VK_TAB,
    VK_CLEAR,
    VK_RETURN,
    VK_SHIFT,
    VK_CONTROL,
    VK_MENU,
    VK_PAUSE,
    VK_CAPITAL,
    VK_ESCAPE,
    VK_SPACE,
    VK_PRIOR,
    VK_NEXT,
    VK_END,
    VK_HOME,
    VK_LEFT,
    VK_UP,
    VK_RIGHT,
    VK_DOWN,
    VK_SELECT,
    VK_EXECUTE,
    VK_SNAPSHOT,
    VK_INSERT,
    VK_DELETE,
    VK_HELP,
    VK_LWIN,
    VK_RWIN,
    VK_APPS,
    VK_MULTIPLY,
    VK_ADD,
    VK_SEPARATOR,
    VK_SUBTRACT,
    VK_DECIMAL,
    VK_DIVIDE,
    VK_NUMLOCK,
    VK_SCROLL,
    VK_LSHIFT,
    VK_RSHIFT,
    VK_LCONTROL,
    VK_RCONTROL,
    VK_LMENU,
    VK_RMENU,
    VK_ATTN,
    VK_CRSEL,
    VK_EXSEL,
    VK_EREOF,
    VK_PLAY,
    VK_ZOOM,
    VK_NONAME,
    VK_PA1,
    VK_OEM_CLEAR
};

BYTE virtualkeycode_val(value val)
{
    if (Is_long(val))
        return virtual_key_codes_noparam[Int_val(val)];
    switch (Tag_val(val)) {
        case 0:
            return '0' + (BYTE)Int_val(val);
        case 1:
            return 'a' + (BYTE)(Int_val(val) - 'A');
        case 2:
            return VK_NUMPAD0 + (BYTE)Int_val(val);
        case 3:
            return VK_F1 + (BYTE)(Int_val(val) - 1);
        default:
            return (BYTE)Int_val(val);
    }
}

value alloc_virtualkeycode(BYTE code)
{
    CAMLparam0();
    int i;
    CAMLlocal1(val);

    for (i = 0; i < sizeof(virtual_key_codes_noparam) / sizeof(virtual_key_codes_noparam[0]); ++i)
        if (virtual_key_codes_noparam[i] == code)
            CAMLreturn(Val_int(i));
    if (code >= '0' && code <= '9') {
        val = alloc(1, 0);
        Store_field(val, 0, Val_int(code - '0'));
        CAMLreturn(val);
    }
    if (code >= 'A' && code <= 'Z') {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int(code));
        CAMLreturn(val);
    }
    if (code >= VK_NUMPAD0 && code <= VK_NUMPAD9) {
        val = alloc(1, 2);
        Store_field(val, 0, Val_int(code - VK_NUMPAD0));
        CAMLreturn(val);
    }
    if (code >= VK_F1 && code <= VK_F24) {
        val = alloc(1, 3);
        Store_field(val, 0, Val_int(code - VK_F1 + 1));
        CAMLreturn(val);
    }
    val = alloc(1, 4);
    Store_field(val, 0, Val_int(code));
    CAMLreturn(val);
}

DWORD keydata_val(value val)
{
    WORD lo, hi;

    lo = Int_val(Field(val, 0));
    hi = Int_val(Field(val, 1));
    if (Bool_val(Field(val, 2)))
        hi |= KF_EXTENDED;
    if (Bool_val(Field(val, 3)))
        hi |= KF_DLGMODE;
    if (Bool_val(Field(val, 4)))
        hi |= KF_MENUMODE;
    if (Bool_val(Field(val, 5)))
        hi |= KF_ALTDOWN;
    if (Bool_val(Field(val, 6)))
        hi |= KF_REPEAT;
    if (Bool_val(Field(val, 7)))
        hi |= KF_UP;
    return MAKELONG(lo, hi);
}

value alloc_keydata(DWORD data)
{
    CAMLparam0();
    WORD lo, hi;
    CAMLlocal1(val);

    lo = LOWORD(data);
    hi = HIWORD(data);
    val = alloc_tuple(8);
    Store_field(val, 0, Val_int(lo));
    Store_field(val, 1, Val_int(LOBYTE(hi)));
    Store_field(val, 2, Val_bool((hi & KF_EXTENDED) != 0));
    Store_field(val, 3, Val_bool((hi & KF_DLGMODE) != 0));
    Store_field(val, 4, Val_bool((hi & KF_MENUMODE) != 0));
    Store_field(val, 5, Val_bool((hi & KF_ALTDOWN) != 0));
    Store_field(val, 6, Val_bool((hi & KF_REPEAT) != 0));
    Store_field(val, 7, Val_bool((hi & KF_UP) != 0));
    CAMLreturn(val);
}

/* Scroll bars */

static DWORD scroll_actions_noparam[] = {
    SB_BOTTOM,
    SB_ENDSCROLL,
    SB_LEFT,
    SB_LINEDOWN,
    SB_LINELEFT,
    SB_LINERIGHT,
    SB_LINEUP,
    SB_PAGEDOWN,
    SB_PAGELEFT,
    SB_PAGERIGHT,
    SB_PAGEUP,
    SB_RIGHT,
    SB_TOP
};
Define_static_constants(scroll_actions_noparam);

void scrollaction_val(value val, int* code, int* param)
{
    if (Is_long(val)) {
        *code = Constant_val(val, scroll_actions_noparam);
        *param = 0;
    } else {
        if (Tag_val(val) == 0)
            *code = SB_THUMBPOSITION;
        else
            *code = SB_THUMBTRACK;
        *param = Int_val(Field(val, 0));
    }
}

value alloc_scrollaction(int code, int param)
{
    CAMLparam0();
    CAMLlocal1(val);

    if (code == SB_THUMBPOSITION) {
        val = alloc(1, 0);
        Store_field(val, 0, Val_int(param));
    } else if (code == SB_THUMBTRACK) {
        val = alloc(1, 1);
        Store_field(val, 0, Val_int(param));
    } else
        val = Val_constant(code, scroll_actions_noparam);
    CAMLreturn(val);
}

value alloc_getscrollinfo(SCROLLINFO* info)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(4);
    v = alloc_tuple(2);
    Store_field(v, 0, Val_int(info->nMin));
    Store_field(v, 1, Val_int(info->nMax));
    Store_field(val, 0, v);
    Store_field(val, 1, Val_int(info->nPage));
    Store_field(val, 2, Val_int(info->nPos));
    Store_field(val, 3, Val_int(info->nTrackPos));
    CAMLreturn(val);
}

void setscrollinfo_val(value val, SCROLLINFO* info)
{
    value v;

    info->cbSize = sizeof(SCROLLINFO);
    info->fMask = 0;
    if (Is_some(Field(val, 0))) {
        info->fMask |= SIF_RANGE;
        v = Value_of(Field(val, 0));
        info->nMin = Int_val(Field(v, 0));
        info->nMax = Int_val(Field(v, 1));
    }
    if (Is_some(Field(val, 1))) {
        info->fMask |= SIF_PAGE;
        info->nPage = Int_val(Value_of(Field(val, 1)));
    }
    if (Is_some(Field(val, 2))) {
        info->fMask |= SIF_POS;
        info->nPos = Int_val(Value_of(Field(val, 2)));
    }
    if (Bool_val(Field(val, 3)))
        info->fMask |= SIF_DISABLENOSCROLL;
}

/* Processes */

STARTUPINFO* new_startupinfo(value val)
{
    STARTUPINFO* si;
    value handles;

    si = (STARTUPINFO*)malloc(sizeof(STARTUPINFO));
    memset(si, 0, sizeof(STARTUPINFO));
    si->cb = sizeof(STARTUPINFO);
    si->lpDesktop = new_stringoption(Field(val, 0));
    si->lpTitle = new_stringoption(Field(val, 1));
    if (Is_some(Field(val, 2))) {
        si->dwFlags |= STARTF_USEPOSITION;
        si->dwX = X(Value_of(Field(val, 2)));
        si->dwY = Y(Value_of(Field(val, 2)));
    }
    if (Is_some(Field(val, 3))) {
        si->dwFlags |= STARTF_USESIZE;
        si->dwXSize = Cx(Value_of(Field(val, 3)));
        si->dwYSize = Cy(Value_of(Field(val, 3)));
    }
    if (Is_some(Field(val, 4))) {
        si->dwFlags |= STARTF_USECOUNTCHARS;
        si->dwXCountChars = Cx(Value_of(Field(val, 4)));
        si->dwYCountChars = Cy(Value_of(Field(val, 4)));
    }
    if (Is_some(Field(val, 5))) {
        si->dwFlags |= STARTF_USEFILLATTRIBUTE;
        si->dwFillAttribute = flags_val(Value_of(Field(val, 5)), color_attributes);
    }
    si->dwFlags |= flags_val(Field(val, 6), startupinfo_options);
    if (Is_some(Field(val, 7))) {
        si->dwFlags |= STARTF_USESHOWWINDOW;
        si->wShowWindow = (WORD)Constant_val(Value_of(Field(val, 7)), show_window_options);
    }
    if (Is_some(Field(val, 8))) {
        si->dwFlags |= STARTF_USESTDHANDLES;
        handles = Value_of(Field(val, 8));
        si->hStdInput = Handle_val(Field(handles, 0));
        si->hStdOutput = Handle_val(Field(handles, 1));
        si->hStdError = Handle_val(Field(handles, 2));
    }
    return si;
}

void free_startupinfo(STARTUPINFO* si)
{
    free_stringoption(si->lpDesktop);
    free_stringoption(si->lpTitle);
    free(si);
}

value alloc_startupinfo(STARTUPINFO* si)
{
    CAMLparam0();
    CAMLlocal3(val, v, handles);
    POINT pt;
    SIZE s;

    val = alloc_tuple(9);
    if (si->lpDesktop) {
        v = alloc_some(copy_string(si->lpDesktop));
        Store_field(val, 0, v);
    } else
        Store_field(val, 0, Val_none);
    if (si->lpTitle) {
        v = alloc_some(copy_string(si->lpTitle));
        Store_field(val, 1, v);
    } else
        Store_field(val, 1, Val_none);
    if (si->dwFlags & STARTF_USEPOSITION) {
        pt.x = si->dwX;
        pt.y = si->dwY;
        v = alloc_some(alloc_point(pt));
        Store_field(val, 2, v);
    } else
        Store_field(val, 2, Val_none);
    if (si->dwFlags & STARTF_USESIZE) {
        s.cx = si->dwXSize;
        s.cy = si->dwYSize;
        v = alloc_some(alloc_size(s));
        Store_field(val, 3, v);
    } else
        Store_field(val, 3, Val_none);
    if (si->dwFlags & STARTF_USECOUNTCHARS) {
        s.cx = si->dwXCountChars;
        s.cy = si->dwYCountChars;
        v = alloc_some(alloc_size(s));
        Store_field(val, 4, v);
    } else
        Store_field(val, 4, Val_none);
    if (si->dwFlags & STARTF_USEFILLATTRIBUTE) {
        v = alloc_some(Alloc_flags(si->dwFillAttribute, color_attributes));
        Store_field(val, 5, v);
    } else
        Store_field(val, 5, Val_none);
    v = Alloc_flags(si->dwFlags, startupinfo_options);
    Store_field(val, 6, v);
    if (si->dwFlags & STARTF_USESHOWWINDOW) {
        v = alloc_some(Val_constant(si->wShowWindow, show_window_options));
        Store_field(val, 7, v);
    } else
        Store_field(val, 7, Val_none);
    if (si->dwFlags & STARTF_USESTDHANDLES) {
        handles = alloc_tuple(3);
        v = alloc_handle(si->hStdInput);
        Store_field(handles, 0, v);
        v = alloc_handle(si->hStdOutput);
        Store_field(handles, 1, v);
        v = alloc_handle(si->hStdError);
        Store_field(handles, 2, v);
        v = alloc_some(handles);
        Store_field(val, 8, v);
    } else
        Store_field(val, 8, Val_none);
    CAMLreturn(val);
}

create_process_params* new_create_process_params(value val)
{
    create_process_params* cpp;
    value env;
    int num_env, env_length, i;
    char* p;

    cpp = malloc(sizeof(create_process_params));
    cpp->application_name = new_stringoption(Field(val, 0));
    cpp->command_line = new_stringoption(Field(val, 1));
    if (Is_some(Field(val, 2))) {
        cpp->process_attributes = malloc(sizeof(SECURITY_ATTRIBUTES));
        securityattributes_val(Value_of(Field(val, 2)), cpp->process_attributes);
    } else
        cpp->process_attributes = NULL;
    if (Is_some(Field(val, 3))) {
        cpp->thread_attributes = malloc(sizeof(SECURITY_ATTRIBUTES));
        securityattributes_val(Value_of(Field(val, 3)), cpp->thread_attributes);
    } else
        cpp->thread_attributes = NULL;
    cpp->inherit_handles = Bool_val(Field(val, 4));
    cpp->creation_flags = flags_val(Field(val, 5), create_process_options);
    if (Is_some(Field(val, 6)))
        cpp->creation_flags |= Constant_val(Value_of(Field(val, 6)), priority_classes);
    if (Is_some(Field(val, 7))) {
        env = Value_of(Field(val, 7));
        num_env = Wosize_val(env);
        env_length = 1;
        for (i = 0; i < num_env; ++i)
            env_length += string_length(Field(env, i)) + 1;
        cpp->environment = malloc(env_length);
        p = cpp->environment;
        for (i = 0; i < num_env; ++i) {
            strcpy(p, String_val(Field(env, i)));
            p += string_length(Field(env, i)) + 1;
        }
        *p = '\0';
    } else
        cpp->environment = NULL;
    cpp->current_directory = new_stringoption(Field(val, 8));
    cpp->startup_info = new_startupinfo(Field(val, 9));
    return cpp;
}

void free_create_process_params(create_process_params* cpp)
{
    free_stringoption(cpp->application_name);
    free_stringoption(cpp->command_line);
    if (cpp->process_attributes)
        free(cpp->process_attributes);
    if (cpp->thread_attributes)
        free(cpp->thread_attributes);
    if (cpp->environment)
        free(cpp->environment);
    free_stringoption(cpp->current_directory);
    free_startupinfo(cpp->startup_info);
    free(cpp);
}

value alloc_processinformation(PROCESS_INFORMATION* pi)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(4);
    v = alloc_handle(pi->hProcess);
    Store_field(val, 0, v);
    v = alloc_handle(pi->hThread);
    Store_field(val, 1, v);
    v = copy_int32(pi->dwProcessId);
    Store_field(val, 2, v);
    v = copy_int32(pi->dwThreadId);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

value alloc_terminationstatus(DWORD ts)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    if (ts == STILL_ACTIVE)
        val = Val_int(0);
    else {
        val = alloc(1, 0);
        v = copy_int32(ts);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

/* Interprocess communication */

void fill_copydatastruct(COPYDATASTRUCT* cds, value val)
{
    value v;
    PVOID data;
    int size;

    cds->dwData = Int32_val(Field(val, 0));
    if (Is_some(Field(val, 1))) {
        v = Value_of(Field(val, 1));
        sizedraw_val(v, &data, &size);
        cds->cbData = size;
        if (is_sizedraw_fixed(v))
            cds->lpData = data;
        else {
            cds->lpData = malloc(size);
            memcpy(cds->lpData, data, size);
        }
    } else {
        cds->cbData = 0;
        cds->lpData = NULL;
    }
}

void empty_copydatastruct(COPYDATASTRUCT* cds, value val)
{
    value v;

    if (Is_some(Field(val, 1))) {
        v = Value_of(Field(val, 1));
        cds->cbData = 0;
        if (!is_sizedraw_fixed(v))
            free(cds->lpData);
        cds->lpData = NULL;
    }
}

value alloc_copydatastruct(COPYDATASTRUCT* cds)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);

    val = alloc_tuple(2);
    v = copy_int32(cds->dwData);
    Store_field(val, 0, v);
    if (cds->lpData) {
        v = alloc_sizedraw(cds->lpData, cds->cbData);
        vv = alloc_some(v);
        Store_field(val, 1, vv);
    } else
        Store_field(val, 1, Val_none);
    CAMLreturn(val);
}

/********************************************************************/
/* Message handling */

/* Messages */

value pack_message(UINT msg, WPARAM wparam, LPARAM lparam)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    val = alloc_tuple(3);
    Store_field(val, 0, Val_int(msg));
    v = copy_int32(wparam);
    Store_field(val, 1, v);
    v = copy_int32(lparam);
    Store_field(val, 2, v);
    CAMLreturn(val);
}

void msg_val(value val, MSG* msg)
{
    value contents;

    msg->hwnd = Handle_val(Field(val, 0));
    contents = Field(val, 1);
    msg->message = Int_val(Field(contents, 0));
    msg->wParam = Int32_val(Field(contents, 1));
    msg->lParam = Int32_val(Field(contents, 2));
    msg->time = Int32_val(Field(val, 2));
    msg->pt = point_val(Field(val, 3));
}

value alloc_msg(MSG* msg)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);

    val = alloc_tuple(4);
    v = alloc_handle(msg->hwnd);
    Store_field(val, 0, v);
    v = alloc_tuple(3);
    Store_field(v, 0, Val_int(msg->message));
    vv = copy_int32(msg->wParam);
    Store_field(v, 1, vv);
    vv = copy_int32(msg->lParam);
    Store_field(v, 2, vv);
    Store_field(val, 1, v);
    v = copy_int32(msg->time);
    Store_field(val, 2, v);
    v = alloc_point(msg->pt);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

/* Specialized message handlers */

CAMLprim value deliver_mouse_message(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    POINT p;
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Alloc_flags(Wparam(msg), mouse_modifiers);
    p.x = GET_X_LPARAM(Lparam(msg));
    p.y = GET_Y_LPARAM(Lparam(msg));
    args[3] = alloc_point(p);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_mouse_nc_message(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    POINT p;
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_constant(Wparam(msg), hit_test_results);
    p.x = GET_X_LPARAM(Lparam(msg));
    p.y = GET_Y_LPARAM(Lparam(msg));
    args[3] = alloc_point(p);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_char_message(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_int((char)Wparam(msg));
    args[3] = alloc_keydata(Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_key_message(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_virtualkeycode((BYTE)Wparam(msg));
    args[3] = alloc_keydata(Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_ctlcolor_message(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HDC)Wparam(msg));
    args[3] = alloc_handle((HWND)Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value return_ctlcolor_message(value result)
{
    return copy_int32((LRESULT)Handle_val(result));
}

/********************************************************************/
/* Window procedures */

static class_data* first_class_data = NULL;

static void extract_name(char* buf, const char* name)
{
    if (HIWORD((DWORD)name) != 0) {
        strncpy(buf, name, 255);
        buf[255] = '\0';
    } else
        sprintf(buf, "#%d", (int)(DWORD)name);
}

class_data* find_class_data(const char* name)
{
    char buf[256];
    class_data* cd;

    extract_name(buf, name);
    for (cd = first_class_data; cd != NULL; cd = cd->next)
        if (!strcmp(cd->name, buf))
            return cd;
    return NULL;
}

class_data* make_class_data(const char* name)
{
    class_data* cd;

    cd = find_class_data(name);
    if (cd == NULL) {
        cd = malloc(sizeof(class_data));
        cd->next = first_class_data;
        extract_name(cd->name, name);
        register_global_root(&cd->function);
        cd->function = Val_int(0);
        first_class_data = cd;
    }
    return cd;
}

void free_class_data(const char* name)
{
    char buf[256];
    class_data* cd;
    class_data** cdp;

    extract_name(buf, name);
    for (cdp = &first_class_data; (cd = *cdp) != NULL; cdp = &cd->next)
        if (!strcmp(cd->name, buf))
            break;
    if (cd != NULL) {
        *cdp = cd->next;
        remove_global_root(&cd->function);
        free(cd);
    }
}

window_data* new_window_data(void)
{
    window_data* wd;

    wd = malloc(sizeof(window_data));
    register_global_root(&wd->function);
    wd->function = Val_int(0);
    register_global_root(&wd->action);
    wd->action = Val_int(0);
    return wd;
}

void free_window_data(window_data* wd)
{
    remove_global_root(&wd->function);
    remove_global_root(&wd->action);
    free(wd);
}

static char window_data_propname[] = "OCaml-Win32 window data";

window_data* get_window_data(HWND hwnd)
{
    return (window_data*)GetProp(hwnd, window_data_propname);
}

window_data* make_window_data(HWND hwnd)
{
    window_data* wd;

    wd = get_window_data(hwnd);
    if (wd == NULL) {
        wd = new_window_data();
        SetProp(hwnd, window_data_propname, (HANDLE)wd);
    }
    return wd;
}

void remove_window_data(HWND hwnd)
{
    window_data* wd;

    wd = (window_data*)RemoveProp(hwnd, window_data_propname);
    if (wd != NULL)
        free_window_data(wd);
}

static LRESULT window_proc_impl(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAMLparam0();
    window_data* wd;
    char name[256];
    class_data* cd;
    CAMLlocal3(wnd, msg, result);
    LRESULT lresult;

    wd = make_window_data(hwnd);
    if (!Is_block(wd->function)) {
        GetClassName(hwnd, name, sizeof(name));
        cd = find_class_data(name);
        if (cd != NULL)
            wd->function = cd->function;
    }
    if (Is_block(wd->function)) {
        wnd = alloc_handle(hwnd);
        msg = pack_message(uMsg, wParam, lParam);
        result = callback2(wd->function, wnd, msg);
        lresult = Int32_val(result);
    } else
        lresult = 0;
    if (uMsg == WM_NCDESTROY)
        remove_window_data(hwnd);
    CAMLreturn(lresult);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lresult;

    leave_blocking_section();
    lresult = window_proc_impl(hwnd, uMsg, wParam, lParam);
    enter_blocking_section();
    return lresult;
}

BOOL dialog_proc_impl(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CAMLparam0();
    window_data* wd;
    CAMLlocal3(wnd, msg, result);
    LRESULT lresult;

    wd = make_window_data(hwnd);
    if (uMsg == WM_INITDIALOG)
        wd->function = ((window_data*)lParam)->function;
    if (Is_block(wd->function)) {
        wnd = alloc_handle(hwnd);
        msg = pack_message(uMsg, wParam, lParam);
        result = callback2(wd->function, wnd, msg);
        lresult = Int32_val(result);
    } else
        lresult = FALSE;
    if (uMsg == WM_NCDESTROY)
        remove_window_data(hwnd);
    CAMLreturn(lresult);
}

BOOL CALLBACK dialog_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lresult;

    leave_blocking_section();
    lresult = dialog_proc_impl(hwnd, uMsg, wParam, lParam);
    enter_blocking_section();
    return lresult;
}

CAMLprim value set_dialog_result(wnd, result)
{
    SetWindowLong(Handle_val(wnd), DWL_MSGRESULT, Int32_val(result));
    return Val_unit;
}

/********************************************************************/
/* Window utilities */

CAMLprim value set_window_action(value wnd, value action)
{
    CAMLparam2(wnd, action);
    HWND hwnd;
    window_data* wd;

    hwnd = Handle_val(wnd);
    wd = (window_data*)GetProp(hwnd, window_data_propname);
    if (wd == NULL)
        failwith("set_window_action requires a Caml window or dialog");
    wd->action = action;
    CAMLreturn(Val_unit);
}

CAMLprim value do_window_action(value wnd)
{
    CAMLparam1(wnd);
    window_data* wd;

    wd = (window_data*)GetProp(Handle_val(wnd), window_data_propname);
    if (wd != NULL && Is_block(wd->action))
        callback(wd->action, Val_unit);
    CAMLreturn(Val_unit);
}

/********************************************************************/
/* Errors */

static value* win32_error_exception = NULL;

void raise_win32_error(DWORD error)
{
    if (win32_error_exception == NULL)
        win32_error_exception = caml_named_value("win32 error");
    raise_with_arg(*win32_error_exception, Val_int((int)error));
}

void raise_last_error(void)
{
    raise_win32_error(GetLastError());
}

static value* region_error_exception = NULL;

void raise_region_error(void)
{
    if (region_error_exception == NULL)
        region_error_exception = caml_named_value("win32 region error");
    raise_constant(*region_error_exception);
}

value val_regioncomplexity(int i)
{
    if (i == ERROR)
        raise_region_error();
    return Val_constant(i, region_complexities);
}

static value* combo_box_error_exception = NULL;

void raise_combo_box_error(value error_type)
{
    if (combo_box_error_exception == NULL)
        combo_box_error_exception = caml_named_value("win32 combo box error");
    raise_with_arg(*combo_box_error_exception, error_type);
}

int check_combo_box_result(int result)
{
    if (result == CB_ERR)
        raise_combo_box_error(Val_int(0));
    if (result == CB_ERRSPACE)
        raise_combo_box_error(Val_int(1));
    return result;
}

static value* list_box_error_exception = NULL;

void raise_list_box_error(value error_type)
{
    if (list_box_error_exception == NULL)
        list_box_error_exception = caml_named_value("win32 list box error");
    raise_with_arg(*list_box_error_exception, error_type);
}

int check_list_box_result(int result)
{
    if (result == LB_ERR)
        raise_list_box_error(Val_int(0));
    if (result == LB_ERRSPACE)
        raise_list_box_error(Val_int(1));
    return result;
}

/********************************************************************/
/* Global variables */

CAMLprim value get_globals(value unit)
{
    CAMLparam1(unit);
    CAMLlocal2(val, v);
    LPSTR cmdline;
    STARTUPINFO si;

    val = alloc_tuple(3);
    v = alloc_handle(GetModuleHandle(NULL));
    Store_field(val, 0, v);
    cmdline = GetCommandLine();
    if (*cmdline == '"') {
        while (*++cmdline != '\0' && *cmdline != '"')
            ;
        if (*cmdline == '"')
            cmdline++;
    } else {
        while (*cmdline > ' ')
            cmdline++;
    }
    v = copy_string(cmdline);
    Store_field(val, 1, v);
    si.dwFlags = 0;
    GetStartupInfo(&si);
    if (!si.dwFlags & STARTF_USESHOWWINDOW)
        si.wShowWindow = SW_SHOWDEFAULT;
    Store_field(val, 2, Val_constant(si.wShowWindow, show_window_options));

    CAMLreturn(val);
}
