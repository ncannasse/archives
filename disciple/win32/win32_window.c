/* OCaml-Win32
 * win32_window.c
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
/* User Interface Services: Windowing: Windows */

CAMLprim value adjust_window_rect(value rect, value style, value has_menu)
{
    RECT r;
    LARGE_INTEGER li;

    rect_val(rect, &r);
    li.QuadPart = Int64_val(style);
    if (!AdjustWindowRectEx(&r, li.LowPart, Bool_val(has_menu), li.HighPart))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value arrange_iconic_windows(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    UINT result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = ArrangeIconicWindows(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_int((int)result));
}

CAMLprim value bring_window_to_top(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = BringWindowToTop(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value cascade_windows(value wnd, value options, value rect_option, value kids)
{
    CAMLparam4(wnd, options, rect_option, kids);
    HWND hwnd;
    UINT opt;
    RECT r;
    RECT* rp;
    int num_kids;
    HWND* kids_array;
    WORD result;

    hwnd = Handle_val(wnd);
    opt = flags_val(options, tile_options);
    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rp = &r;
    } else
        rp = NULL;

    num_kids = list_length(kids);
    if (num_kids)
        kids_array = malloc(num_kids * sizeof(HWND*));
    else
        kids_array = NULL;
    num_kids = 0;
    for (; !Is_nil(kids); kids = Tail(kids))
        kids_array[num_kids++] = Handle_val(Head(kids));

    enter_blocking_section();
    result = CascadeWindows(hwnd, opt, rp, num_kids, kids_array);
    leave_blocking_section();
    if (kids_array)
        free(kids_array);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_int((int)result));
}

CAMLprim value child_window_from_point(value wnd, value point)
{
    return alloc_handle(ChildWindowFromPoint(Handle_val(wnd), point_val(point)));
}

CAMLprim value child_window_from_point_ex(value wnd, value point, value options)
{
    return alloc_handle(ChildWindowFromPointEx(
        Handle_val(wnd),
        point_val(point),
        flags_val(options, child_window_options)));
}

CAMLprim value close_window(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = CloseWindow(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value create_window(value createstruct)
{
    CAMLparam1(createstruct);
    CREATESTRUCT* cs;
    HWND hwnd;
    CAMLlocal1(result);

    cs = new_createstruct(createstruct);
    enter_blocking_section();
    hwnd = CreateWindowEx(
        cs->dwExStyle,
        cs->lpszClass,
        cs->lpszName,
        cs->style,
        cs->x,
        cs->y,
        cs->cx,
        cs->cy,
        cs->hwndParent,
        cs->hMenu,
        cs->hInstance,
        cs->lpCreateParams);
    leave_blocking_section();
    free_createstruct(cs);
    if (!hwnd)
        raise_last_error();
    result = alloc_handle(hwnd);
    CAMLreturn(result);
}

CAMLprim value destroy_window(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = DestroyWindow(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

static BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lParam)
{
    CAMLparam0();
    CAMLlocal3(func, h, result);

    func = *(value*)lParam;
    h = alloc_handle(hwnd);
    result = callback(func, h);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_child_windows(value parent, value enum_func)
{
    CAMLparam2(parent, enum_func);
    value* p;
    BOOL result;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    result = EnumChildWindows(Handle_val(parent), enum_windows_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value enum_thread_windows(value thread_id, value enum_func)
{
    CAMLparam2(thread_id, enum_func);
    value* p;
    BOOL result;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    result = EnumThreadWindows((DWORD)Int32_val(thread_id), enum_windows_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value enum_windows(value enum_func)
{
    CAMLparam1(enum_func);
    value* p;
    BOOL result;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    result = EnumWindows(enum_windows_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value find_window(value class_name, value window_name)
{
    CAMLparam2(class_name, window_name);
    LPTSTR cn;
    char* wn;
    HWND result;
    CAMLlocal1(val);

    cn = new_atomname(class_name);
    wn = new_stringoption(window_name);
    enter_blocking_section();
    result = FindWindow(cn, wn);
    leave_blocking_section();
    free_atomname(cn);
    free_stringoption(wn);
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value find_window_ex(value parent, value child_after, value class_name, value window_name)
{
    CAMLparam4(parent, child_after, class_name, window_name);
    HWND hparent, hchild;
    LPTSTR cn;
    char* wn;
    HWND result;
    CAMLlocal1(val);

    hparent = Handle_val(parent);
    hchild = Handle_val(child_after);
    cn = new_atomname(class_name);
    wn = new_stringoption(window_name);
    enter_blocking_section();
    result = FindWindowEx(hparent, hchild, cn, wn);
    leave_blocking_section();
    free_atomname(cn);
    free_stringoption(wn);
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value get_client_rect(value wnd)
{
    RECT r;

    if (!GetClientRect(Handle_val(wnd), &r))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value get_desktop_window(value unit)
{
    return alloc_handle(GetDesktopWindow());
}

CAMLprim value get_foreground_window(value unit)
{
    return alloc_handle(GetForegroundWindow());
}

CAMLprim value get_last_active_popup(value wnd)
{
    return alloc_handle(GetLastActivePopup(Handle_val(wnd)));
}

CAMLprim value get_parent(value wnd)
{
    return alloc_handle(GetParent(Handle_val(wnd)));
}

CAMLprim value get_window_placement(value wnd)
{
    WINDOWPLACEMENT wpl;

    wpl.length = sizeof(wpl);
    if (!GetWindowPlacement(Handle_val(wnd), &wpl))
        raise_last_error();
    return alloc_windowplacement(&wpl);
}

CAMLprim value get_window_rect(value wnd)
{
    RECT r;

    if (!GetWindowRect(Handle_val(wnd), &r))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value get_window_text(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    int length;
    char* buffer;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    length = GetWindowTextLength(hwnd);
    leave_blocking_section();
    if (length > 0) {
        buffer = malloc(length + 1);
        enter_blocking_section();
        GetWindowText(hwnd, buffer, length + 1);
        leave_blocking_section();
        val = copy_string(buffer);
        free(buffer);
    } else
        val = copy_string("");
    CAMLreturn(val);
}

CAMLprim value get_window_thread_process_id(value wnd)
{
    CAMLparam1(wnd);
    DWORD thread_id, process_id;
    CAMLlocal2(val, v);

    thread_id = GetWindowThreadProcessId(Handle_val(wnd), &process_id);
    val = alloc_tuple(2);
    v = copy_int32(thread_id);
    Store_field(val, 0, v);
    v = copy_int32(process_id);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value is_child(value parent, value child)
{
    return Val_bool(IsChild(Handle_val(parent), Handle_val(child)));
}

CAMLprim value is_iconic(value wnd)
{
    return Val_bool(IsIconic(Handle_val(wnd)));
}

CAMLprim value is_window(value wnd)
{
    return Val_bool(IsWindow(Handle_val(wnd)));
}

CAMLprim value is_window_unicode(value wnd)
{
    return Val_bool(IsWindowUnicode(Handle_val(wnd)));
}

CAMLprim value is_window_visible(value wnd)
{
    return Val_bool(IsWindowVisible(Handle_val(wnd)));
}

CAMLprim value is_zoomed(value wnd)
{
    return Val_bool(IsZoomed(Handle_val(wnd)));
}

CAMLprim value move_window(value wnd, value position, value size, value repaint)
{
    CAMLparam4(wnd, position, size, repaint);
    HWND hwnd;
    POINT pos;
    SIZE siz;
    BOOL result;

    hwnd = Handle_val(wnd);
    pos = point_val(position);
    siz = size_val(size);
    enter_blocking_section();
    result = MoveWindow(hwnd, pos.x, pos.y, siz.cx, siz.cy, Bool_val(repaint));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value open_icon(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = OpenIcon(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_foreground_window(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = SetForegroundWindow(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_parent(value child, value parent)
{
    CAMLparam2(child, parent);
    HWND c, p, result;
    CAMLlocal1(val);

    c = Handle_val(child);
    p = Handle_val(parent);
    enter_blocking_section();
    result = SetParent(c, p);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value set_window_placement(value wnd, value windowplacement, value options)
{
    CAMLparam3(wnd, windowplacement, options);
    HWND hwnd;
    WINDOWPLACEMENT wpl;
    BOOL result;

    hwnd = Handle_val(wnd);
    windowplacement_val(windowplacement, &wpl, options);
    enter_blocking_section();
    result = SetWindowPlacement(hwnd, &wpl);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_window_pos(value windowpos)
{
    CAMLparam1(windowpos);
    WINDOWPOS wp;
    BOOL result;

    windowpos_val(windowpos, &wp);
    enter_blocking_section();
    result = SetWindowPos(wp.hwnd, wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_window_pos_multi(value params)
{
    CAMLparam1(params);
    int len;
    HDWP hdwp;
    WINDOWPOS wp;
    BOOL result;

    len = list_length(params);
    enter_blocking_section();
    hdwp = BeginDeferWindowPos(len);
    leave_blocking_section();
    if (!hdwp)
        raise_last_error();
    for (; !Is_nil(params); params = Tail(params)) {
        windowpos_val(Head(params), &wp);
        enter_blocking_section();
        hdwp = DeferWindowPos(hdwp, wp.hwnd, wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags);
        leave_blocking_section();
        if (!hdwp)
            raise_last_error();
    }
    enter_blocking_section();
    result = EndDeferWindowPos(hdwp);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_window_text(value wnd, value text)
{
    CAMLparam2(wnd, text);
    HWND hwnd;
    char* txt;
    BOOL result;

    hwnd = Handle_val(wnd);
    txt = new_string(text);
    enter_blocking_section();
    result = SetWindowText(hwnd, txt);
    leave_blocking_section();
    free_string(txt);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value show_owned_popups(value wnd, value show)
{
    CAMLparam2(wnd, show);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = ShowOwnedPopups(hwnd, Bool_val(show));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value show_window(value wnd, value option)
{
    CAMLparam2(wnd, option);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = ShowWindow(hwnd, Constant_val(option, show_window_options));
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value show_window_async(value wnd, value option)
{
    CAMLparam2(wnd, option);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = ShowWindowAsync(hwnd, Constant_val(option, show_window_options));
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value tile_windows(value wnd, value options, value rect_option, value kids)
{
    CAMLparam4(wnd, options, rect_option, kids);
    HWND hwnd;
    UINT opt;
    RECT r;
    RECT* rp;
    int num_kids;
    HWND* kids_array;
    WORD result;

    hwnd = Handle_val(wnd);
    opt = flags_val(options, tile_options);
    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rp = &r;
    } else
        rp = NULL;

    num_kids = list_length(kids);
    if (num_kids)
        kids_array = malloc(num_kids * sizeof(HWND*));
    else
        kids_array = NULL;
    num_kids = 0;
    for (; !Is_nil(kids); kids = Tail(kids))
        kids_array[num_kids++] = Handle_val(Head(kids));

    enter_blocking_section();
    result = TileWindows(hwnd, opt, rp, num_kids, kids_array);
    leave_blocking_section();
    if (kids_array)
        free(kids_array);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_int((int)result));
}

CAMLprim value window_from_point(value point)
{
    return alloc_handle(WindowFromPoint(point_val(point)));
}

/* Messages */

CAMLprim value on_wm_moving_sizing(value id, value handler, value wnd, value msg)
{
    CAMLparam4(id, handler, wnd, msg);
    RECT* r;
    CAMLlocalN(args, 4);
    CAMLlocal2(result, ret);

    ret = Val_none;
    if (Int_val(Field(msg, 0)) == Int_val(id)) {
        r = (LPRECT)Lparam(msg);
        args[0] = wnd;
        args[1] = msg;
        args[2] = Val_constant(Wparam(msg), sizing_sides);
        args[3] = alloc_rect(r);
        result = callbackN(handler, 4, args);
        if (Is_block(result)) {
            if (Tag_val(result) == 0)
                ret = alloc_some(Field(result, 0));
            else {
                rect_val(Field(result, 0), r);
                ret = alloc_some(copy_int32(TRUE));
            }
        }
    }
    CAMLreturn(ret);
}

CAMLprim value deliver_wm_activateapp(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_bool((BOOL)Wparam(msg));
    args[3] = copy_int32(Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_compacting(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_int(Wparam(msg)));
    CAMLreturn(result);
}

CAMLprim value deliver_wm_create(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(cs, result);

    cs = alloc_createstruct((LPCREATESTRUCT)Lparam(msg));
    result = callback3(handler, wnd, msg, cs);
    CAMLreturn(result);
}

CAMLprim value return_wm_create(value result)
{
    return copy_int32(Bool_val(result) ? 0 : -1);
}

CAMLprim value deliver_wm_enable(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_bool((BOOL)Wparam(msg)));
    CAMLreturn(result);
}

CAMLprim value deliver_wm_geticon(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_constant(Wparam(msg), icon_options));
    CAMLreturn(result);
}

CAMLprim value return_wm_geticon(value result)
{
    return copy_int32((LRESULT)Handle_val(result));
}

CAMLprim value on_wm_getminmaxinfo(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    MINMAXINFO* mmi;
    CAMLlocal3(minmaxinfo, result, ret);

    ret = Val_none;
    if (Int_val(Field(msg, 0)) == WM_GETMINMAXINFO) {
        mmi = (LPMINMAXINFO)Lparam(msg);
        minmaxinfo = alloc_minmaxinfo(mmi);
        result = callback3(handler, wnd, msg, minmaxinfo);
        if (Is_block(result)) {
            if (Tag_val(result) == 0)
                ret = alloc_some(Field(result, 0));
            else {
                minmaxinfo_val(Field(result, 0), mmi);
                ret = alloc_some(copy_int32(0));
            }
        }
    }
    CAMLreturn(ret);
}

CAMLprim value on_wm_gettext(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    int text_max;
    char* text;
    CAMLlocal2(result, ret);
    int len;

    ret = Val_none;
    if (Int_val(Field(msg, 0)) == WM_GETTEXT) {
        text_max = Wparam(msg);
        text = (char*)Lparam(msg);
        result = callback2(handler, wnd, msg);
        if (Is_block(result)) {
            if (Tag_val(result) == 0)
                ret = alloc_some(Field(result, 0));
            else {
                len = strlen(String_val(Field(result, 0))) + 1;
                if (len > text_max)
                    len = text_max;
                memcpy(text, String_val(Field(result, 0)), len - 1);
                text[len-1] = '\0';
                ret = alloc_some(copy_int32(len));
            }
        }
    }
    CAMLreturn(ret);
}

CAMLprim value return_wm_gettextlength(value result)
{
    return copy_int32(Int_val(result));
}

CAMLprim value deliver_wm_move(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    POINT p;
    CAMLlocal2(point, result);

    p.x = GET_X_LPARAM(Lparam(msg));
    p.y = GET_Y_LPARAM(Lparam(msg));
    point = alloc_point(p);
    result = callback3(handler, wnd, msg, point);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_ncactivate(value handler, value msg, value wnd)
{
    CAMLparam3(handler, msg, wnd);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_bool((BOOL)Wparam(msg)));
    CAMLreturn(result);
}

CAMLprim value deliver_wm_nccreate(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(cs, result);

    cs = alloc_createstruct((LPCREATESTRUCT)Lparam(msg));
    result = callback3(handler, wnd, msg, cs);
    CAMLreturn(result);
}

CAMLprim value return_wm_nccreate(value result)
{
    return copy_int32(Bool_val(result));
}

CAMLprim value deliver_wm_parentnotify(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal3(pn, v, result);
    POINT p;

    switch (LOWORD(Wparam(msg))) {
        case WM_CREATE:
        case WM_DESTROY:
            pn = alloc(2, LOWORD(Wparam(msg)) == WM_CREATE ? 0 : 1);
            Store_field(pn, 0, Val_int(HIWORD(Wparam(msg))));
            v = alloc_handle((HWND)Lparam(msg));
            Store_field(pn, 1, v);
            break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            pn = alloc(1,
                LOWORD(Wparam(msg)) == WM_LBUTTONDOWN ? 2 :
                LOWORD(Wparam(msg)) == WM_MBUTTONDOWN ? 3 : 4);
            p.x = GET_X_LPARAM(Lparam(msg));
            p.y = GET_Y_LPARAM(Lparam(msg));
            v = alloc_point(p);
            Store_field(pn, 0, v);
            break;
    }
    result = callback3(handler, wnd, msg, pn);
    CAMLreturn(result);
}

CAMLprim value return_wm_querydragicon(value result)
{
    return copy_int32((LRESULT)Handle_val(result));
}

CAMLprim value return_wm_queryopen(value result)
{
    return copy_int32(Bool_val(result));
}

CAMLprim value deliver_wm_seticon(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_constant(Wparam(msg), icon_options);
    args[3] = alloc_handle((HICON)Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value return_wm_seticon(value result)
{
    return copy_int32((LRESULT)Handle_val(result));
}

CAMLprim value deliver_wm_settext(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(s, result);

    s = copy_string((char*)Lparam(msg));
    result = callback3(handler, wnd, msg, s);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_showwindow(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_bool(Wparam(msg));
    args[3] = Val_constant(Lparam(msg), show_window_reasons);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_size(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    SIZE s;
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_constant(Wparam(msg), size_types);
    s.cx = LOWORD(Lparam(msg));
    s.cy = HIWORD(Lparam(msg));
    args[3] = alloc_size(s);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_stylechange(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    STYLESTRUCT* ss;
    LARGE_INTEGER old_li, new_li;
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    ss = (LPSTYLESTRUCT)Lparam(msg);
    if (Wparam(msg) == GWL_STYLE) {
        args[2] = Val_bool(FALSE);
        old_li.LowPart = ss->styleOld;
        old_li.HighPart = GetWindowLong(Handle_val(wnd), GWL_EXSTYLE);
        new_li.LowPart = ss->styleNew;
        new_li.HighPart = old_li.HighPart;
    } else {
        args[2] = Val_bool(TRUE);
        old_li.LowPart = GetWindowLong(Handle_val(wnd), GWL_STYLE);
        old_li.HighPart = ss->styleOld;
        new_li.LowPart = old_li.LowPart;
        new_li.HighPart = ss->styleNew;
    }
    args[3] = copy_int64(old_li.QuadPart);
    args[4] = copy_int64(new_li.QuadPart);
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_windowposchanged(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    WINDOWPOS* wp;
    CAMLlocal2(windowpos, result);

    wp = (LPWINDOWPOS)Lparam(msg);
    windowpos = alloc_windowpos(wp);
    result = callback3(handler, wnd, msg, windowpos);
    CAMLreturn(result);
}

CAMLprim value on_wm_windowposchanging(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    WINDOWPOS* wp;
    CAMLlocal3(windowpos, result, ret);

    ret = Val_none;
    if (Int_val(Field(msg, 0)) == WM_WINDOWPOSCHANGING) {
        wp = (LPWINDOWPOS)Lparam(msg);
        windowpos = alloc_windowpos(wp);
        result = callback3(handler, wnd, msg, windowpos);
        if (Is_block(result)) {
            if (Tag_val(result) == 0)
                ret = alloc_some(Field(result, 0));
            else {
                windowpos_val(Field(result, 0), wp);
                ret = alloc_some(copy_int32(0));
            }
        }
    }
    CAMLreturn(ret);
}
