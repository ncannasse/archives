
function delete( x : 'any ) : void { }
function trace( x : 'any ) : void { }
function ord( x : String ) : number { return 0; }
function chr( x : int ) : String { return ""; }
function random(x : int) : number { return 0; }
function int(x : number) : number { return 0; }
function string( x : number ) : String { return ""; }
function registerClass( link : String, cl : 'any ) : void { }
function parseInt( x : String, base : int ) : Number { return 0; }
function getTimer() : int { return 0; }
function getURL( url : String, frame : String, params : String ) : void { }
function loadMovie( url : String, target : MovieClip ) : void { }

intrinsic class Std {

	static function attachMC( mc : MovieClip , link : String, depth : int ) : MovieClip;
	static function createEmptyMC( mc : MovieClip, depth : int ) : MovieClip;
	static function duplicateMC( mc : MovieClip, depth : int ) : MovieClip;
	static function getVar( mc : MovieClip, v : String ) : 'any;
	static function setVar( mc : MovieClip, v : String, vval : 'any ) : void;
	static function getGlobal( v : String ) : 'any;
	static function getRoot() : 'any;
	static function createTextField( mc : MovieClip, depth : int, x : number, y : number, width : int, height : int ) : TextField;

	static function cast(x : 'a) : 'b;

	static function hitTest( mc1 : MovieClip, mc2 : MovieClip ) : bool;
	static function replace( str : String, sub : String, by : String ) : String;

	static function fps() : float;
	static public var tmod : float = 0;
	static public var deltaT : float = 0;
	static public var frameCount : int = 0;

	static function randomProbas( a : Array ) : int;
	static function newHash() : hash;

	static function xmouse() : Number;
	static function ymouse() : Number;
}
