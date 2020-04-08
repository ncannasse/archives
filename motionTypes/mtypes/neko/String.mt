native class String {

	static var DEPENDENCY = StringTools;
	static var MODULE = "mtypes/std";
	static var BASE64MIME : String;

	var _length : int;

	function new( s : String ) : void;
	function charAt( c : int ) : String;
	function at( c : int ) : int;
	function sub( start : int, len : int ) : String;
	function split( s : String ) : List<String>;
	function toString() : String;

	static function trim( s : String ) : String;
	static function ltrim( s : String ) : String;
	static function rtrim( s : String ) : String;
	static function right( s : String, l : int ) : String;
	static function left( s : String, l : int ) : String;
	static function lpad( s : String, c : String, l : int ) : String;
	static function rpad( s : String, c : String, l : int ) : String;
	static function upperCase( s : String ) : String;
	static function lowerCase( s : String ) : String;
	static function ucfirst( s : String ) : String;

	static function find( s : String, need : String, start : int ) : int;
	static function rfind( s : String, need : String, start : int ) : int;
	static function replace( s : String, need : String, repl : String ) : String;

	static function baseEncode( s : String, base : String ) : String;
	static function baseDecode( s : String, base : String ) : String;

	static function msub( s : String, start : int, len: int ) : String;
	static function md5( s : String ) : String;

	static function startsWith( s : String, s2 : String ) : bool;
	static function endsWith( s : String, s2 : String ) : bool;

	static function utf8Encode( s : String ) : String;
	static function utf8Decode( s : String ) : String;
	static function utf8Iter( s : String, f : int -> void ) : void;
	
}
