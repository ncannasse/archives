native class System
{

	static public var exactSettings : bool;
	static public var useCodePage : bool;

	static function setClipboard( s : String ) : bool;
	static function showSettings( panel : int ) : void;

	static var security : {
		sandboxType : String
	};

	// CALLBACKS
//	function onStatus( ??? ) : void;       // unknown parameters...

}
