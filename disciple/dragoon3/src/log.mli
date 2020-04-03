type logtype =
	MSG
	| WARNING
	| ERROR
	| TRACE

type logmsg = logtype * string

val log : logtype -> string -> unit
val fetch : unit -> logmsg option
val event : (unit -> unit) ref
