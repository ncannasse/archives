/* OCaml-Win32
 * win32_button.c
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
/* User Interface Services: Controls: Buttons */

CAMLprim value check_dlg_button(value wnd, value id, value state)
{
    CAMLparam3(wnd, id, state);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = CheckDlgButton(hwnd, Int_val(id), Constant_val(state, button_states));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value check_radio_button(value wnd, value first_id, value last_id, value id)
{
    CAMLparam4(wnd, first_id, last_id, id);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = CheckRadioButton(hwnd, Int_val(first_id), Int_val(last_id), Int_val(id));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value is_dlg_button_checked(value wnd, value id)
{
    CAMLparam2(wnd, id);
    HWND hwnd;
    DWORD result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = IsDlgButtonChecked(hwnd, Int_val(id));
    leave_blocking_section();
    CAMLreturn(Val_constant(result, button_states));
}

/* Styles */

static DWORD button_style_options[] = {
    BS_3STATE,
    BS_AUTO3STATE,
    BS_AUTOCHECKBOX,
    BS_AUTORADIOBUTTON,
    BS_BITMAP,
    BS_BOTTOM,
    BS_CENTER,
    BS_CHECKBOX,
    BS_DEFPUSHBUTTON,
    BS_FLAT,
    BS_GROUPBOX,
    BS_ICON,
    BS_LEFT,
    BS_LEFTTEXT,
    BS_MULTILINE,
    BS_NOTIFY,
    BS_OWNERDRAW,
    BS_PUSHBUTTON,
    BS_PUSHLIKE,
    BS_RADIOBUTTON,
    BS_RIGHT,
    BS_RIGHTBUTTON,
    BS_TEXT,
    BS_TOP,
    BS_VCENTER
};
Define_static_constants(button_style_options);

CAMLprim value button_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, button_style_options));
}

/* Messages */

CAMLprim value unpack_bm_getcheck(value result)
{
    return Val_constant(Int32_val(result), button_states);
}

CAMLprim value pack_bm_getimage(value type)
{
    return pack_message(BM_GETIMAGE, Constant_val(type, image_types), 0);
}

CAMLprim value unpack_bm_getimage(value result)
{
    return alloc_handle((HANDLE)Int32_val(result));
}

CAMLprim value unpack_bm_getstate(value result)
{
    CAMLparam1(result);
    DWORD state;
    CAMLlocal2(val, v);

    state = Int32_val(result);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_constant(state & 0x03, button_states));
    v = Alloc_flags(state & (~0x03), button_state_exs);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value pack_bm_setcheck(value state)
{
    return pack_message(BM_SETCHECK, Constant_val(state, button_states), 0);
}

CAMLprim value pack_bm_setimage(value type, value new_image)
{
    return pack_message(BM_SETIMAGE, Constant_val(type, image_types), (LPARAM)Handle_val(new_image));
}

CAMLprim value unpack_bm_setimage(value result)
{
    return alloc_handle((HANDLE)Int32_val(result));
}

CAMLprim value pack_bm_setstate(value state)
{
    return pack_message(BM_SETSTATE, Bool_val(state), 0);
}

CAMLprim value pack_bm_setstyle(value options, value redraw)
{
    return pack_message(BM_SETSTYLE, flags_val(options, button_style_options), MAKELPARAM(Bool_val(redraw), 0));
}
