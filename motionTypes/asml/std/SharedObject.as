intrinsic class SharedObject {

	public var data : 'any;

	static function getLocal( name : String, path : String ) : SharedObject;
	function getSize() : Number;
	function clear() : void;
	function flush() : bool;


	function onStatus( st : 'any ) : void;

}