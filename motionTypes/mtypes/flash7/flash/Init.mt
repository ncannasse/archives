class flash.Init {

	static var __m = Std.cast(new flash.geom.Matrix());
	static var __tx = Std.cast("$tx".substring(1));
	static var __ty = Std.cast("$ty".substring(1));

	function drawMC( mc : MovieClip, x : float, y : float ) {
		__m[__tx] = x;
		__m[__ty] = y;
		Std.cast(this).draw(mc,__m);
	}
	
	static function init() {
		__m.identity();
		Std.cast(flash.display.BitmapData).prototype.drawMC = Std.cast(flash.Init).prototype.drawMC;
	}
	
}