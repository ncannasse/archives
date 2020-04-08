native class flash.filters.ConvolutionFilter extends flash.filters.Filter {

	var alpha : float;
	var color : float;
	var clamp : bool;
	var preserveAlpha : bool;
	var bias : float;
	var divisor : float;
	var matrix : Array<Array<float>>;
	var matrixX : float;
	var matrixY : float;

	function new() : void;
	function clone() : flash.filters.ConvolutionFilter;

}