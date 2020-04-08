class sql.Result<'item> {

	var _length : int;
	var _nfields : int;
	var current : 'item;

	function next() : bool {
		Std.neko('
			var c = result_next(this.@r);
			this.current = c;
			if( c == null )
				return false;
			var f = $objfields(c);
			var i = 0;
			var l = $asize(f);
			while( i < l ) {
				var v = $objget(c,f[i]);
				if( $typeof(v) == $tstring )
					$objset(c,f[i],String.new(v));
				i = i + 1;
			}
		');
		return true;
	}

	function getResult( n : int ) : String {
		return Std.neko("String.new(result_get(this.@r,n))");
	}

	function getIntResult( n : int ) : int {
		return Std.neko("try{ result_get_int(this.@r,n) } catch e { null }");
	}

	function getFloatResult( n : int ) : float {
		return Std.neko("result_get_float(this.@r,n)");
	}

	function results() : List<'item> {
		return Std.neko('
			var l = List.new();
			while( this.next() )
			l.add(this.current);
			l;
		');
	}

	static var init = Std.neko('
		var lib = Sql.lib.@s;
		result_get_length = $loader.loadprim(lib+"@result_get_length",1);
		result_get_nfields = $loader.loadprim(lib+"@result_get_nfields",1);
		result_next = $loader.loadprim(lib+"@result_next",1);
		result_get = $loader.loadprim(lib+"@result_get",2);
		result_get_int = $loader.loadprim(lib+"@result_get_int",2);
		result_get_float = $loader.loadprim(lib+"@result_get_float",2);
		result_set_conv_date = $loader.loadprim(lib+"@result_set_conv_date",2);

		@sql_Result.current = null;
		@sql_Result.get_length = function() {
			return result_get_length(this.@r);
		};
		@sql_Result.get_nfields = function() {
			return result_get_nfields(this.@r);
		};
	');

}
