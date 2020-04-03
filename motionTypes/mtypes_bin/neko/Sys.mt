class Sys {

	static function args() : Array<String> {
		return Std.neko('
			var a = $loader.args;
			var s = $asize(a);
			var i = 0;
			while( i < s ) {
				a[i] = String.new(a[i]);
				i = i + 1;
			}
			Array.new1(a);
		');
	}

	static function getCwd() : String {
		return Std.neko("String.new(sys_get_cwd())");
	}

	static function setCwd( s : String ) : void {
		Std.neko("String.new(sys_set_cwd(s.@s))");
	}

	static function setLocale( loc : String ) : bool {
		return Std.neko("set_locale(loc.@s)");
	}

	static function command( cmd : String ) : int {
		return Std.neko("sys_command(cmd.@s)");
	}

	static function readDir( dir : String ) : List<String> {
		return Std.neko('
			var a = sys_read_dir(dir.@s);
			var l = List.new();
			while( a != null ){
				l.add(String.new(a[0]));
				a = a[1];
			}
			return l;
		');
		
	}

	static function delete( file : String ) : void {
		Std.neko("sys_delete(file.@s)");
	}

	static function callstack() : Array<String> {
		return Std.neko("stack_to_array($callstack())");
	}

	static function excstack() : Array<String> {
		return Std.neko("stack_to_array($excstack())");
	}

	static function sleep( seconds : float ) : void {
		Std.neko("sys_sleep(seconds)");
	}

	static function time() : float {
		return Std.neko("sys_time()");
	}

	static var __init = Std.neko('
		set_locale = $loader.loadprim("std@set_time_locale",1);
		sys_command = $loader.loadprim("std@sys_command",1);
		sys_get_cwd = $loader.loadprim("std@get_cwd",0);
		sys_set_cwd = $loader.loadprim("std@set_cwd",1);
		sys_delete = $loader.loadprim("std@file_delete",1);
		sys_read_dir = $loader.loadprim("std@sys_read_dir",1);
		sys_sleep = $loader.loadprim("std@sys_sleep",1);
		sys_time = $loader.loadprim("std@sys_time",0);

		stack_to_array = function(a) {
			var i = 0;
			var l = $asize(a);
			while( i < l ) {
				var k = a[i];
				a[i] = if( k == null )
					"Called from a C function";
				else if( $typeof(k) == $tarray )
					"Called from "+k[0]+" line "+k[1];
				else
					"Called from "+k;
				i = i + 1;
			}
			return Array.new1(a);
		}
	');

}
