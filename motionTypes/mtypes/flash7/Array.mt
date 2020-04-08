native class Array<'item>
{
	public var length : int;

	function new() : void;

	function push(x : 'item) : int;
	function pop() : 'item;
	function shift() : 'item;
	function unshift( x : 'item ) : void;
	function slice( startIndex : int, endIndex : int) : Array<'item>;
	function join( delimiter : String) : String;
	function splice( startIndex : int, deleteCount : int) : void;
	function insert( index : int, x : 'item ) : void;
	function toString() : String;
	function reverse() : void;

	function sort( f : 'item -> 'item -> int ) : void;

	// ADDITIONAL FUNCTIONS
	function remove( x : 'item ) : bool;
	function duplicate() : Array<'item>;
}


