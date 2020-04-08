class Tools {

	static function delay( f : void -> void, time : int ) {
		var id;
		id = Std.getGlobal("setInterval")(fun() {
			Std.getGlobal("clearInterval")(id);
			f();
		},time);
	}

	static function replace(str : String,search : String,replace : String) {
		if( str == null )
			return null;

		var slen = search.length;
		if( slen == 1 )
			return str.split(search).join(replace);

		var npos, pos = 0;
		var s = "";
		while(true) {
			npos = str.indexOf(search,pos);
			if( npos == -1 )
				break;
			s += str.slice(pos,npos)+replace;
			pos = npos + slen;
		}
		return s + str.substring(pos);
	}

	static function lines( str : String ) : Array<String> {
		var i;
		var a = new Array();
		var p = 0;
		var l = str.length;
		for(i=0;i<l;i++) {
			var c = str.charCodeAt(i);
			if( c == 10 || c == 13 ) {
				a.push(str.slice(p,i));
				if( c == 13 && str.charCodeAt(i+1) == 10 )
					i++;
				p = i + 1;
			}
		}
		a.push(str.slice(p,i));
		return a;
	}

	static function shuffle( _a ) {
		var i;
		var a = _a.duplicate();
		var l = a.length;
		for(i=0;i<l;i++) {
			var p1 = Std.random(l);
			var p2 = Std.random(l);
			var s = a[p1];
			a[p1] = a[p2];
			a[p2] = s;
		}
		return a;
	}

	static function rootDelta( mc : MovieClip ) : { dx : float, dy : float } {
		var dx = 0;
		var dy = 0;
		while( mc._parent != null ) {
			dx += mc._x;
			dy += mc._y;
			mc = mc._parent;
		}
		return { dx : dx, dy : dy };
	}

	static function padZeros( x : int, ndigits : int ) : String {
		var s = string(x);
		while( s.length < ndigits )
			s = "0"+s;
		return s;
	}

	static function randomProbas( a : Array<int> ) : int {
        var n = 0;
        var i;
        for(i=a.length-1;i>=0;i--)
            n += a[i];
        n = Std.random(n);
        i = 0;
        while( n >= a[i] ) {
            n -= a[i];
            i++;
        }
        return i;
	}

	static function distMC( mc1 : MovieClip, mc2 : MovieClip ) : float {
		var b1 = mc1.getBounds(mc1);
		var b2 = mc2.getBounds(mc1);
		var dx = (b1.xMin + b1.xMax) / 2 - (b2.xMin + b2.xMax) / 2;
		var dy = (b1.yMin + b1.yMax) / 2 - (b2.yMin + b2.yMax) / 2;
		return Math.sqrt(dx*dx+dy*dy);
	}

	static function subs( mc : MovieClip ) : Array<MovieClip> {
		var a = new Array();
		downcast(Std).forin(mc,fun(k) {
			var mc2 = Std.cast(mc)[k];
			if( downcast(Std)._typeof(mc2) == "movieclip" && mc2._parent == mc )
				a.push(mc2);
		});
		return a;
	}

	static var __x = "$x".substring(1);
	static var __y = "$y".substring(1);

	static function localToGlobal( mc : MovieClip, x : float, y : float ) : { x : float, y : float } {
		var o = new Hash();
		o.set(__x,x);
		o.set(__y,y);
		downcast(mc).localToGlobal(o);
		return {
			x : o.get(__x),
			y : o.get(__y)
		};
	}

	static function globalToLocal( mc : MovieClip, x : float, y : float ) : { x : float, y : float } {
		var o = new Hash();
		o.set(__x,x);
		o.set(__y,y);
		downcast(mc).globalToLocal(o);
		return {
			x : o.get(__x),
			y : o.get(__y)
		};
	}

	static var strings = null;

	static function initStrings() {
		var a = ["$linear","$radial","$matrixType","$box","$x","$y","$w","$h","$r"];
		var i;
		strings = new Array();
		for(i=0;i<a.length;i++) {
			var s = a[i];
			strings[i] = s.substring(1);
		}
	}

	static function beginGradientFill(
		mc : MovieClip,
		linear : bool,
		colors : Array<int>,
		alphas : Array<int>,
		coefs : Array<int>,
		matrix : {
			x : float,
			y : float,
			w : float,
			h : float,
			r : float
		}
	) {
		if( strings == null )
			initStrings();
		var m = new Hash();
		m.set(strings[2],Std.cast(strings[3]));
		m.set(strings[4],matrix.x);
		m.set(strings[5],matrix.y);
		m.set(strings[6],matrix.w);
		m.set(strings[7],matrix.h);
		m.set(strings[8],matrix.r);
		downcast(mc).beginGradientFill(
			linear?strings[0]:strings[1],
			colors,
			alphas,
			coefs,
			m
		);
	}

}