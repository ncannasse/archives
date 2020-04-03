intrinsic class MovieClip
{
	public var useHandCursor : bool;
	public var _x : float;
	public var _y : float;
	public var _xmouse : float;
	public var _ymouse : float;
	public var _xscale : float;
	public var _yscale : float;
	public var _width : float;
	public var _height : float;
	public var _alpha : float;
	public var _visible : bool;
	public var _rotation : float;
	public var _name : String;
	public var _currentframe : int;
	public var _totalframes : int;
	public var _parent : MovieClip;

	private static function MovieClip() : void;

	function localToGlobal(p : IFPoint) : void;
	function globalToLocal(p : IFPoint) : void;

	function hitTest( x : number, y : number, flag : bool ) : bool;

	function getBytesLoaded() : int;
	function getBytesTotal() : int;
	function getDepth() : int;
	function swapDepths( depth : int ) : void;
    function getNextHighestDepth() : int;
	function setMask( mc : MovieClip) : void;

	function play() : void;
	function stop() : void;
	function nextFrame() : void;
	function prevFrame() : void;
	function gotoAndPlay(frame : String) : void;
	function gotoAndStop(frame : String) : void;

	function removeMovieClip() : void;

	// DRAWING
	function clear() : void;
	function moveTo( x : number, y : number ) : void;
	function lineTo( x : number, y : number ) : void;
	function curveTo( controlX : number, controlY : number, anchorX : number, anchorY : number) : void;
	function lineStyle( size : number, color : number, alpha : number ) : void;
	function beginFill( rgb : number, alpha : number ) : void;
	function endFill() : void;

    // CALLBACKS
	function onPress() : void;
    function onRelease() : void;
	function onMouseDown() : void;
	function onMouseUp() : void;
	function onRollOver() : void;
	function onRollOut() : void;
}
