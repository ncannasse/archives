/* OCaml-Win32
 * win32_dc.c
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
/* Graphics and Multimedia Services: GDI: Device Contexts */

CAMLprim value cancel_dc(value hdc)
{
    if (!CancelDC(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value create_compatible_dc(value hdc)
{
    HDC result;

    result = CreateCompatibleDC(Handle_val(hdc));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value delete_dc(value hdc)
{
    if (!DeleteDC(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value delete_object(value hgdiobj)
{
    if (!DeleteObject(Handle_val(hgdiobj)))
        raise_last_error();
    return Val_unit;
}

static int CALLBACK enum_brushes_proc(LPVOID lpLogObject, LPARAM lpData)
{
    CAMLparam0();
    CAMLlocal3(func, lb, result);

    func = *(value*)lpData;
    lb = alloc_logbrush(lpLogObject);
    result = callback(func, lb);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_brushes(value hdc, value enum_func)
{
    CAMLparam2(hdc, enum_func);
    value* p;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    EnumObjects(Handle_val(hdc), OBJ_BRUSH, enum_brushes_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

static int CALLBACK enum_pens_proc(LPVOID lpLogObject, LPARAM lpData)
{
    CAMLparam0();
    CAMLlocal3(func, lp, result);

    func = *(value*)lpData;
    lp = alloc_logpen(lpLogObject);
    result = callback(func, lp);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_pens(value hdc, value enum_func)
{
    CAMLparam2(hdc, enum_func);
    value* p;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    EnumObjects(Handle_val(hdc), OBJ_PEN, enum_pens_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value get_current_object(value hdc, value object_type)
{
    return alloc_handle(GetCurrentObject(Handle_val(hdc), Constant_val(object_type, object_types)));
}

CAMLprim value get_dc(value hwnd)
{
    HDC result;

    result = GetDC(Handle_val(hwnd));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_dc_ex(value hwnd, value clip, value options)
{
    HDC result;

    result = GetDCEx(Handle_val(hwnd), Handle_val(clip), flags_val(options, get_dc_ex_options));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_dc_org_ex(value hdc)
{
    POINT p;

    if (!GetDCOrgEx(Handle_val(hdc), &p))
        raise_last_error();
    return alloc_point(p);
}

CAMLprim value get_object_type(value hgdiobj)
{
    DWORD result;

    result = GetObjectType(Handle_val(hgdiobj));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, object_types);
}

CAMLprim value get_stock_object(value stock_object)
{
    HGDIOBJ result;

    result = GetStockObject(Constant_val(stock_object, stock_objects));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value release_dc(value hwnd, value hdc)
{
    return Val_bool(ReleaseDC(Handle_val(hwnd), Handle_val(hdc)));
}

CAMLprim value restore_dc(value hdc, value saved_dc)
{
    if (!RestoreDC(Handle_val(hdc), Int_val(saved_dc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value save_dc(value hdc)
{
    int result;

    result = SaveDC(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value select_object(value hdc, value hgdiobj)
{
    HGDIOBJ result;

    result = SelectObject(Handle_val(hdc), Handle_val(hgdiobj));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value select_region(value hdc, value hrgn)
{
    DWORD result;

    result = (DWORD)SelectObject(Handle_val(hdc), Handle_val(hrgn));
    if (result == GDI_ERROR)
        raise_last_error();
    return val_regioncomplexity(result);
}

/* Messages */

CAMLprim value deliver_wm_devmodechange(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(s, result);

    s = copy_string((char*)Lparam(msg));
    result = callback3(handler, wnd, msg, s);
    CAMLreturn(result);
}
