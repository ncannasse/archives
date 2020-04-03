class File {

	function new( name : String ) : void {
		Std.neko("this.@f = name.@s");
	}

	function contents() : String {
		return Std.neko("String.new(file_contents(this.@f))");
	}

	function create() : void {
		Std.neko(' this.@f = file_open(this.@f,"wb")');
	}

	function open() : void {
		Std.neko(' this.@f = file_open(this.@f,"rb")');
	}

	function append() : void {
		Std.neko(' this.@f = file_open(this.@f,"ab")');
	}

	function close() : void {
		Std.neko('
			var f = this.@f;
			if( $typeof(f) != $tstring )
				this.@f = std_file_name(f);
			file_close(f);
		');
	}

	function exists() : bool {
		return Std.neko("sys_exists(this.@f)");
	}

	function read( n : int ) : String {
		return Std.neko('
			var s = $smake(n);
			var l = file_read(this.@f,s,0,n)
			if( l == n )
				return String.new(s);
			if( l == 0 )
				$throw(String.new("File.read"));
			String.new( $ssub(s,0,l) );
		');
	}

	function write( s : String ) : void {
		Std.neko('
			s = s.@s;
			var len = $ssize(s);
			if file_write(this.@f,s,0,len) != len
				$throw(String.new("File.write"));
		');
	}

	function flush() : void {
		Std.neko('
			file_flush(this.@f);
		');
	}

	function toString() : String {
		return Std.neko("String.new(this.__string())");
	}

	function __string() : void {
		return Std.neko('"#file:"+ file_name(this.@f)');
	}

	function name() : String {
		return Std.neko("String.new(file_name(this.@f))");
	}

	function absoluteName() : String {
		return Std.neko("String.new(file_full_path(file_name(this.@f)))");
	}

	static var __init = Std.neko('
		file_contents = $loader.loadprim("std@file_contents",1);
		file_open = $loader.loadprim("std@file_open",2);
		file_close = $loader.loadprim("std@file_close",1);
		file_read = $loader.loadprim("std@file_read",4);
		file_write = $loader.loadprim("std@file_write",4);
		file_flush = $loader.loadprim("std@file_flush", 1);
		sys_exists = $loader.loadprim("std@sys_exists",1);
		file_full_path = $loader.loadprim("std@file_full_path",1);
		std_file_name = $loader.loadprim("std@file_name",1);

		file_name = function(f) {
			if( $typeof(f) == $tstring )
				return f;
			return std_file_name(f);
		}
	');
}
