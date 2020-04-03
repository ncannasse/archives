/* OCaml-Win32
 * win32_menu.c
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
/* User Interface Services: Resources: Menus */

CAMLprim value check_menu_item(value menu, value item, value spec_type, value check)
{
    DWORD result;

    result = CheckMenuItem(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) |
            (Bool_val(check) ? MF_CHECKED : MF_UNCHECKED));
    if (result == 0xFFFFFFFF)
        raise_last_error();
    return Val_bool(result == MF_CHECKED);
}

CAMLprim value check_menu_radio_item(value menu, value first, value last, value check, value spec_type)
{
    BOOL result;

    result = CheckMenuRadioItem(
        Handle_val(menu),
        Int_val(first),
        Int_val(last),
        Int_val(check),
        Constant_val(spec_type, menu_item_specifier_types));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value create_menu(value unit)
{
    HMENU result;

    result = CreateMenu();
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_popup_menu(value unit)
{
    HMENU result;

    result = CreatePopupMenu();
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value delete_menu(value menu, value item, value spec_type)
{
    if (!DeleteMenu(Handle_val(menu), Int_val(item), Constant_val(spec_type, menu_item_specifier_types)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value destroy_menu(value menu)
{
    if (!DestroyMenu(Handle_val(menu)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_menu_bar(value wnd)
{
    if (!DrawMenuBar(Handle_val(wnd)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value enable_menu_item(value menu, value item, value spec_type, value state)
{
    UINT result;

    result = EnableMenuItem(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) |
            Constant_val(state, menu_item_enable_states));
    if (result == 0xFFFFFFFF)
        raise_last_error();
    return Val_constant(result, menu_item_enable_states);
}

CAMLprim value get_menu(value wnd)
{
    return alloc_handle(GetMenu(Handle_val(wnd)));
}

CAMLprim value get_menu_default_item(value menu, value spec_type, value options)
{
    UINT result;

    result = GetMenuDefaultItem(
        Handle_val(menu),
        Constant_val(spec_type, menu_item_specifier_types) == MF_BYPOSITION,
        flags_val(options, get_menu_default_item_options));
    if (result == (UINT)-1)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_menu_item_count(value menu)
{
    int result;

    result = GetMenuItemCount(Handle_val(menu));
    if (result == -1)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_menu_item_id(value menu, value item)
{
    return Val_int((int)GetMenuItemID(Handle_val(menu), Int_val(item)));
}

CAMLprim value get_menu_item_info(value menu, value item, value spec_type, value max_length)
{
    CAMLparam4(menu, item, spec_type, max_length);
    int len;
    char* buf;
    MENUITEMINFO mii;
    BOOL result;
    CAMLlocal1(val);

    len = Int_val(max_length) + 1;
    buf = malloc(len);
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
    mii.dwTypeData = buf;
    mii.cch = len;
    result = GetMenuItemInfo(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) == MF_BYPOSITION,
        &mii);
    if (!result) {
        free(buf);
        raise_last_error();
    }
    val = alloc_getmenuiteminfo(&mii);
    free(buf);
    CAMLreturn(val);
}

CAMLprim value get_menu_item_rect(value wnd, value menu, value item)
{
    RECT r;

    if (!GetMenuItemRect(Handle_val(wnd), Handle_val(menu), Int_val(item), &r))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value get_sub_menu(value menu, value item)
{
    return alloc_handle(GetSubMenu(Handle_val(menu), Int_val(item)));
}

CAMLprim value get_system_menu(value wnd, value revert)
{
    return alloc_handle(GetSystemMenu(Handle_val(wnd), Bool_val(revert)));
}

CAMLprim value hilite_menu_item(value wnd, value menu, value item, value spec_type, value hilite)
{
    BOOL result;

    result = HiliteMenuItem(
        Handle_val(wnd),
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) |
            (Bool_val(hilite) ? MF_HILITE : MF_UNHILITE));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value insert_menu_item(value menu, value item, value spec_type, value info)
{
    MENUITEMINFO mii;
    BOOL result;

    setmenuiteminfo_val(info, &mii);
    result = InsertMenuItem(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) == MF_BYPOSITION,
        &mii);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value is_menu(value menu)
{
    return Val_bool(IsMenu(Handle_val(menu)));
}

CAMLprim value load_menu(value hinstance, value name)
{
    HMENU result;

    result = LoadMenu(Handle_val(hinstance), resname_val(name));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value menu_item_from_point(value wnd, value menu, value point)
{
    int result;

    result = MenuItemFromPoint(Handle_val(wnd), Handle_val(menu), point_val(point));
    if (result == -1)
        return Val_none;
    else
        return alloc_some(Val_int(result));
}

CAMLprim value remove_menu(value menu, value item, value spec_type)
{
    if (!RemoveMenu(Handle_val(menu), Int_val(item), Constant_val(spec_type, menu_item_specifier_types)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_menu(value wnd, value menu)
{
    if (!SetMenu(Handle_val(wnd), Handle_val(menu)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_menu_default_item(value menu, value item, value spec_type)
{
    BOOL result;

    result = SetMenuDefaultItem(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) == MF_BYPOSITION);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_menu_item_bitmaps(value menu, value item, value spec_type, value unchecked, value checked)
{
    BOOL result;

    result = SetMenuItemBitmaps(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types),
        Handle_val(unchecked),
        Handle_val(checked));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_menu_item_info(value menu, value item, value spec_type, value info)
{
    MENUITEMINFO mii;
    BOOL result;

    setmenuiteminfo_val(info, &mii);
    result = SetMenuItemInfo(
        Handle_val(menu),
        Int_val(item),
        Constant_val(spec_type, menu_item_specifier_types) == MF_BYPOSITION,
        &mii);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value track_popup_menu(value menu, value options, value point, value wnd, value rect_option)
{
    CAMLparam5(menu, options, point, wnd, rect_option);
    HMENU hmenu;
    UINT opt;
    POINT pt;
    HWND hwnd;
    TPMPARAMS tpmp;
    TPMPARAMS* tpmpp;
    int result;
    CAMLlocal1(val);

    hmenu = Handle_val(menu);
    opt = flags_val(options, track_popup_menu_options) | TPM_RETURNCMD;
    pt = point_val(point);
    hwnd = Handle_val(wnd);
    if (Is_some(rect_option)) {
        tpmp.cbSize = sizeof(TPMPARAMS);
        rect_val(Value_of(rect_option), &tpmp.rcExclude);
        tpmpp = &tpmp;
    } else
        tpmpp = NULL;
    enter_blocking_section();
    result = TrackPopupMenuEx(hmenu, opt, pt.x, pt.y, hwnd, tpmpp);
    leave_blocking_section();
    if (result == 0)
        val = Val_none;
    else
        val = alloc_some(Val_int(result));
    CAMLreturn(val);
}

/* Messages */

CAMLprim value deliver_wm_contextmenu(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    POINT pt;
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HWND)Wparam(msg));
    pt.x = GET_X_LPARAM(Lparam(msg));
    pt.y = GET_Y_LPARAM(Lparam(msg));
    args[3] = alloc_point(pt);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_menuloop(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_bool((BOOL)Wparam(msg)));
    CAMLreturn(result);
}

CAMLprim value deliver_wm_nextmenu(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    MDINEXTMENU* mnm;
    CAMLlocalN(args, 6);
    CAMLlocal1(result);

    mnm = (LPMDINEXTMENU)Lparam(msg);
    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_virtualkeycode((BYTE)Wparam(msg));
    args[3] = alloc_handle(mnm->hmenuIn);
    args[4] = alloc_handle(mnm->hmenuNext);
    args[5] = alloc_handle(mnm->hwndNext);
    result = callbackN(handler, 6, args);
    CAMLreturn(result);
}
