let magic = Obj.magic 

open M3d
open Mtlib
open Input
open Xml

let z_base = 0.1

type face = {
	mutable f_texture : Vfs.fid;
	mutable f_rect : vector4;
}

type pos = (float * float)
type size = (float * float)

type event =  (unit -> unit)

type type_action =
	| Button_Click
	| Button_DbClick
	| Button_Rollout
	| Button_Rollover
	| Button_Down of int
	| Button_Up

class type component =
  object	
	method pos : pos -> unit
	method get_pos : unit -> pos
	method size : size -> unit
	method get_size : unit -> size
	method destroy : unit -> unit
	method visible : bool -> unit
	method is_visible : unit -> bool
  end	

class type clickable =
  object
  	method on_click :  event -> unit
	method on_dblclick :  event -> unit
	method on_rollover :  event -> unit
	method on_rollout :  event -> unit
	method on_down : (int -> unit) -> unit
	method on_up : event -> unit
  end

class type clickable_aera =
  object
	inherit component
	inherit clickable
  end

class type gfx =
  object	
	inherit component
	method face : face -> unit
  end	

class type panel = 
  object
	inherit gfx
  end

class type button =
  object
	inherit gfx
	inherit clickable
  end

class type adv_button =
  object	
	inherit button
  end

class type progress_bar =
  object	
	inherit button	
	method update : float -> unit
  end

class type scroll_bar =
  object
	inherit component
	method set_value : int -> unit
	method get_value : unit -> int
	method on_modify : event -> unit
  end

class type checkbox =
  object	
	inherit adv_button
	method set_state : int -> unit
	method get_state : unit -> int
  end

(*******************************************************)

let list_component = MList.empty()

let no_event () = ()


(***********************************************)
(***********     I Component     ***************)
(***********************************************)


class virtual icomponent =
  object (this)

	val mutable px = 0.
	val mutable py = 0.
	val mutable pz = 0.
	val mutable sx = 0.
	val mutable sy = 0.
	val mutable vbl = true

	method pos (posx,posy) = 
		px <- posx;
		py <- posy;

	method get_pos () = 
		(px,py)

	method posz p =
		pz <- p

	method get_posz () =
		pz

 	method size (sizex,sizey) = 
		sx <- sizex;
		sy <- sizey
			
	method get_size ()= 
		(sx,sy)

	method is_selected  (mx,my) = 
		let px2 = px +. sx/.2. in
		let py2 = py +. sy/.2. in
		let px1 = px -. sx/.2. in
		let py1 = py -. sy/.2. in
		(mx >= px1 && my >= py1 && mx <= px2 && my <= py2)

	method virtual handle_events : (float*float) -> type_action -> unit

	method virtual visible : bool -> unit

	method is_visible () = vbl

	method destroy () = ()

  end




(***********************************************)
(*************       I gfx       ***************)
(***********************************************)

class virtual igfx f parent =
  object (this)

	inherit icomponent as super

	val mutable sprite = Sprite.attach f.f_texture f.f_rect
	val mutable parent = (magic parent : ipanel option)

	initializer
		this#posz (match parent with
			| None -> 
				MList.add list_component (this :> icomponent);
				generate_z()
			| Some panel ->
				panel#add_obj (this :> icomponent)
				panel#generate_z()
		)

	method face f =
		Sprite.detach sprite;
		sprite <- Sprite.attach f.f_texture f.f_rect ;
		this#pos (px,py);
		this#posz pz;
		this#size (sx,sy)

	method texture v =
		Sprite.face sprite v

	method pos (posx,posy) = 
		Sprite.pos sprite { x = posx; y = posy; z = pz };
		px <- posx;
		py <- posy

	method posz p =
		Sprite.pos sprite { x = px; y = py; z = p };
		pz <- p

 	method size (sizex,sizey) = 
		Sprite.size sprite (sizex,sizey);
		sx <- sizex;
		sy <- sizey
			
	method get_face () = Sprite.get_face sprite

	method visible b =
		if b <> vbl then begin
			if b = false then begin
				Sprite.detach sprite;
				vbl <- false
			end	else begin
				sprite <- Sprite.attach f.f_texture f.f_rect;
				this#pos (px,py);
				this#posz pz;
				this#size (sx,sy);
				vbl <- true
			end
	end
				
	method destroy () =
		Sprite.detach sprite
		
  end

(***********************************************)
(***********      I PANEL       ****************)
(***********************************************)

and ipanel (parent:panel option) (f:face) (p:pos) (s:size) (n:string) = 
  object(this)
	
	inherit igfx f parent as super

	val table = Hashtbl.create 0
	
	initializer
		match parent with
			| None -> 
				MList.add list_component (this :> icomponent);
				let pz = generate_z() in
				vbl <- true;
				this#pos p;
				this#posz pz;
				this#size s;
			| Some panel ->
				let real_panel = (magic panel : ipanel) in
				let pz = real_panel#generate_z() in
				let (pan_px,pan_py) = real_panel#get_pos() in
				let (pan_sx,pan_sy) = real_panel#get_size() in
				
				let px = pan_px +. ((fst p) *. pan_sx/.2.) in
				let py = pan_py +. ((snd p) *. pan_sy/.2.) in
				let sx = (fst s) *. pan_sx/.2. in
				let sy = (snd s) *. pan_sy/.2. in
				vbl <- true;
				this#pos (px,py);
				this#posz pz;
				this#size (sx,sy);
				real_panel#add_obj n ( this :> icomponent )


			 
	method add_obj (name:string) (obj:icomponent) =
		Hashtbl.add table name obj;

	method handle_events mouse action =
		Hashtbl.iter (fun _ obj -> obj#handle_events mouse action) table 
				
	method visible b =
		Hashtbl.iter (fun _ obj -> obj#visible b) table;
		super#visible b
	
	method destroy () =
		Hashtbl.iter (fun _ obj -> obj#destroy ()) table;
		super#destroy();
		
	
	method destroy_item (name:string) =
		try (
			Hashtbl.remove table name
		)
		with Not_found -> failwith ("ce bouton n'existe pas dans ce panel : " ^ name ^ " !!!");
		
	method state ((fx:float),(fy:float)) (type_action:type_action) = ()	


  end


(***********************************************)
(**********    I CLICKABLE          ************)
(***********************************************)

class iclickable =
  object
	val mutable aclick = no_event
	val mutable adbclick = no_event
	val mutable arollout = no_event
	val mutable arollover = no_event 
	val mutable aup = no_event
	val mutable adown = (fun _ -> ())

	method on_click a = aclick <- a
	method on_dblclick a = adbclick <- a
	method on_rollover a =  arollover<- a
	method on_rollout a = arollout <- a
	method on_down a = adown <- a
	method on_up a = aup <- a

	method call_action a =
		match a with
		| Button_Click -> aclick ()
		| Button_DbClick -> adbclick ()
		| Button_Rollover -> arollover ()
		| Button_Rollout -> arollout ()
		| Button_Down t -> adown t
		| Button_Up -> aup ()

  end

(***********************************************)
(**********    I CLICKABLE AERA     ************)
(***********************************************)

class iclickable_aera parent p s n=
  object(this)
	inherit icomponent as super
	inherit iclickable
	
	initializer
		match parent with
		| None -> 
			MList.add list_component (this :> icomponent);
			let pz = z_base /. 2. in
			this#pos p;
			this#posz pz;
			this#size s;
		| Some panel ->
			let real_panel = (magic panel : ipanel) in
			let pz = real_panel#get_posz() /. 2. in
			let (pan_px,pan_py) = real_panel#get_pos() in
			let (pan_sx,pan_sy) = real_panel#get_size() in
			let px = pan_px +. ((fst p) *. pan_sx/.2.) in
			let py = pan_py +. ((snd p) *. pan_sy/.2.) in
			let sx = (fst s) *. pan_sx/.2. in
			let sy = (snd s) *. pan_sy/.2. in
			real_panel#add_obj n (this :> icomponent);
			this#pos (px,py);
			this#posz pz;
			this#size (sx,sy);
	
		
	method handle_events mouse action =
		if ( this#is_selected mouse) then 
			match action with
				| Button_Click -> 
					this#call_action Button_Click
				| Button_DbClick ->	
					this#call_action Button_DbClick
				| Button_Rollout -> 
					this#call_action Button_Rollout
				| Button_Rollover ->
					this#call_action Button_Rollover
				| Button_Up -> 
					this#call_action Button_Up
				| Button_Down t ->
					this#call_action (Button_Down t)
	
	method destroy () =
		super#destroy();
		try
			MList.remove list_component (this :> icomponent)
		with Not_found -> ();
				
	method visible b = vbl <- b

  end




(***********************************************)
(***********      I BUTTON      ****************)
(***********************************************)
class ibutton (parent:panel option) f (p:pos) (s:size) (v:bool) (n:string) =
  object(this)
	
	inherit igfx f as super
	inherit iclickable

	initializer
		match parent with
		| None -> 
			MList.add list_component (this :> icomponent);
			let pz = z_base /. 2. in
			this#visible v;
			this#pos p;
			this#posz pz;
			this#size s;
		| Some panel ->
			let real_panel = (magic panel : ipanel) in
			let pz = real_panel#get_posz() /. 2. in
			let (pan_px,pan_py) = real_panel#get_pos() in
			let (pan_sx,pan_sy) = real_panel#get_size() in
			let px = pan_px +. ((fst p) *. pan_sx/.2.) in
			let py = pan_py +. ((snd p) *. pan_sy/.2.) in
			let sx = (fst s) *. pan_sx/.2. in
			let sy = (snd s) *. pan_sy/.2. in
			real_panel#add_obj n (this :> icomponent);
				this#visible v;
				this#pos (px,py);
				this#posz pz;
				this#size (sx,sy);
	

	method handle_events mouse action =
		if ( this#is_selected mouse) then 
			match action with
				| Button_Click -> 
					this#call_action Button_Click
				| Button_DbClick ->	
					this#call_action Button_DbClick
				| Button_Rollout -> 
					this#call_action Button_Rollout
				| Button_Rollover ->
					this#call_action Button_Rollover
				| Button_Up -> 
					this#call_action Button_Up
				| Button_Down t ->
					this#call_action (Button_Down t)

	method destroy () =
		super#destroy();
		try
			MList.remove list_component (this :> icomponent)
		with Not_found -> ();
					
  end

(***********************************************)
(***********     I ADV BUTTON    ***************)
(***********************************************)
		
class iadv_button (parent:panel option) f_normal f_mouse_over f_push (p:pos) (s:size) (v:bool) (n:string) =
object(this)
	
	inherit ibutton parent f_normal p s v n
	val mutable normal = f_normal
	val mutable mouse_over = f_mouse_over
	val mutable push = f_push
	val mutable is_rollover = false
	val mutable is_down = false

	method handle_events (mouse:pos) (action:type_action) =
			if this#is_visible () then (
		if (this#is_selected mouse)
		then
			match action with
				| Button_Rollover ->
					if not is_rollover 
					then (
						this#face mouse_over;
						is_rollover <-true
						)
				| Button_Down t ->
					if not is_down
					then (
						this#face push;
						is_rollover <- true;
						is_down <- true
					)
				| Button_Up -> 
					this#face normal;
					is_rollover <- false;
					is_down <- false;
					this#call_action Button_Click
				| _ -> ()
				
		else
		if is_rollover || is_down
		then (
			this#face normal;
			is_rollover <- false;
			is_down <- false
			)		
	)				
	
end


(************************************************)
(**********     I PROGRESS BAR      *************)
(************************************************)

class iprogress_bar parent face_effectue face_reste position taille nom =
object (this)

	inherit ibutton parent face_reste position taille true "bprogress"

	val bdone = new ibutton parent face_effectue position (0.,0.) true "bdone"
	
	initializer
		bdone#texture { tx=face_effectue.f_rect.tx ; ty=face_effectue.f_rect.ty ; tx2=face_effectue.f_rect.tx ; ty2=face_effectue.f_rect.ty2 ; } ;
		let p = (this#get_posz () /.1.5) in
		bdone#posz p;

		match parent with
		| None -> 
			let pz = z_base /. 2. in
			this#posz pz;
		
			bdone#texture { tx=face_effectue.f_rect.tx ; ty=face_effectue.f_rect.ty ; tx2=face_effectue.f_rect.tx ; ty2=face_effectue.f_rect.ty2 ; } ;
			let p = (this#get_posz () /.1.5) in
			bdone#posz p


		| Some panel ->
			let real_panel = (magic panel : ipanel) in
			let pz = real_panel#get_posz() /. 2. in
			let (pan_px,pan_py) = real_panel#get_pos() in
			let (pan_sx,pan_sy) = real_panel#get_size() in
			let px = pan_px +. ((fst position) *. pan_sx/.2.) in
			let py = pan_py +. ((snd position) *. pan_sy/.2.) in
			let sx = (fst taille) *. pan_sx/.2. in
			let sy = (snd taille) *. pan_sy/.2. in
			real_panel#add_obj nom (this :> icomponent);
				this#posz pz;
				this#size (sx,sy);
				bdone#texture { tx=face_effectue.f_rect.tx ; ty=face_effectue.f_rect.ty ; tx2=face_effectue.f_rect.tx ; ty2=face_effectue.f_rect.ty2 ; } ;
				let p = (this#get_posz () /.1.5) in
				bdone#posz p



	method update percent = 

		
		if percent > 0. then

		let (px,py) =  (this#get_pos()) in
		let (sx,sy) = (this#get_size()) in
		if percent < 100. then
		(
			let x1 = px -. (sx/.2.) +.((sx*.percent/.100.)/.2.)in
			let hx1 = (sx*.(percent/.100.)) in
			bdone#pos (x1,py);
			bdone#size (hx1,sy);
			let f=bdone#get_face () in
			bdone#texture { tx=f.tx ; ty=f.ty ; tx2=f.tx +. (sx *. percent /. 100.) ; ty2=f.ty2 ; } 
		
		);

	method visible v =
		this#visible v;
		bdone#visible v;

	method handle_events (mouse:pos) (action:type_action)= ()

	method destroy () =
	()
	
	
end

(************************************************)
(**********      I SCROLL BAR       *************)
(************************************************)

class iscroll_bar parent f_fond f_lf f_rf f_pad p s n nb orient=
object (this)

	inherit icomponent as super

	val lf = new ibutton parent f_lf p s true n
	val rf = new ibutton parent f_rf p s true n
	val pad = new ibutton parent f_pad p s true n
	val lfond = new iclickable_aera parent p s n
	val rfond = new iclickable_aera parent p s n
	val fond = new ipanel parent f_fond p s n
	val mutable modify = no_event
	val orientation = orient
	val mutable nbpos = nb
	val mutable posmin = 0
	val mutable posmax = nb*2-2
	val mutable pospad = 0

	initializer
		let pz =
		(match parent with
		| None -> z_base /. 2. 
		| Some panel -> (magic panel : ipanel)#get_posz ()/.2.) in
		let pos =
		(match parent with
		| None -> p 
		| Some panel -> 
			let (x,y) = (magic panel : ipanel)#get_pos () in
			let (sx,sy) = (magic panel : ipanel)#get_size () in
			(x +. ((fst p) *. sx/.2.),y +. ((snd p) *. sy/.2.))
			) in
		let size =
		(match parent with
		| None -> s 
		| Some panel ->
			let (sx,sy) = (magic panel : ipanel)#get_size () in
			((fst s) *. sx/.2.,(snd s) *. sy/.2.)

			) in
		this#pos pos;
		this#posz pz;
		this#size size;
		fond#pos pos;
		fond#size size;
		fond#posz pz;
		let (px,py) = this#get_pos () in
		let (sx,sy) = this#get_size () in
		if orient == 0 then begin
			fond#size (sx-.2.*.sy,sy);
			lf#pos (px -. (sx/.2.-.sy/.2.),py);
			lf#posz (pz/.2.);
			lf#size (sy,sy);
			rf#pos (px +. sx/.2. -. sy/.2.,py);
			rf#posz (pz/.2.);
			rf#size (sy,sy);
			lfond#pos (px -. (sx/.2.-.sy),py);
			lfond#posz (pz/.2.);
			lfond#size (0.,sy);
			let tmp_sx = sx -. 2.*.sy -. ((sx-. (2.*.sy)) /. ((itof nbpos))) in 
			rfond#size (tmp_sx ,sy);
			rfond#pos (px +. (sx/.2.-.sy -. tmp_sx/.2.),py);
			rfond#posz (pz/.2.);
			pad#pos ( px -. ((sx -. 2.*.sy)/.2.) +.( ((sx-. (2.*.sy)) /.((itof nbpos)))/.2.),py);
			pad#posz (pz/.2.);
			pad#size ((sx-. (2.*.sy)) /. ((itof nbpos)),sy);
		end
		else begin
			fond#size (sx,sy-.2.*.sx);
			lf#pos (px ,py -. (sy/.2.-.sx/.2.));
			lf#posz (pz/.2.);
			lf#size (sx,sx);
			rf#pos (px ,py+. sy/.2. -. sx/.2.);
			rf#posz (pz/.2.);
			rf#size (sx,sx);
			lfond#pos (px,py-.(sy/.2.-.sx));
			lfond#posz (pz/.2.);
			lfond#size (sx,0.);
			let tmp_sy = sy -. 2.*.sx -. ((sy-. (2.*.sx)) /. ((itof nbpos))) in 
			rfond#size (sx ,tmp_sy);
			rfond#pos (px ,py+. (sy/.2.-.sx -. tmp_sy/.2.));
			rfond#posz (pz/.2.);
			pad#pos ( px ,py-. ((sy -. 2.*.sx)/.2.) +.( ((sy-. (2.*.sx)) /.((itof nbpos)))/.2.));
			pad#posz (pz/.2.);
			pad#size (sx,(sy-. (2.*.sx)) /. ((itof nbpos)));
		end;

		lf#visible true;
		rf#visible true;
		lfond#visible true;
		rfond#visible true;
		pad#visible true;

		rf#on_click ( fun () -> 
			let (x,y) = pad#get_pos () in
			if ( pospad != posmax )  then (
				let (fsx,fsy) = this#get_size() in
				let (sx,sy) = pad#get_size () in
				let (lfx,lfy) = lfond#get_pos () in
				let (slfx,slfy) = lfond#get_size () in
				let (rfx,rfy) = rfond#get_pos () in
				let (srfx,srfy) = rfond#get_size () in
				if orient == 0 then begin 
					pad#pos (x +. sx/.2.,y);
					lfond#size (slfx +. sx/.2.,slfy);
					lfond#pos (lfx +. sx/.4.,lfy);
					rfond#size (srfx -. sx/.2.,srfy);
					rfond#pos (rfx +. sx/.4.,rfy);

				end else begin
					pad#pos (x ,y +. sy/.2.);
					lfond#size (slfx ,slfy+. sy/.2.);
					lfond#pos (lfx ,lfy+. sy/.4.);
					rfond#size (srfx,srfy -. sy/.2.);
					rfond#pos (rfx ,rfy+. sy/.4.);

				end;
				pospad <- pospad+1;
				modify ()
				)
			);

		rf#on_down ( fun t ->
			if t>200 then begin
				Mouse.reset_left();
				rf#call_action (Button_Click)
			end	
			);

		lf#on_click ( fun () -> 
			let (x,y) = pad#get_pos () in
			if ( pospad != posmin )  then (
				let (fsx,fsy) = this#get_size() in
				let (sx,sy) = pad#get_size () in
				let (lfx,lfy) = lfond#get_pos () in
				let (slfx,slfy) = lfond#get_size () in
				let (rfx,rfy) = rfond#get_pos () in
				let (srfx,srfy) = rfond#get_size () in
				if orient = 0 then begin
					pad#pos (x -. sx/.2.,y);
					lfond#size (slfx -. sx/.2.,slfy);
					lfond#pos (lfx -. sx/.4.,lfy);
					rfond#size (srfx +. sx/.2.,srfy);
					rfond#pos (rfx -. sx/.4.,rfy);

				end else begin
					pad#pos (x,y -. sy/.2.);
					lfond#size (slfx,slfy -. sy/.2.);
					lfond#pos (lfx,lfy -. sy/.4.);
					rfond#size (srfx,srfy +. sy/.2.);
					rfond#pos (rfx,rfy -. sy/.4.);
				end;
				pospad <- pospad-1;
				modify ()
			)
			);

		lf#on_down ( fun t ->
			if t>200 then begin
			Mouse.reset_left();
				lf#call_action (Button_Click)
			end
		);
	
		rfond#on_click ( fun () -> 
			let (x,y) = pad#get_pos () in
			if ( pospad != posmax )  then (
				if pospad = posmax-1 then rf#call_action Button_Click
				else
					let (fsx,fsy) = this#get_size() in
					let (sx,sy) = pad#get_size () in
					let (lfx,lfy) = lfond#get_pos () in
					let (slfx,slfy) = lfond#get_size () in
					let (rfx,rfy) = rfond#get_pos () in
					let (srfx,srfy) = rfond#get_size () in
					if orient = 0 then begin					
						pad#pos (x +. sx,y);
						lfond#size (slfx +. sx,slfy);
						lfond#pos (lfx +. sx/.2.,lfy);
						rfond#size (srfx -. sx,srfy);
						rfond#pos (rfx +. sx/.2.,rfy);

					end else begin
						pad#pos (x,y +. sy);
						lfond#size (slfx,slfy +. sy);
						lfond#pos (lfx,lfy +. sy/.2.);
						rfond#size (srfx,srfy -. sy);
						rfond#pos (rfx,rfy +. sy/.2.);
					end;
					pospad <- pospad+2;
					modify ()
				)
			);

		rfond#on_down ( fun t ->
			if t>200 then begin
				Mouse.reset_left();
				rfond#call_action (Button_Click)
			end
		);

		lfond#on_click ( fun () -> 
			let (x,y) = pad#get_pos () in
			if ( pospad != posmin )  then (
				if pospad = posmin+1 then lf#call_action Button_Click
				else
					let (fsx,fsy) = this#get_size() in
					let (sx,sy) = pad#get_size () in
					let (lfx,lfy) = lfond#get_pos () in
					let (slfx,slfy) = lfond#get_size () in
					let (rfx,rfy) = rfond#get_pos () in
					let (srfx,srfy) = rfond#get_size () in
					if orient = 0 then begin
						pad#pos (x -. sx,y);
						lfond#size (slfx -. sx,slfy);
						lfond#pos (lfx -. sx/.2.,lfy);
						rfond#size (srfx +. sx,srfy);
						rfond#pos (rfx -. sx/.2.,rfy);
					end else begin
						pad#pos (x,y -. sy);
						lfond#size (slfx,slfy -. sy);
						lfond#pos (lfx,lfy -. sy/.2.);
						rfond#size (srfx,srfy +. sy);
						rfond#pos (rfx,rfy -. sy/.2.);
					end;
					pospad <- pospad-2;
					modify ()
			)
			);
	
		lfond#on_down ( fun t ->
			if t>200 then begin
				Mouse.reset_left();
				lfond#call_action (Button_Click)
			end
		);

		

	method set_value valeur =
		if valeur <0 || (itof valeur) > (itof nbpos) then failwith ("unacceptable value")
		else begin
			pospad <- valeur*2;
		
			let (sx,sy) = this#get_size () in
			let (px,py) = this#get_pos () in
			let (padsx,padsy) = pad#get_size () in

			if orientation = 0 then begin
				pad#pos (px -. (sx/.2.) +. sy +. (itof pospad)/.2. *. padsx +. padsx/.2.,py);
				lfond#size (padsx *. ((itof pospad)/.2.), padsy);
				lfond#pos (px -. (sx/.2.) +. sy +. (padsx *. ((itof pospad)/.2.) /.2.) ,py );
				rfond#size (padsx *. ((itof (nbpos*2 - pospad))/.2.),padsy);
				rfond#pos (px +. (sx/.2.) -. sy -. padsx *. ((itof (nbpos*2 - pospad))/.4.),py)
			end else begin
				pad#pos (px ,py -. (sy/.2.) +. sx +. (itof pospad)/.2. *. padsy +. padsy/.2.);
				lfond#size (padsx , padsy *. ((itof pospad)/.2.));
				lfond#pos (px ,py  -. (sy/.2.) +. sx +. (padsy *. ((itof pospad)/.2.) /.2.));
				rfond#size (padsx ,padsy*. ((itof (nbpos*2 - pospad))/.2.));
				rfond#pos (px ,py+. (sy/.2.) -. sx -. padsy *. ((itof (nbpos*2 - pospad))/.4.))
			end
		end
		
	method get_value () =
		pospad

	method on_modify action = 
		modify <- action

	method visible b =
		lf#visible b;
		rf#visible b;
		pad#visible b;
		lfond#visible b;
		rfond#visible b;
		this#visible b

	method handle_events (m:pos) (ta:type_action)= ()

end

(************************************************)
(*************      I_CHECKBOX       ************) 
(************************************************)

class icheckbox (parent:panel option) f_normal f_push (p:pos) (s:size) (v:bool) (n:string) =
  object (this)
	
  	inherit ibutton parent f_normal  (p:pos) (s:size) (v:bool) (n:string)

	val mutable on_off = 0

	initializer
		this#on_click ( fun t ->
		let x = this#get_state() in
		if x=0
			then this#set_state 1
			else this#set_state 0);


	method set_state (etat:int) =
		if (on_off != etat)
			then (
				if on_off = 0 then 
					(this#face f_push)
				else 
					(this#face f_normal);
			on_off <- etat;
			)	

	method get_state () =
		on_off


  end




(************************************************)


(************************************************)
(**********    HANDLE_EVENTS    *****************)
(************************************************)

let handle_events () =
	let m = Input.Mouse.pos() in 
	let state = Input.Mouse.left() in
	MList.iter (fun obj -> 
		match state with
			| Up -> obj#handle_events m Button_Rollover
			| Down -> obj#handle_events m Button_Click
			| Released -> obj#handle_events m Button_Up
			| Pressed t -> obj#handle_events m (Button_Down t)
	) list_component
	
(************************************************)
(*************      CLOSE       *****************)
(************************************************)

let close () =
	MList.iter ( fun obj -> obj#destroy () ) list_component;
	MList.clear list_component
			

(************************************************)
(**********       NEW_BUTTON        *************)
(************************************************)

let new_button ~parent ~face ~position ~taille ~visible ~nom = 
	let b = new ibutton parent face position taille visible nom in
	(b :> button)


(************************************************)
(**********     NEW_ADV_BUTTON      *************)
(************************************************)

let new_adv_button ~parent ~face_normale ~face_rollover ~face_click ~position ~taille ~visible ~nom =
	let b = new iadv_button parent face_normale face_rollover face_click position taille visible nom in
	(b :> adv_button)	

(************************************************)
(**********       NEW_PANEL         *************)
(************************************************)

let new_panel ~parent ~face ~position ~taille ~nom =	
	let pan = new ipanel parent face position taille nom in
	(pan :> panel)

	

(************************************************)
(**********    NEW_PROGRESS_BAR     *************)
(************************************************)

let new_progress_bar ~parent ~face_effectue ~face_reste ~position ~taille ~nom =
	let bar = new iprogress_bar parent face_effectue face_reste position taille nom in
	(bar :> progress_bar)

(************************************************)
(**********     NEW_SCROLL_BAR      *************)
(************************************************)

let new_scroll_bar ~parent ~face_fond ~face_fleche_initiale ~face_fleche_finale ~face_curseur ~position ~taille ~nom ~nb_positions ~orientation =
	let scroll = new iscroll_bar parent face_fond face_fleche_initiale face_fleche_finale face_curseur position taille nom nb_positions orientation in
	(scroll :> scroll_bar)

(************************************************)
(**********     NEW_CHECKBOX        *************)
(************************************************)

let new_checkbox ~parent ~face_normale  ~face_click ~position ~taille ~visible ~nom =
	let b = new icheckbox parent face_normale face_click position taille visible nom in
	(b :> checkbox)

(************************************************)
