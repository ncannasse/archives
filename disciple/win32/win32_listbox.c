/* OCaml-Win32
 * win32_listbox.c
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
/* User Interface Services: Controls: List Boxes */

CAMLprim value dlg_dir_list(value wnd, value path, value list_id, value static_id, value options)
{
    CAMLparam5(wnd, path, list_id, static_id, options);
    HWND hwnd;
    UINT type;
    char path_buf[_MAX_PATH+1];
    int result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    type = flags_val(options, dlg_dir_list_options);
    strncpy(path_buf, String_val(path), _MAX_PATH);
    path_buf[_MAX_PATH] = '\0';
    enter_blocking_section();
    result = DlgDirList(hwnd, path_buf, Int_val(list_id), Int_val(static_id), type);
    leave_blocking_section();
    if (result == 0)
        raise_last_error();
    val = copy_string(path_buf);
    CAMLreturn(val);
}

CAMLprim value dlg_dir_select(value wnd, value list_id)
{
    CAMLparam2(wnd, list_id);
    HWND hwnd;
    char path_buf[_MAX_PATH+1];
    BOOL result;
    CAMLlocal2(val, v);

    hwnd = Handle_val(wnd);
    path_buf[0] = '\0';
    enter_blocking_section();
    result = DlgDirSelectEx(hwnd, path_buf, sizeof(path_buf), Int_val(list_id));
    leave_blocking_section();
    val = alloc_tuple(2);
    v = copy_string(path_buf);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(result));
    CAMLreturn(val);
}

/* Styles */

static DWORD listbox_style_options[] = {
    LBS_DISABLENOSCROLL,
    LBS_EXTENDEDSEL,
    LBS_HASSTRINGS,
    LBS_MULTICOLUMN,
    LBS_MULTIPLESEL,
    LBS_NODATA,
    LBS_NOINTEGRALHEIGHT,
    LBS_NOREDRAW,
    LBS_NOSEL,
    LBS_NOTIFY,
    LBS_OWNERDRAWFIXED,
    LBS_OWNERDRAWVARIABLE,
    LBS_SORT,
    LBS_STANDARD,
    LBS_USETABSTOPS,
    LBS_WANTKEYBOARDINPUT
};
Define_static_constants(listbox_style_options);

CAMLprim value listbox_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, listbox_style_options));
}

/* Messages */

CAMLprim value issue_lb_addfile(value sender, value filename)
{
    CAMLparam2(sender, filename);
    char* fn;
    CAMLlocal2(contents, result);

    fn = new_string(filename);
    contents = pack_message(LB_ADDFILE, 0, (LPARAM)fn);
    result = callback(sender, contents);
    free_string(fn);
    CAMLreturn(Val_int(check_list_box_result(Int32_val(result))));
}

CAMLprim value issue_lb_addstring(value sender, value string)
{
    CAMLparam2(sender, string);
    char* s;
    CAMLlocal2(contents, result);

    s = new_string(string);
    contents = pack_message(LB_ADDSTRING, 0, (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    CAMLreturn(Val_int(check_list_box_result(Int32_val(result))));
}

CAMLprim value pack_lb_addstring_data(value data)
{
    return pack_message(LB_ADDSTRING, 0, Int32_val(data));
}

CAMLprim value unpack_lb_addstring_data(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value pack_lb_deletestring(value index)
{
    return pack_message(LB_DELETESTRING, Int_val(index), 0);
}

CAMLprim value unpack_lb_deletestring(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value issue_lb_dir(value sender, value path, value options)
{
    CAMLparam3(sender, path, options);
    char* pth;
    CAMLlocal2(contents, result);

    pth = new_string(path);
    contents = pack_message(LB_DIR, flags_val(options, dlg_dir_list_options), (LPARAM)pth);
    result = callback(sender, contents);
    free_string(pth);
    CAMLreturn(Val_int(check_list_box_result(Int32_val(result))));
}

CAMLprim value issue_lb_findstring(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(LB_FINDSTRING, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == LB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_lb_findstring_data(value start, value data)
{
    return pack_message(LB_FINDSTRING, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_lb_findstring_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == LB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value issue_lb_findstringexact(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(LB_FINDSTRINGEXACT, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == LB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_lb_findstringexact_data(value start, value data)
{
    return pack_message(LB_FINDSTRINGEXACT, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_lb_findstringexact_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == LB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value unpack_lb_getanchorindex(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value unpack_lb_getcaretindex(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value unpack_lb_getcount(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value unpack_lb_getcursel(value result)
{
    int res;

    res = Int32_val(result);
    if (res == LB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value unpack_lb_gethorizontalextent(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_lb_getitemdata(value index)
{
    return pack_message(LB_GETITEMDATA, Int_val(index), 0);
}

CAMLprim value unpack_lb_getitemdata(value result)
{
    return copy_int32(check_list_box_result(Int32_val(result)));
}

CAMLprim value pack_lb_getitemheight(value index)
{
    return pack_message(LB_GETITEMHEIGHT, Int_val(index), 0);
}

CAMLprim value unpack_lb_getitemheight(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value issue_lb_getitemrect(value sender, value index)
{
    CAMLparam2(sender, index);
    RECT r;
    CAMLlocal3(contents, result, rect);

    contents = pack_message(LB_GETITEMRECT, Int_val(index), (LPARAM)&r);
    result = callback(sender, contents);
    check_list_box_result(Int32_val(result));
    rect = alloc_rect(&r);
    CAMLreturn(rect);
}

CAMLprim value pack_lb_getsel(value index)
{
    return pack_message(LB_GETSEL, Int_val(index), 0);
}

CAMLprim value unpack_lb_getsel(value result)
{
    int res;

    res = Int32_val(result);
    check_list_box_result(res);
    return Val_bool(result > 0);
}

CAMLprim value unpack_lb_getselcount(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value issue_lb_getselitems(value sender, value max_count)
{
    CAMLparam2(sender, max_count);
    int max_cnt, cnt, i;
    int* buf;
    CAMLlocal3(contents, result, items);

    max_cnt = Int_val(max_count);
    buf = malloc(max_cnt * sizeof(int));
    contents = pack_message(LB_GETSELITEMS, max_cnt, (LPARAM)buf);
    result = callback(sender, contents);
    cnt = Int32_val(result);
    if (cnt < 0) {
        free(buf);
        check_list_box_result(cnt);
    }
    items = alloc_tuple(cnt);
    for (i = 0; i < cnt; ++i)
        Store_field(items, i, Val_int(buf[i]));
    free(buf);
    CAMLreturn(items);
}

CAMLprim value issue_lb_gettext(value sender, value index, value max_length)
{
    CAMLparam3(sender, index, max_length);
    char* s;
    CAMLlocal3(contents, result, string);
    int res;

    s = malloc(Int_val(max_length) + 1);
    contents = pack_message(LB_GETTEXT, Int_val(index), (LPARAM)s);
    result = callback(sender, contents);
    res = Int32_val(result);
    if (res < 0) {
        free(s);
        check_list_box_result(res);
    }
    string = copy_string(s);
    free(s);
    CAMLreturn(string);
}

CAMLprim value pack_lb_gettextlen(value index)
{
    return pack_message(LB_GETTEXTLEN, Int_val(index), 0);
}

CAMLprim value unpack_lb_gettextlen(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value unpack_lb_gettopindex(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_lb_initstorage(value count, value size)
{
    return pack_message(LB_INITSTORAGE, Int_val(count), Int_val(size));
}

CAMLprim value unpack_lb_initstorage(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value issue_lb_insertstring(value sender, value index, value string)
{
    CAMLparam3(sender, index, string);
    char* s;
    CAMLlocal2(contents, result);

    s = new_string(string);
    contents = pack_message(LB_INSERTSTRING, Int_val(index), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    CAMLreturn(Val_int(check_list_box_result(Int32_val(result))));
}

CAMLprim value pack_lb_insertstring_data(value index, value data)
{
    return pack_message(LB_INSERTSTRING, Int_val(index), Int32_val(data));
}

CAMLprim value unpack_lb_insertstring_data(value result)
{
    return Val_int(check_list_box_result(Int32_val(result)));
}

CAMLprim value pack_lb_itemfrompoint(value point)
{
    return pack_message(LB_ITEMFROMPOINT, 0, MAKELPARAM(X(point), Y(point)));
}

CAMLprim value unpack_lb_itemfrompoint(value result)
{
    CAMLparam1(result);
    DWORD res;
    CAMLlocal1(val);

    res = Int32_val(result);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(LOWORD(res)));
    Store_field(val, 1, Val_bool(HIWORD(res)));
    CAMLreturn(val);
}

CAMLprim value issue_lb_selectstring(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(LB_SELECTSTRING, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == LB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_lb_selectstring_data(value start, value data)
{
    return pack_message(LB_SELECTSTRING, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_lb_selectstring_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == LB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value pack_lb_selitemrange(value first, value last, value select)
{
    return pack_message(LB_SELITEMRANGE, Bool_val(select), MAKELPARAM(Int_val(first), Int_val(last)));
}

CAMLprim value unpack_lb_selitemrange(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_selitemrangeex(value first, value last, value select)
{
    return pack_message(LB_SELITEMRANGEEX, Int_val(first), Int_val(last));
}

CAMLprim value unpack_lb_selitemrangeex(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setanchorindex(value index)
{
    return pack_message(LB_SETANCHORINDEX, Int_val(index), 0);
}

CAMLprim value unpack_lb_setanchorindex(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setcaretindex(value index, value partial_scroll)
{
    return pack_message(LB_SETCARETINDEX, Int_val(index), MAKELPARAM(Bool_val(partial_scroll), 0));
}

CAMLprim value unpack_lb_setcaretindex(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setcolumnwidth(value width)
{
    return pack_message(LB_SETCOLUMNWIDTH, Int_val(width), 0);
}

CAMLprim value pack_lb_setcount(value count)
{
    return pack_message(LB_SETCOUNT, Int_val(count), 0);
}

CAMLprim value unpack_lb_setcount(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setcursel(value index)
{
    return pack_message(LB_SETCURSEL, Int_val(index), 0);
}

CAMLprim value pack_lb_sethorizontalextent(value extent)
{
    return pack_message(LB_SETHORIZONTALEXTENT, Int_val(extent), 0);
}

CAMLprim value pack_lb_setitemdata(value index, value data)
{
    return pack_message(LB_SETITEMDATA, Int_val(index), Int32_val(data));
}

CAMLprim value unpack_lb_setitemdata(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setitemheight(value index, value height)
{
    return pack_message(LB_SETITEMHEIGHT, Int_val(index), MAKELPARAM(Int_val(height), 0));
}

CAMLprim value unpack_lb_setitemheight(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_lb_setsel(value index, value select)
{
    return pack_message(LB_SETSEL, Bool_val(select), Int_val(index));
}

CAMLprim value unpack_lb_setsel(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value issue_lb_settabstops(value sender, value stop_list)
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
    contents = pack_message(LB_SETTABSTOPS, num_stops, (LPARAM)stops);
    result = callback(sender, contents);
    if (stops)
        free(stops);
    CAMLreturn(Val_bool(Int32_val(result)));
}

CAMLprim value pack_lb_settopindex(value index)
{
    return pack_message(LB_SETTOPINDEX, Int_val(index), 0);
}

CAMLprim value unpack_lb_settopindex(value result)
{
    check_list_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value deliver_wm_deleteitem(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    DELETEITEMSTRUCT* delis;
    CAMLlocal3(val, v, result);

    delis = (DELETEITEMSTRUCT*)Lparam(msg);
    val = alloc_tuple(5);
    Store_field(val, 0, Val_constant(delis->CtlType, control_types));
    Store_field(val, 1, Val_int(delis->CtlID));
    Store_field(val, 2, Val_int(delis->itemID));
    v = alloc_handle(delis->hwndItem);
    Store_field(val, 3, v);
    v = copy_int32(delis->itemData);
    Store_field(val, 4, v);
    result = callback3(handler, wnd, msg, val);
    CAMLreturn(result);
}
