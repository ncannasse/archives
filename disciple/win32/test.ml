(* OCaml-Win32
 * test.ml
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
 *)

open Win32

let dlg_processors = [
    on_wm_initdialog (fun ~wnd ~msg ~focus ->
        message_return true)
    ;
    on_wm_command (fun ~wnd ~msg ~notify_code ~id ~ctrl ->
        if id == control_id_of_standard IDOK && notify_code == bn_clicked then
            end_dialog ~dlg:wnd ~result:0
        else
            ignore (message_box ~wnd ~text:("id:" ^ string_of_int id ^ " notify:" ^ string_of_int notify_code) ~caption:"WM_COMMAND" ~options:[MB_OK]);
        message_handled)
]

let window_size = ref { cx = 0; cy = 0 }
let connect_point = ref { x = 0; y = 0 }
let num_clicks = ref 0
let logfonts = ref []
let font = ref null_handle

let prop_name = An_string "Hello"

let fill_listbox listbox =
    let dc = get_dc ~wnd:listbox in
    enum_font_families ~dc ~logfont:default_logfont ~func:(fun ~logfont ~metric ~info ->
        ignore (send ~wnd:listbox ~issuer:issue_lb_addstring ~str:info.efi_full_name);
        logfonts := logfont :: !logfonts;
        true);
    ignore (release_dc ~wnd:listbox ~dc);
    logfonts := List.rev !logfonts

let subclass edit =
    let ep = get_window_proc ~wnd:edit in
    let new_ep = subclassing_window_proc ~base:ep ~processors:[
        on_wm_char (fun ~wnd ~msg ~char ~data ->
            if char = '.' then
              begin
                ignore (message_box ~wnd:null_hwnd ~text:"You typed a period." ~caption:"" ~options:[MB_OK]);
                ignore (set_focus ~wnd:edit);
                message_handled
              end
            else
                message_not_handled) ]
    in
    ignore (set_window_proc ~wnd:edit ~proc:new_ep)

let processors = [
    on_wm_create (fun ~wnd ~msg ~cs ->
        let button = create_window {
            cs_class_name = An_string "BUTTON";
            cs_window_name = "Push me!";
            cs_style = button_style_of_options ([WS_CHILD; WS_VISIBLE], [BS_PUSHBUTTON]);
            cs_position = Some { x = 10; y = 10 };
            cs_size = Some { cx = 100; cy = 30 };
            cs_parent = wnd;
            cs_menu = null_handle;
            cs_child_id = 100;
            cs_instance = the_instance;
            cs_param = null_pointer } in
        send ~wnd:button ~issuer:issue_bm_setstyle ~style:[BS_DEFPUSHBUTTON] ~redraw:false;
        let edit = create_window ~cs:{
            cs_class_name = An_string "EDIT";
            cs_window_name = "";
            cs_style = style_of_options [WS_CHILD; WS_VISIBLE; WS_EX_CLIENTEDGE];
            cs_position = Some { x = 10; y = 50 };
            cs_size = Some { cx = 200; cy = 20 };
            cs_parent = wnd;
            cs_menu = null_handle;
            cs_child_id = 101;
            cs_instance = the_instance;
            cs_param = null_pointer } in
        subclass edit;
        let listbox = create_window ~cs:{
            cs_class_name = An_string "LISTBOX";
            cs_window_name = "";
            cs_style = listbox_style_of_options ([WS_CHILD; WS_VISIBLE; WS_BORDER; WS_HSCROLL; WS_VSCROLL], [LBS_NOTIFY]);
            cs_position = Some { x = 10; y = 100 };
            cs_size = Some { cx = 200; cy = 200 };
            cs_parent = wnd;
            cs_menu = null_handle;
            cs_child_id = 102;
            cs_instance = the_instance;
            cs_param = null_pointer } in
        fill_listbox listbox;
        set_prop ~wnd ~name:prop_name ~data:(Int32.of_int 38);
        message_return true)
    ;
    on_wm_size (fun ~wnd ~msg ~typ ~size ->
        window_size := size;
        message_handled)
    ;
    on_wm_paint (fun ~wnd ~msg ~dc ->
        let ps = begin_paint ~wnd in
        let dc = ps.ps_hdc in
        ignore (move_to ~dc ~pt:{ x = 10; y = 10 });
        line_to ~dc ~pt:!connect_point;
        line_to ~dc ~pt:{ x = !window_size.cx - 10; y = !window_size.cy - 10 };
        end_paint ~wnd ~ps;
        message_handled)
    ;
    on_wm_lbuttondown (fun ~wnd ~msg ~modifiers ~pt ->
        num_clicks := !num_clicks + 1;
        connect_point := pt;
        invalidate_rect ~wnd ~rect:None ~erase:true;
        message_handled)
    ;
    on_wm_mbuttondown (fun ~wnd ~msg ~modifiers ~pt ->
        let s = send_dlg_item ~dlg:wnd ~id:101 ~issuer:issue_em_getline ~line:0 ~max_length:32 in
        ignore (message_box ~wnd ~text:s ~caption:"Edit box says..." ~options:[ MB_OK ]);
        message_handled)
    ;
    on_wm_rbuttondown (fun ~wnd ~msg ~modifiers ~pt ->
        Gc.compact ();
        begin
            try
                let s = load_string ~inst:the_instance ~id:12 ~max_length:256 in
                ignore (message_box ~wnd ~text:s ~caption:"Loaded string" ~options:[ MB_OK ])
            with Error n ->
                ignore (message_box ~wnd ~text:("Error" ^ string_of_int n) ~caption:"Failed loading string" ~options:[ MB_OK ])
        end;
        message_handled)
    ;
    on_wm_command (fun ~wnd ~msg ~notify_code ~id ~ctrl ->
        if is_null_hwnd ctrl && id == 173 then
            ignore (
                dialog_box
                    ~inst:the_instance
                    ~name:(Rn_string "AboutDlg")
                    ~parent:wnd
                    ~proc:(standard_dialog_proc ~processors:dlg_processors));
(*
          begin
            let rsrc = find_resource
                ~inst:the_instance
                ~name:(Rn_string "AboutDlg")
                ~typ:(resource_type_of_standard RT_DIALOG)
            in
            let res = load_resource ~inst:the_instance ~rsrc in
            let data = lock_resource ~rsrc:res in
            let tmpl = unpack_dlgtemplate (Raw_pointer data) in
            let tmpl' = { tmpl with dt_title = "New title for dialog" } in
            ignore (
                dialog_box_indirect
                    ~inst:the_instance
                    ~template:tmpl'
                    ~parent:wnd
                    ~proc:(standard_dialog_proc ~processors:dlg_processors))
          end;
*)
        if not (is_null_hwnd ctrl) && id == 100 && notify_code == bn_clicked then
            ignore (message_box ~wnd ~text:"Clicked!" ~caption:"" ~options:[ MB_OK ]);
        if not (is_null_hwnd ctrl) && id == 102 && notify_code == lbn_selchange then
          begin
            match send ~wnd:ctrl ~issuer:issue_lb_getcursel with
                Some sel ->
                    let lf = List.nth !logfonts sel in
                    let new_font = create_font ~logfont:lf in
                    send ~wnd:(get_dlg_item ~dlg:wnd ~id:101) ~issuer:issue_wm_setfont ~font:new_font ~redraw:true;
                    if not (is_null_handle !font) then
                        delete_object ~obj:!font;
                    font := new_font
              | None -> ()
          end;
        message_handled)
    ;
    on_wm_destroy (fun ~wnd ~msg ->
        remove_prop ~wnd ~name:prop_name;
        post_quit_message ~exit_code:(Int32.of_int !num_clicks);
        message_handled)
]

let test_class_name = An_string "OCamlTestAppClass"

let register_test_class =
    let wc = {
        wc_class_name = test_class_name;
        wc_wnd_proc = standard_window_proc ~processors;
        wc_style = [ CS_OWNDC; CS_VREDRAW; CS_HREDRAW ];
        wc_instance = the_instance;
        wc_icon = null_handle; (* ... *)
        wc_cursor = load_cursor ~inst:null_handle ~name:(resource_name_of_standard IDC_CROSS);
        wc_background = Cb_sys_color COLOR_DESKTOP;
        wc_menu_name = Some (Rn_int 102);
        wc_icon_sm = null_handle;
        wc_cls_extra = 0;
        wc_wnd_extra = 0 } in
    ignore (register_class ~wc)

let win_main () =
    register_test_class;

    let wnd = create_window {
        cs_class_name = test_class_name;
        cs_window_name = "Hello";
        cs_style = style_of_options ([ WS_OVERLAPPEDWINDOW; WS_HSCROLL; WS_VSCROLL; WS_CLIPCHILDREN ]);
        cs_position = None;
        cs_size = Some { cx = 600; cy = 500 };
        cs_parent = null_hwnd;
        cs_menu = null_handle;
        cs_child_id = 0;
        cs_instance = the_instance;
        cs_param = null_pointer } in

    ignore (show_window ~wnd ~show:the_show_command);
    update_window ~wnd;

    let accel = load_accelerators ~inst:the_instance ~name:(Rn_string "Accels") in

    let rec loop () =
        let (ok, msg) = get_message ~wnd:None ~msg_filter:None in
        if ok then
          begin
            if not (translate_accelerator ~wnd ~accel ~msg) then
              begin
                ignore (translate_message ~msg);
                ignore (dispatch_message ~msg)
              end;
            loop ()
          end
        else
            Int32.to_int msg.msg_contents.mc_wparam
    in

    let code = loop () in
    exit code

let _ =
    try
        win_main ()
    with
        e ->
            let s = Printexc.to_string e in
            ignore (message_box ~wnd:null_hwnd ~text:s ~caption:"Uncaught exception" ~options:[MB_OK]);
            exit 1
