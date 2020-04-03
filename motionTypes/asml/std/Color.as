intrinsic class Color {

	static function Color( mc : MovieClip ) : void;

	function setRGB( rgb : int ) : void;
	function getRGB() : Number;
	
	function setTransform( ct : ColorTransform ) : void;
	function getTransform() : ColorTransform;
		
	function reset() : void;

}