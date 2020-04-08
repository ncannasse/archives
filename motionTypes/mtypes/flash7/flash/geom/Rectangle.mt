import flash.geom.Point;
import flash.geom.Rectangle;

native class flash.geom.Rectangle {

	var left : float;
	var top : float;
	var right : float;
	var bottom : float;

/* DISABLED FOR OBFU

	// OR
	var x : float;
	var y : float;
	var width : float;
	var height : float;

	// OR
	var size : Point;
	var bottomRight : Point;
	var topLeft : Point;
*/

	function new( x : float, y : float, w : float, h : float ) : void;

	function equals( r : Rectangle ) : bool;
	function union( r : Rectangle ) : Rectangle;
	function intersects( r : Rectangle ) : bool;
	function intersection( r : Rectangle ) : Rectangle;
	function containsRectangle( r : Rectangle ) : bool;
	function containsPoint( p : Point ) : bool;
	function contains( x : float, y : float ) : bool;
	function offsetPoint( p : Point ) : void;
	function offset( x : float, y : float ) : void;

	function inflatePoint( p : Point ) : void;
	function inflate( x : float, y : float ) : void;
	function isEmpty() : bool;
	function setEmpty() : void;
	function clone() : Rectangle;

	function toString() : String;


}