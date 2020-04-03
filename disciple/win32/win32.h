/* OCaml-Win32
 * win32.h
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

#pragma warning(disable:4146)

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
CAMLextern void enter_blocking_section(void);
CAMLextern void leave_blocking_section(void);

/********************************************************************/
/* Simple type conversions */

char* new_string(value v);
void free_string(char* s);

#define Val_none (Val_int(0))

value alloc_some(value v);

#define Is_some(v) ((v) != Val_int(0))
#define Is_none(v) ((v) == Val_int(0))
#define Value_of(v) (Field((v), 0))

char* stringoption_val(value v);
char* new_stringoption(value v);
void free_stringoption(char* s);

#define Val_nil (Val_int(0))

value alloc_cons(value hd, value tl);

#define Is_nil(v) ((v) == Val_int(0))
#define Head(v) (Field((v), 0))
#define Tail(v) (Field((v), 1))

int list_length(value list);

#define Declare_constants(name) \
    extern DWORD name[]; \
    extern int num_##name

#define Define_constants(name) \
    int num_##name = sizeof(name) / sizeof(DWORD)

#define Define_static_constants(name) \
    static int num_##name = sizeof(name) / sizeof(DWORD)

#define Constant_val(val, constants) (constants[Int_val(val)])

value val_constant_with_count(DWORD constant, int num_constants, DWORD* constants);

#define Val_constant(constant, constants) \
    (val_constant_with_count((constant), num_##constants, constants))

DWORD flags_val(value list, DWORD* constants);

value alloc_flags_with_count(DWORD constant, int num_constants, DWORD* constants);

#define Alloc_flags(constant, constants) \
    (alloc_flags_with_count((constant), num_##constants, constants))

/********************************************************************/
/* Types */

/* Constants */

Declare_constants(activate_flags);
Declare_constants(animation_types);
Declare_constants(arc_directions);
Declare_constants(background_modes);
Declare_constants(binary_types);
Declare_constants(border_options);
Declare_constants(broadcast_system_message_options);
Declare_constants(broadcast_system_message_recipients);
Declare_constants(button_states);
Declare_constants(button_state_exs);
Declare_constants(child_window_options);
Declare_constants(color_attributes);
Declare_constants(control_types);
Declare_constants(copy_image_options);
Declare_constants(create_process_options);
Declare_constants(ctrl_events);
Declare_constants(dialog_codes);
Declare_constants(dib_color_types);
Declare_constants(dlg_dir_list_options);
Declare_constants(draw_caption_options);
Declare_constants(draw_frame_control_options);
Declare_constants(draw_frame_control_types);
Declare_constants(draw_icon_options);
Declare_constants(drive_types);
Declare_constants(enable_scroll_bar_options);
Declare_constants(error_mode_options);
Declare_constants(ext_text_out_options);
Declare_constants(file_attributes);
Declare_constants(file_change_types);
Declare_constants(file_create_options);
Declare_constants(file_flags);
Declare_constants(file_share_options);
Declare_constants(file_types);
Declare_constants(flood_fill_types);
Declare_constants(font_clip_precisions);
Declare_constants(font_families);
Declare_constants(font_output_precisions);
Declare_constants(font_pitches);
Declare_constants(font_pitch_flags);
Declare_constants(font_qualities);
Declare_constants(font_types);
Declare_constants(get_dc_ex_options);
Declare_constants(get_menu_default_item_options);
Declare_constants(get_window_options);
Declare_constants(graphics_modes);
Declare_constants(handle_flags);
Declare_constants(hatch_styles);
Declare_constants(hit_test_results);
Declare_constants(hot_key_modifiers);
Declare_constants(icon_options);
Declare_constants(image_types);
Declare_constants(input_event_types);
Declare_constants(load_image_options);
Declare_constants(map_modes);
Declare_constants(menu_item_enable_states);
Declare_constants(menu_item_enable_states_new);
Declare_constants(menu_item_options);
Declare_constants(menu_item_specifier_types);
Declare_constants(message_box_options);
Declare_constants(mouse_activate_options);
Declare_constants(mouse_modifiers);
Declare_constants(move_file_options);
Declare_constants(msg_wait_options);
Declare_constants(object_types);
Declare_constants(peek_message_options);
Declare_constants(pen_endcap_styles);
Declare_constants(pen_join_styles);
Declare_constants(pen_styles);
Declare_constants(poly_fill_modes);
Declare_constants(print_options);
Declare_constants(priority_classes);
Declare_constants(queue_status_options);
Declare_constants(redraw_window_options);
Declare_constants(region_complexities);
Declare_constants(region_modes);
Declare_constants(scroll_bar_types);
Declare_constants(scroll_window_options);
Declare_constants(seek_methods);
Declare_constants(send_message_timeout_options);
Declare_constants(set_window_placement_options);
Declare_constants(show_window_options);
Declare_constants(show_window_reasons);
Declare_constants(size_types);
Declare_constants(sizing_sides);
Declare_constants(sqos_options);
Declare_constants(startupinfo_options);
Declare_constants(std_handles);
Declare_constants(stock_objects);
Declare_constants(stretch_blt_modes);
Declare_constants(sys_colors);
Declare_constants(system_palette_uses);
Declare_constants(thread_priorities);
Declare_constants(tile_options);
Declare_constants(track_popup_menu_options);

/* Raw memory */

#define Pointer_val(v) (*((PVOID*)(v)))
value alloc_pointer(PVOID h);

PVOID raw_val(value val);
value alloc_raw(PVOID addr);

void sizedraw_val(value val, PVOID* addr, int* size);
BOOL is_sizedraw_fixed(value val);
value alloc_sizedraw(PVOID addr, int size);

/* Handles */

#define Handle_val(v) (*((HANDLE*)(v)))
value alloc_handle(HANDLE h);

/* Access control */

Declare_constants(access_rights);

void securityattributes_val(value val, SECURITY_ATTRIBUTES* sa);
value alloc_securityattributes(SECURITY_ATTRIBUTES* sa);

/* Time */

void filetime_val(value val, FILETIME* ft);
value alloc_filetime(FILETIME* ft);

void systemtime_val(value val, SYSTEMTIME* st);
value alloc_systemtime(SYSTEMTIME* st);

DWORD delay_val(value val);

value alloc_waitresult(DWORD result, int count);

/* Files */

value alloc_finddata(WIN32_FIND_DATA* fd);

value alloc_byhandlefileinformation(BY_HANDLE_FILE_INFORMATION* bhfi);

/* Overlapped I/O */

#define Overlappedptr_val(v) (*(OVERLAPPED**)Data_custom_val(v))

/* Points and rectangles */

#define X(val) (Int_val(Field((val), 0)))
#define Y(val) (Int_val(Field((val), 1)))
POINT point_val(value val);
value alloc_point(POINT point);

#define Left(val) (Int_val(Field((val), 0)))
#define Top(val) (Int_val(Field((val), 1)))
#define Right(val) (Int_val(Field((val), 2)))
#define Bottom(val) (Int_val(Field((val), 3)))
void rect_val(value val, RECT* rect);
value alloc_rect(RECT* rect);

#define Cx(val) (Int_val(Field((val), 0)))
#define Cy(val) (Int_val(Field((val), 1)))
SIZE size_val(value val);
value alloc_size(SIZE size);

void xform_val(value val, XFORM* xform);
value alloc_xform(XFORM* xform);

/* Regions */

int rgndata_size(value val);
void rgndata_val(value val, RGNDATA* data);
value alloc_rgndata(RGNDATA* data);

/* Names encoded in integers */

LPCTSTR atomname_val(value val);
LPTSTR new_atomname(value val);
void free_atomname(LPTSTR string);
value alloc_atomname(LPCTSTR string);

LPCTSTR restype_val(value val);
LPTSTR new_restype(value val);
void free_restype(LPTSTR string);
value alloc_restype(LPCTSTR string);

LPCTSTR resname_val(value val);
LPTSTR new_resname(value val);
void free_resname(LPTSTR string);
value alloc_resname(LPCTSTR string);

/* Control ids */

Declare_constants(standard_control_ids);

/* Window creation */

CREATESTRUCT* new_createstruct(value val);
void free_createstruct(CREATESTRUCT* cs);
value alloc_createstruct(CREATESTRUCT* cs);

/* Window positioning */

void windowpos_val(value val, WINDOWPOS* wp);
value alloc_windowpos(WINDOWPOS* wp);

void windowplacement_val(value val, WINDOWPLACEMENT* wpl, value options);
value alloc_windowplacement(WINDOWPLACEMENT* wpl);

void minmaxinfo_val(value val, MINMAXINFO* mmi);
value alloc_minmaxinfo(MINMAXINFO* mmi);

/* Colors */

#define Rgb_val(val) \
    (RGB( \
        (BYTE)Int_val(Field((val), 0)), \
        (BYTE)Int_val(Field((val), 1)), \
        (BYTE)Int_val(Field((val), 2))))
value alloc_rgb(COLORREF cr);

COLORREF color_val(value val);
value alloc_color(COLORREF cr);

/* Raster operations */

Declare_constants(rop2s);

/* GDI objects */

void bitmap_val(value val, BITMAP* bm);
value alloc_bitmap(BITMAP* bm);

void logbrush_val(value val, LOGBRUSH* lb);
value alloc_logbrush(LOGBRUSH* lb);

void logpen_val(value val, LOGPEN* lp);
value alloc_logpen(LOGPEN* lp);

DWORD extpenstyle_val(value val);
value alloc_extpenstyle(DWORD ps);

/* Fonts */

BYTE charset_val(value val);
value alloc_charset(BYTE cs);

void logfont_val(value val, LOGFONT* lf);
value alloc_logfont(LOGFONT* lf);

value alloc_textmetric(TEXTMETRIC* tm);

value alloc_newtextmetric(NEWTEXTMETRIC* ntm);

value alloc_abc(ABC* abc);

value alloc_abcfloat(ABCFLOAT* abcf);

value alloc_kerningpair(KERNINGPAIR* kp);

/* Text */

void text_val(value val, char** buf_ptr, int* len_ptr);

DWORD textalignment_val(value val);
value alloc_textalignment(DWORD ta);

/* Painting */

void paintstruct_val(value val, PAINTSTRUCT* ps);
value alloc_paintstruct(PAINTSTRUCT* ps);

/* Edge types */

DWORD edgetype_val(value val);

/* Icons */

void iconinfo_val(value val, ICONINFO* ii);
value alloc_iconinfo(ICONINFO* ii);

/* Menus */

value alloc_getmenuiteminfo(MENUITEMINFO* mii);

void setmenuiteminfo_val(value val, MENUITEMINFO* mii);

/* Keyboard input */

BYTE virtualkeycode_val(value val);
value alloc_virtualkeycode(BYTE code);

DWORD keydata_val(value val);
value alloc_keydata(DWORD data);

/* Scroll bars */

void scrollaction_val(value val, int* code, int* param);
value alloc_scrollaction(int code, int param);

value alloc_getscrollinfo(SCROLLINFO* info);

void setscrollinfo_val(value val, SCROLLINFO* info);

/* Processes */

STARTUPINFO* new_startupinfo(value val);
void free_startupinfo(STARTUPINFO* si);
value alloc_startupinfo(STARTUPINFO* si);

typedef struct {
    LPTSTR application_name;
    LPTSTR command_line;
    LPSECURITY_ATTRIBUTES process_attributes;
    LPSECURITY_ATTRIBUTES thread_attributes;
    BOOL inherit_handles;
    DWORD creation_flags;
    LPVOID environment;
    LPTSTR current_directory;
    LPSTARTUPINFO startup_info;
} create_process_params;
create_process_params* new_create_process_params(value val);
void free_create_process_params(create_process_params* cpp);

value alloc_processinformation(PROCESS_INFORMATION* pi);
value alloc_terminationstatus(DWORD ts);

/* Interprocess communication */

void fill_copydatastruct(COPYDATASTRUCT* cds, value val);
void empty_copydatastruct(COPYDATASTRUCT* cds, value val);
value alloc_copydatastruct(COPYDATASTRUCT* cds);

/********************************************************************/
/* Message handling */

/* Messages */

#define Wparam(mc) (Int32_val(Field((mc), 1)))
#define Lparam(mc) (Int32_val(Field((mc), 2)))

value pack_message(UINT msg, WPARAM wparam, LPARAM lparam);

void msg_val(value val, MSG* msg);
value alloc_msg(MSG* msg);

/********************************************************************/
/* Window procedures */

typedef struct struct_class_data {
    struct struct_class_data* next;
    char name[256];
    value function;
} class_data;

class_data* find_class_data(const char* name);
class_data* make_class_data(const char* name);
void free_class_data(const char* name);

typedef struct {
    value function;
    value action;
} window_data;

window_data* new_window_data(void);
void free_window_data(window_data* wd);

window_data* get_window_data(HWND hwnd);
window_data* make_window_data(HWND hwnd);
void remove_window_data(HWND hwnd);

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK dialog_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/********************************************************************/
/* Errors */

void raise_win32_error(DWORD error);
void raise_last_error(void);

void raise_region_error(void);
value val_regioncomplexity(int i);

void raise_combo_box_error(value error_type);
int check_combo_box_result(int result);

void raise_list_box_error(value error_type);
int check_list_box_result(int result);
