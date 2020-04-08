class PrizeeCodec {

	static var chars = "@azAZ09-";
	static var cc_at = chars.charCodeAt(0);
	static var cc_a = chars.charCodeAt(1);
	static var cc_z = chars.charCodeAt(2);
	static var cc_am = chars.charCodeAt(3);
	static var cc_zm = chars.charCodeAt(4);
	static var cc_0 = chars.charCodeAt(5);
	static var cc_9 = chars.charCodeAt(6);
	static var cc_uscore = chars.charCodeAt(7);

	static function c64(x,n) {
		var s = "";
		var chr = String.fromCharCode;
		while( n > 0 ) {
			var i = x&63;
			x >>= 6;
			if( i < 10 )
				s += chr(i+cc_0);
			else {
				if( i < 36 )
					s+=chr(i-10+cc_a);
				else {
					if( i < 62 )
						s+=chr(i-36+cc_am);
					else {
						if( i==62 )
							s+= chr(cc_at);
						else
							s+= chr(cc_uscore);
					}
				}
			}
			n--;
		}
		return s;
	}

	static function d64(s,n) {
		var x = 0;
		while( n > 0 ) {
			var c = s.charCodeAt(n-1);
			if( c >= cc_0 && c <= cc_9 )
				c -= cc_0;
			else {
				if( c >= cc_a && c <= cc_z )
					c += 10-cc_a;
				else {
					if( c >= cc_am && c <= cc_zm )
						c += 36-cc_am;
					else {
						if( c == cc_at )
							c = 62;
						else
							c = 63;
					}
				}
			}
			x+= c<<((n-1)*6);
			n--;
		}
		return x;
	}

	static function encode( key : int, vals : Array<int>, sizes : Array<int> ) : String {
		var i;
		var s = "";
		for(i=0;i<vals.length;i++)
			s += c64(vals[i]^key,sizes[i]);
		return s;
	}

	static function decode( s : String ) : Hash<String> {
		var p = 0;
		var chr = String.fromCharCode;
		var a = new Hash();
		var l = d64(s,2);
		var j;
		p++;
		while(l > 0 ) {
			var k = "";
			var v = "";
			for(j=0;j<l;j++,p++)
				k += chr(d64(s.substring(p*2),2)^0xE4);
			l = d64(s.substring(p*2),2);
			p++;
			for(j=0;j<l;j++,p++)
				v += chr(d64(s.substring(p*2),2)^0x77);
			a.set(k,v);
			l = d64(s.substring(p*2),2);
			p++;
			if( p > s.length )
				return null;
		}
		return a;
	}
}