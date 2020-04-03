(********************************************************************************)
(* Includes *)

open M3d
open Osiris
open Vfs
open Mtlib
open Run
open Log

(********************************************************************************)
(* Globals *)

let mtw_t = Vfs.get_ftype "MTW"
let pic_t = Vfs.get_ftype "PIC"

(********************************************************************************)
(* 3D Funcs *)

type mview = {
	inst : Inst.t;
	mutable transform : Matrix.t;
	mutable pos : vector;
	mutable target : vector;
}

let models = Hashtbl.create 0
let textures = Hashtbl.create 0

let drag_x = ref 0 and
	drag_y = ref 0 and
	drag = ref false and
	can_drag = ref false and
	zoom_y = ref 0 and
	zoom = ref false and
	pan = ref false

let begin_drag () =	
	can_drag := true

let begin_pan () =
	can_drag := true;
	pan := true

let begin_zoom () =
	let _,y = mouse_pos() in
	zoom_y := y;
	zoom := true


let end_drag () =
	can_drag := false;
	drag := false;
	pan := false

let end_zoom () =
	zoom := false

let do_dragzoom x y =
	if !drag then begin
		let x,y = mouse_pos() in
		if not !pan then
			Render.do_transform (Matrix.rot {x=0.0; y=(float_of_int (y- !drag_y))/.100.0; z=(float_of_int (!drag_x-x))/.100.0;})
		else
			Render.do_transform (Matrix.trans { z= (float_of_int (!drag_y-y))/.10.0; y=0.; x= (float_of_int (!drag_x-x))/.10.0; });
		drag_x := x; 
		drag_y := y;
	end else if !zoom then begin
		let _,y = mouse_pos() in
		let zfact = (1.0+.(float_of_int (!zoom_y-y))/.100.0) in			
		Camera.zoom zfact;
		zoom_y := y;
	end else if !can_drag then begin
		let p = (match Document.current() with None -> assert false | Some id -> Document.get id)#panel "3dpanel" in
		let xp = ((float_of_int x) *. 2.0) /. (float_of_int p#get_width) -. 1.0 in
		let yp = ((float_of_int y) *. 2.0) /. (float_of_int p#get_height) -. 1.0 in
		match Render.pick (xp,yp) with
		| [] -> ()
		| _ ->
			let x,y = mouse_pos() in
			drag_x := x;
			drag_y := y;
			drag := true;
	end
		
let show_3d id =
	let data = Hashtbl.find models id in
	Camera.set_pos data.pos;	
	Camera.set_target data.target;
	Inst.visible data.inst true;
	Render.set_transform data.transform;
	Render.set_target ((Document.get id)#panel "3dpanel")#handle;
	Render.start()

let close_3d id =
	Render.stop();
	let data = Hashtbl.find models id in
	Inst.detach data.inst;
	Hashtbl.remove models id

let hide_3d id =
	let data = Hashtbl.find models id in
	Inst.visible data.inst false;
	Render.stop();
	Hashtbl.replace models id { inst = data.inst; transform = Render.get_transform(); pos = Camera.get_pos(); target = Camera.get_target(); }

(********************************************************************************)
(* MTW Funcs *)

let open_model id =	
	Gc.full_major();
	Render.garbage();
	let m = Inst.attach id in
		Hashtbl.add models id { inst = m; transform = Matrix.id(); pos = Camera.get_pos(); target = Camera.get_target(); };
	Document.create id (Dialogs.get "3d.model") (fun x ->
		let anims = x#combobox "anim.list" in
			anims#listprint (fun () -> Inst.anims m);
			anims#on_selchange (fun () -> Inst.anim m (match anims#selected with
				| None -> None
				| Some p -> Some (List.nth (Inst.anims m) p)));
		let b = x#button "anim.stop" in
			b#on_click (fun () -> Inst.anim m None);		
		let b = x#button "anim.reset" in
			b#on_click (fun () -> Inst.reset m; anims#set_selected None);
		let p = x#panel "3dpanel" in
			p#on_ldown begin_drag;
			p#on_lup end_drag;
			p#on_rdown begin_zoom;
			p#on_rup end_zoom;
			p#on_mousemove do_dragzoom;
			p#on_mdown begin_pan;
			p#on_mup end_drag;
			Camera.reset();
			Render.set_transform (Matrix.id())
	)

let import_model pid file =
	let id = Vfs.create_hlink ~name:(Filename.basename file) ~file:file ~parent:pid ~ftype:mtw_t in
		open_model id

(********************************************************************************)
(* Picture Funcs *)

let import_picture pid file =
	ignore( Vfs.create_hlink ~name:(Filename.basename file) ~file:file ~parent:pid ~ftype:pic_t)

let cur_pic = Global.empty "Current picture"

let open_picture id =
	(* check if 'id' is a valid sprite *)
	Sprite.detach (Sprite.attach id {tx = 0.0; ty = 0.0; tx2 = 1.0; ty2 = 1.0});
	Hashtbl.add textures id ();
	Document.create id (Dialogs.get "picture") (fun x -> ())

let show_picture id =
	let sprite = Sprite.attach id {tx = 0.0; ty = 0.0; tx2 = 1.0; ty2 = 1.0} in
		Sprite.size sprite (1.8,1.8);
		Global.set cur_pic sprite;	
		Render.set_target ((Document.get id)#panel "picture")#handle;
		Render.start()

let hide_picture id =
	Render.stop();
	try
		Sprite.detach (Global.get cur_pic);
		Global.undef cur_pic;
	with
		Global_not_initialized _ -> ()

let close_picture id =
	hide_picture id;
	Hashtbl.remove textures id

(********************************************************************************)
(* Register *)

let init () =
	Callback.register "logfunc" (log MSG);
	if not (M3d.init Main.obj#self#handle true) then failwith "Failed to Init W3D";
	Render.set_option (OShowInfos true)
	
let close_all () =	
	Hashtbl.iter (fun id _ -> Document.close id) models;
	Hashtbl.iter (fun id _ -> Document.close id) textures;
	M3d.close()

;;
let data = {
	ft_name = "3D Model";
	ft_t = mtw_t;
	ft_icon = Gfxres.bitmap "BMP_MTW";
	ft_new = None;
	ft_select = None;
	ft_unselect = None;
	ft_open = Some open_model;
	ft_show = Some show_3d;
	ft_hide = Some hide_3d;
	ft_close = Some close_3d;
	ft_context = [];
	ft_delete = None;
	ft_hardlinked = Some {
		fh_ext = ["*.mtw"];
		fh_import = import_model;
	};
	ft_export = None;
	ft_haschilds = false;
} in
	Vfs.register data;

let data = {
	ft_name = "Picture";
	ft_t = pic_t;
	ft_icon = Gfxres.bitmap "BMP_PIC";
	ft_new = None;
	ft_select = None;
	ft_unselect = None;
	ft_delete = None;
	ft_open = Some open_picture;
	ft_show = Some show_picture;
	ft_hide = Some hide_picture;
	ft_close = Some close_picture;
	ft_context = [];
	ft_hardlinked = Some {
		fh_ext = ["*.jpg";"*.bmp";"*.png";"*.tga";"*.dds";"*.dib"];
		fh_import = import_picture;
	};
	ft_export = None;
	ft_haschilds = false;
} in
	Vfs.register data;

Run.register {
	m_name = "M3dview";
	m_init = Some init;
	m_close = Some close_all;
	m_main = Some M3d.handle_events;
}

(********************************************************************************)
