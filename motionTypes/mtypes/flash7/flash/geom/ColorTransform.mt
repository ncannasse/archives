native class flash.geom.ColorTransform {

	var rgb : float;
	var blueOffset : float;
	var greenOffset : float;
	var redOffset : float;
	var alphaOffset : float;
	var blueMultiplier : float;
	var redMultiplier : float;
	var alphaMultiplier : float;

	function new( rm : float, gm : float, bm : float, am : float, ro : float, go : float, bo : float, ao : float ) : void;
	function toString() : String;
	function concat( c : flash.geom.ColorTransform ) : void;

}