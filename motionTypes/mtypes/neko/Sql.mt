class Sql {

	static var lib = "mysql5";

	static function connect( params : {
		host : String,
		port : int,
		user : String,
		pass : String,
		socket : String
	} ) : sql.Connection {
		return Std.neko('
			var o = {
				host => params.host.@s,
				port => params.port,
				user => params.user.@s,
				pass => params.pass.@s,
				socket => (if( params.socket == null ) null else params.socket.@s)
			};
			var c = sql_connect(o);
			var s = $new(null);
			$objsetproto(s,@sql_Connection);
			s.@c = c;
			return s;
		');
	}

	static var __init = Std.neko('
		sql_connect = try $loader.loadprim(Sql.lib.@s+"@connect",1) catch e {
			Sql.lib.@s = "mysql";
			$loader.loadprim(Sql.lib.@s+"@connect",1)
		}
	');

}