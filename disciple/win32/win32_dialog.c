/* OCaml-Win32
 * win32_dialog.c
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
/* User Interface Services: Windowing: Dialog Boxes */

/* Dialog templates */

static DWORD dialog_style_options[] = {
    DS_3DLOOK,
    DS_ABSALIGN,
    DS_CENTER,
    DS_CENTERMOUSE,
    DS_CONTEXTHELP,
    DS_CONTROL,
    DS_FIXEDSYS,
    DS_MODALFRAME,
    DS_NOFAILCREATE,
    DS_NOIDLEMSG,
    DS_SETFOREGROUND,
    DS_SYSMODAL
};
Define_static_constants(dialog_style_options);

CAMLprim value dialog_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, dialog_style_options));
}

typedef struct {
    int alloc_size;
    unsigned char* data;
    int size;
} buffer;

static buffer* new_buffer()
{
    buffer* b;

    b = (buffer*)malloc(sizeof(buffer));
    b->alloc_size = 0;
    b->data = NULL;
    b->size = 0;
    return b;
}

static void free_buffer(buffer* b)
{
    if (b->data)
        free(b->data);
    free(b);
}

static void fit(buffer* b, int amount)
{
    if (b->alloc_size < b->size + amount) {
        b->alloc_size = (b->size + amount + 1023) / 1024 * 1024;
        b->data = realloc(b->data, b->alloc_size);
    }
}

static void add_dword_alignment(buffer* b)
{
    int new_size;

    new_size = (b->size + 3) / 4 * 4;
    if (new_size != b->size) {
        memset(b->data + b->size, 0, new_size - b->size);
        b->size = new_size;
    }
}

static void add_data(buffer* b, int size, const void* data)
{
    fit(b, size);
    memcpy(b->data + b->size, data, size);
    b->size += size;
}

static void add_word(buffer* b, WORD w)
{
    fit(b, sizeof(WORD));
    *(WORD*)(b->data + b->size) = w;
    b->size += sizeof(WORD);
}

static void add_dword(buffer* b, DWORD d)
{
    fit(b, sizeof(DWORD));
    *(DWORD*)(b->data + b->size) = d;
    b->size += sizeof(DWORD);
}

static void add_string(buffer* b, const char* s)
{
    int len;

    len = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    fit(b, len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, s, -1, (LPWSTR)(b->data + b->size), len);
    b->size += len * sizeof(wchar_t);
}

static void add_name(buffer* b, LPCTSTR n)
{
    if (HIWORD((DWORD)n) != 0)
        add_string(b, n);
    else {
        add_word(b, 0xFFFF);
        add_word(b, LOWORD((DWORD)n));
    }
}

static void add_dlgitemtemplateex(buffer* b, value item)
{
    value v;
    LARGE_INTEGER li;
    WORD len;

    add_dword_alignment(b);
    // helpID
    add_dword(b, Int_val(Field(item, 0)));
    // exStyle, style
    li.QuadPart = Int64_val(Field(item, 1));
    add_dword(b, li.HighPart);
    add_dword(b, li.LowPart);
    // x, y
    v = Field(item, 2);
    add_word(b, (WORD)X(v));
    add_word(b, (WORD)Y(v));
    // cx, cy
    v = Field(item, 3);
    add_word(b, (WORD)Cx(v));
    add_word(b, (WORD)Cy(v));
    // id
    // MS docs say this is 2 bytes, but it's actually 4
    add_dword(b, Int_val(Field(item, 4)));
    // windowClass
    add_name(b, atomname_val(Field(item, 5)));
    // title
    add_name(b, resname_val(Field(item, 6)));
    // extraCount, extra
    v = Field(item, 7);
    len = (WORD)string_length(v);
    add_word(b, len);
    if (len != 0) {
        // ... Do we need DWORD alignment here?  Docs are contradictory!
        add_data(b, len, String_val(v));
    }
}

static void add_dlgtemplateex(buffer* b, value templ)
{
    value v, vv;
    LARGE_INTEGER li;

    // dlgVer
    add_word(b, (WORD)Int_val(Field(templ, 0)));
    // signature
    add_word(b, 0xFFFF);
    // helpID
    add_dword(b, Int_val(Field(templ, 1)));
    // exStyle, style
    li.QuadPart = Int64_val(Field(templ, 2));
    if (Is_some(Field(templ, 8)))
        li.LowPart |= DS_SETFONT;
    add_dword(b, li.HighPart);
    add_dword(b, li.LowPart);
    // cDlgItems
    add_word(b, (WORD)list_length(Field(templ, 9)));
    // x, y
    v = Field(templ, 3);
    add_word(b, (WORD)X(v));
    add_word(b, (WORD)Y(v));
    // cx, cy;
    v = Field(templ, 4);
    add_word(b, (WORD)Cx(v));
    add_word(b, (WORD)Cy(v));
    // menu
    v = Field(templ, 5);
    if (Is_some(v))
        add_name(b, resname_val(Value_of(v)));
    else
        add_word(b, 0);
    // windowClass
    v = Field(templ, 6);
    if (Is_some(v))
        add_name(b, atomname_val(Value_of(v)));
    else
        add_word(b, 0);
    // title
    add_string(b, String_val(Field(templ, 7)));
    // pointsize, weight, italic, charset, typeface
    v = Field(templ, 8);
    if (Is_some(v)) {
        vv = Value_of(v);
        add_word(b, (WORD)Int_val(Field(vv, 0)));
        add_word(b, (WORD)Int_val(Field(vv, 1)));
        add_word(b, MAKEWORD((BYTE)Bool_val(Field(vv, 2)), charset_val(Field(vv, 3))));
        add_string(b, String_val(Field(vv, 4)));
    }
    // items
    v = Field(templ, 9);
    for (; !Is_nil(v); v = Tail(v))
        add_dlgitemtemplateex(b, Head(v));
}

static buffer* new_dlgtemplate(value templ)
{
    buffer* b;

    b = new_buffer();
    add_dlgtemplateex(b, templ);
    return b;
}

static void free_dlgtemplate(buffer* b)
{
    free(b);
}

typedef struct {
    value raw;
    int offset;
} parser;

#define Ptr(p) ((void*)((unsigned char*)raw_val((p)->raw) + (p)->offset))

static void parse_dword_alignment(parser* p)
{
    p->offset = (p->offset + 3) / 4 * 4;
}

static WORD parse_word(parser* p)
{
    WORD w;

    w = *(WORD*)Ptr(p);
    p->offset += sizeof(WORD);
    return w;
}

static DWORD parse_dword(parser* p)
{
    DWORD d;

    d = *(DWORD*)Ptr(p);
    p->offset += sizeof(DWORD);
    return d;
}

static char* parse_new_string(parser* p)
{
    int len;
    char* s;

    len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)Ptr(p), -1, NULL, 0, NULL, NULL);
    s = malloc(len);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)Ptr(p), -1, s, len, NULL, NULL);
    while (parse_word(p) != 0)
        ;
    return s;
}

static void parse_free_string(char* s)
{
    free(s);
}

static LPTSTR parse_new_name(parser* p)
{
    if (*(WORD*)Ptr(p) == 0xFFFF) {
        parse_word(p);
        return (LPTSTR)parse_word(p);
    } else
        return parse_new_string(p);
}

static void parse_free_name(LPTSTR name)
{
    if (HIWORD((DWORD)name) != 0)
        free(name);
}

static value parse_string(parser* p)
{
    CAMLparam0();
    CAMLlocal1(val);
    char* s;

    s = parse_new_string(p);
    val = copy_string(s);
    parse_free_string(s);
    CAMLreturn(val);
}

static value parse_atomname(parser* p)
{
    CAMLparam0();
    CAMLlocal1(val);
    LPTSTR name;

    name = parse_new_name(p);
    val = alloc_atomname(name);
    parse_free_name(name);
    CAMLreturn(val);
}

static value parse_resname(parser* p)
{
    CAMLparam0();
    CAMLlocal1(val);
    LPTSTR name;

    name = parse_new_name(p);
    val = alloc_resname(name);
    parse_free_name(name);
    CAMLreturn(val);
}

static value parse_dlgitemtemplate(parser* p)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    LARGE_INTEGER li;
    POINT pt;
    SIZE siz;
    int len;

    parse_dword_alignment(p);
    val = alloc_tuple(8);
    Store_field(val, 0, Val_int(0));
    // style, dwExtendedStyle
    li.LowPart = parse_dword(p);
    li.HighPart = parse_dword(p);
    v = copy_int64(li.QuadPart);
    Store_field(val, 1, v);
    // x, y
    pt.x = parse_word(p);
    pt.y = parse_word(p);
    v = alloc_point(pt);
    Store_field(val, 2, v);
    // cx, cy
    siz.cx = parse_word(p);
    siz.cy = parse_word(p);
    v = alloc_size(siz);
    Store_field(val, 3, v);
    // id
    Store_field(val, 4, Val_int(parse_word(p)));
    // class
    v = parse_atomname(p);
    Store_field(val, 5, v);
    // title
    v = parse_resname(p);
    Store_field(val, 6, v);
    // creation data
    // ... Do we need DWORD alignment here?  Docs are contradictory!
    len = parse_word(p);
    if (len != 0) {
        p->offset -= sizeof(WORD);
        v = alloc_string(len);
        memcpy(String_val(v), Ptr(p), len);
        p->offset += len;
    } else
        v = copy_string("");
    Store_field(val, 7, v);
    CAMLreturn(val);
}

static value parse_dlgitemtemplateex(parser* p)
{
    CAMLparam0();
    CAMLlocal2(val, v);
    LARGE_INTEGER li;
    POINT pt;
    SIZE siz;
    int len;

    parse_dword_alignment(p);
    val = alloc_tuple(8);
    // helpID
    Store_field(val, 0, Val_int(parse_dword(p)));
    // exStyle, style
    li.HighPart = parse_dword(p);
    li.LowPart = parse_dword(p);
    v = copy_int64(li.QuadPart);
    Store_field(val, 1, v);
    // x, y
    pt.x = parse_word(p);
    pt.y = parse_word(p);
    v = alloc_point(pt);
    Store_field(val, 2, v);
    // cx, cy
    siz.cx = parse_word(p);
    siz.cy = parse_word(p);
    v = alloc_size(siz);
    Store_field(val, 3, v);
    // id
    // MS docs say this is 2 bytes, but it's actually 4
    Store_field(val, 4, Val_int(parse_dword(p)));
    // windowClass
    v = parse_atomname(p);
    Store_field(val, 5, v);
    // title
    v = parse_resname(p);
    Store_field(val, 6, v);
    // extraCount, extra
    len = parse_word(p);
    if (len != 0) {
        // ... Do we need DWORD alignment here?  Docs are contradictory!
        v = alloc_string(len);
        memcpy(String_val(v), Ptr(p), len);
        p->offset += len;
    } else
        v = copy_string("");
    Store_field(val, 7, v);
    CAMLreturn(val);
}

static value parse_dlgitemtemplate_list(parser* p, int count)
{
    CAMLparam0();
    CAMLlocal3(val, hd, tl);

    if (count == 0)
        val = Val_nil;
    else {
        hd = parse_dlgitemtemplate(p);
        tl = parse_dlgitemtemplate_list(p, count - 1);
        val = alloc_cons(hd, tl);
    }
    CAMLreturn(val);
}

static value parse_dlgitemtemplateex_list(parser* p, int count)
{
    CAMLparam0();
    CAMLlocal3(val, hd, tl);

    if (count == 0)
        val = Val_nil;
    else {
        hd = parse_dlgitemtemplateex(p);
        tl = parse_dlgitemtemplateex_list(p, count - 1);
        val = alloc_cons(hd, tl);
    }
    CAMLreturn(val);
}

static value parse_dlgtemplate(parser* p)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);
    LARGE_INTEGER li;
    BOOL has_font_info;
    int item_count;
    POINT pt;
    SIZE siz;

    val = alloc_tuple(10);
    Store_field(val, 0, Val_int(1));
    Store_field(val, 1, Val_int(0));
    // style, dwExtendedStyle
    li.LowPart = parse_dword(p);
    li.HighPart = parse_dword(p);
    has_font_info = (li.LowPart & DS_SETFONT) != 0;
    li.LowPart &= ~DS_SETFONT;
    v = copy_int64(li.QuadPart);
    Store_field(val, 2, v);
    // cdit
    item_count = parse_word(p);
    // x, y
    pt.x = parse_word(p);
    pt.y = parse_word(p);
    v = alloc_point(pt);
    Store_field(val, 3, v);
    // cx, cy
    siz.cx = parse_word(p);
    siz.cy = parse_word(p);
    v = alloc_size(siz);
    Store_field(val, 4, v);
    // menu
    if (*(WORD*)Ptr(p) != 0)
        v = alloc_some(parse_resname(p));
    else {
        parse_word(p);
        v = Val_none;
    }
    Store_field(val, 5, v);
    // class
    if (*(WORD*)Ptr(p) != 0)
        v = alloc_some(parse_atomname(p));
    else {
        parse_word(p);
        v = Val_none;
    }
    Store_field(val, 6, v);
    // title
    v = parse_string(p);
    Store_field(val, 7, v);
    // point size, typeface
    if (has_font_info) {
        v = alloc_tuple(5);
        Store_field(v, 0, Val_int(parse_word(p)));
        Store_field(v, 1, Val_int(FW_DONTCARE));
        Store_field(v, 2, Val_bool(FALSE));
        vv = alloc_charset(DEFAULT_CHARSET);
        Store_field(v, 3, vv);
        vv = parse_string(p);
        Store_field(v, 4, vv);
        v = alloc_some(v);
    } else
        v = Val_none;
    Store_field(val, 8, v);
    // items
    v = parse_dlgitemtemplate_list(p, item_count);
    Store_field(val, 9, v);
    CAMLreturn(val);
}

static value parse_dlgtemplateex(parser* p)
{
    CAMLparam0();
    CAMLlocal3(val, v, vv);
    LARGE_INTEGER li;
    BOOL has_font_info;
    int item_count;
    POINT pt;
    SIZE siz;
    WORD w;

    val = alloc_tuple(10);
    // dlgVer
    Store_field(val, 0, Val_int(parse_word(p)));
    // signature
    parse_word(p);
    // helpID
    Store_field(val, 1, Val_int((int)parse_dword(p)));
    // exStyle, style
    li.HighPart = parse_dword(p);
    li.LowPart = parse_dword(p);
    has_font_info = (li.LowPart & DS_SETFONT) != 0;
    li.LowPart &= ~DS_SETFONT;
    v = copy_int64(li.QuadPart);
    Store_field(val, 2, v);
    // cDlgItems
    item_count = parse_word(p);
    // x, y
    pt.x = parse_word(p);
    pt.y = parse_word(p);
    v = alloc_point(pt);
    Store_field(val, 3, v);
    // cx, cy
    siz.cx = parse_word(p);
    siz.cy = parse_word(p);
    v = alloc_size(siz);
    Store_field(val, 4, v);
    // menu
    if (*(WORD*)Ptr(p) != 0)
        v = alloc_some(parse_resname(p));
    else {
        parse_word(p);
        v = Val_none;
    }
    Store_field(val, 5, v);
    // windowClass
    if (*(WORD*)Ptr(p) != 0)
        v = alloc_some(parse_atomname(p));
    else {
        parse_word(p);
        v = Val_none;
    }
    Store_field(val, 6, v);
    // title
    v = parse_string(p);
    Store_field(val, 7, v);
    // pointsize, weight, italic, charset, typeface
    if (has_font_info) {
        v = alloc_tuple(5);
        Store_field(v, 0, Val_int(parse_word(p)));
        Store_field(v, 1, Val_int(parse_word(p)));
        w = parse_word(p);
        Store_field(v, 2, Val_bool(LOBYTE(w)));
        vv = alloc_charset(HIBYTE(w));
        Store_field(v, 3, vv);
        vv = parse_string(p);
        Store_field(v, 4, vv);
        v = alloc_some(v);
    } else
        v = Val_none;
    Store_field(val, 8, v);
    // items
    v = parse_dlgitemtemplateex_list(p, item_count);
    Store_field(val, 9, v);
    CAMLreturn(val);
}

static value alloc_dlgtemplate(value raw)
{
    parser p;
    WORD sig;

    p.raw = raw;
    p.offset = 2;
    sig = parse_word(&p);
    p.offset = 0;
    if (sig == 0xFFFF)
        return parse_dlgtemplateex(&p);
    else
        return parse_dlgtemplate(&p);
}

CAMLprim value pack_dlgtemplate(value templ)
{
    CAMLparam1(templ);
    buffer* b;
    CAMLlocal1(val);

    b = new_dlgtemplate(templ);
    val = alloc_string(b->size);
    memcpy(String_val(val), b->data, b->size);
    free_dlgtemplate(b);
    CAMLreturn(val);
}

CAMLprim value unpack_dlgtemplate(value buf)
{
    return alloc_dlgtemplate(buf);
}

/* Functions */

CAMLprim value create_dialog(value inst, value name, value parent, value proc)
{
    CAMLparam4(inst, name, parent, proc);
    HINSTANCE hinst;
    LPTSTR nam;
    HWND hparent;
    window_data* wd;
    DLGPROC dp;
    LPARAM param;
    HWND hwnd;
    CAMLlocal1(result);

    hinst = Handle_val(inst);
    nam = new_resname(name);
    hparent = Handle_val(parent);
    if (Tag_val(proc) == 0) {
        wd = new_window_data();
        wd->function = Field(proc, 0);
        dp = dialog_proc;
        param = (LPARAM)wd;
    } else {
        wd = NULL;
        dp = Pointer_val(Field(proc, 0));;
        param = 0;
    }
    enter_blocking_section();
    hwnd = CreateDialogParam(hinst, nam, hparent, dp, param);
    leave_blocking_section();
    free_resname(nam);
    if (wd != NULL)
        free_window_data(wd);
    if (hwnd == NULL)
        raise_last_error();
    result = alloc_handle(hwnd);
    CAMLreturn(result);
}

CAMLprim value create_dialog_indirect(value inst, value templ, value parent, value proc)
{
    CAMLparam4(inst, templ, parent, proc);
    HINSTANCE hinst;
    buffer* b;
    HWND hparent;
    window_data* wd;
    DLGPROC dp;
    LPARAM param;
    HWND hwnd;
    CAMLlocal1(result);

    hinst = Handle_val(inst);
    b = new_dlgtemplate(templ);
    hparent = Handle_val(parent);
    if (Tag_val(proc) == 0) {
        wd = new_window_data();
        wd->function = Field(proc, 0);
        dp = dialog_proc;
        param = (LPARAM)wd;
    } else {
        wd = NULL;
        dp = Pointer_val(Field(proc, 0));;
        param = 0;
    }
    enter_blocking_section();
    hwnd = CreateDialogIndirectParam(hinst, (LPDLGTEMPLATE)b->data, hparent, dp, param);
    leave_blocking_section();
    free_dlgtemplate(b);
    if (wd != NULL)
        free_window_data(wd);
    if (hwnd == NULL)
        raise_last_error();
    result = alloc_handle(hwnd);
    CAMLreturn(result);
}

CAMLprim value create_dialog_indirect_param(value inst, value templ, value parent, value proc, value param)
{
    CAMLparam5(inst, templ, parent, proc, param);
    HINSTANCE hinst;
    buffer* b;
    HWND hparent;
    DLGPROC prc;
    LPARAM parm;
    HWND hwnd;
    CAMLlocal1(val);

    hinst = Handle_val(inst);
    b = new_dlgtemplate(templ);
    hparent = Handle_val(parent);
    prc = Pointer_val(proc);
    parm = Int32_val(param);
    enter_blocking_section();
    hwnd = CreateDialogIndirectParam(hinst, (LPDLGTEMPLATE)b->data, hparent, prc, parm);
    leave_blocking_section();
    free_dlgtemplate(b);
    if (hwnd == NULL)
        raise_last_error();
    val = alloc_handle(hwnd);
    CAMLreturn(val);
}

CAMLprim value create_dialog_param(value inst, value name, value parent, value proc, value param)
{
    CAMLparam5(inst, name, parent, proc, param);
    HINSTANCE hinst;
    LPTSTR nam;
    HWND hparent;
    DLGPROC prc;
    LPARAM parm;
    HWND hwnd;
    CAMLlocal1(val);

    hinst = Handle_val(inst);
    nam = new_resname(name);
    hparent = Handle_val(parent);
    prc = Pointer_val(proc);
    parm = Int32_val(param);
    enter_blocking_section();
    hwnd = CreateDialogParam(hinst, nam, hparent, prc, parm);
    leave_blocking_section();
    free_resname(nam);
    if (hwnd == NULL)
        raise_last_error();
    val = alloc_handle(hwnd);
    CAMLreturn(val);
}

CAMLprim value def_dlg_proc(value wnd, value contents)
{
    CAMLparam2(wnd, contents);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT dlg_result;
    CAMLlocal1(result);

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    dlg_result = DefDlgProc(hwnd, Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    result = copy_int32(dlg_result);
    CAMLreturn(result);
}

CAMLprim value dialog_box(value inst, value name, value parent, value proc)
{
    CAMLparam4(inst, name, parent, proc);
    HINSTANCE hinst;
    LPTSTR nam;
    HWND hparent;
    window_data* wd;
    DLGPROC dp;
    LPARAM param;
    int result;

    hinst = Handle_val(inst);
    nam = new_resname(name);
    hparent = Handle_val(parent);
    if (Tag_val(proc) == 0) {
        wd = new_window_data();
        wd->function = Field(proc, 0);
        dp = dialog_proc;
        param = (LPARAM)wd;
    } else {
        wd = NULL;
        dp = Pointer_val(Field(proc, 0));;
        param = 0;
    }
    enter_blocking_section();
    result = DialogBoxParam(hinst, nam, hparent, dp, param);
    leave_blocking_section();
    free_resname(nam);
    if (wd != NULL)
        free_window_data(wd);
    if (result == -1)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value dialog_box_indirect(value inst, value templ, value parent, value proc)
{
    CAMLparam4(inst, templ, parent, proc);
    HINSTANCE hinst;
    buffer* b;
    HWND hparent;
    window_data* wd;
    DLGPROC dp;
    LPARAM param;
    int result;

    hinst = Handle_val(inst);
    b = new_dlgtemplate(templ);
    hparent = Handle_val(parent);
    if (Tag_val(proc) == 0) {
        wd = new_window_data();
        wd->function = Field(proc, 0);
        dp = dialog_proc;
        param = (LPARAM)wd;
    } else {
        wd = NULL;
        dp = Pointer_val(Field(proc, 0));;
        param = 0;
    }
    enter_blocking_section();
    result = DialogBoxIndirectParam(hinst, (LPDLGTEMPLATE)b->data, hparent, dp, param);
    leave_blocking_section();
    free_dlgtemplate(b);
    if (result == -1)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value dialog_box_indirect_param(value inst, value templ, value parent, value proc, value param)
{
    CAMLparam5(inst, templ, parent, proc, param);
    HINSTANCE hinst;
    buffer* b;
    HWND hparent;
    DLGPROC prc;
    LPARAM parm;
    int result;

    hinst = Handle_val(inst);
    b = new_dlgtemplate(templ);
    hparent = Handle_val(parent);
    prc = Pointer_val(proc);
    parm = Int32_val(param);
    enter_blocking_section();
    result = DialogBoxIndirectParam(hinst, (LPDLGTEMPLATE)b->data, hparent, prc, parm);
    leave_blocking_section();
    free_dlgtemplate(b);
    if (result == -1)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value dialog_box_param(value inst, value name, value parent, value proc, value param)
{
    CAMLparam5(inst, name, parent, proc, param);
    HINSTANCE hinst;
    LPTSTR nam;
    HWND hparent;
    DLGPROC prc;
    LPARAM parm;
    int result;

    hinst = Handle_val(inst);
    nam = new_resname(name);
    hparent = Handle_val(parent);
    prc = Pointer_val(proc);
    parm = Int32_val(param);
    enter_blocking_section();
    result = DialogBoxParam(hinst, nam, hparent, prc, parm);
    leave_blocking_section();
    free_resname(nam);
    if (result == -1)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value end_dialog(value wnd, value result)
{
    CAMLparam2(wnd, result);
    HWND hwnd;
    BOOL ok;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    ok = EndDialog(hwnd, Int_val(result));
    leave_blocking_section();
    if (!ok)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value get_dialog_base_units(value unit)
{
    LONG result;
    SIZE s;

    result = GetDialogBaseUnits();
    s.cx = LOWORD(result);
    s.cy = HIWORD(result);
    return alloc_size(s);
}

CAMLprim value get_dlg_ctrl_id(value wnd)
{
    int result;

    result = GetDlgCtrlID(Handle_val(wnd));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_dlg_item(value wnd, value id)
{
    HWND result;

    result = GetDlgItem(Handle_val(wnd), Int_val(id));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_dlg_item_int(value wnd, value id, value is_signed)
{
    CAMLparam3(wnd, id, is_signed);
    HWND hwnd;
    BOOL translated;
    UINT result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = GetDlgItemInt(hwnd, Int_val(id), &translated, Bool_val(is_signed));
    leave_blocking_section();
    if (!translated)
        raise_last_error();
    val = copy_int64((int64)result);
    CAMLreturn(val);
}

CAMLprim value get_dlg_item_text(value wnd, value id, value max_length)
{
    CAMLparam3(wnd, id, max_length);
    HWND hwnd;
    int n;
    char* s;
    CAMLlocal1(result);

    hwnd = Handle_val(wnd);
    n = Int_val(max_length) + 1;
    s = malloc(n);
    enter_blocking_section();
    GetDlgItemText(hwnd, Int_val(id), s, n);
    leave_blocking_section();
    result = copy_string(s);
    free(s);
    CAMLreturn(result);
}

CAMLprim value get_next_dlg_group_item(value dialog, value control, value previous)
{
    CAMLparam3(dialog, control, previous);
    HWND dlg, ctrl, result;
    CAMLlocal1(val);

    dlg = Handle_val(dialog);
    ctrl = Handle_val(control);
    enter_blocking_section();
    result = GetNextDlgGroupItem(dlg, ctrl, Bool_val(previous));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value get_next_dlg_tab_item(value dialog, value control, value previous)
{
    CAMLparam3(dialog, control, previous);
    HWND dlg, ctrl, result;
    CAMLlocal1(val);

    dlg = Handle_val(dialog);
    ctrl = Handle_val(control);
    enter_blocking_section();
    result = GetNextDlgTabItem(dlg, ctrl, Bool_val(previous));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value is_dialog_message(value wnd, value msg)
{
    CAMLparam2(wnd, msg);
    HWND hwnd;
    MSG m;
    BOOL result;

    hwnd = Handle_val(wnd);
    msg_val(msg, &m);
    enter_blocking_section();
    result = IsDialogMessage(hwnd, &m);
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value map_dialog_rect(value wnd, value rect)
{
    RECT r;

    rect_val(rect, &r);
    if (!MapDialogRect(Handle_val(wnd), &r))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value message_box(value wnd, value text, value caption, value type)
{
    CAMLparam4(wnd, text, caption, type);
    HWND hwnd;
    char* txt;
    char* capt;
    UINT typ;
    int result;

    hwnd = Handle_val(wnd);
    txt = new_string(text);
    capt = new_string(caption);
    typ = flags_val(type, message_box_options);
    enter_blocking_section();
    result = MessageBox(hwnd, txt, capt, typ);
    leave_blocking_section();
    free_string(txt);
    free_string(capt);
    if (result == 0)
        raise_last_error();
    CAMLreturn(Val_constant(result, standard_control_ids));
}

CAMLprim value send_dlg_item_message(value wnd, value id, value contents)
{
    CAMLparam3(wnd, id, contents);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = SendDlgItemMessage(hwnd, Int_val(id), Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value set_dlg_item_int(value wnd, value id, value val)
{
    CAMLparam3(wnd, id, val);
    HWND hwnd;
    int64 i;
    BOOL result;

    hwnd = Handle_val(wnd);
    i = Int64_val(val);
    enter_blocking_section();
    result = SetDlgItemInt(hwnd, Int_val(id), (UINT)i, i < (int64)0);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_dlg_item_text(value wnd, value id, value string)
{
    CAMLparam3(wnd, id, string);
    HWND hwnd;
    char* s;
    BOOL result;

    hwnd = Handle_val(wnd);
    s = new_string(string);
    enter_blocking_section();
    result = SetDlgItemText(hwnd, Int_val(id), s);
    leave_blocking_section();
    free_string(s);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

/* Messages */

CAMLprim value return_dm_getdefid(value result)
{
    LRESULT res;

    if (Is_some(result))
        res = MAKELONG(Int_val(Value_of(result)), DC_HASDEFID);
    else
        res = 0;
    return copy_int32(res);
}

CAMLprim value unpack_dm_getdefid(value result)
{
    LRESULT res;

    res = Int32_val(result);
    if (HIWORD(res) == DC_HASDEFID)
        return alloc_some(Val_int(LOWORD(res)));
    else
        return Val_none;
}

CAMLprim value deliver_dm_setdefid(value handler, value wnd, value msg)
{
     return callback3(handler, wnd, msg, Val_int(Wparam(msg)));
}

CAMLprim value pack_dm_setdefid(value id)
{
    return pack_message(DM_SETDEFID, Int_val(id), 0);
}

CAMLprim value deliver_wm_enteridle(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_constant(Wparam(msg), input_event_types);
    args[3] = alloc_handle((HWND)Lparam(msg));
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_getdlgcode(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    MSG* msg_ptr;
    CAMLlocal2(full_msg, result);

    msg_ptr = (LPMSG)Lparam(msg);
    if (msg_ptr != NULL)
        full_msg = alloc_some(alloc_msg(msg_ptr));
    else
        full_msg = Val_none;
    result = callback3(handler, wnd, msg, full_msg);
    CAMLreturn(result);
}

CAMLprim value return_wm_getdlgcode(value result)
{
    return copy_int32(flags_val(result, dialog_codes));
}

CAMLprim value deliver_wm_initdialog(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HWND)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}

CAMLprim value return_wm_initdialog(value result)
{
    return copy_int32(Bool_val(result));
}

CAMLprim value deliver_wm_nextdlgctl(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal3(param, val, result);

    if (LOWORD(Lparam(msg))) {
        param = alloc(1, 0);
        val = alloc_handle((HWND)Wparam(msg));
        Store_field(param, 0, val);
    } else if (Wparam(msg))
        param = Val_int(0);
    else
        param = Val_int(1);
    result = callback3(handler, wnd, msg, param);
    CAMLreturn(result);
}

CAMLprim value pack_wm_nextdlgctl(value param)
{
    if (Is_block(param))
        return pack_message(WM_NEXTDLGCTL, (WPARAM)Handle_val(Field(param, 0)), TRUE);
    else if (Int_val(param) == 0)
        return pack_message(WM_NEXTDLGCTL, 1, FALSE);
    else
        return pack_message(WM_NEXTDLGCTL, 0, FALSE);
}
