native class Xml {

	static var MODULE = "mtypes/std";

	var att : 'a;
	var text : String;
	var cdata : bool;
	var node : String;
	var children : List<Xml>;
	var parent : Xml;

	function new( s : String ) : void;
	function get( att : String ) : String;
	function set( att : String, v : String ) : String;

	function nodes() : List<Xml>;
	function firstChild() : Xml;
	function firstNode() : Xml;

	function toString() : String;

}