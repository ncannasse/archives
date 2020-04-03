/* OCaml-Win32
 * win32_windowclass.c
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
/* User Interface Services: Windowing: Window Classes */

static DWORD class_style_options[] = {
    CS_BYTEALIGNCLIENT,
    CS_BYTEALIGNWINDOW,
    CS_CLASSDC,
    CS_DBLCLKS,
    CS_GLOBALCLASS,
    CS_HREDRAW,
    CS_NOCLOSE,
    CS_OWNDC,
    CS_PARENTDC,
    CS_SAVEBITS,
    CS_VREDRAW
};
Define_static_constants(class_style_options);

static HBRUSH classbackground_val(value val)
{
    if (Is_long(val))
        return NULL;
    else if (Tag_val(val) == 0)
        return (HBRUSH)(1 + Constant_val(Field(val, 0), sys_colors));
    else
        return Handle_val(Field(val, 0));
}

static value alloc_classbackground(HBRUSH brush)
{
    CAMLparam0();
    CAMLlocal2(val, v);

    if (brush == NULL)
        val = Val_int(0);
    else if ((DWORD)brush < 32) {
        val = alloc(1, 0);
        Store_field(val, 0, Val_constant((int)((DWORD)brush - 1), sys_colors));
    } else {
        val = alloc(1, 1);
        v = alloc_handle(brush);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value get_class_info(value inst, value name)
{
    CAMLparam2(inst, name);
    WNDCLASSEX wc;
    CAMLlocal3(val, v, vv);
    class_data* cd;

    wc.cbSize = sizeof(wc);
    if (!GetClassInfoEx(Handle_val(inst), atomname_val(name), &wc))
        raise_last_error();
    val = alloc_tuple(11);
    v = Alloc_flags(wc.style, class_style_options);
    Store_field(val, 0, v);
    if (wc.lpfnWndProc == window_proc &&
            (cd = find_class_data(wc.lpszClassName)) != NULL) {
        v = alloc(1, 0);
        Store_field(v, 0, cd->function);
    } else {
        v = alloc(1, 1);
        vv = alloc_pointer(wc.lpfnWndProc);
        Store_field(v, 0, vv);
    }
    Store_field(val, 1, v);
    Store_field(val, 2, Val_int(wc.cbClsExtra));
    Store_field(val, 3, Val_int(wc.cbWndExtra));
    v = alloc_handle(wc.hInstance);
    Store_field(val, 4, v);
    v = alloc_handle(wc.hIcon);
    Store_field(val, 5, v);
    v = alloc_handle(wc.hCursor);
    Store_field(val, 6, v);
    v = alloc_classbackground(wc.hbrBackground);
    Store_field(val, 7, v);
    if (wc.lpszMenuName)
        v = alloc_some(alloc_resname(wc.lpszMenuName));
    else
        v = Val_none;
    Store_field(val, 8, v);
    v = alloc_atomname(wc.lpszClassName);
    Store_field(val, 9, v);
    v = alloc_handle(wc.hIconSm);
    Store_field(val, 9, v);
    CAMLreturn(val);
}

CAMLprim value get_class_long(value wnd, value index)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), Int_val(index));
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return copy_int32(result);
}

CAMLprim value get_class_name(value wnd)
{
    char name[256];

    if (!GetClassName(Handle_val(wnd), name, sizeof(name)))
        raise_last_error();
    return copy_string(name);
}

CAMLprim value get_class_word(value wnd, value index)
{
    WORD result;

    SetLastError(0);
    result = GetClassWord(Handle_val(wnd), Int_val(index));
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_window_long(value wnd, value index)
{
    LONG result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), Int_val(index));
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return copy_int32(result);
}

CAMLprim value register_class(value wndclass, value handlers)
{
    CAMLparam2(wndclass, handlers);
    LPTSTR name, menu_name;
    WNDCLASSEX wc;
    CAMLlocal1(val);
    class_data* cd;
    ATOM result;

    name = new_atomname(Field(wndclass, 9));
    if (Is_some(Field(wndclass, 8)))
        menu_name = new_resname(Value_of(Field(wndclass, 8)));
    else
        menu_name = NULL;
    wc.cbSize = sizeof(wc);
    wc.style = flags_val(Field(wndclass, 0), class_style_options);
    val = Field(wndclass, 1);
    if (Tag_val(val) == 0) {
        wc.lpfnWndProc = window_proc;
        cd = make_class_data(name);
        cd->function = Field(val, 0);
    } else
        wc.lpfnWndProc = Pointer_val(Field(val, 0));
    wc.cbClsExtra = Int_val(Field(wndclass, 2));
    wc.cbWndExtra = Int_val(Field(wndclass, 3));
    wc.hInstance = Handle_val(Field(wndclass, 4));
    wc.hIcon = Handle_val(Field(wndclass, 5));
    wc.hCursor = Handle_val(Field(wndclass, 6));
    wc.hbrBackground = classbackground_val(Field(wndclass, 7));
    wc.lpszMenuName = menu_name;
    wc.lpszClassName = name;
    wc.hIconSm = Handle_val(Field(wndclass, 10));
    result = RegisterClassEx(&wc);
    free_atomname(name);
    if (menu_name != NULL)
        free_resname(menu_name);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_class_long(value wnd, value index, value new_long)
{
    CAMLparam3(wnd, index, new_long);
    HWND hwnd;
    LONG l;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    l = Int32_val(new_long);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, Int_val(index), l);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value set_class_word(value wnd, value index, value new_word)
{
    CAMLparam3(wnd, index, new_word);
    HWND hwnd;
    WORD result;

    hwnd = Handle_val(wnd);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassWord(hwnd, Int_val(index), (WORD)Int_val(new_word));
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_window_long(value wnd, value index, value new_long)
{
    CAMLparam3(wnd, index, new_long);
    HWND hwnd;
    LONG l, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    l = Int32_val(new_long);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, Int_val(index), l);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value unregister_class(value name, value inst)
{
    CAMLparam2(name, inst);
    WNDCLASSEX wc;
    BOOL ok;

    ok = GetClassInfoEx(Handle_val(inst), atomname_val(name), &wc);
    if (!UnregisterClass(atomname_val(name), Handle_val(inst)))
        raise_last_error();
    if (ok)
        free_class_data(wc.lpszClassName);
    CAMLreturn(Val_unit);
}

/* Specialized functions to get/set class/window info */

CAMLprim value get_class_atom(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCW_ATOM);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_class_cls_extra(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_CBCLSEXTRA);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_class_wnd_extra(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_CBWNDEXTRA);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_class_background(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_HBRBACKGROUND);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_classbackground((HBRUSH)result);
}

CAMLprim value get_class_cursor(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_HCURSOR);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HCURSOR)result);
}

CAMLprim value get_class_icon(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_HICON);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HICON)result);
}

CAMLprim value get_class_icon_sm(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_HICONSM);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HICON)result);
}

CAMLprim value get_class_module(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_HMODULE);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HMODULE)result);
}

CAMLprim value get_class_menu_name(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_MENUNAME);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    if (result != 0)
        return alloc_some(alloc_resname((LPCTSTR)result));
    else
        return Val_none;
}

CAMLprim value get_class_style(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_STYLE);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Alloc_flags(result, class_style_options);
}

CAMLprim value get_class_wndproc(value wnd)
{
    CAMLparam1(wnd);
    DWORD result;
    WNDPROC wp;
    char name[256];
    class_data* cd;
    CAMLlocal2(val, v);

    SetLastError(0);
    result = GetClassLong(Handle_val(wnd), GCL_WNDPROC);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    wp = (WNDPROC)result;
    cd = NULL;
    if (wp == window_proc) {
        GetClassName(Handle_val(wnd), name, sizeof(name));
        cd = find_class_data(name);
    }
    if (cd != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, cd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(wp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value get_window_style(value wnd)
{
    LARGE_INTEGER li;

    SetLastError(0);
    li.LowPart = GetWindowLong(Handle_val(wnd), GWL_STYLE);
    li.HighPart = GetWindowLong(Handle_val(wnd), GWL_EXSTYLE);
    if (GetLastError() != 0)
        raise_last_error();
    return copy_int64(li.QuadPart);
}

CAMLprim value get_window_proc(value wnd)
{
    CAMLparam1(wnd);
    DWORD result;
    WNDPROC wp;
    window_data* wd;
    CAMLlocal2(val, v);

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), GWL_WNDPROC);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    wp = (WNDPROC)result;
    if (wp == window_proc && (wd = get_window_data(Handle_val(wnd))) != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, wd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(wp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value get_window_instance(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), GWL_HINSTANCE);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HINSTANCE)result);
}

CAMLprim value get_window_parent(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), GWL_HWNDPARENT);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return alloc_handle((HWND)result);
}

CAMLprim value get_window_id(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), GWL_ID);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_window_user_data(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), GWL_USERDATA);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return copy_int32(result);
}

CAMLprim value get_dialog_proc(value wnd)
{
    CAMLparam1(wnd);
    DWORD result;
    DLGPROC dp;
    window_data* wd;
    CAMLlocal2(val, v);

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), DWL_DLGPROC);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    dp = (DLGPROC)result;
    if (dp == dialog_proc && (wd = get_window_data(Handle_val(wnd))) != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, wd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(dp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value get_dialog_msg_result(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), DWL_MSGRESULT);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return copy_int32(result);
}

CAMLprim value get_dialog_user_data(value wnd)
{
    DWORD result;

    SetLastError(0);
    result = GetWindowLong(Handle_val(wnd), DWL_USER);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    return copy_int32(result);
}

CAMLprim value set_class_cls_extra(value wnd, value extra)
{
    CAMLparam2(wnd, extra);
    HWND hwnd;
    DWORD result;

    hwnd = Handle_val(wnd);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_CBCLSEXTRA, Int_val(extra));
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_class_wnd_extra(value wnd, value extra)
{
    CAMLparam2(wnd, extra);
    HWND hwnd;
    DWORD result;

    hwnd = Handle_val(wnd);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_CBWNDEXTRA, Int_val(extra));
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_class_background(value wnd, value bg)
{
    CAMLparam2(wnd, bg);
    HWND hwnd;
    HBRUSH hbg;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hbg = classbackground_val(bg);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)hbg);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_classbackground((HBRUSH)result);
    CAMLreturn(val);
}

CAMLprim value set_class_cursor(value wnd, value cursor)
{
    CAMLparam2(wnd, cursor);
    HWND hwnd;
    HCURSOR hcursor;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hcursor = Handle_val(cursor);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_HCURSOR, (LONG)hcursor);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_handle((HCURSOR)result);
    CAMLreturn(val);
}

CAMLprim value set_class_icon(value wnd, value icon)
{
    CAMLparam2(wnd, icon);
    HWND hwnd;
    HICON hicon;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hicon = Handle_val(icon);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_HICON, (LONG)hicon);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_handle((HICON)result);
    CAMLreturn(val);
}

CAMLprim value set_class_icon_sm(value wnd, value icon)
{
    CAMLparam2(wnd, icon);
    HWND hwnd;
    HICON hicon;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hicon = Handle_val(icon);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_HICONSM, (LONG)hicon);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_handle((HICON)result);
    CAMLreturn(val);
}

CAMLprim value set_class_module(value wnd, value module)
{
    CAMLparam2(wnd, module);
    HWND hwnd;
    HMODULE hmodule;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hmodule = Handle_val(module);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_HMODULE, (LONG)hmodule);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_handle((HMODULE)result);
    CAMLreturn(val);
}

CAMLprim value set_class_menu_name(value wnd, value name)
{
    CAMLparam2(wnd, name);
    HWND hwnd;
    LPTSTR n;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    if (Is_some(name))
        n = new_resname(Value_of(name));
    else
        n = NULL;
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_MENUNAME, (LONG)n);
    leave_blocking_section();
    if (n)
        free_resname(n);
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    if (result != 0)
        val = alloc_some(alloc_resname((LPCTSTR)result));
    else
        val = Val_none;
    CAMLreturn(val);
}

CAMLprim value set_class_style(value wnd, value style)
{
    CAMLparam2(wnd, style);
    HWND hwnd;
    DWORD sty, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    sty = flags_val(style, class_style_options);
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_STYLE, sty);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = Alloc_flags(result, class_style_options);
    CAMLreturn(val);
}

CAMLprim value set_class_wndproc(value wnd, value proc)
{
    CAMLparam2(wnd, proc);
    HWND hwnd;
    DWORD result;
    WNDPROC wp;
    char name[256];
    class_data* cd;
    CAMLlocal2(val, v);

    hwnd = Handle_val(wnd);
    if (Tag_val(proc) == 0) {
        wp = window_proc;
        GetClassName(Handle_val(wnd), name, sizeof(name));
        cd = make_class_data(name);
        cd->function = Field(proc, 0);
    } else
        wp = Pointer_val(Field(proc, 0));
    SetLastError(0);
    enter_blocking_section();
    result = SetClassLong(hwnd, GCL_WNDPROC, (LONG)wp);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    wp = (WNDPROC)result;
    cd = NULL;
    if (wp == window_proc) {
        GetClassName(Handle_val(wnd), name, sizeof(name));
        cd = find_class_data(name);
    }
    if (cd != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, cd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(wp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value set_window_style(value wnd, value style)
{
    CAMLparam2(wnd, style);
    HWND hwnd;
    LARGE_INTEGER li;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    li.QuadPart = Int64_val(style);
    SetLastError(0);
    enter_blocking_section();
    li.LowPart = SetWindowLong(hwnd, GWL_STYLE, li.LowPart);
    li.HighPart = SetWindowLong(hwnd, GWL_EXSTYLE, li.HighPart);
    leave_blocking_section();
    if (GetLastError() != 0)
        raise_last_error();
    val = copy_int64(li.QuadPart);
    CAMLreturn(val);
}

CAMLprim value set_window_proc(value wnd, value proc)
{
    CAMLparam2(wnd, proc);
    HWND hwnd;
    DWORD result;
    WNDPROC wp;
    window_data* wd;
    CAMLlocal2(val, v);

    hwnd = Handle_val(wnd);
    if (Tag_val(proc) == 0) {
        wd = make_window_data(Handle_val(wnd));
        wd->function = Field(proc, 0);
        wp = window_proc;
    } else
        wp = Pointer_val(Field(proc, 0));
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, GWL_WNDPROC, (LONG)wp);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    if (Tag_val(proc) != 0)
        remove_window_data(Handle_val(wnd));
    wp = (WNDPROC)result;
    if (wp == window_proc && (wd = get_window_data(Handle_val(wnd))) != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, wd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(wp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value set_window_instance(value wnd, value inst)
{
    CAMLparam2(wnd, inst);
    HWND hwnd;
    HINSTANCE hinst;
    DWORD result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    hinst = Handle_val(inst);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, GWL_HINSTANCE, (LONG)hinst);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = alloc_handle((HINSTANCE)result);
    CAMLreturn(val);
}

CAMLprim value set_window_id(value wnd, value id)
{
    CAMLparam2(wnd, id);
    HWND hwnd;
    DWORD result;

    hwnd = Handle_val(wnd);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, GWL_ID, Int_val(id));
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value set_window_user_data(value wnd, value data)
{
    CAMLparam2(wnd, data);
    HWND hwnd;
    DWORD d, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    d = Int32_val(data);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, GWL_USERDATA, d);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value set_dialog_proc(value wnd, value proc)
{
    CAMLparam2(wnd, proc);
    HWND hwnd;
    DWORD result;
    DLGPROC dp;
    window_data* wd;
    CAMLlocal2(val, v);

    hwnd = Handle_val(wnd);
    if (Tag_val(proc) == 0) {
        wd = make_window_data(Handle_val(wnd));
        wd->function = Field(proc, 0);
        dp = dialog_proc;
    } else
        dp = Pointer_val(Field(proc, 0));
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, DWL_DLGPROC, (LONG)dp);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    if (Tag_val(proc) != 0)
        remove_window_data(Handle_val(wnd));
    dp = (DLGPROC)result;
    if (dp == dialog_proc && (wd = get_window_data(Handle_val(wnd))) != NULL) {
        val = alloc(1, 0);
        Store_field(val, 0, wd->function);
    } else {
        val = alloc(1, 1);
        v = alloc_pointer(dp);
        Store_field(val, 0, v);
    }
    CAMLreturn(val);
}

CAMLprim value set_dialog_msg_result(value wnd, value msg_result)
{
    CAMLparam2(wnd, msg_result);
    HWND hwnd;
    DWORD mr, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    mr = Int32_val(msg_result);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, DWL_MSGRESULT, mr);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value set_dialog_user_data(value wnd, value data)
{
    CAMLparam2(wnd, data);
    HWND hwnd;
    DWORD d, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    d = Int32_val(data);
    SetLastError(0);
    enter_blocking_section();
    result = SetWindowLong(hwnd, DWL_USER, d);
    leave_blocking_section();
    if (result == 0 && GetLastError() != 0)
        raise_last_error();
    val = copy_int32(result);
    CAMLreturn(val);
}
