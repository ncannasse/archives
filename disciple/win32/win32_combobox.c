/* OCaml-Win32
 * win32_combobox.c
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
/* User Interface Services: Controls: Combo Boxes */

CAMLprim value dlg_dir_list_combo_box(value wnd, value path, value combo_id, value static_id, value options)
{
    CAMLparam5(wnd, path, combo_id, static_id, options);
    HWND hwnd;
    char path_buf[_MAX_PATH+1];
    DWORD opt;
    int result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    strncpy(path_buf, String_val(path), _MAX_PATH);
    path_buf[_MAX_PATH] = '\0';
    opt = flags_val(options, dlg_dir_list_options);
    enter_blocking_section();
    result = DlgDirListComboBox(hwnd, path_buf, Int_val(combo_id), Int_val(static_id), opt);
    leave_blocking_section();
    if (result == 0)
        raise_last_error();
    val = copy_string(path_buf);
    CAMLreturn(val);
}

CAMLprim value dlg_dir_select_combo_box(value wnd, value combo_id)
{
    CAMLparam2(wnd, combo_id);
    HWND hwnd;
    char path_buf[_MAX_PATH+1];
    BOOL result;
    CAMLlocal2(val, v);

    hwnd = Handle_val(wnd);
    path_buf[0] = '\0';
    enter_blocking_section();
    result = DlgDirSelectComboBoxEx(hwnd, path_buf, sizeof(path_buf), Int_val(combo_id));
    leave_blocking_section();
    val = alloc_tuple(2);
    v = copy_string(path_buf);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(result));
    CAMLreturn(val);
}

/* Styles */

static DWORD combobox_style_options[] = {
    CBS_AUTOHSCROLL,
    CBS_DISABLENOSCROLL,
    CBS_DROPDOWN,
    CBS_DROPDOWNLIST,
    CBS_HASSTRINGS,
    CBS_LOWERCASE,
    CBS_NOINTEGRALHEIGHT,
    CBS_OEMCONVERT,
    CBS_OWNERDRAWFIXED,
    CBS_OWNERDRAWVARIABLE,
    CBS_SIMPLE,
    CBS_SORT,
    CBS_UPPERCASE
};
Define_static_constants(combobox_style_options);

CAMLprim value combobox_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, combobox_style_options));
}

/* Messages */

CAMLprim value issue_cb_addstring(value sender, value string)
{
    CAMLparam2(sender, string);
    char* s;
    CAMLlocal2(contents, result);

    s = new_string(string);
    contents = pack_message(CB_ADDSTRING, 0, (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    CAMLreturn(Val_int(check_combo_box_result(Int32_val(result))));
}

CAMLprim value pack_cb_addstring_data(value data)
{
    return pack_message(CB_ADDSTRING, 0, Int32_val(data));
}

CAMLprim value unpack_cb_addstring_data(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value pack_cb_deletestring(value index)
{
    return pack_message(CB_DELETESTRING, Int_val(index), 0);
}

CAMLprim value unpack_cb_deletestring(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value issue_cb_dir(value sender, value path, value options)
{
    CAMLparam3(sender, path, options);
    char* pth;
    CAMLlocal2(contents, result);

    pth = new_string(path);
    contents = pack_message(CB_DIR, flags_val(options, dlg_dir_list_options), (LPARAM)pth);
    result = callback(sender, contents);
    free_string(pth);
    CAMLreturn(Val_int(check_combo_box_result(Int32_val(result))));
}

CAMLprim value issue_cb_findstring(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(CB_FINDSTRING, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == CB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_cb_findstring_data(value start, value data)
{
    return pack_message(CB_FINDSTRING, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_cb_findstring_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == CB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value issue_cb_findstringexact(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(CB_FINDSTRINGEXACT, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == CB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_cb_findstringexact_data(value start, value data)
{
    return pack_message(CB_FINDSTRINGEXACT, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_cb_findstringexact_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == CB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value unpack_cb_getcount(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value unpack_cb_getcursel(value result)
{
    int res;

    res = Int32_val(result);
    if (res == CB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value issue_cb_getdroppedcontrolrect(value sender)
{
    CAMLparam1(sender);
    RECT r;
    CAMLlocal3(contents, result, rect);

    contents = pack_message(CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&r);
    result = callback(sender, contents);
    if (Int32_val(result) == 0)
        raise_last_error();
    rect = alloc_rect(&r);
    CAMLreturn(rect);
}

CAMLprim value unpack_cb_getdroppedstate(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value unpack_cb_getdroppedwidth(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value issue_cb_geteditsel(value sender)
{
    CAMLparam1(sender);
    CAMLlocal2(contents, val);
    DWORD start, end;

    contents = pack_message(CB_GETEDITSEL, (WPARAM)&start, (LPARAM)&end);
    callback(sender, contents);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(start));
    Store_field(val, 1, Val_int(end));
    CAMLreturn(val);
}

CAMLprim value unpack_cb_getextendedui(value result)
{
    return Val_bool(Int32_val(result));
}

CAMLprim value unpack_cb_gethorizontalextent(value result)
{
    return Val_int(Int32_val(result));
}

CAMLprim value pack_cb_getitemdata(value index)
{
    return pack_message(CB_GETITEMDATA, Int_val(index), 0);
}

CAMLprim value unpack_cb_getitemdata(value result)
{
    return copy_int32(check_combo_box_result(Int32_val(result)));
}

CAMLprim value pack_cb_getitemheight(value index)
{
    return pack_message(CB_GETITEMHEIGHT, Int_val(index), 0);
}

CAMLprim value unpack_cb_getitemheight(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value issue_cb_getlbtext(value sender, value index, value max_length)
{
    CAMLparam3(sender, index, max_length);
    char* s;
    CAMLlocal3(contents, result, string);
    int res;

    s = malloc(Int_val(max_length) + 1);
    contents = pack_message(CB_GETLBTEXT, Int_val(index), (LPARAM)s);
    result = callback(sender, contents);
    res = Int32_val(result);
    if (res < 0) {
        free(s);
        check_combo_box_result(res);
    }
    string = copy_string(s);
    free(s);
    CAMLreturn(string);
}

CAMLprim value pack_cb_getlbtextlen(value index)
{
    return pack_message(CB_GETLBTEXTLEN, Int_val(index), 0);
}

CAMLprim value unpack_cb_getlbtextlen(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value unpack_cb_gettopindex(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value pack_cb_initstorage(value count, value size)
{
    return pack_message(CB_INITSTORAGE, Int_val(count), Int_val(size));
}

CAMLprim value unpack_cb_initstorage(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value issue_cb_insertstring(value sender, value index, value string)
{
    CAMLparam3(sender, index, string);
    char* s;
    CAMLlocal2(contents, result);

    s = new_string(string);
    contents = pack_message(CB_INSERTSTRING, Int_val(index), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    CAMLreturn(Val_int(check_combo_box_result(Int32_val(result))));
}

CAMLprim value pack_cb_insertstring_data(value index, value data)
{
    return pack_message(CB_INSERTSTRING, Int_val(index), Int32_val(data));
}

CAMLprim value unpack_cb_insertstring_data(value result)
{
    return Val_int(check_combo_box_result(Int32_val(result)));
}

CAMLprim value pack_cb_limittext(value limit)
{
    return pack_message(CB_LIMITTEXT, Int_val(limit), 0);
}

CAMLprim value issue_cb_selectstring(value sender, value start, value string)
{
    CAMLparam3(sender, start, string);
    char* s;
    CAMLlocal3(contents, result, val);
    int res;

    s = new_string(string);
    contents = pack_message(CB_SELECTSTRING, Int_val(start), (LPARAM)s);
    result = callback(sender, contents);
    free_string(s);
    res = Int32_val(result);
    if (res == CB_ERR)
        val = Val_none;
    else
        val = alloc_some(Val_int(res));
    CAMLreturn(val);
}

CAMLprim value pack_cb_selectstring_data(value start, value data)
{
    return pack_message(CB_SELECTSTRING, Int_val(start), Int32_val(data));
}

CAMLprim value unpack_cb_selectstring_data(value result)
{
    int res;

    res = Int32_val(result);
    if (res == CB_ERR)
        return Val_none;
    else
        return alloc_some(Val_int(res));
}

CAMLprim value pack_cb_setcursel(value index)
{
    return pack_message(CB_SETCURSEL, Int_val(index), 0);
}

CAMLprim value pack_cb_setdroppedwidth(value width)
{
    return pack_message(CB_SETDROPPEDWIDTH, Int_val(width), 0);
}

CAMLprim value unpack_cb_setdroppedwidth(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_seteditsel(value start_sel, value end_sel)
{
    return pack_message(CB_SETEDITSEL, 0, MAKELPARAM(Int_val(start_sel), Int_val(end_sel)));
}

CAMLprim value unpack_cb_seteditsel(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_setextendedui(value extended)
{
    return pack_message(CB_SETEXTENDEDUI, Bool_val(extended), 0);
}

CAMLprim value unpack_cb_setextendedui(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_sethorizontalextent(value extent)
{
    return pack_message(CB_SETHORIZONTALEXTENT, Int_val(extent), 0);
}

CAMLprim value pack_cb_setitemdata(value index, value data)
{
    return pack_message(CB_SETITEMDATA, Int_val(index), Int32_val(data));
}

CAMLprim value unpack_cb_setitemdata(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_setitemheight(value index, value height)
{
    return pack_message(CB_SETITEMHEIGHT, Int_val(index), Int_val(height));
}

CAMLprim value unpack_cb_setitemheight(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_settopindex(value index)
{
    return pack_message(CB_SETTOPINDEX, Int_val(index), 0);
}

CAMLprim value unpack_cb_settopindex(value result)
{
    check_combo_box_result(Int32_val(result));
    return Val_unit;
}

CAMLprim value pack_cb_showdropdown(value show)
{
    return pack_message(CB_SHOWDROPDOWN, Bool_val(show), 0);
}

CAMLprim value deliver_wm_compareitem(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    COMPAREITEMSTRUCT* cis;
    CAMLlocal3(val, v, result);

    cis = (COMPAREITEMSTRUCT*)Lparam(msg);
    val = alloc_tuple(7);
    Store_field(val, 0, Val_constant(cis->CtlType, control_types));
    Store_field(val, 1, Val_int(cis->CtlID));
    v = alloc_handle(cis->hwndItem);
    Store_field(val, 2, v);
    Store_field(val, 3, Val_int(cis->itemID1));
    v = copy_int32(cis->itemData1);
    Store_field(val, 4, v);
    Store_field(val, 5, Val_int(cis->itemID2));
    v = copy_int32(cis->itemData2);
    Store_field(val, 6, v);
    result = callback3(handler, wnd, msg, val);
    CAMLreturn(result);
}

CAMLprim value return_wm_compareitem(value result)
{
    return copy_int32(Int_val(result));
}

static DWORD owner_draw_actions[] = {
    ODA_DRAWENTIRE,
    ODA_FOCUS,
    ODA_SELECT
};
Define_static_constants(owner_draw_actions);

static DWORD owner_draw_states[] = {
    ODS_CHECKED,
    ODS_COMBOBOXEDIT,
    ODS_DEFAULT,
    ODS_DISABLED,
    ODS_FOCUS,
    ODS_GRAYED,
    ODS_SELECTED
};
Define_static_constants(owner_draw_states);

CAMLprim value deliver_wm_drawitem(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    DRAWITEMSTRUCT* dis;
    CAMLlocal3(val, v, result);

    dis = (DRAWITEMSTRUCT*)Lparam(msg);
    val = alloc_tuple(9);
    Store_field(val, 0, Val_constant(dis->CtlType, control_types));
    Store_field(val, 1, Val_int(dis->CtlID));
    Store_field(val, 2, Val_int(dis->itemID));
    v = Alloc_flags(dis->itemAction, owner_draw_actions);
    Store_field(val, 3, v);
    v = Alloc_flags(dis->itemState, owner_draw_states);
    Store_field(val, 4, v);
    v = alloc_handle(dis->hwndItem);
    Store_field(val, 5, v);
    v = alloc_handle(dis->hDC);
    Store_field(val, 6, v);
    v = alloc_rect(&dis->rcItem);
    Store_field(val, 7, v);
    v = copy_int32(dis->itemData);
    Store_field(val, 8, v);
    result = callback3(handler, wnd, msg, val);
    CAMLreturn(result);
}

CAMLprim value return_wm_drawitem(value result)
{
    return copy_int32(TRUE);
}

CAMLprim value on_wm_measureitem(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    MEASUREITEMSTRUCT* mis;
    CAMLlocal4(val, v, result, ret);

    ret = Val_none;
    if (Int_val(Field(msg, 0)) == WM_MEASUREITEM) {
        mis = (MEASUREITEMSTRUCT*)Lparam(msg);
        val = alloc_tuple(4);
        Store_field(val, 0, Val_constant(mis->CtlType, control_types));
        Store_field(val, 1, Val_int(mis->CtlID));
        Store_field(val, 2, Val_int(mis->itemID));
        v = copy_int32(mis->itemData);
        Store_field(val, 3, v);
        result = callback3(handler, wnd, msg, val);
        if (Is_block(result)) {
            if (Tag_val(result) == 0)
                ret = alloc_some(Field(result, 0));
            else {
                mis->itemWidth = Cx(result);
                mis->itemHeight = Cy(result);
                ret = alloc_some(copy_int32(TRUE));
            }
        }
    }
    CAMLreturn(ret);
}
