intrinsic class String
{
	function String( string : String) : void;
	
	function toUpperCase() : String;
	function toLowerCase() : String;
	
	function charAt( index : int ) : String;
	function charCodeAt( index : int ) : int;

	function indexOf( value : String, startIndex : int) : int;
	function lastIndexOf( value : String, startIndex : int) : int;

	function slice( index1 : int, index2 : int) : String;
	function substring( index1: int, index2 : int) : String;
	function substr( index1 : int, index2 : int) : String;

	static function fromCharCode( c : int ):String;

	public var length : int;
}


