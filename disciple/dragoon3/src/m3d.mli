type sprite
type inst

exception Error of string

type gfx =
	| Sprite of sprite
	| Inst of inst

type opt =
	| OShowInfos of bool
	| ODoShading of bool
	| OBGColor of (int * int * int)
	| OFullScreen of bool

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

type color

module Vector :
  sig
	type t = vector
	val add : t -> t -> t
	val point_line_dist : a:t -> b:t -> p:t -> float
	val dist : t -> t -> float
  end

module Matrix :
  sig
    type t
	type ref

    val mult : t -> t -> t
	val id : unit -> t
    val rot : vector -> t
    val trans : vector -> t
    val scale : vector -> t
	val trs : vector -> vector -> vector -> t

	val eval : ref -> t
  end

module Inst :
  sig
	type t = inst

	val attach : Vfs.fid -> t
	val detach : t -> unit
	val reset : t -> unit

	val is_visible : t -> bool
	val get_time : t -> float
	val get_speed : t -> float
	val get_pos : t -> Matrix.ref	

	val anim : t -> string option -> unit
	val anims : t -> string list
	val time : t -> float -> unit
	val speed : t -> float -> unit
	val visible : t -> bool -> unit
	val color : t -> color -> unit
	val fade : t -> color -> float -> unit
	val texture : t -> Vfs.fid -> unit

	val pos : t -> Matrix.t -> unit	
	val transform : t -> Matrix.t -> unit
	val dest : t -> Matrix.t -> float -> unit

	val action : t -> (unit -> unit) -> unit
	val do_action : t -> unit
  end

module Render :
  sig
    val start : unit -> unit
    val stop : unit -> unit
    val set_target : Osiris.handle -> unit
    val set_transform : Matrix.t -> unit
	val get_transform : unit -> Matrix.t
	val do_transform : Matrix.t -> unit
	val print : string -> unit	
	val clear_print : unit -> unit
	val pick : (float * float) -> Inst.t list
	val garbage : unit -> unit
	val set_option : opt -> unit
	val update_time : unit -> unit
	val projection : (float * float) -> float -> (float * float)
	val time : unit -> float
	val elapsed_time : unit -> float

	val draw_line : vector -> vector -> color -> unit
  end

module Camera :
  sig
    val zoom : float -> unit
    val reset : unit -> unit
    val set_pos : vector -> unit
    val set_target : vector -> unit
	val get_pos : unit -> vector
	val get_target : unit -> vector    
	val goto : vector -> vector -> float -> unit
	val stop : unit -> unit
  end

module Sprite :
  sig
	type t = sprite

	val attach : Vfs.fid -> vector4 -> t
	val detach : t -> unit

	val pos : t -> vector -> unit
	val size : t -> (float * float) -> unit	
	val dest : t -> (float * float) -> float -> unit
	val face : t -> vector4 -> unit
	val color : t -> color -> unit
	val fade : t -> color -> float -> unit

	val get_pos : t -> vector
	val get_size : t -> (float * float)	
	val get_face : t -> vector4

	val all : vector4
  end


module Events :
  sig
	val add_delayed : (unit -> unit) -> float -> unit
  end

module Color :
  sig
	val rgb : int -> color
	val compose : int -> int -> int -> color
	val opaque : color -> color
	val alpha : color -> int -> color
	val zero : color
	val full : color
  end

module FX :
  sig	 
	type param_val =
		| PInt of int
		| PFloat of float
		| PBool of bool
		| PMatrix of Matrix.t
		| PVector of vector
		| PMatrixRef of Matrix.ref

	type param = (string * param_val)

	val load_library : string -> bool
	val get_names : unit -> string list
	val play : string -> param list -> unit

	val color_op : gfx -> texture_op -> unit
	val alpha_op : gfx -> texture_op -> unit
	val src_blend : gfx -> blend -> unit
	val dst_blend : gfx -> blend -> unit
  end

module Text :
  sig

	type font
	type t

	val create_font : string -> int -> font
	
	val create : font -> t	

	val remove : t -> unit
	val string : t -> string -> unit
	val pos : t -> vector -> unit
	val color : t -> color -> unit
	val dest : t -> (float * float) -> float -> unit
	val center : t -> bool -> unit

  end

val init : Osiris.handle -> bool -> bool
val handle_events : unit -> unit
val close : unit -> unit