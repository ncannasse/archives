native class Sound {

	function new( mc : MovieClip ) : void;

	function attachSound( s : String ) : void;
	function getBytesLoaded() : int;
	function getBytesTotal() : int;

	function loadSound( url:String, isStreaming:bool ) : void;

	function getPan() : float;
	function getVolume() : float;

	function setPan( pan : float ) : void;
	function setVolume( vol : float ) : void;

	function start( offset : float, loop : int ) : void;
	function stop() : void;

	var position : float;
	var duration : float;

	function onLoad( fl_success : bool ) : void;
	function onSoundComplete() : void;


	// only for mt.SoundManager
	var flag_playing : bool;
}