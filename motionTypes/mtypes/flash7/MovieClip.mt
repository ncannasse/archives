native class MovieClip
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

	public var tabEnabled : bool;
	public var tabChildren : bool;


	function startDrag() : void;
	function stopDrag() : void;

	function hitTest( x : float, y : float, flag : bool ) : bool;

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
	function getBounds( ref : MovieClip ) : { xMin : float, xMax : float, yMin : float, yMax : float };
	function getSWFVersion() : int;

	function removeMovieClip() : void;

	// ATTACH
	function attachBitmap(bd:flash.display.BitmapData,d:int):void

	// DRAWING
	function clear() : void;
	function moveTo( x : float, y : float ) : void;
	function lineTo( x : float, y : float ) : void;
	function curveTo( controlX : float, controlY : float, anchorX : float, anchorY : float) : void;
	function lineStyle( size : float, color : int, alpha : int ) : void;
	function beginFill( rgb : int, alpha : int ) : void;
	function endFill() : void;

	// CALLBACKS
	function onPress() : void;
	function onRelease() : void;
	function onReleaseOutside() : void;
	function onMouseDown() : void;
	function onMouseMove() : void;
	function onMouseUp() : void;
	function onRollOver() : void;
	function onRollOut() : void;
	function onDragOut() : void;
	function onDragOver() : void;

	// FLASH8
	function beginBitmapFill(bmp:flash.display.BitmapData,m:flash.geom.Matrix,reapeat:bool,smoothing:bool) : void
	var cacheAsBitmap : bool;
	var filters : Array<flash.filters.Filter>;
	var blendMode : BlendMode;

	// cadeau de noel pour benjamin
	var smc : MovieClip;
}
