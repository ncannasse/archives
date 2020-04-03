intrinsic class Sound {

	static function Sound( mc : MovieClip ) : void;

	function attachSound( s : String ) : void;
	function getBytesLoaded() : number;
	function getBytesTotal() : number;

	function getPan() : number;
	function getVolume() : number;
	
	function setPan( pan : number ) : void;
	function setVolume( vol : number ) : void;

	function start( offset : Number, loop : number ) : void;
	function stop() : void;

	var position : float;
	var duration : float;

	function onLoad() : void;
	function onSoundComplete() : void;
}