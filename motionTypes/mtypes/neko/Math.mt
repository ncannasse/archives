class Math {

	static var PI : float = Std.neko(' $loader.loadprim("std@math_pi",0)() ');

	static var sqrt : float -> float = Std.neko(' $loader.loadprim("std@math_sqrt",1) ');
	static var abs : float -> float = Std.neko(' $loader.loadprim("std@math_abs",1) ');
	static var cos : float -> float = Std.neko(' $loader.loadprim("std@math_cos",1) ');
	static var sin : float -> float = Std.neko(' $loader.loadprim("std@math_sin",1) ');
	static var tan : float -> float = Std.neko(' $loader.loadprim("std@math_tan",1) ');
	static var log : float -> float = Std.neko(' $loader.loadprim("std@math_log",1) ');
	static var exp : float -> float = Std.neko(' $loader.loadprim("std@math_exp",1) ');
	static var pow : float -> float -> float = Std.neko(' $loader.loadprim("std@math_pow",2) ');
	static var ceil : float -> int = Std.neko(' $loader.loadprim("std@math_ceil",1) ');
	static var floor : float -> int = Std.neko(' $loader.loadprim("std@math_floor",1) ');
	static var round : float -> int = Std.neko(' $loader.loadprim("std@math_round",1) ');
	static var acos : float -> float = Std.neko(' $loader.loadprim("std@math_acos",1) ');
	static var asin : float -> float = Std.neko(' $loader.loadprim("std@math_asin",1) ');
	static var atan : float -> float = Std.neko(' $loader.loadprim("std@math_atan",1) ');
	static var atan2 : float -> float -> float = Std.neko(' $loader.loadprim("std@math_atan2",2) ');

	static function min( x : 'a, y : 'a) : 'a {
		if( x > y )
			return y;
		return x;
	}

	static function max( x : 'a, y : 'a) : 'a {
		if( y > x )
			return y;
		return x;
	}

	static var __init = null;
}