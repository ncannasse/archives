/* OCaml-Win32
 * win32_staticcontrol.c
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
/* User Interface Services: Controls: Static Controls */

/* Styles */

static DWORD static_style_options[] = {
    SS_BITMAP,
    SS_BLACKFRAME,
    SS_BLACKRECT,
    SS_CENTER,
    SS_CENTERIMAGE,
    SS_ENDELLIPSIS,
    SS_ENHMETAFILE,
    SS_ETCHEDFRAME,
    SS_ETCHEDHORZ,
    SS_ETCHEDVERT,
    SS_GRAYFRAME,
    SS_GRAYRECT,
    SS_ICON,
    SS_LEFT,
    SS_LEFTNOWORDWRAP,
    SS_NOPREFIX,
    SS_NOTIFY,
    SS_OWNERDRAW,
    SS_PATHELLIPSIS,
    SS_REALSIZEIMAGE,
    SS_RIGHT,
    SS_RIGHTJUST,
    SS_SIMPLE,
    SS_SUNKEN,
    SS_WHITEFRAME,
    SS_WHITERECT,
    SS_WORDELLIPSIS
};
Define_static_constants(static_style_options);

CAMLprim value static_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, static_style_options));
}

/* Messages */

CAMLprim value unpack_stm_geticon(value result)
{
    return alloc_handle((HICON)Int32_val(result));
}

CAMLprim value pack_stm_getimage(value type)
{
    return pack_message(STM_GETIMAGE, Constant_val(type, image_types), 0);
}

CAMLprim value unpack_stm_getimage(value result)
{
    return alloc_handle((HANDLE)Int32_val(result));
}

CAMLprim value pack_stm_seticon(value icon)
{
    return pack_message(STM_SETICON, (WPARAM)Handle_val(icon), 0);
}

CAMLprim value unpack_stm_seticon(value result)
{
    return alloc_handle((HICON)Int32_val(result));
}

CAMLprim value pack_stm_setimage(value type, value image)
{
    return pack_message(STM_SETIMAGE, Constant_val(type, image_types), (LPARAM)Handle_val(image));
}

CAMLprim value unpack_stm_setimage(value result)
{
    return alloc_handle((HANDLE)Int32_val(result));
}
