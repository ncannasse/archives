/* OCaml-Win32
 * win32_color.c
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
/* Graphics and Multimedia Services: GDI: Colors */

CAMLprim value create_halftone_palette(value hdc)
{
    HPALETTE result;

    result = CreateHalftonePalette(Handle_val(hdc));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_nearest_color(value hdc, value color)
{
    COLORREF result;

    result = GetNearestColor(Handle_val(hdc), color_val(color));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value get_nearest_palette_index(value hpalette, value color)
{
    UINT result;

    result = GetNearestPaletteIndex(Handle_val(hpalette), color_val(color));
    if (result == CLR_INVALID)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_system_palette_use(value hdc)
{
    UINT result;

    result = GetSystemPaletteUse(Handle_val(hdc));
    if (result == SYSPAL_ERROR)
        raise_last_error();
    return Val_constant(result, system_palette_uses);
}

CAMLprim value realize_palette(value dc)
{
    CAMLparam1(dc);
    HANDLE hdc;
    UINT result;

    hdc = Handle_val(dc);
    enter_blocking_section();
    result = RealizePalette(hdc);
    leave_blocking_section();
    if (result == GDI_ERROR)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value resize_palette(value hpalette, value size)
{
    if (!ResizePalette(Handle_val(hpalette), Int_val(size)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value select_palette(value dc, value palette, value force_background)
{
    CAMLparam3(dc, palette, force_background);
    HDC hdc;
    HPALETTE hpalette, result;

    hdc = Handle_val(dc);
    hpalette = Handle_val(palette);
    enter_blocking_section();
    result = SelectPalette(hdc, hpalette, Bool_val(force_background));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(alloc_handle(result));
}

CAMLprim value set_system_palette_use(value hdc, value use)
{
    UINT result;

    result = SetSystemPaletteUse(Handle_val(hdc), Constant_val(use, system_palette_uses));
    if (result == SYSPAL_ERROR)
        raise_last_error();
    return Val_constant(result, system_palette_uses);
}

CAMLprim value unrealize_object(value hgdiobj)
{
    if (!UnrealizeObject(Handle_val(hgdiobj)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value update_colors(value hdc)
{
    if (!UpdateColors(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

/* Messages */

CAMLprim value deliver_wm_palettechange(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HWND)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}

CAMLprim value return_wm_querynewpalette(value result)
{
    return copy_int32(Bool_val(result));
}
