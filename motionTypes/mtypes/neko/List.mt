native class List<'item> {

	static var MODULE = "mtypes/std";

	var _length : int;

	function new() : void;
	function add( x : 'item ) : void;
	function push( x : 'item ) : void;
	function remove( x : 'item ) : bool;
	function at( p : int ) : 'item;
	function pop() : 'item;
	function first() : 'item;
	function last() : 'item;

	function iter( f : 'item -> void ) : void;
	function find( f : 'item -> 'a ) : 'a;
	function exists( f : 'item -> bool ) : bool;
	function join( sep : String ) : String;

	function toArray() : Array<'item>;
	function toString() : String;

	function map( f : 'item -> 'x ) : List<'x>;
	function filter( f : 'item -> bool ) : List<'x>;

}