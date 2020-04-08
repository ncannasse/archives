native class MovieClipLoader {

	function new() : void;

	function getProgress( mc : MovieClip ) : { bytesTotal : int, bytesLoaded : int };
	function loadClip( url : String, mc : MovieClip ) : bool;
	function unloadClip( mc : MovieClip ) : void;

	function onLoadComplete( mc : MovieClip ) : void;
	function onLoadError( mc : MovieClip, error : String ) : void;
 	function onLoadInit( mc : MovieClip ) : void;
	function onLoadProgress( mc : MovieClip, loaded : int, total : int ) : void;
	function onLoadStart( mc : MovieClip ) : void;

}