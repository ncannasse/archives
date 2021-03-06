
intrinsic class TextField
{
	public var _x : float;
	public var _y : float;
	public var _xscale : float;
	public var _yscale : float;
	public var _width : float;
	public var _height : float ;
	public var _alpha : float;
	public var _visible : bool;
	public var _parent : MovieClip;

	public var autoSize : String;
	public var background : bool;
	public var backgroundColor : int;
	public var border : bool;
	public var borderColor : int;
	public var bottomScroll : int;
	public var condenseWhite : bool;
	public var embedFonts : bool;
	public var hscroll : int;
	public var html : bool;
	public var htmlText : String;
	public var length : int;
	public var maxChars : int;
	public var maxhscroll : int;
	public var maxscroll : int;
	public var multiline : bool;
	public var password : bool;
	public var restrict : String;
	public var scroll : int;
	public var selectable : bool;
	public var tabEnabled : bool;
	public var tabIndex : int;
	public var text : String;
	public var textColor : int;
	public var textHeight : int;
	public var textWidth : int;
	public var type : String;
	public var variable : String;
	public var wordWrap : bool;

    function onSetFocus(mc:MovieClip) : void;
    function onKillFocus(mc:MovieClip) : void;
    
	function removeTextField() : void;
}


