native class Selection
{
	static function setFocus(instName:String);
	static function getFocus():String;

	static function setSelection(start:int, end:int);

	static function getCaretIndex():int;
	static function getEndIndex():int;
	static function getBeginIndex():int;

}