native class StringBuf {

	static var MODULE = "mtypes/std";

	function new() : void;
	function add( x : 'a ) : void;
	function addSub( s : String, p : int, len : int ) : void;
	function addChar( c : int ) : void;
	function toString() : String;

}