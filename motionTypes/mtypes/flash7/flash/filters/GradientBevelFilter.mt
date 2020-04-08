native class flash.filters.GradientBevelFilter extends flash.filters.Filter {

	var type : String;
	var knockout : bool;
	var strength : float;
	var quality : float;
	var blurX : float;
	var blurY : float;
	var ratios : Array<float>;
	var alphas : Array<float>;
	var colors : Array<int>;
	var angle : float;
	var distance : float;

	function new() : void;
	function clone() : flash.filters.GradientBevelFilter;

}