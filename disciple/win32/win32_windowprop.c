/* OCaml-Win32
 * win32_windowprop.c
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
/* User Interface Services: Windowing: Window Properties */

static BOOL CALLBACK prop_enum_proc(HWND hwnd, LPTSTR lpszString, HANDLE hData, DWORD dwData)
{
    CAMLparam0();
    CAMLlocal4(func, name, data, result);

    func = *(value*)dwData;
    name = alloc_atomname(lpszString);
    data = copy_int32((DWORD)hData);
    result = callback2(func, name, data);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_props(value hwnd, value enum_func)
{
    CAMLparam2(hwnd, enum_func);
    value* p;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = enum_func;
    EnumPropsEx(Handle_val(hwnd), prop_enum_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value get_prop(value hwnd, value name)
{
    HANDLE h;

    h = GetProp(Handle_val(hwnd), atomname_val(name));
    return copy_int32((int)h);
}

CAMLprim value remove_prop(value hwnd, value name)
{
    RemoveProp(Handle_val(hwnd), atomname_val(name));
    return Val_unit;
}

CAMLprim value set_prop(value hwnd, value name, value data)
{
    if (!SetProp(Handle_val(hwnd), atomname_val(name), (HANDLE)Int32_val(data)))
        raise_last_error();
    return Val_unit;
}
