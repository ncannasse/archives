native class flash.external.ExternalInterface {

	static var available : bool;
	static function addCallback( methodName : String, instance : {}, method : 'a ) : bool;
	static var call : 'a; // methodName : String -> arg1 -> arg2 -> ... -> ret

}