class Date {

	var time : Int32;

	function new( f : String ) : void {
		this.time = Std.neko("date_new(if( f == null ) null else f.@s)");
	}

	function withTime( h : int, m : int, s : int ) : Date {
		return Std.neko("Date.new1(date_set_hour(this.time,h,m,s))");
	}

	function withDay( y : int, m : int, d : int ) : Date {
		return Std.neko("Date.new1(date_set_day(this.time,y,m,d))");
	}

	function getTime() : { h : int, m : int, s : int } {
		return Std.neko("date_get_hour(this.time)");
	}

	function getDay() : { y : int, m :  int, d : int } {
		return Std.neko("date_get_day(this.time)");
	}

	function format( fmt : String ) : String {
		return Std.neko("String.new(date_format(this.time,fmt.@s))");
	}

	function sub( d : Date ) : Date {
		return Std.neko("Date.new1(Int32.sub(this.time,d.time))");
	}

	function add( d : Date ) : Date {
		return Std.neko("Date.new1(Int32.add(this.time,d.time))");
	}

	function toString() : String {
		return Std.neko("String.new(this.__string())");
	}

	function __string() : void {
		return Std.neko("date_format(this.time,null)");
	}

	function __compare( d : Date ) : int {
		return Int32.compare(this.time,d.time);
	}

	static function now() : Date {
		return Std.neko("Date.new1(date_now())");
	}

	static function ofTime( t : Int32 ) : Date {
		return Std.neko("Date.new1(t)");
	}

	static var __init = Std.neko('
		date_new = $loader.loadprim("std@date_new",1);
		date_now = $loader.loadprim("std@date_now",0);
		date_set_hour = $loader.loadprim("std@date_set_hour",4);
		date_set_day = $loader.loadprim("std@date_set_day",4);
		date_get_hour = $loader.loadprim("std@date_get_hour",1);
		date_get_day = $loader.loadprim("std@date_get_day",1);
		date_format = $loader.loadprim("std@date_format",2);

		Date.new1 = function(d) {
			var o = $new(null);
			$objsetproto(o,@Date);
			o.time = d;
			return o;
		}
	');

}