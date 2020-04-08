native class flash.display.BitmapData {


	var width : int;
	var height : int;
	var rectangle : flash.geom.Rectangle;
	var transparent : bool;

	function new( width : int, height : int, alpha : bool, color : int ) : void;

	function getPixel( x : int, y : int ) : int;
	function setPixel( x : int, y : int, color : int ) : void;
	function getPixel32( x : int, y : int ) : int;
	function setPixel32( x : int, y : int, color : int ) : void;
	function fillRect( r : flash.geom.Rectangle, color : int ) : void;
	function draw( mc:{}, matrix:flash.geom.Matrix, colorTransform:flash.geom.ColorTransform, blendMode:BlendMode, clipRect:flash.geom.Rectangle, smooth:bool ) : void;
	function clone() : flash.display.BitmapData;
	function dispose() : void;

	// ADDONS : need to execute flash.Init.init() !!
	function drawMC( mc:MovieClip, x : float, y : float ) : void;

	// UNKNOWN

	function copyPixels(sourceBitmap:flash.display.BitmapData, sourceRect:flash.geom.Rectangle, destPoint:flash.geom.Point, alphaBitmap:flash.display.BitmapData, alphaPoint:flash.geom.Point, mergeAlpha:bool);
	function applyFilter(bmp:flash.display.BitmapData,sourceRect:flash.geom.Rectangle, destPoint:flash.geom.Point, fl:flash.filters.BitmapFilter):flash.display.BitmapData;
	function scroll( dx : int, dy : int ) : void;
	function threshold();
	function pixelDissolve();
	function floodFill( x : int, y : int, color : int ) : void;
	function getColorBoundsRect();
	function perlinNoise(baseX:int, baseY:int, numOctaves:int, randomSeed:int, stitch:bool, fractalNoise:bool, channelOptions:int, grayScale:bool, offsets:Array<flash.geom.Point>);
	function colorTransform(rect:flash.geom.Rectangle,ct:flash.geom.ColorTransform);
	function hitTest(firstPoint:flash.geom.Point, firstAlphaThreshold:float, secondObject:{}, secondBitmapPoint:flash.geom.Point, secondAlphaThreshold:float):bool;
	function paletteMap();
	function merge();
	function noise();
	function copyChannel();
	function generateFilterRect();
	static function loadBitmap(id:String):flash.display.BitmapData


}