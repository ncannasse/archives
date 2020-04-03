intrinsic class Key
{
	static public var ALT         : int =   18;
	static public var ENTER       : int =   13;
	static public var SPACE       : int =   32;
	static public var UP          : int =   38;
	static public var DOWN        : int =   40;
	static public var LEFT        : int =   37;
	static public var RIGHT       : int =   39;
	static public var PGUP        : int =   33;
	static public var PGDN        : int =   34;
	static public var HOME        : int =   36;
	static public var END         : int =   35;
	static public var TAB         : int =   9;
	static public var CONTROL     : int =   17;
	static public var SHIFT       : int =   16;
	static public var ESCAPE      : int =   27;
	static public var INSERT      : int =   45;
	static public var DELETEKEY   : int =   46;
	static public var BACKSPACE   : int =   8;
	static public var CAPSLOCK    : int =   20;

	static function getAscii() : int;
	static function getCode() : int;
	static function isDown(code: int) : bool;
	static function isToggled(code : int) : bool;

    static function addListener(listener : KeyListener) : void;
}


