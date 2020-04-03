/* OCaml-Win32
 * win32_scrollbar.c
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
/* User Interface Services: Controls: Scroll Bars */

CAMLprim value enable_scroll_bar(value wnd, value type, value option)
{
    CAMLparam3(wnd, type, option);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = EnableScrollBar(
        hwnd,
        Constant_val(type, scroll_bar_types),
        Constant_val(option, enable_scroll_bar_options));
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value get_scroll_info(value wnd, value type)
{
    SCROLLINFO si;

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    if (!GetScrollInfo(Handle_val(wnd), Constant_val(type, scroll_bar_types), &si))
        raise_last_error();
    return alloc_getscrollinfo(&si);
}

CAMLprim value get_scroll_pos(value wnd, value type)
{
    return Val_int(GetScrollPos(Handle_val(wnd), Constant_val(type, scroll_bar_types)));
}

CAMLprim value get_scroll_range(value wnd, value type)
{
    CAMLparam2(wnd, type);
    int min, max;
    CAMLlocal1(val);

    if (!GetScrollRange(Handle_val(wnd), Constant_val(type, scroll_bar_types), &min, &max))
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(min));
    Store_field(val, 1, Val_int(max));
    CAMLreturn(val);
}

CAMLprim value scroll_dc(value hdc, value amount, value scroll, value clip, value update)
{
    RECT scroll_rect, clip_rect, update_rect;
    BOOL result;

    rect_val(scroll, &scroll_rect);
    rect_val(clip, &clip_rect);
    result = ScrollDC(
        Handle_val(hdc),
        X(amount), Y(amount),
        &scroll_rect,
        &clip_rect,
        Handle_val(update),
        &update_rect);
    if (!result)
        raise_last_error();
    return alloc_rect(&update_rect);
}

CAMLprim value scroll_window_native(value wnd, value amount, value scroll, value clip, value update, value options)
{
    CAMLparam5(wnd, amount, scroll, clip, update);
    CAMLxparam1(options);
    HWND hwnd;
    POINT amt;
    RECT scroll_rect, clip_rect;
    HRGN upd;
    RECT update_rect;
    UINT opt;
    int result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    amt = point_val(amount);
    rect_val(scroll, &scroll_rect);
    rect_val(clip, &clip_rect);
    upd = Handle_val(update);
    opt = flags_val(options, scroll_window_options);
    enter_blocking_section();
    result = ScrollWindowEx(
        hwnd,
        amt.x, amt.y,
        &scroll_rect,
        &clip_rect,
        upd,
        &update_rect,
        opt);
    leave_blocking_section();
    if (result == ERROR)
        raise_last_error();
    val = alloc_rect(&update_rect);
    CAMLreturn(val);
}

CAMLprim value scroll_window_bytecode(value* argv, int argn)
{
    return scroll_window_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value set_scroll_info(value wnd, value type, value info, value redraw)
{
    CAMLparam4(wnd, type, info, redraw);
    HWND hwnd;
    SCROLLINFO si;
    int result;

    hwnd = Handle_val(wnd);
    setscrollinfo_val(info, &si);
    enter_blocking_section();
    result = SetScrollInfo(
        hwnd,
        Constant_val(type, scroll_bar_types),
        &si,
        Bool_val(redraw));
    leave_blocking_section();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_scroll_pos(value wnd, value type, value pos, value redraw)
{
    CAMLparam4(wnd, type, pos, redraw);
    HWND hwnd;
    int result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = SetScrollPos(
        hwnd,
        Constant_val(type, scroll_bar_types),
        Int_val(pos),
        Bool_val(redraw));
    leave_blocking_section();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_scroll_range(value wnd, value type, value range, value redraw)
{
    CAMLparam4(wnd, type, range, redraw);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = SetScrollRange(
        hwnd,
        Constant_val(type, scroll_bar_types),
        Int_val(Field(range, 0)),
        Int_val(Field(range, 1)),
        Bool_val(redraw));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value show_scroll_bar(value wnd, value type, value show)
{
    CAMLparam3(wnd, type, show);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = ShowScrollBar(hwnd, Constant_val(type, scroll_bar_types), Bool_val(show));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

/* Styles */

static DWORD scrollbar_style_options[] = {
    SBS_BOTTOMALIGN,
    SBS_HORZ,
    SBS_LEFTALIGN,
    SBS_RIGHTALIGN,
    SBS_SIZEBOX,
    SBS_SIZEBOXBOTTOMRIGHTALIGN,
    SBS_SIZEBOXTOPLEFTALIGN,
    SBS_SIZEGRIP,
    SBS_TOPALIGN,
    SBS_VERT
};
Define_static_constants(scrollbar_style_options);

CAMLprim value scrollbar_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, scrollbar_style_options));
}

/* Messages */

CAMLprim value pack_sbm_enable_arrows(value option)
{
    return pack_message(SBM_ENABLE_ARROWS, Constant_val(option, enable_scroll_bar_options), 0);
}

CAMLprim value unpack_sbm_enable_arrows(value result)
{
    if (Int32_val(result) != TRUE)
        raise_last_error();
    return Val_unit;
}

CAMLprim value unpack_sbm_getpos(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value issue_sbm_getrange(value sender)
{
    CAMLparam1(sender);
    int min, max;
    CAMLlocal2(contents, val);

    contents = pack_message(SBM_GETRANGE, (WPARAM)&min, (LPARAM)&max);
    callback(sender, contents);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(min));
    Store_field(val, 1, Val_int(max));
    CAMLreturn(val);
}

CAMLprim value issue_sbm_getscrollinfo(value sender)
{
    CAMLparam1(sender);
    SCROLLINFO si;
    CAMLlocal3(contents, result, val);

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    contents = pack_message(SBM_GETSCROLLINFO, 0, (LPARAM)&si);
    result = callback(sender, contents);
    if (Int32_val(result) != TRUE)
        raise_last_error();
    val = alloc_getscrollinfo(&si);
    CAMLreturn(val);
}

CAMLprim value pack_sbm_setpos(value pos, value redraw)
{
    return pack_message(SBM_SETPOS, Int_val(pos), Bool_val(redraw));
}

CAMLprim value unpack_sbm_setpos(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_sbm_setrange(value range)
{
    return pack_message(SBM_SETRANGE, Int_val(Field(range, 0)), Int_val(Field(range, 1)));
}

CAMLprim value unpack_sbm_setrange(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_sbm_setrangeredraw(value range)
{
    return pack_message(SBM_SETRANGEREDRAW, Int_val(Field(range, 0)), Int_val(Field(range, 1)));
}

CAMLprim value unpack_sbm_setrangeredraw(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value issue_sbm_setscrollinfo(value sender, value info, value redraw)
{
    CAMLparam3(sender, info, redraw);
    SCROLLINFO si;
    CAMLlocal2(contents, result);

    setscrollinfo_val(info, &si);
    contents = pack_message(SBM_SETSCROLLINFO, Bool_val(redraw), (LPARAM)&si);
    result = callback(sender, contents);
    CAMLreturn(Val_int(Int32_val(result)));
}

CAMLprim value deliver_wm_scroll(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_scrollaction(LOWORD(Lparam(msg)), HIWORD(Lparam(msg)));
    args[3] = alloc_handle((HWND)Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}
