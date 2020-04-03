type face = {
	mutable f_texture : Vfs.fid;
	mutable f_rect : M3d.vector4;
}

type pos = (float * float)
type size = (float * float)

type event =  (unit -> unit)

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

val handle_events : unit -> unit
val close : unit -> unit

val new_button : parent:(panel option) -> face:face -> position:pos -> taille:size -> visible:bool -> nom:string -> button
val new_adv_button : parent:(panel option) -> face_normale:face -> face_rollover:face -> face_click:face -> position:pos -> taille:size -> visible:bool -> nom:string -> adv_button
val new_panel : parent:(panel option) -> face:face -> position:pos -> taille:size -> nom:string -> panel
val new_progress_bar : parent:(panel option) -> face_effectue:face -> face_reste:face -> position:pos -> taille:size -> nom:string -> progress_bar
val new_scroll_bar : parent:(panel option) -> face_fond:face -> face_fleche_initiale:face -> face_fleche_finale:face -> face_curseur:face -> position:pos -> taille:size -> nom:string -> nb_positions:int -> orientation:int -> scroll_bar
val new_checkbox : parent:(panel option) -> face_normale:face -> face_click:face -> position:pos -> taille:size -> visible:bool -> nom:string -> checkbox
