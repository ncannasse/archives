class Socket {

	function new() : void {
		Std.neko("this.@s = socket_new(false)");
	}

	function connect( host : String, port : int ) : void {
		Std.neko("socket_connect(this.@s,host_resolve(host.@s),port)");
	}

	function send( data : String ) : void {
		Std.neko("socket_write(this.@s,data.@s)");
	}

	function setTimeout( t : int ) : void {
		Std.neko("socket_set_timeout(this.@s,t)");
	}

	function waitForRead() : void {
		Std.neko("socket_select($array(this.@s),null,null,null)");
	}

	function shutdown( read : bool, write : bool ) {
		Std.neko("socket_shutdown(this.@s,read,write)");
	}

	function receive() : String {
		return Std.neko("String.new(socket_read(this.@s))");
	}

	function readUntil( end: String ) : String {
		var buf = new StringBuf();
		
		var endChar = Std.neko(" $sget(end.@s,0) ");
		var last : int;
		do {	
			last = Std.neko("socket_recv_char(this.@s)");
			buf.addChar( last );
		} while( last != endChar );
		return buf.toString();
	}

	function readLine() : String {
		return readUntil("\n");
	}

	function read( length : int ) : String {
		var buf = new StringBuf();

		for( var i=0; i<length; i++ ){
			buf.addChar( Std.neko("socket_recv_char(this.@s)") );
		}
		return buf.toString();
	}

	function close() : void {
		Std.neko("socket_close(this.@s)");
	}

	static function resolve( host : String ) : Int32 {
		return Std.neko("host_resolve(host.@s)");
	}

	static function reverse( ip : Int32 ) : String {
		return Std.neko("String.new(host_reverse(ip))");
	}

	static var __init = Std.neko('
		socket_new = $loader.loadprim("std@socket_new",1);
		socket_connect = $loader.loadprim("std@socket_connect",3);
		socket_write = $loader.loadprim("std@socket_write",2);
		socket_read = $loader.loadprim("std@socket_read",1);
		socket_close = $loader.loadprim("std@socket_close",1);
		socket_set_timeout = $loader.loadprim("std@socket_set_timeout",2);
		socket_recv_char = $loader.loadprim("std@socket_recv_char",1);
		socket_select = $loader.loadprim("std@socket_select",4);
		socket_shutdown = $loader.loadprim("std@socket_shutdown",3);
		host_resolve = $loader.loadprim("std@host_resolve",1);
		host_reverse = $loader.loadprim("std@host_reverse",1);
	');

}
