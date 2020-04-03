/* OCaml-Win32
 * win32_editcontrol.c
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
/* User Interface Services: Controls: Edit Controls */

/* Styles */

static DWORD edit_style_options[] = {
    ES_AUTOHSCROLL,
    ES_AUTOVSCROLL,
    ES_CENTER,
    ES_LEFT,
    ES_LOWERCASE,
    ES_MULTILINE,
    ES_NOHIDESEL,
    ES_NUMBER,
    ES_OEMCONVERT,
    ES_PASSWORD,
    ES_READONLY,
    ES_RIGHT,
    ES_UPPERCASE,
    ES_WANTRETURN
};
Define_static_constants(edit_style_options);

CAMLprim value edit_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, edit_style_options));
}

/* Messages */

CAMLprim value unpack_em_canundo(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value pack_em_charfrompos(value point)
{
    return pack_message(EM_CHARFROMPOS, 0, MAKELPARAM(X(point), Y(point)));
}

CAMLprim value unpack_em_charfrompos(value result)
{
    CAMLparam1(result);
    CAMLlocal1(val);

    val = alloc_tuple(2);
    Store_field(val, 0, LOWORD(Int32_val(result)));
    Store_field(val, 1, HIWORD(Int32_val(result)));
    CAMLreturn(val);
}

CAMLprim value pack_em_fmtlines(value add_eol)
{
    return pack_message(EM_FMTLINES, Bool_val(add_eol), 0);
}

CAMLprim value unpack_em_getfirstvisibleline(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value unpack_em_gethandle(value result)
{
    return alloc_handle((HANDLE)Int32_val(result));
}

CAMLprim value unpack_em_getlimittext(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value issue_em_getline(value sender, value line, value max_length)
{
    CAMLparam3(sender, line, max_length);
    int max_len;
    char* s;
    CAMLlocal3(contents, result, val);

    max_len = Int_val(max_length);
    s = malloc(max_len + 1);
    *(WORD*)s = (WORD)max_len;
    contents = pack_message(EM_GETLINE, Int_val(line), (LPARAM)s);
    result = callback(sender, contents);
    s[Int32_val(result)] = '\0';
    val = copy_string(s);
    free(s);
    CAMLreturn(val);
}

CAMLprim value unpack_em_getlinecount(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value unpack_em_getmargins(value result)
{
    CAMLparam1(result);
    CAMLlocal1(val);

    val = alloc_tuple(2);
    Store_field(val, 0, LOWORD(Int32_val(result)));
    Store_field(val, 1, HIWORD(Int32_val(result)));
    CAMLreturn(val);
}

CAMLprim value unpack_em_getmodify(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value unpack_em_getpasswordchar(value result)
{
    LRESULT l;

    l = Int32_val(result);
    if (l == 0)
        return Val_none;
    else
        return alloc_some(Val_int((char)l));
}

CAMLprim value issue_em_getrect(value sender)
{
    CAMLparam1(sender);
    CAMLlocal2(contents, rect);
    RECT r;

    contents = pack_message(EM_GETRECT, 0, (LPARAM)&r);
    callback(sender, contents);
    rect = alloc_rect(&r);
    CAMLreturn(rect);
}

CAMLprim value issue_em_getsel(value sender)
{
    CAMLparam1(sender);
    CAMLlocal2(contents, val);
    DWORD start, end;

    contents = pack_message(EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    callback(sender, contents);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(start));
    Store_field(val, 1, Val_int(end));
    CAMLreturn(val);
}

CAMLprim value unpack_em_getthumb(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_em_limittext(value limit)
{
    return pack_message(EM_LIMITTEXT, Int_val(limit), 0);
}

CAMLprim value pack_em_linefromchar(value char_index)
{
    return pack_message(EM_LINEFROMCHAR, Int_val(char_index), 0);
}

CAMLprim value unpack_em_linefromchar(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_em_lineindex(value line)
{
    return pack_message(EM_LINEINDEX, Int_val(line), 0);
}

CAMLprim value unpack_em_lineindex(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_em_linelength(value line)
{
    return pack_message(EM_LINELENGTH, Int_val(line), 0);
}

CAMLprim value unpack_em_linelength(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_em_linescroll(value x, value y)
{
    return pack_message(EM_LINESCROLL, Int_val(x), Int_val(y));
}

CAMLprim value pack_em_posfromchar(value char_index)
{
    return pack_message(EM_POSFROMCHAR, Int_val(char_index), 0);
}

CAMLprim value unpack_em_posfromchar(value result)
{
    DWORD d;
    POINT p;

    d = Int32_val(result);
    p.x = GET_X_LPARAM(d);
    p.y = GET_Y_LPARAM(d);
    return alloc_point(p);
}

CAMLprim value issue_em_replacesel(value sender, value can_undo, value string)
{
    CAMLparam3(sender, can_undo, string);
    char* s;
    CAMLlocal1(contents);

    s = new_string(string);
    contents = pack_message(EM_REPLACESEL, Bool_val(can_undo), (LPARAM)s);
    callback(sender, contents);
    free_string(s);
    CAMLreturn(Val_unit);
}

CAMLprim value pack_em_scroll(value action)
{
    int code, param;

    scrollaction_val(action, &code, &param);
    return pack_message(EM_SCROLL, code, 0);
}

CAMLprim value unpack_em_scroll(value result)
{
    LRESULT l;

    l = Int32_val(result);
    if (HIWORD(l) == TRUE)
        return alloc_some(Val_int(LOWORD(l)));
    else
        return Val_none;
}

CAMLprim value pack_em_sethandle(value handle)
{
    return pack_message(EM_SETHANDLE, (WPARAM)Handle_val(handle), 0);
}

CAMLprim value pack_em_setmargins(value spec)
{
    DWORD flags;
    int left, right;
    value v;

    flags = 0;
    left = right = 0;
    if (Is_block(spec)) {
        v = Field(spec, 0);
        if (Is_some(Field(v, 0))) {
            flags |= EC_LEFTMARGIN;
            left = Int_val(Value_of(Field(v, 0)));
        }
        if (Is_some(Field(v, 1))) {
            flags |= EC_RIGHTMARGIN;
            right = Int_val(Value_of(Field(v, 0)));
        }
    } else
        flags = EC_USEFONTINFO;
    return pack_message(EM_SETMARGINS, flags, MAKELONG(left, right));
}

CAMLprim value pack_em_setmodify(value modified)
{
    return pack_message(EM_SETMODIFY, Bool_val(modified), 0);
}

CAMLprim value pack_em_setpasswordchar(value new_char)
{
    return pack_message(EM_SETPASSWORDCHAR, Int_val(new_char), 0);
}

CAMLprim value pack_em_setreadonly(value readonly)
{
    return pack_message(EM_SETREADONLY, Bool_val(readonly), 0);
}

CAMLprim value unpack_em_setreadonly(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value issue_em_setrect(value sender, value rect)
{
    CAMLparam2(sender, rect);
    RECT r;
    CAMLlocal1(contents);

    rect_val(rect, &r);
    contents = pack_message(EM_SETRECT, 0, (LPARAM)&r);
    callback(sender, contents);
    CAMLreturn(Val_unit);
}

CAMLprim value issue_em_setrectnp(value sender, value rect)
{
    CAMLparam2(sender, rect);
    RECT r;
    CAMLlocal1(contents);

    rect_val(rect, &r);
    contents = pack_message(EM_SETRECTNP, 0, (LPARAM)&r);
    callback(sender, contents);
    CAMLreturn(Val_unit);
}

CAMLprim value pack_em_setsel(value start_sel, value end_sel)
{
    return pack_message(EM_SETSEL, Int_val(start_sel), Int_val(end_sel));
}

CAMLprim value issue_em_settabstops(value sender, value stop_list)
{
    CAMLparam2(sender, stop_list);
    int num_stops;
    DWORD* stops;
    CAMLlocal2(contents, result);

    num_stops = list_length(stop_list);
    if (num_stops)
        stops = malloc(num_stops * sizeof(DWORD));
    else
        stops = NULL;
    num_stops = 0;
    for (; !Is_nil(stop_list); stop_list = Tail(stop_list))
        stops[num_stops++] = Int_val(Head(stop_list));
    contents = pack_message(EM_SETTABSTOPS, num_stops, (LPARAM)stops);
    result = callback(sender, contents);
    if (stops)
        free(stops);
    CAMLreturn(Val_bool(Int32_val(result)));
}

CAMLprim value unpack_em_undo(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value unpack_wm_undo(value result)
{
    return Val_bool(Int32_val(result));
}
