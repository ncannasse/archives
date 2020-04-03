open Osiris
open Vfs

type sprite
type inst 

type opt =
	| OShowInfos of bool
	| ODoShading of bool
	| OBGColor of (int * int * int)
	| OFullScreen of bool

type vector = {
	x : float;
	y : float;
	z : float;
}

type vector4 = {
	tx : float;
	ty : float;
	tx2 : float;
	ty2 : float;
}

exception Error of string

type color = int32

type gfx =
	| Sprite of sprite
	| Inst of inst

type texture_op =
	| Top_SelectArg1
	| Top_SelectArg2
	| Top_Modulate
	| Top_Modulate2X
	| Top_Modulate4X
	| Top_Add
	| Top_AddSigned
	| Top_AddSigned2X
	| Top_Subtract
	| Top_AddSmooth
	| Top_BlendDiffuseAlpha
	| Top_BlendTextureAlpha
	(* color op only *)
	| Top_ModulateAlpha_AddColor
	| Top_ModulateColor_AddAlpha 
	| Top_ModulateInvAlpha_AddColor
	| Top_ModulateInvColor_AddAlpha

type blend =
	| Blend_Zero
	| Blend_One
	| Blend_SrcColor 
	| Blend_InvSrcColor 
	| Blend_SrcAlpha 
	| Blend_InvSrcAlpha 
	| Blend_DestAlpha 
	| Blend_InvDestAlpha 
	| Blend_DestColor 
	| Blend_InvDestColor 
	| Blend_SrcAlphaSat

module Vector =
  struct
	type t = vector
	let add va vb = { x = va.x +. vb.x; y = va.y +. vb.y; z = va.z +. vb.z }
	let dist va vb = sqrt ( (va.x -. vb.x) *. (va.x -. vb.x ) +.
							(va.y -. vb.y) *. (va.y -. vb.y ) +.
							(va.z -. vb.z) *. (va.z -. vb.z ) )
	external point_line_dist : a:t -> b:t -> p:t -> float = "point_line_dist"
  end

module Matrix =
  struct
	type t
	type ref
	external id : unit -> t = "mat_id"
	external mult : t -> t -> t = "mat_mult"
	external rot : vector -> t = "mat_rot"
	external trans : vector -> t = "mat_trans"
	external scale : vector -> t = "mat_scale"
	external trs : vector -> vector -> vector -> t = "mat_trs"
	external eval : ref -> t = "mat_ref_eval"
  end

module Inst =
  struct
	type t = inst

	external attach : Vfs.fid -> t = "m3d_inst_attach"
	external detach : t -> unit = "m3d_inst_detach"
	external reset : t -> unit = "m3d_inst_reset"

	external is_visible : t -> bool = "m3d_inst_is_visible"
	external get_time : t -> float = "m3d_inst_get_time"
	external get_speed : t -> float = "m3d_inst_get_speed"
	external get_pos : t -> Matrix.ref = "m3d_inst_get_pos"

	external anim : t -> string option -> unit = "m3d_inst_set_current_anim"
	external anims : t -> string list = "m3d_inst_anim_list"
	external time : t -> float -> unit = "m3d_inst_set_time"
	external speed : t -> float -> unit = "m3d_inst_set_speed"
	external visible : t -> bool -> unit = "m3d_inst_set_visible"

	external pos : t -> Matrix.t -> unit = "m3d_inst_set_pos"
	external transform : t -> Matrix.t -> unit = "m3d_inst_do_transform"
	external dest : t -> Matrix.t -> float -> unit = "m3d_inst_set_dest"
	external color : t -> color -> unit = "m3d_inst_set_color"
	external fade : t -> color -> float -> unit = "m3d_inst_color_fade"
	external texture : t -> Vfs.fid -> unit = "m3d_inst_set_texture"

	external action : t -> (unit -> unit) -> unit = "m3d_inst_set_action"
	external do_action : t -> unit = "m3d_inst_do_action"

  end

module Sprite =
  struct
	type t = sprite

	external attach : fid -> vector4 -> t = "m3d_sprite_attach"
	external detach : t -> unit = "m3d_sprite_detach"

	external pos : t -> vector -> unit = "m3d_sprite_set_pos"
	external size : t -> (float * float) -> unit = "m3d_sprite_set_size"
	external face : t -> vector4 -> unit = "m3d_sprite_set_coords"
	external dest : t -> (float * float) -> float -> unit = "m3d_sprite_set_dest"
	external color : t -> color -> unit = "m3d_sprite_set_color"
	external fade : t -> color -> float -> unit = "m3d_sprite_set_fade"

	external get_pos : t -> vector = "m3d_sprite_get_pos"
	external get_size : t -> (float * float) = "m3d_sprite_get_size"
	external get_face : t -> vector4 = "m3d_sprite_get_coords"

	let all = { tx = 0. ; ty = 0. ; tx2 = 1. ; ty2 = 1. }

  end

module Render =
  struct
	external start : unit -> unit = "m3d_render_start"
	external stop : unit -> unit = "m3d_render_stop"
	external set_target : handle -> unit = "m3d_set_render_target"
	external set_transform : Matrix.t -> unit = "m3d_set_world_transform"
	external get_transform : unit -> Matrix.t = "m3d_get_world_transform"
	external do_transform : Matrix.t -> unit = "m3d_do_world_transform"
	external print : string -> unit = "m3d_print"
	external clear_print : unit -> unit = "m3d_clear_print"
	external pick : (float * float) -> Inst.t list = "m3d_pick"
	external garbage : unit -> unit = "m3d_do_garbage"
	external set_option : opt -> unit = "m3d_set_option"
	external update_time : unit -> unit = "m3d_update_time"
	external projection : (float * float) -> float -> (float * float) = "m3d_projection"
	external time : unit -> float = "m3d_get_render_time"
	external elapsed_time : unit -> float = "m3d_get_elapsed_time"

	external draw_line : vector -> vector -> color -> unit = "m3d_draw_line"
  end

module FX =
  struct

	type param_val =
		| PInt of int
		| PFloat of float
		| PBool of bool
		| PMatrix of Matrix.t
		| PVector of vector
		| PMatrixRef of Matrix.ref

	type param = (string * param_val)

	external load_library : string -> bool = "m3d_load_library"
	external get_names : unit -> string list = "m3d_get_fx_names"
	external play : string -> param list -> unit = "m3d_fx_play"

	external color_op : gfx -> texture_op -> unit = "m3d_fx_color_op"
	external alpha_op : gfx -> texture_op -> unit = "m3d_fx_alpha_op"
	external src_blend : gfx -> blend -> unit = "m3d_fx_src_blend"
	external dst_blend : gfx -> blend -> unit = "m3d_fx_dst_blend"
  end

module Camera =
  struct
	external zoom : float -> unit = "m3d_camera_zoom"
	external reset : unit -> unit = "m3d_camera_reset"
	external set_pos : vector -> unit = "m3d_camera_set_pos"
	external set_target : vector -> unit = "m3d_camera_set_target"
	external get_pos : unit -> vector = "m3d_camera_get_pos"
	external get_target : unit -> vector = "m3d_camera_get_target"
	external goto : vector -> vector -> float -> unit = "m3d_camera_goto"
	external stop : unit -> unit = "m3d_camera_stop"
  end

module Events =
  struct	
	external add_delayed : (unit -> unit) -> float -> unit = "m3d_event_add_delayed"
  end

module Color =
  struct

	let alpha_mask = (Int32.shift_left (Int32.of_int 0xFF) 24)
	let color_mask = Int32.of_int 0xFFFFFF

	let opaque c =
		Int32.logor alpha_mask c

	let compose r g b =
		opaque (Int32.of_int ( (r lsl 16) lor (g lsl 8) lor b))
		
	let rgb x =
		opaque (Int32.of_int x)

	let alpha c a =
		Int32.logor			
			(Int32.logand c color_mask)
			(Int32.shift_left (Int32.of_int a) 24)
	
	let zero = Int32.zero
	let full = Int32.minus_one

  end

module Text =
  struct

	type font
	type t

	external create_font : string -> int -> font = "m3d_text_create_font"
	external create : font -> t = "m3d_text_create"
	external remove : t -> unit = "m3d_text_remove"

	external string : t -> string -> unit = "m3d_text_set_string"
	external pos : t -> vector -> unit = "m3d_text_set_pos"
	external color : t -> color -> unit = "m3d_text_set_color"
	external dest : t -> (float * float) -> float -> unit = "m3d_text_set_dest"
	external center : t -> bool -> unit = "m3d_text_set_center"

  end

external init : handle -> bool -> bool = "m3d_init"
external handle_events : unit -> unit = "m3d_handle_events"
external do_close : unit -> unit = "m3d_close"

let close () =
	Gc.full_major();
	do_close();
	Gc.full_major();
	Gc.full_major();
	Gc.full_major()

;;
Callback.register_exception "m3d exception" (Error "");
