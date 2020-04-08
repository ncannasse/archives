native class String
{
	function new( string : String) : void;

	function toUpperCase() : String;
	function toLowerCase() : String;

	function charAt( index : int ) : String;
	function charCodeAt( index : int ) : int;

	function indexOf( value : String, startIndex : int) : int;
	function lastIndexOf( value : String, startIndex : int) : int;

	function slice( index1 : int, index2 : int) : String;
	function substring( start : int ) : String;
	function substr( start : int, length : int) : String;
	function split( delimiter : String ) : Array<String> ;

	function toString() : String;

	static function fromCharCode( c : int ):String;

	public var length : int;
}