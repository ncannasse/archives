class Random {

	function new() {
		Std.neko("this.@r = random_new()");
	}

	function setSeed( s : int ) {
		Std.neko("random_set_seed(this.@r,s)");
	}

	function int( max : int ) : int {
		return Std.neko("random_int(this.@r,max)");
	}

	function float() : float {
		return Std.neko("random_float(this.@r)");
	}

	static var __init = Std.neko('
		random_new = $loader.loadprim("std@random_new",0);
		random_set_seed = $loader.loadprim("std@random_set_seed",2);
		random_int = $loader.loadprim("std@random_int",2);
		random_float = $loader.loadprim("std@random_float",1);
	');

}