intrinsic class Array
{
	public var length : int;

	function Array() : void;
	
	function push(x : 'any) : int;
	function pop() : 'any;
	function shift() : 'any;
	function unshift( x : 'any ) : void;
	function slice( startIndex : int, endIndex : int) : Array;
	function join( delimiter : String) : String;
	function splice( startIndex : int, deleteCount : int) : Array;
	function toString() : String;
	function reverse() : void;

	function sortOn( x : String ) : void;
	function sort( f ) : void;

	// ADDITIONAL FUNCTIONS
	function remove( x : 'any ) : bool;
	function shuffle();
}


