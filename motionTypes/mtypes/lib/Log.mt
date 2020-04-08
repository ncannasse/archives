native class Log {

	public static var ttrace : TextField ;
	public static var tprint : TextField ;

	static function reset() : void;
	static function clear() : void;
	static function trace(x : 'a) : void;
	static function print(x : 'a) : void;
	static function setColor(x : int) : void;
	static function init( mc : MovieClip ) : void;
	static function destroy() : void;
	static function toString(o:'a) : String;
}
