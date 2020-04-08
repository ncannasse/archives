class Regexp {

	var r : void;

	static function newOptions( r : String, o : String ) : Regexp {
		var t = new Regexp(r);
		t.r = Std.neko("regexp_new_options(r.@s,o.@s)");
		return t;
	}

	function new( r : String ) : void {
		this.r = Std.neko("regexp_new(r.@s)");
	}

	function match( s : String ) : bool {
		return Std.neko("regexp_match(this.r,s.@s,0,$ssize(s.@s))");
	}

	function exactMatch( s : String ) : bool {
		return Std.neko("
			s = s.@s;
			if( regexp_match(this.r,s,0,$ssize(s)) ) {
				var m = regexp_matched_pos(this.r,0);
				( m.pos == 0 && m.len == $ssize(s) );
			} else
				false
		");
	}

	function matched( n : int ) : String {
		return Std.neko('
			var m = regexp_matched(this.r,n);
			if( m == null ) m = "";
			String.new(m)
		');
	}

	function matchedPos( n : int ) : { pos : int, len : int } {
		return Std.neko("regexp_matched_pos(this.r,n)");
	}

	function replace( from : String, by : String ) : String {
		return Std.neko("String.new(regexp_replace(this.r,from.@s,by.@s))");
	}

	function replaceAll( from : String, by : String ) : String {
		return Std.neko("String.new(regexp_replace_all(this.r,from.@s,by.@s))");
	}

	function replaceFun( from : String, by : (Regexp -> String) ) : String {
		var me = this;
		return Std.neko("String.new(regexp_replace_fun(this.r,from.@s,function(r){ return by(me).@s }))");
	}

	static var __init = Std.neko('
		regexp_new = $loader.loadprim("regexp@regexp_new",1);
		regexp_new_options = $loader.loadprim("regexp@regexp_new_options",2);
		regexp_match = $loader.loadprim("regexp@regexp_match",4);
		regexp_replace = $loader.loadprim("regexp@regexp_replace",3);
		regexp_replace_all = $loader.loadprim("regexp@regexp_replace_all",3);
		regexp_replace_fun = $loader.loadprim("regexp@regexp_replace_fun",3);
		regexp_matched = $loader.loadprim("regexp@regexp_matched",2);
		regexp_matched_pos = $loader.loadprim("regexp@regexp_matched_pos",2);
	');

}
