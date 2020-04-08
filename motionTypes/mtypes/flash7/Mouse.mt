native class Mouse
{
   static function hide();
   static function show();

   static function addListener(l : {
	   onMouseDown : void -> void,
	   onMouseUp : void -> void,
	   onMouseMove : void -> void,
	   onMouseWheel : float -> void
   });

   static function removeListener(l : {
	   onMouseDown : void -> void,
	   onMouseUp : void -> void,
	   onMouseMove : void -> void,
	   onMouseWheel : float -> void
   });
}
