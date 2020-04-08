native class LoadVars {

	function new() : void;

	function addRequestHeader( name : String, value : String ) : void;
	function getBytesLoaded() : int;
	function getBytesTotal() : int;
	function load( url : String ) : void;
	function send( url : String, target : String, method : String ) : void;
	function sendAndLoad( url : String, target : LoadVars, method : String ) : void;

	function toString() : String;

	var contentType : String;
	var loaded : bool;

	function onData( d : String ) : void;
	function onLoad( success : bool ) : void;

}