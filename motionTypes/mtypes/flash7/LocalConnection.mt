native class LocalConnection {

	function new() : void;
	function send( cnx : String, method : String, p1 : 'a, p2 : 'b, p3 : 'c ) : bool;
	function connect( cnx : String ) : bool;
	function close() : void;

}