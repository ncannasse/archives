(* module Mouse

 - CLIENT
 - gestion de l'affichage de la souris

*)

open Input
open Mtlib
open M3d

type cursor =
	| CRNormal
	| CRSelect
	| CRCancel

let cursor = Global.empty "Mouse Cursor"
let msize = ref (0.,0.)
let ctype = ref CRCancel
let visible = ref true

let set_cursor c =
	if !ctype <> c then begin
		let m = Global.get cursor in
		let size = (
			match c with
			| CRNormal ->
				Sprite.face m Constants.normal_cursor_zone;
				Sprite.size m Constants.normal_mouse_size;
				Constants.normal_mouse_size
			| CRSelect ->
				Sprite.face m Constants.select_cursor_zone;
				Sprite.size m Constants.select_mouse_size;
				(0.,0.)
			| CRCancel ->
				Sprite.face m Constants.cancel_cursor_zone;
				Sprite.size m Constants.cancel_mouse_size;
				(0.,0.))
		in
		msize := size;
		ctype := c;
	end

let curdelta = ref (0,0)

let process() =
	Input.process();
	let mx,my = Input.Mouse.pos() in
	Sprite.pos (Global.get cursor) { x= mx +. (fst !msize) /. 2.; y= my +. (snd !msize) /. 2.; z=0.000001; };
	let dx = ref 0 and dy = ref 0 in
	if mx < -0.95 then decr dx else if mx > 0.95 then incr dx;
	if my < -0.95 then decr dy else if my > 0.95 then incr dy;
	match !dx,!dy with
	| 0,0 -> Camera.stop(); curdelta := (0,0)
	| x,y when (x,y) = !curdelta -> ()
	| x,y ->
		let delta = { x = (itof x) *. -200000.; y = (itof y) *. -200000.; z = 0. } in
		let p,t = Camera.get_pos(), Camera.get_target() in
		Camera.goto (Vector.add p delta) (Vector.add t delta) 1000.;
		curdelta := (x,y)

let init h =
	if not (Input.init h) then failwith "Failed to Init INPUT";
	let c = (Sprite.attach Constfiles.fMouseCursors Sprite.all) in	
	Global.set cursor c;
	set_cursor CRNormal

let show b =
	if b <> !visible then begin
		visible := b;
		Sprite.fade (Global.get cursor) (if b then Color.full else Color.zero) 0.1;
	end

let close() =
	Sprite.detach (Global.get cursor);
	Global.undef cursor

let click() =
	Input.Mouse.left() = Down

let rclick() =
	Input.Mouse.right() = Down

let pos() =
	Input.Mouse.pos()
