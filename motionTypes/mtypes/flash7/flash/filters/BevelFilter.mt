native class flash.filters.BevelFilter extends flash.filters.Filter {

	var type : String;
	var blurX : float;
	var blurY : float;
	var knockout : bool;
	var strength : float;
	var quality : float;
	var shadowAlpha : float;
	var shadowColor : int;
	var highlightAlpha : float;
	var highlightColor : int;
	var angle : float;
	var distance : float;

	function new() : void;
	function clone() : flash.filters.BevelFilter;

}
