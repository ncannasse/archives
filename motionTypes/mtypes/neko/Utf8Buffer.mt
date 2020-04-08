class Utf8Buffer {

	function new() {
		Std.neko("this.b = utf8_buf_alloc(0)");
	}

	function add( x : 'a ) {
		Std.neko("
			var b = this.b;
			utf8_iter($string(x),function(c) { utf8_buf_add(b,c) });
		");
	}

	function addChar( c : int ) {
		Std.neko("utf8_buf_add(this.b,c)");
	}

	function toString() : String {
		return Std.neko("utf8_buf_content(this.b)");
	}

	static var __init = Std.neko('
		utf8_buf_alloc = $loader.loadprim("std@utf8_buf_alloc",1);
		utf8_buf_add = $loader.loadprim("std@utf8_buf_add",2);
		utf8_buf_content = $loader.loadprim("std@utf8_buf_content",1);
		utf8_iter = $loader.loadprim("std@utf8_iter",2);
	');

}