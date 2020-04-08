native class Int32 {

	static var MODULE = "mtypes/std";

	var __int32 : void;

	static function of_int(x : int) : Int32;
	static function to_int(x : Int32) : int;
	static function add(x : Int32, y : Int32) : Int32;
	static function sub(x : Int32, y : Int32) : Int32;
	static function mul(x : Int32, y : Int32) : Int32;
	static function div(x : Int32, y : Int32) : Int32;
	static function mod(x : Int32, y : Int32) : Int32;
	static function shl(x : Int32, y : Int32) : Int32;
	static function shr(x : Int32, y : Int32) : Int32;
	static function ushr(x : Int32, y : Int32) : Int32;
	static function and(x : Int32, y : Int32) : Int32;
	static function or(x : Int32, y : Int32) : Int32;
	static function xor(x : Int32, y : Int32) : Int32;
	static function neg(x : Int32, y : Int32) : Int32;
	static function complement(x : Int32, y : Int32) : Int32;
	static function compare(x : Int32, y : Int32) : int;

}