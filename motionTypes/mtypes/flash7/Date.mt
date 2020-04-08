native class Date
{
	function new();

	function getFullYear() : int;
	function getYear() : int;
	function getMonth() : int;
	function getDate() : int;
	function getDay() : int;
	function getHours() : int;
	function getMinutes() : int;
	function getSeconds() : int;
	function getMilliseconds() : int;

	function getUTCFullYear() : int;
	function getUTCYear() : int;
	function getUTCMonth() : int;
	function getUTCDate() : int;
	function getUTCDay() : int;
	function getUTCHours() : int;
	function getUTCMinutes() : int;
	function getUTCSeconds() : int;
	function getUTCMilliseconds() : int;

	function setFullYear(year : int, month : int, day : int) : void;
	function setMonth(value : int) : void;
	function setDate(value : int) : void;
	function setHours(value : int) : void;
	function setMinutes(value : int) : void;
	function setSeconds(value : int) : void;
	function setMilliseconds(value : int) : void;

	function setUTCFullYear(value : int) : void;
	function setUTCMonth(value : int) : void;
	function setUTCDate(value : int) : void;
	function setUTCHours(value : int) : void;
	function setUTCMinutes(value : int) : void;
	function setUTCSeconds(value : int) : void;
	function setUTCMilliseconds(value : int) : void;

	function getTime() : int;
	function setTime(value : int) : void;
	function getTimezoneOffset() : int;
	function setYear(value : int) : void;

	function toString() : String;

	static function UTC() : int;
}


