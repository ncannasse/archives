native class flash.filters.GlowFilter extends flash.filters.Filter {

	var blurX : float;
	var blurY : float;
	var knockout : bool;
	var strength : float;
	var quality : float;
	var inner : bool;
	var alpha : float;
	var color : float;

	function new() : void;
	function clone() : flash.filters.GlowFilter;

}
