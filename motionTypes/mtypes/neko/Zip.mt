class Zip {

	static function compress( s : String ) : String {
		var out = null;
		Std.neko('
			s = s.@s;
			var d = @deflate_init(9);
			@set_flush_mode(d,"FINISH");
			var size = @deflate_bound(d,$ssize(s));
			out = $smake(size);
			var r = @deflate_buffer(d,s,0,out,0);
			if( $not(r.done) || r.read != $ssize(s) )
				$throw("Compression failed");
			out = $ssub(out,0,r.write);
			@deflate_end(d);
		');
		return new String(out);
	}

	static function uncompress( s : String ) : String {
		var out = null;
		Std.neko('
			var d = @inflate_init(null);
			var tmp = $smake(1 << 16);
			var b = @buffer_new();
			var pos = 0;
			@set_flush_mode(d,"SYNC");
			while( true ) {
				var r = @inflate_buffer(d,s.@s,pos,tmp,0);
				@buffer_add_sub(b,tmp,0,r.write);
				pos += r.read;
				if( r.done )
					break;
			}
			@inflate_end(d);
			out = @buffer_to_string(b);
		');
		return new String(out);
	}

	static var __init = Std.neko('
		@deflate_init = $loader.loadprim("zlib@deflate_init",1);
		@deflate_bound = $loader.loadprim("zlib@deflate_bound",2);
		@deflate_buffer = $loader.loadprim("zlib@deflate_buffer",5);
		@deflate_end = $loader.loadprim("zlib@deflate_end",1);
		@set_flush_mode = $loader.loadprim("zlib@set_flush_mode",2);

		@inflate_init = $loader.loadprim("zlib@inflate_init",1);
		@inflate_buffer = $loader.loadprim("zlib@inflate_buffer",5);
		@inflate_end = $loader.loadprim("zlib@inflate_end",1);

		@buffer_new = $loader.loadprim("std@buffer_new",0);
		@buffer_add_sub = $loader.loadprim("std@buffer_add_sub",4);
		@buffer_to_string = $loader.loadprim("std@buffer_string",1);
	');

}