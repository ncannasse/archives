native class Std {

	static function attachMC( mc : MovieClip , link : String, depth : int ) : MovieClip;
	static function createEmptyMC( mc : MovieClip, depth : int ) : MovieClip;
	static function duplicateMC( mc : MovieClip, depth : int ) : MovieClip;
	static function getVar( mc : MovieClip, v : String ) : 'a;
	static function setVar( mc : MovieClip, v : String, vval : 'a ) : void;
	static function getGlobal( v : String ) : 'a;
	static function setGlobal( v : String, vv : 'a ) : void;
	static function getRoot() : MovieClip;
	static function createTextField( mc : MovieClip, depth : int ) : TextField;

	static function getTimer() : int;
	static function hitTest( mc1 : MovieClip, mc2 : MovieClip ) : bool;
	static function random( x : int ) : int;

	static function xmouse() : float;
	static function ymouse() : float;

	static function escape( x : String ) : String;
	static function unescape( x : String ) : String;
	static function parseInt( x : String, n : int ) : int;
	static function toString( n : 'a ) : String;
	static function toStringBase( x : int, n : int ) : String;
	static function isNaN( n : float ) : bool;

	static function throw( x : 'a ) : 'b;
	static function cast( x : 'a ) : 'b;
	static function registerClass( link : String, x : 'a ) : void;
	static function deleteField( x : {} , f : String ) : void;

	static function fscommand( command:String, args:String ) : void;
}
