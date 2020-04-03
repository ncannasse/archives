
type module_reg = {
	m_name : string;
	m_init : (unit -> unit) option;
	m_close : (unit -> unit) option;
	m_main	: (unit -> unit) option;
}

exception Exit_application
exception Exception_no_display
exception Click_handled

let do_register = ref None

let on_register func =
	do_register := Some func

let register mr =
	match !do_register with
	| None -> failwith "Register event not set"
	| Some func -> func mr

let fast_register name = 
	register {
		m_name = name;
		m_init = None;
		m_close = None;
		m_main = None;
	}

let exc_fun = ref (fun _ _ -> ())

let on_exc f = exc_fun := f

let process_exc str e = !exc_fun str e