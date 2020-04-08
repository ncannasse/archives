class IHash<'item> {
	
	var h : void;
	
	function new(){
		this.h = Std.neko("$hnew(3)");
	}
	
	function get( k : int ) : 'item {
		return Std.neko("$hget(this.h,k,null)");
	}
	
	function set( k : int, v : 'item ) : void{
		Std.neko("$hset(this.h,k,v,null)");
	}
	
	function remove( k : int ) : bool {
		return Std.neko("$hremove(this.h,k,null)");
	}
	
	function exists( k : int ) : bool {
		return Std.neko("$hmem(this.h,k,null)");
	}
	
	function iter( f : int -> 'item -> void ) : void{
		Std.neko("$hiter(this.h,function(k,v){ f(k,v) })");
	}
	
	function toString() : String {
		var s = new StringBuf();
		var o = {i: 0, first: true};
		s.add("#ihash[");
		iter(fun(k,v){
			if( o.first )
				o.first = false;
			else
				s.add(", ");
			s.add(k);
			s.add(" => ");
			s.add(v);
		});
		s.add("]");
		return s.toString();
	}

}
