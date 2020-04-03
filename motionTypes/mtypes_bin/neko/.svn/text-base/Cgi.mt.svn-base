class Cgi {

	static var MOD_NEKO : bool;

	static function getHostName() : String {
		return Std.neko("String.new(get_host_name())");
	}

	static function getClientIP() : String {
		return Std.neko("String.new(get_client_ip())");
	}

	static function getURI() : String {
		return Std.neko("String.new(get_uri())");
	}

	static function redirect( uri : String ) : void {
		return Std.neko("cgi_redirect(uri.@s)");
	}

	static function header( k : String, v : String ) : void {
		Std.neko("cgi_set_header(k.@s,v.@s)");
	}

	static function setReturnCode( r : int ) : void {
		Std.neko("set_return_code(r)");
	}

	static function getClientHeader( k : String ) : String {
		return Std.neko("String.new(get_client_header(k.@s))");
	}

	static function getParamsString() : String {
		return Std.neko("String.new(get_params_string())");
	}

	static function getPostData() : String {
		return Std.neko("String.new(get_post_data())");
	}

	static function getParams() : Hash<String> {
		return Std.neko('
			var h = Hash.new();
			var o = $new(null);
			var p = get_params();
			while( p != null ) {
				o.@s = p[0];
				h.set(o,String.new(p[1]));
				p = p[2];
			}
			return h;
		');
	}

	static function getCookies() : Hash<String> {
		return Std.neko('
			var h = Hash.new();
			var o = $new(null);
			var p = get_cookies();
			while( p != null ) {
				o.@s = p[0];
				h.set(o,String.new(p[1]));
				p = p[2];
			}
			return h;
		');
	}

	static function setCookie( k : String, v : String ) : void {
		Std.neko("set_cookie(k.@s,v.@s)");
	}

	static function urlEncode( s : String ) : String {
		return Std.neko("String.new(url_encode(s.@s))");
	}

	static function urlDecode( s : String ) : String {
		return Std.neko("String.new(url_decode(s.@s))");
	}

	static function getCwd() : String {
		return Std.neko("String.new(get_cwd())");
	}

	static function setMain( f : void -> 'a ) : void {
		Std.neko("set_main(f)");
	}

	static function getMultipartData( maxSize : int ) : Hash<String> {
		var h = new Hash();
		var buf = { r : null };
		var cur = { r : null };
		Std.neko("parse_multipart_data")(fun(p,_) {
			if( cur.r != null )
				h.set(cur.r,buf.r.toString());
			buf.r = new StringBuf();
			cur.r = new String(p);
			maxSize -= cur.r._length;
			if( maxSize < 0 )
				Std.throw("Max size reached");
		},fun(str,pos,len) {
			maxSize -= len;
			if( maxSize < 0 )
				Std.throw("Max size reached");
			buf.r.addSub(str,pos,len);
		});
		if( cur.r != null )
			h.set(cur.r,buf.r.toString());
		return h;
	}

	static var __init = Std.neko('
		get_env = $loader.loadprim("std@get_env",1);
		url_encode = $loader.loadprim("std@url_encode",1);
		url_decode = $loader.loadprim("std@url_decode",1);
		var ver = get_env("MOD_NEKO");
		if( ver != null ) {
			Cgi.MOD_NEKO = true;

			var lib = if( ver == "2" ) "mod_neko2@" else "mod_neko@";
			set_main = $loader.loadprim(lib+"cgi_set_main",1);
			get_host_name = $loader.loadprim(lib+"get_host_name",0);
			get_client_ip = $loader.loadprim(lib+"get_client_ip",0);
			get_uri = $loader.loadprim(lib+"get_uri",0);
			cgi_redirect = $loader.loadprim(lib+"redirect",1);
			cgi_set_header = $loader.loadprim(lib+"set_header",2);
			set_return_code = $loader.loadprim(lib+"set_return_code",1);
			get_client_header = $loader.loadprim(lib+"get_client_header",1);
			get_params_string = $loader.loadprim(lib+"get_params_string",0);
			get_post_data = $loader.loadprim(lib+"get_post_data",0);
			get_params = $loader.loadprim(lib+"get_params",0);
			get_cookies = $loader.loadprim(lib+"get_cookies",0);
			set_cookie = $loader.loadprim(lib+"set_cookie",2);
			get_cwd = $loader.loadprim(lib+"cgi_get_cwd",0);
			parse_multipart_data = try $loader.loadprim(lib+"parse_multipart_data",2) catch e function(a,b) { $throw("Not available") };
		} else {
			Cgi.MOD_NEKO = false;

			var content_type = $array("text/html");
			set_main = function(f) { @Cgi.main = f };
			get_host_name = function() { return "localhost" };
			get_client_ip = function() { return "127.0.0.1" };
			get_uri = function() { return "/?" + $loader.args[0] };
			cgi_redirect = function(v) { $print("Location: ",v,"\n") };
			cgi_set_header = function(h,v) { };
			set_return_code = function(i) { };
			get_client_header = function(h) { return null; };
			get_params_string = function() { return $loader.args[0] };
			get_post_data = function() { return "" };
			get_params = function() {
				var l = { ref => null };
				String.new($loader.args[0]).split(String.new(";")).iter(function(f) {
					var ll = f.split(String.new("="));
					if( ll.get_length() == 2 ) l.ref = $array(ll.at(0).@s,ll.at(1).@s,l.ref);
				});
				return l.ref;
			};
			get_cookies = function() { return null; }
			set_cookie = function(k,v) { $print("Cookie: ",k,"=",v,"\n"); };
			get_cwd = $loader.loadprim("std@get_cwd",0);
		}
	');

}
