open Intf
open Xml

(***** INTERFACE *****)

class type interface =
  object
    method add_component : string -> component -> unit
  	method add_panel : string -> panel -> unit 
	method add_button : string -> adv_button -> unit 
	method get_component : string -> component
	method get_panel : string -> panel
	method get_button : string -> adv_button 
	method destroy : unit -> unit
  end


val create_interface : xml -> panel option -> interface