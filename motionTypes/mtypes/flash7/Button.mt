native class Button {

	var _x : float;
	var _y : float;
	var _width : float;
	var _height : float;
	var _xscale : float;
	var _yscale : float;

	var _alpha : float
	var _visible : bool;
	var useHandCursor : bool;
	var enabled : bool;

	var _focusrect : bool;
	var _name : String;
	var _parent : MovieClip;
	var _rotation : float;

	var _url : String;
	var trackAsMenu : bool;

	var _xmouse : float;
	var _ymouse : float;

	var onDragOut : void -> void;
 	var onDragOver : void -> void
 	var onKeyUp : int -> void;
 	var onKillFocus : MovieClip -> void;
 	var onPress : void -> void;
 	var onRelease : void -> void;
 	var onReleaseOutside : void -> void;
	var onRollOut : void -> void;
 	var onRollOver : void -> void;
	var onSetFocus : MovieClip -> void;

}