/* OCaml-Win32
 * win32_ipc.c
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
/* User Interface Services: User Input: Keyboard Accelerators */

/* Messages */

CAMLprim value deliver_wm_copydata(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HWND)Wparam(msg));
    args[3] = alloc_copydatastruct((PCOPYDATASTRUCT)Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value return_wm_copydata(value result)
{
    return copy_int32(Bool_val(result));
}

CAMLprim value issue_wm_copydata(value sender, value source_wnd, value cds)
{
    CAMLparam3(sender, source_wnd, cds);
    COPYDATASTRUCT c;
    CAMLlocal2(contents, result);

    fill_copydatastruct(&c, cds);
    contents = pack_message(WM_COPYDATA, (WPARAM)Handle_val(source_wnd), (LPARAM)&c);
    result = callback(sender, contents);
    empty_copydatastruct(&c, cds);
    CAMLreturn(Val_bool(Int32_val(result)));
}
