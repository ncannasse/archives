
type module_reg = {
	m_name : string;
	m_init : (unit -> unit) option;
	m_close : (unit -> unit) option;
	m_main	: (unit -> unit) option;
}

exception Exit_application
exception Exception_no_display
exception Click_handled

val register : module_reg -> unit
val fast_register : string -> unit
val on_register : (module_reg -> unit) -> unit

val on_exc : (string -> exn -> unit) -> unit
val process_exc : string -> exn -> unit