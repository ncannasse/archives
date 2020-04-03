/* OCaml-Win32
 * win32_accelerator.c
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

CAMLprim value load_accelerators(value hinstance, value name)
{
    HACCEL result;

    result = LoadAccelerators(Handle_val(hinstance), resname_val(name));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value translate_accelerator(value wnd, value accel, value msg)
{
    CAMLparam3(wnd, accel, msg);
    HWND hwnd;
    HACCEL haccel;
    MSG m;
    int result;

    hwnd = Handle_val(wnd);
    haccel = Handle_val(accel);
    msg_val(msg, &m);
    enter_blocking_section();
    result = TranslateAccelerator(hwnd, haccel, &m);
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

/* Messages */

CAMLprim value deliver_wm_initmenu(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HMENU)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_initmenupopup(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HMENU)Wparam(msg));
    args[3] = Val_int(LOWORD(Lparam(msg)));
    args[4] = Val_bool(HIWORD(Lparam(msg)));
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

static DWORD sys_commands_normal[] = {
    SC_CLOSE,
    SC_CONTEXTHELP,
    SC_DEFAULT,
    SC_HOTKEY,
    SC_HSCROLL,
    SC_KEYMENU,
    SC_MAXIMIZE,
    SC_MINIMIZE,
    SC_MOUSEMENU,
    SC_MOVE,
    SC_NEXTWINDOW,
    SC_PREVWINDOW,
    SC_RESTORE,
    SC_SCREENSAVE,
    SC_SIZE,
    SC_TASKLIST,
    SC_VSCROLL
};
Define_static_constants(sys_commands_normal);

CAMLprim value deliver_wm_syscommand(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal2(point, result);
    POINT pt;

    args[0] = wnd;
    args[1] = msg;
    if (Wparam(msg) == SC_MONITORPOWER) {
        if (Lparam(msg) == 1)
            args[2] = Val_int(sizeof(sys_commands_normal) / sizeof(sys_commands_normal[0]));
        else
            args[2] = Val_int(sizeof(sys_commands_normal) / sizeof(sys_commands_normal[0])) + 1;
        args[3] = Val_int(2);
    } else {
        args[2] = Val_constant(Wparam(msg), sys_commands_normal);
        pt.x = GET_X_LPARAM(Lparam(msg));
        pt.y = GET_Y_LPARAM(Lparam(msg));
        if ((WORD)pt.y == (WORD)-1)
            args[3] = Val_int(0);
        else if (pt.y == 0)
            args[3] = Val_int(1);
        else {
            args[3] = alloc(1, 0);
            point = alloc_point(pt);
            Store_field(args[3], 0, point);
        }
    }
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}
