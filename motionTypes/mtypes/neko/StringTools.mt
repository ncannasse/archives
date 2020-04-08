class StringTools {

	static function left( str : String, l : int ) : String {
		if( l >= 0 ){
			return str.sub(0,l);
		}else{
			return str.sub(0,str._length + l);
		}
	}
	static function right( str : String, l : int ) : String {
		if( l >= 0 ){
			return str.sub(str._length - l,l);
		}else{
			return str.sub(-l,str._length + l);
		}
	}

	static var __init = Std.neko('
		base_encode = $loader.loadprim("std@base_encode",2);
		base_decode = $loader.loadprim("std@base_decode",2);
		make_md5 = $loader.loadprim("std@make_md5", 1);
		utf8_buf_alloc = $loader.loadprim("std@utf8_buf_alloc",1);
		utf8_buf_add = $loader.loadprim("std@utf8_buf_add",2);
		utf8_buf_content = $loader.loadprim("std@utf8_buf_content",1);
		utf8_iter = $loader.loadprim("std@utf8_iter",2);

		String.utf8Encode = function( str ){
			var sl = $ssize( str.@s );
			var buf = utf8_buf_alloc( sl );
			var i = 0;
			while( i < sl ){
				utf8_buf_add(buf,$sget(str.@s,i));
				i += 1;
			}
			return String.new( utf8_buf_content(buf) );
		}

		String.utf8Decode = function( str ){
			var sl = $ssize( str.@s );
			var ret = $smake(sl);
			var i = $array(0);
			utf8_iter(str.@s,function(val){
				if( val == 8364 ){
					val = 164;
				}else{
					if( val > 255 ){
						$throw("utf8Decode: bad char ("+val+")");
					}
				}
				$sset(ret,i[0],val);
				i[0] += 1;
			});
			return String.new( $ssub(ret,0,i[0]) );
		}

		String.utf8Iter = function(s,f) {
			utf8_iter(s.@s,f);
		}

		String.md5 = function( str ){
			return String.new(base_encode(make_md5(str.@s), "0123456789abcdef"));
		}

		String.ltrim = function( str ){
			var l = $ssize(str.@s);
			var i = 0;
			var v = $sget(str.@s,i);
			while( i < l && ( (v >= 9 && v <= 13) ||  v == 32 ) ){
				i = i + 1;
				v = $sget(str.@s,i);
			}

			return String.new($ssub(str.@s,i,l-i));
		}

		String.rtrim = function( str ){
			var l = $ssize(str.@s);
			var i = 0;
			var v = $sget(str.@s,l-i-1);
			while( i < l && ( (v >= 9 && v <= 13) ||  v == 32 ) ){
				i = i + 1;
				v = $sget(str.@s,l-i-1);
			}

			return String.new($ssub(str.@s,0,l-i));
		}

		String.trim = function( str ){
			return String.ltrim(String.rtrim(str));
		}

		String.rpad = function( str, c, l ){
			var s = $scopy(str.@s);
			var sl = $ssize(s);
			var cl = $ssize(c.@s);
			while( sl < l ){
				if( l - sl < cl ){
					s = s + $ssub(c.@s,0,l-sl);
					sl = l;
				}else{
					s = s + c.@s;
					sl = sl + cl;
				}
			}
			return String.new(s);
		}

		String.lpad = function( str, c, l ){
			var s = "";
			var sl = $ssize(str.@s);
			var cl = $ssize(c.@s);
			while( sl < l ){
				if( l - sl < cl ){
					s = s + $ssub(c.@s,0,l-sl);
					sl = l;
				}else{
					s = s + c.@s;
					sl = sl + cl;
				}
			}
			return String.new(s+$scopy(str.@s));
		}

		String.lowerCase = function(str){
			var s = $scopy(str.@s);
			var sl = $ssize(s);
			var i = 0;
			while( i < sl ){
				var v = $sget(s,i);
				if( v >= 65 && v <= 90 ){
					$sset(s,i,v+32);
				}
				i = i + 1;
			}
			return String.new(s);
		}

		String.upperCase = function(str){
			var s = $scopy(str.@s);
			var sl = $ssize(s);
			var i = 0;
			while( i < sl ){
				var v = $sget(s,i);
				if( v >= 97 && v <= 122 ){
					$sset(s,i,v-32);
				}
				i = i + 1;
			}
			return String.new(s);
		}

		String.ucfirst = function( str ){
			var s = $scopy(str.@s);
			var v = $sget(s,0);
			if( v >= 97 && v <= 122 ){
				$sset(s,0,v-32);
			}
			return String.new(s);
		}

		String.find = function( str, need, i ){
			if( i >= $ssize(str.@s) ) return null;

			return $sfind( str.@s, i, need.@s );
		}

		String.rfind = function( str, need, i ){
			var s = str.@s;
			var l = $ssize(s);
			var ns = need.@s;
			var nl = $ssize(ns);
			if( i < 0 ) $throw(String.new("startpos: "+i+" < 0"));
			while( i < l - nl + 1 ){
				if( $sget(s,l-i-1) == $sget(ns,nl-1) ){
					var ok = l-i-nl;
					var t = 1;
					while( t < nl ){
						if( $sget(s,l-i-t-1) != $sget(ns,nl-1-t) ){
							ok = null;
							break;
						}
						t = t + 1;
					}
					if( ok != null) return ok;
				}
				i = i + 1;
			}
			return null;
		}

		String.replace = function( str, need, repl ){
			return str.split(need).join(repl);
		}

		String.baseEncode = function( s, base ){
			return String.new(base_encode(s.@s,base.@s));
		}

		String.baseDecode = function( s, base ){
			return String.new(base_decode(s.@s,base.@s));
		}

		String.BASE64MIME = String.new("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

		String.msub = function( s, start, len ){
			var sl = $ssize(s.@s);
			if( sl == 0 ) return s;

			if( start < 0 ){
				start = sl + start;
			}
			if( len == null ){
				len = sl - start;
			}else if( len < 0 ){
				len = sl + len - start;
			}

			if( start + len > sl ){
				len = sl - start;
			}

			return String.new( $ssub(s.@s,start,len) );
		}

		String.startsWith = function( s, s2 ){
			var sl = $ssize(s.@s);
			var s2l = $ssize(s2.@s);
			if( s2l > sl ) return false;
			return $ssub(s.@s,0,s2l) == s2.@s;
		}

		String.endsWith = function( s, s2 ){
			var sl = $ssize(s.@s);
			var s2l = $ssize(s2.@s);
			if( s2l > sl ) return false;
			return $ssub(s.@s,sl-s2l,s2l) == s2.@s;
		}


		String.right = StringTools.right;
		String.left = StringTools.left;


	');

}
