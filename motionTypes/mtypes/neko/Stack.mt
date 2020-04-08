class Stack {

	static function callStack() : List<String> {
		return Std.neko("@make_stack($callstack(),1)");
	}

	static function excStack() : List<String> {
		return Std.neko("@make_stack($excstack(),0)");
	}

	static var __init = Std.neko('

		@sc = String.new("Called from a C Function");

		@make_stack = function(s,delta) {
			var l = List.new();
			var n = $asize(s) - delta;
			var i = 0;
			while( i < n ) {
				var p = s[i];
				if( p == null )
					l.add(sc);
				else if( $typeof(p) == $tstring )
					l.add(String.new("Called from module "+p));
				else
					l.add(String.new("Called from "+p[0]+" line "+p[1]));
				i = i + 1;
			}
			return l;
		}
	
	');

}