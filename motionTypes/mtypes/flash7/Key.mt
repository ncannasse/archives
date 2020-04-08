native class Key
{
	static public var ALT         : int;
	static public var ENTER       : int;
	static public var SPACE       : int;
	static public var UP          : int;
	static public var DOWN        : int;
	static public var LEFT        : int;
	static public var RIGHT       : int;
	static public var PGUP        : int;
	static public var PGDN        : int;
	static public var HOME        : int;
	static public var END         : int;
	static public var TAB         : int;
	static public var CONTROL     : int;
	static public var SHIFT       : int;
	static public var ESCAPE      : int;
	static public var INSERT      : int;
	static public var DELETEKEY   : int;
	static public var BACKSPACE   : int;
	static public var CAPSLOCK    : int;

	static public var _listeners : Array<{onKeyDown:void->void, onKeyUp:void->void}>
	
	static function getAscii() : int;
	static function getCode() : int;
	static function isDown(code: int) : bool;
	static function isToggled(code : int) : bool;


    static function addListener( listener:{onKeyDown:void->void, onKeyUp:void->void});
    static function removeListener( listener:{onKeyDown:void->void, onKeyUp:void->void});
}


