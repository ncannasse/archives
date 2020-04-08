native class Hash<'item> {

	function new() : void;
	function get( k : String ) : 'item;
	function set( k : String, i : 'item ) : void;
	function remove( k : String ) : bool;
	function exists( k : String ) : bool;
	function iter( f : String -> 'item -> void ) : void;

}