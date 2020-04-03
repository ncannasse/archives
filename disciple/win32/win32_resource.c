/* OCaml-Win32
 * win32_resource.c
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
/* User Interface Services: Resources: Resources */

CAMLprim value begin_update_resource(value filename, value delete_existing_resources)
{
    CAMLparam2(filename, delete_existing_resources);
    char* fn;
    HANDLE result;
    CAMLlocal1(val);

    fn = new_string(filename);
    enter_blocking_section();
    result = BeginUpdateResource(fn, Bool_val(delete_existing_resources));
    leave_blocking_section();
    free_string(fn);
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value copy_image(value image, value type, value size, value options)
{
    HANDLE result;

    result = CopyImage(
        Handle_val(image),
        Constant_val(type, image_types),
        Cx(size), Cy(size),
        flags_val(options, copy_image_options));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value end_update_resource(value handle, value discard)
{
    CAMLparam2(handle, discard);
    HANDLE h;
    BOOL result;

    h = Handle_val(handle);
    enter_blocking_section();
    result = EndUpdateResource(h, Bool_val(discard));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

static BOOL CALLBACK enum_res_name_proc(HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam)
{
    CAMLparam0();
    CAMLlocal3(func, resname, result);

    func = *(value*)lParam;
    resname = alloc_resname(lpszName);
    result = callback(func, resname);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_resource_names(value hinstance, value type, value func)
{
    CAMLparam3(hinstance, type, func);
    LPTSTR typ;
    value* p;
    BOOL result;

    typ = new_restype(type);
    p = malloc(sizeof(value));
    register_global_root(p);
    *p = func;
    result = EnumResourceNames(Handle_val(hinstance), typ, enum_res_name_proc, (LONG)p);
    free_restype(typ);
    remove_global_root(p);
    free(p);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

static BOOL CALLBACK enum_res_type_proc(HANDLE hModule, LPTSTR lpszType, LONG lParam)
{
    CAMLparam0();
    CAMLlocal3(func, restype, result);

    func = *(value*)lParam;
    restype = alloc_restype(lpszType);
    result = callback(func, restype);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_resource_types(value hinstance, value func)
{
    CAMLparam2(hinstance, func);
    value* p;
    BOOL result;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = func;
    result = EnumResourceTypes(Handle_val(hinstance), enum_res_type_proc, (LONG)p);
    remove_global_root(p);
    free(p);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value find_resource(value hinstance, value name, value type)
{
    HRSRC result;

    result = FindResource(Handle_val(hinstance), resname_val(name), restype_val(type));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value load_image(value hinstance, value name, value type, value size, value options)
{
    HANDLE result;

    result = LoadImage(
        Handle_val(hinstance),
        resname_val(name),
        Constant_val(type, image_types),
        Cx(size), Cy(size),
        flags_val(options, load_image_options));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value load_image_from_file(value filename, value type, value size, value options)
{
    CAMLparam4(filename, type, size, options);
    char* fn;
    SIZE siz;
    UINT opt;
    HANDLE result;
    CAMLlocal1(val);

    fn = new_string(filename);
    siz = size_val(size);
    opt = flags_val(options, load_image_options) | LR_LOADFROMFILE;
    enter_blocking_section();
    result = LoadImage(NULL, fn, Constant_val(type, image_types), siz.cx, siz.cy, opt);
    leave_blocking_section();
    free_string(fn);
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value load_resource(value hinstance, value hrsrc)
{
    HANDLE result;

    result = LoadResource(Handle_val(hinstance), Handle_val(hrsrc));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value lock_resource(value handle)
{
    PVOID result;

    result = LockResource(Handle_val(handle));
    if (!result)
        raise_last_error();
    return alloc_pointer(result);
}

CAMLprim value sizeof_resource(value hinstance, value handle)
{
    DWORD result;

    result = SizeofResource(Handle_val(hinstance), Handle_val(handle));
    if (result == 0)
        raise_last_error();
    return copy_int64(result);
}
