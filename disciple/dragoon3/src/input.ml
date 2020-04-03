
type state =
	| Up
	| Down
	| Pressed of int
	| Released

let is_down = function
	| Down | Pressed _ -> true
	| _ -> false

module Mouse =
  struct
	
	external pos : unit -> (float * float) = "winput_mouse_pos"
	external set_pos : (float * float) -> unit = "winput_set_mouse_pos"

	external left : unit -> state = "winput_mouse_left"
	external right : unit -> state = "winput_mouse_right"
	external middle : unit -> state	= "winput_mouse_middle"

	external reset_left : unit -> unit = "winput_mouse_reset_left"

  end

exception Invalid_keyname of string

module Keyb =
  struct

	type key

	let bindings = Hashtbl.create 0

	external key : string -> key = "winput_get_key"
	external name : key -> string = "winput_name_of_key"
	
	external state : key -> state = "winput_key_state"
	external current : unit -> key list = "winput_current_keys"

	let bind f l =
		let keys = List.rev (List.sort compare l) in
		Hashtbl.replace bindings keys f

	let unbind l =
		let keys = List.rev (List.sort compare l) in
		Hashtbl.remove bindings keys

	let process () =
		try
			(Hashtbl.find bindings (current())) ();
		with
			Not_found -> ()

	let left = key "Left"
	let right = key "Right"
	let up = key "Up"
	let down = key "Down"

  end

external init : Osiris.handle -> bool = "winput_init"
external input_process : unit -> unit = "winput_process"
external input_close : unit -> unit = "winput_close"

let process () =
	input_process();
	Keyb.process()

let close () =
	Hashtbl.clear Keyb.bindings;
	input_close()

;;
Callback.register_exception "input keyname exception" (Invalid_keyname "")
