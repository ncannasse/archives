class sql.Connection {

	static var r_deadlock = new Regexp("Deadlock found");
	static var r_timeout = new Regexp("Lock wait timeout");

	function selectDB( db : String ) : void {
		Std.neko("sql_select_db(this.@c,db.@s)");
	}

	function isDeadlock( e : 'a ) : bool {
		return Std.neko("$typeof(e) == $tobject && e.deadlock == true");
	}

	function request( s : String ) : sql.Result<'a> {
		try {
			return Std.neko('
				var r = sql_request(this.@c,s.@s);
				result_set_conv_date(r,function(d) { return Date.new1(d) });
				var o = $new(null);
				$objsetproto(o,@sql_Result);
				o.@r = r;
				return o;
			');
		} catch( e ) {
			var msg = null;
			Std.neko("
				if( $typeof(e) != $tobject || $typeof(e.msg) != $tstring ) $rethrow(e);
				msg = String.new(e.msg);
			");
			if( r_deadlock.match(msg) || r_timeout.match(msg) )
				e.deadlock = true;
			Std.rethrow(e);
			return null;
		}
	}

	function close() : void {
		Std.neko("sql_close(this.@c)");
	}

	function escape( s : String ) : String {
		return Std.neko("String.new(sql_escape(this.@c,s.@s))");
	}

	static var __use_date = Date;
	static var init = Std.neko('
		sql_select_db = $loader.loadprim("mysql@select_db",2);
		sql_request = $loader.loadprim("mysql@request",2);
		sql_close = $loader.loadprim("mysql@close",1);
		sql_escape = $loader.loadprim("mysql@escape",2);
	');

}