
intrinsic class XMLSocket 
{
	function XMLSocket() : void;
	function connect( host : String, port : int ) : bool;
	function close() : void;
	function send( data : String ) : void;


	function onClose() : void;
	function onConnect( flag : bool ) : void;
	function onData( data : String ) : void;
	function onXML( x : XML ) : void;

}

