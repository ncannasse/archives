/* OCaml-Win32
 * win32_message.c
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
/* User Interface Services: Windowing: Messages and Message Queues */

CAMLprim value broadcast_system_message(value options, value recipients, value contents)
{
    CAMLparam3(options, recipients, contents);
    DWORD opt;
    DWORD recipient_flags;
    WPARAM wparam;
    LPARAM lparam;
    long result;
    CAMLlocal2(val, v);

    opt = flags_val(options, broadcast_system_message_options);
    recipient_flags = flags_val(recipients, broadcast_system_message_recipients);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = BroadcastSystemMessage(
        opt,
        &recipient_flags,
        Int_val(Field(contents, 0)),
        wparam,
        lparam);
    leave_blocking_section();
    if (result == -1)
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_bool(result > 0));
    v = Alloc_flags(recipient_flags, broadcast_system_message_recipients);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value dispatch_message(value msg)
{
    CAMLparam1(msg);
    MSG m;
    LONG result;
    CAMLlocal1(val);

    msg_val(msg, &m);
    enter_blocking_section();
    result = DispatchMessage(&m);
    leave_blocking_section();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value get_input_state(value unit)
{
    return Val_bool(GetInputState());
}

CAMLprim value get_message(value hwnd_option, value msg_filter_option)
{
    CAMLparam2(hwnd_option, msg_filter_option);
    HWND hwnd;
    int msg_filter_min, msg_filter_max;
    MSG msg;
    BOOL result;
    CAMLlocal2(val, v);

    if (Is_some(hwnd_option))
        hwnd = Handle_val(Value_of(hwnd_option));
    else
        hwnd = NULL;
    if (Is_some(msg_filter_option)) {
        msg_filter_min = Int_val(Field(Value_of(msg_filter_option), 0));
        msg_filter_max = Int_val(Field(Value_of(msg_filter_option), 1));
    } else
        msg_filter_min = msg_filter_max = 0;
    enter_blocking_section();
    result = GetMessage(&msg, hwnd, msg_filter_min, msg_filter_max);
    leave_blocking_section();
    if (result == -1)
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_bool(result));
    v = alloc_msg(&msg);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value get_message_extra_info(value unit)
{
    return copy_int32(GetMessageExtraInfo());
}

CAMLprim value get_message_pos(value unit)
{
    DWORD result;
    POINT pt;

    result = GetMessagePos();
    pt.x = GET_X_LPARAM(result);
    pt.y = GET_Y_LPARAM(result);
    return alloc_point(pt);
}

CAMLprim value get_message_time(value unit)
{
    return copy_int32(GetMessageTime());
}

CAMLprim value get_queue_status(value options)
{
    CAMLparam1(options);
    DWORD result;
    CAMLlocal2(val, v);

    result = GetQueueStatus(flags_val(options, queue_status_options));
    val = alloc_tuple(2);
    v = Alloc_flags(HIWORD(result), queue_status_options);
    Store_field(val, 0, v);
    v = Alloc_flags(LOWORD(result), queue_status_options);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value in_send_message(value unit)
{
    return Val_bool(InSendMessage());
}

CAMLprim value peek_message(value hwnd_option, value msg_filter_option, value options)
{
    CAMLparam3(hwnd_option, msg_filter_option, options);
    HWND hwnd;
    int msg_filter_min, msg_filter_max;
    DWORD flags;
    MSG msg;
    BOOL result;
    CAMLlocal1(val);

    if (Is_some(hwnd_option)) {
        hwnd = Handle_val(Value_of(hwnd_option));
        if (hwnd == NULL)
            hwnd = (HWND)-1;
    } else
        hwnd = NULL;
    if (Is_some(msg_filter_option)) {
        msg_filter_min = Int_val(Field(Value_of(msg_filter_option), 0));
        msg_filter_max = Int_val(Field(Value_of(msg_filter_option), 1));
    } else
        msg_filter_min = msg_filter_max = 0;
    flags = flags_val(options, peek_message_options);
    enter_blocking_section();
    result = PeekMessage(&msg, hwnd, msg_filter_min, msg_filter_max, flags);
    leave_blocking_section();
    if (result)
        val = alloc_some(alloc_msg(&msg));
    else
        val = Val_none;
    CAMLreturn(val);
}

CAMLprim value post_message(value wnd, value contents)
{
    BOOL result;

    result = PostMessage(
        Handle_val(wnd),
        Int_val(Field(contents, 0)),
        Int32_val(Field(contents, 1)),
        Int32_val(Field(contents, 2)));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value post_quit_message(value exit_code)
{
    PostQuitMessage(Int32_val(exit_code));
    return Val_unit;
}

CAMLprim value post_thread_message(value thread, value contents)
{
    BOOL result;

    result = PostThreadMessage(
        (DWORD)Int32_val(thread),
        Int_val(Field(contents, 0)),
        Int32_val(Field(contents, 1)),
        Int32_val(Field(contents, 2)));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value register_window_message(value string)
{
    UINT result;

    result = RegisterWindowMessage(String_val(string));
    if (!result)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value reply_message(value result)
{
    return Val_bool(ReplyMessage(Int32_val(result)));
}

CAMLprim value send_message(value wnd, value contents)
{
    CAMLparam2(wnd, contents);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = SendMessage(hwnd, Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    val = copy_int32(result);
    CAMLreturn(val);
}

static VOID send_async_proc_impl(HWND hwnd, UINT uMsg, DWORD dwData, LRESULT lResult)
{
    CAMLparam0();
    value* p;
    CAMLlocal3(func, h, res);

    p = (value*)dwData;
    func = *p;
    h = alloc_handle(hwnd);
    res = copy_int32(lResult);
    callback3(func, h, Val_int(uMsg), res);
    remove_global_root(p);
    free(p);
    CAMLreturn0;
}

static VOID CALLBACK send_async_proc(HWND hwnd, UINT uMsg, DWORD dwData, LRESULT lResult)
{
    leave_blocking_section();
    send_async_proc_impl(hwnd, uMsg, dwData, lResult);
    enter_blocking_section();
}

CAMLprim value send_message_callback(value wnd, value contents, value callback_func)
{
    CAMLparam3(wnd, contents, callback_func);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    value* p;
    BOOL result;

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    p = malloc(sizeof(value));
    register_global_root(p);
    *p = callback_func;
    enter_blocking_section();
    result = SendMessageCallback(
        hwnd,
        Int_val(Field(contents, 0)),
        wparam,
        lparam,
        send_async_proc,
        (DWORD)p);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value send_message_timeout(value wnd, value contents, value options, value timeout)
{
    CAMLparam4(wnd, contents, options, timeout);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    UINT opt;
    DWORD result;
    LRESULT succeeded;
    DWORD err;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    opt = flags_val(options, send_message_timeout_options);
    enter_blocking_section();
    succeeded = SendMessageTimeout(
        hwnd,
        Int_val(Field(contents, 0)),
        wparam,
        lparam,
        opt,
        Int_val(timeout),
        &result);
    leave_blocking_section();
    if (succeeded)
        val = alloc_some(copy_int32(result));
    else {
        err = GetLastError();
        if (err != 0)
            raise_win32_error(err);
        val = Val_none;
    }
    CAMLreturn(val);
}

CAMLprim value send_notify_message(value wnd, value contents)
{
    CAMLparam2(wnd, contents);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    BOOL result;

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = SendNotifyMessage(hwnd, Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_message_extra_info(value val)
{
    return copy_int32(SetMessageExtraInfo(Int32_val(val)));
}

CAMLprim value translate_message(value msg)
{
    MSG m;

    msg_val(msg, &m);
    return Val_bool(TranslateMessage(&m));
}

CAMLprim value wait_message(value unit)
{
    CAMLparam1(unit);
    BOOL result;

    enter_blocking_section();
    result = WaitMessage();
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}
