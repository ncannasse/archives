import flash.geom.Point;

native class flash.geom.Point {

/* disabled FOR OBFU

	var x : float;
	var y : float;
	var length : float;
*/

	function new( x : float, y : float ) : void;

	function normalize() : void;
	function add( p : Point ) : Point;
	function subtract( p : Point ) : Point;
	function equals( p : Point ) : bool;
	function offset( dx : float, dy : float ) : void;
	function clone() : Point;
	function toString() : String;


	static function distance( p1 : Point, p2 : Point ) : float;
	static function interpolate( p1 : Point, p2 : Point, f : float ) : Point;
	static function polar( dist : float, angle : float ) : Point;

}