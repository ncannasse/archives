/* OCaml-Win32
 * win32_control.c
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
/* User Interface Services: Controls: Controls */

/* Messages */

CAMLprim value return_wm_getfont(value result)
{
    return copy_int32((LRESULT)Handle_val(result));
}

CAMLprim value unpack_wm_getfont(value result)
{
    return alloc_handle((HFONT)Int32_val(result));
}

CAMLprim value deliver_wm_setfont(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HFONT)Wparam(msg));
    args[3] = Val_bool(LOWORD(Lparam(msg)));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value pack_wm_setfont(value hfont, value redraw)
{
    return pack_message(WM_SETFONT, (WPARAM)Handle_val(hfont), MAKELPARAM(Bool_val(redraw), 0));
}

CAMLprim value deliver_wm_command(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_int(HIWORD(Wparam(msg)));
    args[3] = Val_int(LOWORD(Wparam(msg)));
    args[4] = alloc_handle((HWND)Lparam(msg));
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}
