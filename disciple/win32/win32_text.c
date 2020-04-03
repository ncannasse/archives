/* OCaml-Win32
 * win32_text.c
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
/* Graphics and Multimedia Services: GDI: Fonts and Text */

CAMLprim value add_font_resource(value filename)
{
    CAMLparam1(filename);
    char* fn;
    int result;

    fn = new_string(filename);
    enter_blocking_section();
    result = AddFontResource(fn);
    leave_blocking_section();
    free_string(fn);
    if (result == 0)
        raise_last_error();
    CAMLreturn(Val_int(result));
}

CAMLprim value create_font(value logfont)
{
    LOGFONT lf;
    HANDLE result;

    logfont_val(logfont, &lf);
    result = CreateFontIndirect(&lf);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_scalable_font_resource(value hidden, value font_res, value font_file, value current_path)
{
    CAMLparam4(hidden, font_res, font_file, current_path);
    char* res;
    char* file;
    char* path;
    BOOL result;

    res = new_string(font_res);
    file = new_string(font_file);
    path = new_string(current_path);
    enter_blocking_section();
    result = CreateScalableFontResource(Bool_val(hidden), res, file, path);
    leave_blocking_section();
    free_string(res);
    free_string(file);
    free_string(path);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

static DWORD draw_text_options_noparam[] = {
    DT_BOTTOM,
    DT_CALCRECT,
    DT_CENTER,
    DT_EDITCONTROL,
    DT_END_ELLIPSIS,
    DT_EXPANDTABS,
    DT_EXTERNALLEADING,
    DT_INTERNAL,
    DT_LEFT,
    DT_MODIFYSTRING,
    DT_NOCLIP,
    DT_NOPREFIX,
    DT_PATH_ELLIPSIS,
    DT_RIGHT,
    DT_RTLREADING,
    DT_SINGLELINE,
    DT_TOP,
    DT_VCENTER,
    DT_WORDBREAK,
    DT_WORD_ELLIPSIS
};
Define_static_constants(draw_text_options_noparam);

CAMLprim value draw_text(value hdc, value text, value rect, value options)
{
    CAMLparam4(hdc, text, rect, options);
    char* buf;
    int len;
    char* s;
    RECT r;
    DWORD flags;
    DRAWTEXTPARAMS dtp;
    CAMLlocal3(option, string, val);
    int result;

    text_val(text, &buf, &len);
    s = malloc(len + 5);
    memcpy(s, buf, len);
    s[len] = '\0';
    rect_val(rect, &r);
    flags = 0;
    memset(&dtp, 0, sizeof(dtp));
    dtp.cbSize = sizeof(dtp);
    for (; !Is_nil(options); options = Tail(options)) {
        option = Head(options);
        if (Is_long(option))
            flags |= Constant_val(option, draw_text_options_noparam);
        else if (Tag_val(option) == 0) {
            dtp.iLeftMargin = Int_val(Field(option, 0));
            dtp.iRightMargin = Int_val(Field(option, 1));
        } else {
            flags |= DT_TABSTOP;
            dtp.iTabLength = Int_val(Field(option, 0));
        }
    }

    result = DrawTextEx(Handle_val(hdc), s, len, &r, flags, &dtp);
    if (result == 0) {
        free(s);
        raise_last_error();
    }
    string = copy_string(s);
    free(s);

    val = alloc_tuple(4);
    Store_field(val, 0, Val_int(result));
    Store_field(val, 1, string);
    if (flags & DT_CALCRECT)
        rect = alloc_rect(&r);
    Store_field(val, 2, rect);
    Store_field(val, 3, Val_int(dtp.uiLengthDrawn));
    CAMLreturn(val);
}

static int CALLBACK enum_font_fam_ex_proc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
    CAMLparam0();
    CAMLlocal5(func, logfont, textmetric, info, v);
    CAMLlocal1(result);

    func = *(value*)lParam;
    logfont = alloc_logfont(&lpelfe->elfLogFont);
    info = alloc_tuple(5);
    v = copy_string(lpelfe->elfFullName);
    Store_field(info, 0, v);
    v = copy_string(lpelfe->elfStyle);
    Store_field(info, 1, v);
    v = copy_string(lpelfe->elfScript);
    Store_field(info, 2, v);
    v = Alloc_flags(FontType, font_types);
    Store_field(info, 3, v);
    if (FontType & TRUETYPE_FONTTYPE) {
        v = alloc_newtextmetric(&lpntme->ntmTm);
        textmetric = Field(v, 0);
        v = alloc_some(v);
        Store_field(info, 4, v);
    } else {
        textmetric = alloc_textmetric((TEXTMETRIC*)lpntme);
        Store_field(info, 4, Val_none);
    }
    result = callback3(func, logfont, textmetric, info);
    CAMLreturn(Bool_val(result));
}

CAMLprim value enum_font_families(value hdc, value logfont, value func)
{
    CAMLparam3(hdc, logfont, func);
    LOGFONT lf;
    value* p;

    logfont_val(logfont, &lf);
    p = malloc(sizeof(value));
    register_global_root(p);
    *p = func;
    EnumFontFamiliesEx(Handle_val(hdc), &lf, (FONTENUMPROC)enum_font_fam_ex_proc, (LPARAM)p, 0);
    remove_global_root(p);
    free(p);
    CAMLreturn(Val_unit);
}

CAMLprim value ext_text_out_native(value hdc, value point, value options, value rect_option, value text, value dx_option)
{
    RECT r;
    RECT* rp;
    char* buf;
    int len;
    value dx_array;
    INT* dx;
    int i;
    BOOL result;

    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rp = &r;
    } else
        rp = NULL;
    text_val(text, &buf, &len);
    if (Is_some(dx_option)) {
        dx_array = Value_of(dx_option);
        dx = malloc((len-1) * sizeof(INT));
        for (i = 0; i < len - 1; ++i)
            dx[i] = Int_val(Field(dx_array, i));
    } else
        dx = NULL;
    result = ExtTextOut(
        Handle_val(hdc),
        X(point), Y(point),
        flags_val(options, ext_text_out_options),
        &r,
        buf,
        len,
        dx);
    if (dx)
        free(dx);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value ext_text_out_bytecode(value* argv, int argn)
{
    return ext_text_out_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value get_aspect_ratio_filter(value hdc)
{
    SIZE s;

    if (!GetAspectRatioFilterEx(Handle_val(hdc), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value get_char_abc_widths(value hdc, value first, value last)
{
    CAMLparam3(hdc, first, last);
    int len, i;
    ABC* abc;
    CAMLlocal2(val, v);

    len = last - first + 1;
    abc = malloc(len * sizeof(ABC));
    if (!GetCharABCWidths(Handle_val(hdc), Int_val(first), Int_val(last), abc)) {
        free(abc);
        raise_last_error();
    }
    val = alloc_tuple(len);
    for (i = 0; i < len; ++i) {
        v = alloc_abc(&abc[i]);
        Store_field(val, i, v);
    }
    free(abc);
    CAMLreturn(val);
}

CAMLprim value get_char_abc_widths_float(value hdc, value first, value last)
{
    CAMLparam3(hdc, first, last);
    int len, i;
    ABCFLOAT* abcf;
    CAMLlocal2(val, v);

    len = last - first + 1;
    abcf = malloc(len * sizeof(ABCFLOAT));
    if (!GetCharABCWidthsFloat(Handle_val(hdc), Int_val(first), Int_val(last), abcf)) {
        free(abcf);
        raise_last_error();
    }
    val = alloc_tuple(len);
    for (i = 0; i < len; ++i) {
        v = alloc_abcfloat(&abcf[i]);
        Store_field(val, i, v);
    }
    free(abcf);
    CAMLreturn(val);
}

CAMLprim value get_char_width(value hdc, value first, value last)
{
    CAMLparam3(hdc, first, last);
    int len, i;
    INT* widths;
    CAMLlocal1(val);

    len = last - first + 1;
    widths = malloc(len * sizeof(INT));
    if (!GetCharWidth32(Handle_val(hdc), Int_val(first), Int_val(last), widths)) {
        free(widths);
        raise_last_error();
    }
    val = alloc_tuple(len);
    for (i = 0; i < len; ++i)
        Store_field(val, i, Val_int(widths[i]));
    free(widths);
    CAMLreturn(val);
}

CAMLprim value get_char_width_float(value hdc, value first, value last)
{
    CAMLparam3(hdc, first, last);
    int len, i;
    FLOAT* widths;
    CAMLlocal1(val);

    len = last - first + 1;
    widths = malloc(len * sizeof(FLOAT));
    if (!GetCharWidthFloat(Handle_val(hdc), Int_val(first), Int_val(last), widths)) {
        free(widths);
        raise_last_error();
    }
    val = alloc(len * Double_wosize, Double_array_tag);
    for (i = 0; i < len; ++i)
        Store_double_field(val, i, widths[i]);
    free(widths);
    CAMLreturn(val);
}

CAMLprim value get_font_data(value hdc, value table, value offset)
{
    CAMLparam3(hdc, table, offset);
    DWORD len;
    char* buf;
    CAMLlocal1(val);

    len = GetFontData(Handle_val(hdc), Int_val(table), Int_val(offset), NULL, 0);
    if (len == GDI_ERROR)
        raise_last_error();
    buf = malloc(len);
    GetFontData(Handle_val(hdc), Int_val(table), Int_val(offset), buf, len);
    val = alloc_string(len);
    memcpy(String_val(val), buf, len);
    CAMLreturn(val);
}

CAMLprim value get_kerning_pairs(value hdc)
{
    CAMLparam1(hdc);
    DWORD num, i;
    KERNINGPAIR* kp;
    CAMLlocal2(val, v);

    num = GetKerningPairs(Handle_val(hdc), 0, NULL);
    if (num != 0) {
        kp = malloc(num * sizeof(KERNINGPAIR));
        GetKerningPairs(Handle_val(hdc), num, kp);
        val = alloc_tuple(num);
        for (i = 0; i < num; ++i) {
            v = alloc_kerningpair(&kp[i]);
            Store_field(val, i, v);
        }
        free(kp);
    } else
        val = Atom(0);
    CAMLreturn(val);
}

CAMLprim value get_tabbed_text_extent(value hdc, value text, value stops)
{
    char* buf;
    int len;
    int num, i;
    INT* st;
    DWORD result;
    SIZE s;

    text_val(text, &buf, &len);
    num = Wosize_val(stops);
    if (num != 0) {
        st = malloc(num * sizeof(INT));
        for (i = 0; i < num; ++i)
            st[i] = Int_val(Field(stops, i));
    } else
        st = NULL;
    result = GetTabbedTextExtent(Handle_val(hdc), buf, len, num, st);
    if (st)
        free(st);
    if (result == 0)
        raise_last_error();
    s.cx = LOWORD(result);
    s.cy = HIWORD(result);
    return alloc_size(s);
}

CAMLprim value get_text_align(value hdc)
{
    UINT result;

    result = GetTextAlign(Handle_val(hdc));
    if (result == GDI_ERROR)
        raise_last_error();
    return alloc_textalignment(result);
}

CAMLprim value get_text_character_extra(value hdc)
{
    int result;

    result = GetTextCharacterExtra(Handle_val(hdc));
    if (result == 0x80000000)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_text_color(value hdc)
{
    COLORREF result;

    result = GetTextColor(Handle_val(hdc));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value get_text_extent_ex_point(value hdc, value text, value max_extent)
{
    CAMLparam3(hdc, text, max_extent);
    char* buf;
    int len;
    INT fit, i;
    INT* dx;
    SIZE s;
    BOOL result;
    CAMLlocal2(val, v);

    text_val(text, &buf, &len);
    dx = malloc(len * sizeof(INT));
    result = GetTextExtentExPoint(
        Handle_val(hdc),
        buf,
        len,
        Int_val(max_extent),
        &fit,
        dx,
        &s);
    if (!result) {
        free(dx);
        raise_last_error();
    }
    val = alloc_tuple(2);
    v = alloc_size(s);
    Store_field(val, 0, v);
    if (fit != 0) {
        v = alloc_tuple(fit);
        for (i = 0; i < fit; ++i)
            Store_field(v, i, Val_int(dx[i]));
    } else
        v = Atom(0);
    Store_field(val, 1, v);
    free(dx);
    CAMLreturn(val);
}

CAMLprim value get_text_extent_point(value hdc, value text)
{
    char* buf;
    int len;
    SIZE s;

    text_val(text, &buf, &len);
    if (!GetTextExtentPoint32(Handle_val(hdc), buf, len, &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value get_text_face(value hdc)
{
    CAMLparam1(hdc);
    int len;
    char* s;
    CAMLlocal1(val);

    len = GetTextFace(Handle_val(hdc), 0, NULL);
    if (len == 0)
        raise_last_error();
    s = malloc(len);
    GetTextFace(Handle_val(hdc), len, s);
    val = copy_string(s);
    free(s);
    CAMLreturn(val);
}

CAMLprim value get_text_metrics(value hdc)
{
    TEXTMETRIC tm;

    if (!GetTextMetrics(Handle_val(hdc), &tm))
        raise_last_error();
    return alloc_textmetric(&tm);
}

CAMLprim value remove_font_resource(value string)
{
    CAMLparam1(string);
    char* s;
    BOOL result;

    s = new_string(string);
    enter_blocking_section();
    result = RemoveFontResource(s);
    leave_blocking_section();
    free_string(s);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_mapper_flags(value hdc, value flag)
{
    DWORD result;

    result = SetMapperFlags(Handle_val(hdc), Bool_val(flag));
    if (result == GDI_ERROR)
        raise_last_error();
    return Val_bool(result);
}

CAMLprim value set_text_align(value hdc, value alignment)
{
    UINT result;

    result = SetTextAlign(Handle_val(hdc), textalignment_val(alignment));
    if (result == GDI_ERROR)
        raise_last_error();
    return alloc_textalignment(result);
}

CAMLprim value set_text_character_extra(value hdc, value extra)
{
    int result;

    result = SetTextCharacterExtra(Handle_val(hdc), Int_val(extra));
    if (result == 0x80000000)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value set_text_color(value hdc, value color)
{
    COLORREF result;

    result = SetTextColor(Handle_val(hdc), color_val(color));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value set_text_justification(value hdc, value extra, value count)
{
    if (!SetTextJustification(Handle_val(hdc), Int_val(extra), Int_val(count)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value tabbed_text_out(value hdc, value point, value text, value stops, value tab_origin)
{
    char* buf;
    int len;
    int num, i;
    INT* st;
    DWORD result;
    SIZE s;

    text_val(text, &buf, &len);
    num = Wosize_val(stops);
    if (num != 0) {
        st = malloc(num * sizeof(INT));
        for (i = 0; i < num; ++i)
            st[i] = Int_val(Field(stops, i));
    } else
        st = NULL;
    result = TabbedTextOut(
        Handle_val(hdc),
        X(point), Y(point),
        buf,
        len,
        num,
        st,
        Int_val(tab_origin));
    if (st)
        free(st);
    if (result == 0)
        raise_last_error();
    s.cx = LOWORD(result);
    s.cy = HIWORD(result);
    return alloc_size(s);
}

CAMLprim value text_out(value hdc, value point, value text)
{
    char* buf;
    int len;

    text_val(text, &buf, &len);
    if (!TextOut(Handle_val(hdc), X(point), Y(point), buf, len))
        raise_last_error();
    return Val_unit;
}
