/* OCaml-Win32
 * win32_icon.c
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
/* User Interface Services: Resources: Icons */

CAMLprim value copy_icon(value hicon)
{
    HICON result;

    result = CopyIcon(Handle_val(hicon));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_icon_native(value hinstance, value size, value planes, value bits_per_pel, value and_bits, value xor_bits)
{
    HICON result;

    result = CreateIcon(
        Handle_val(hinstance),
        Cx(size), Cy(size),
        (BYTE)Int_val(planes),
        (BYTE)Int_val(bits_per_pel),
        raw_val(and_bits),
        raw_val(xor_bits));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_icon_bytecode(value* argv, int argn)
{
    return create_icon_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value create_icon_indirect(value iconinfo)
{
    ICONINFO ii;
    HICON result;

    iconinfo_val(iconinfo, &ii);
    result = CreateIconIndirect(&ii);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value destroy_icon(value hicon)
{
    if (!DestroyIcon(Handle_val(hicon)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_icon(value hdc, value point, value hicon)
{
    if (!DrawIcon(Handle_val(hdc), X(point), Y(point), Handle_val(hicon)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_icon_ex_native(value hdc, value point, value hicon, value size, value step, value flicker_free, value options)
{
    BOOL result;

    result = DrawIconEx(
        Handle_val(hdc),
        X(point), Y(point),
        Handle_val(hicon),
        Cx(size), Cy(size),
        Int_val(step),
        Handle_val(flicker_free),
        flags_val(options, draw_icon_options));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_icon_ex_bytecode(value* argv, int argn)
{
    return draw_icon_ex_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
}

CAMLprim value extract_associated_icon(value hinstance, value filename, value index)
{
    CAMLparam3(hinstance, filename, index);
    char s[_MAX_PATH+1];
    WORD w;
    HICON result;
    CAMLlocal2(val, v);

    strncpy(s, String_val(filename), _MAX_PATH);
    s[_MAX_PATH] = '\0';
    w = (WORD)Int_val(index);
    result = ExtractAssociatedIcon(Handle_val(hinstance), s, &w);
    if (!result)
        raise_last_error();
    val = alloc_tuple(3);
    v = alloc_handle(result);
    Store_field(val, 0, v);
    v = copy_string(s);
    Store_field(val, 1, v);
    Store_field(val, 2, Val_int(w));
    CAMLreturn(val);
}

CAMLprim value get_icon_info(value hicon)
{
    ICONINFO ii;

    if (!GetIconInfo(Handle_val(hicon), &ii))
        raise_last_error();
    return alloc_iconinfo(&ii);
}

CAMLprim value get_icon_info_by_name(value name)
{
    ICONINFO ii;

    if (!GetIconInfo((HICON)resname_val(name), &ii))
        raise_last_error();
    return alloc_iconinfo(&ii);
}

CAMLprim value load_icon(value hinstance, value name)
{
    HICON result;

    result = LoadIcon(Handle_val(hinstance), resname_val(name));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

/* Messages */

CAMLprim value deliver_wm_erasebkgnd(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HDC)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}

CAMLprim value return_wm_erasebkgnd(value result)
{
    return copy_int32(Bool_val(result));
}

CAMLprim value deliver_wm_iconerasebkgnd(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HDC)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}
