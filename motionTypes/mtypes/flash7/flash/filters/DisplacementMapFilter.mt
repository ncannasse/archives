native class flash.filters.DisplacementMapFilter extends flash.filters.Filter {

	var alpha : float;
	var color : float;
	var mode : String;
	var scaleX : float;
	var scaleY : float;
	var componentX : float;
	var componentY : float;
	var mapPoint : flash.geom.Point;
	var mapBitmap : 'a;

	function new() : void;
	function clone() : flash.filters.DisplacementMapFilter;

}