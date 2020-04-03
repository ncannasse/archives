/* OCaml-Win32
 * win32_bitmap.c
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
/* Graphics and Multimedia Services: GDI: Bitmaps */

CAMLprim value bit_blt_native(value dest_hdc, value dest_point, value size, value src_hdc, value src_point, value rop3)
{
    BOOL result;

    result = BitBlt(
        Handle_val(dest_hdc),
        X(dest_point), Y(dest_point),
        Cx(size), Cy(size),
        Handle_val(src_hdc),
        X(src_point), Y(src_point),
        Int_val(rop3));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value bit_blt_bytecode(value* argv, int argn)
{
    return bit_blt_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value create_bitmap(value size, value planes, value bits_per_pel, value bits)
{
    HBITMAP result;

    result = CreateBitmap(
        Cx(size), Cy(size),
        Int_val(planes),
        Int_val(bits_per_pel),
        raw_val(bits));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_bitmap_indirect(value bitmap)
{
    BITMAP bm;
    HBITMAP result;

    bitmap_val(bitmap, &bm);
    result = CreateBitmapIndirect(&bm);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_compatible_bitmap(value hdc, value size)
{
    HBITMAP result;

    result = CreateCompatibleBitmap(Handle_val(hdc), Cx(size), Cy(size));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value flood_fill(value hdc, value point, value color, value type)
{
    BOOL result;

    result = ExtFloodFill(
        Handle_val(hdc),
        X(point), Y(point),
        color_val(color),
        Constant_val(type, flood_fill_types));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_bitmap_dimension(value hbitmap)
{
    SIZE s;

    if (!GetBitmapDimensionEx(Handle_val(hbitmap), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value get_pixel(value hdc, value point)
{
    COLORREF result;

    result = GetPixel(Handle_val(hdc), X(point), Y(point));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value get_stretch_blt_mode(value hdc)
{
    int result;

    result = GetStretchBltMode(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, stretch_blt_modes);
}

CAMLprim value load_bitmap(value hinstance, value name)
{
    HBITMAP result;

    result = LoadBitmap(Handle_val(hinstance), resname_val(name));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value mask_blt_native(value dest_hdc, value dest_point, value size, value src_hdc, value src_point, value mask, value mask_point, value foreground_rop, value background_rop)
{
    BOOL result;

    result = MaskBlt(
        Handle_val(dest_hdc),
        X(dest_point), Y(dest_point),
        Cx(size), Cy(size),
        Handle_val(src_hdc),
        X(src_point), Y(src_point),
        Handle_val(mask),
        X(mask_point), Y(mask_point),
        MAKEROP4(Int_val(foreground_rop), Int_val(background_rop)));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value mask_blt_bytecode(value* argv, int argn)
{
    return mask_blt_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
}

CAMLprim value plg_blt_native(value dest_hdc, value dest_points, value src_hdc, value src_point, value size, value mask)
{
    int i;
    POINT dest_point_array[3];
    HBITMAP mask_hbitmap;
    POINT mask_point;
    BOOL result;

    for (i = 0; i < 3; ++i)
        dest_point_array[i] = point_val(Field(dest_points, i));
    if (Is_some(mask)) {
        mask_hbitmap = Handle_val(Field(Value_of(mask), 0));
        mask_point = point_val(Field(Value_of(mask), 1));
    } else {
        mask_hbitmap = NULL;
        mask_point.x = mask_point.y = 0;
    }
    result = PlgBlt(
        Handle_val(dest_hdc),
        dest_point_array,
        Handle_val(src_hdc),
        X(src_point), Y(src_point),
        Cx(size), Cy(size),
        Handle_val(mask_hbitmap),
        mask_point.x, mask_point.y);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value plg_blt_bytecode(value* argv, int argn)
{
    return plg_blt_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value set_bitmap_dimension(value hbitmap, value size)
{
    SIZE s;

    if (!SetBitmapDimensionEx(Handle_val(hbitmap), Cx(size), Cy(size), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value set_pixel(value hdc, value point, value color)
{
    COLORREF result;

    result = SetPixel(Handle_val(hdc), X(point), Y(point), color_val(color));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value set_stretch_blt_mode(value hdc, value mode)
{
    int result;

    result = SetStretchBltMode(Handle_val(hdc), Constant_val(mode, stretch_blt_modes));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, stretch_blt_modes);
}

CAMLprim value stretch_blt_native(value dest_hdc, value dest_point, value dest_size, value src_hdc, value src_point, value src_size, value rop3)
{
    BOOL result;

    result = StretchBlt(
        Handle_val(dest_hdc),
        X(dest_point), Y(dest_point),
        Cx(dest_size), Cy(dest_size),
        Handle_val(src_hdc),
        X(src_point), Y(src_point),
        Cx(src_size), Cy(src_size),
        Int_val(rop3));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value stretch_blt_bytecode(value* argv, int argn)
{
    return stretch_blt_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
}
