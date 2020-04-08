import flash.geom.Point;
import flash.geom.Matrix;

native class flash.geom.Matrix {

/* DISABLED FOR OBFU

	// 3x2 affine 2D matrix
	var a : float;
	var b : float;
	var c : float;
	var d : float;
	var tx : float;
	var ty : float;
*/

	function new() : void;

	function transformPoint( p : Point ) : Point;
	function deltaTransformPoint( p : Point ) : void;
	function toString() : String;
	function scale( sx : float, sy : float ) : void;
	function translate( tx : float, ty : float ) : void;
	function rotate( r : float ) : void;
	function identity() : void;
	function invert() : void;
	function concat( m : Matrix ) : void;
	function clone() : Matrix;

	var createGradientBox : void; // Function
	var createBox : void; // Function


}