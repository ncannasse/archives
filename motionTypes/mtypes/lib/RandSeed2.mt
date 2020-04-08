class RandSeed2 {

	static var NSEEDS = 25;
	static var MAX = 7;
	static var INIT_SEEDS = [
		0x95f24dab, 0x0b685215, 0xe76ccae7, 0xaf3ec239, 0x715fad23,
		0x24a590ad, 0x69e4b5ef, 0xbf456141, 0x96bc1b7b, 0xa7bdf825,
		0xc1de75b7, 0x8858a9c9, 0x2da87693, 0xb657f9dd, 0xffdc8a9f,
		0x8121da71, 0x8b823ecb, 0x885d05f5, 0x4e20cd47, 0x5a9ad5d9,
		0x512c0c03, 0xea857ccd, 0x4cc1d30f, 0x8891a8a1, 0xa6b7aadb
	];

	var init : bool;
	var seed : int;
	var cur : int;
	var seeds : Array<int>;

	public function new() {
		init = false;
		seed = 1;
	}

	public function setSeed(s) {
		init = false;
		seed = s;
	}

	function doInit() {
		cur = 0;
		var i = 0;
		init = true;
		seeds = new Array();
		while( i < NSEEDS ) {
			seeds.push(INIT_SEEDS[i] ^ seed);
			i++;
		}
	}

	public function int() {
		if( !init )
			doInit();
		if( cur == NSEEDS ) {
			var kk = 0;
			while( kk < NSEEDS-MAX ) {
				var s = seeds[kk];
				seeds[kk] = seeds[kk+MAX] ^ (s >> 1);
				if( Std.cast(s) % 2 )
				 	seeds[kk] ^= 0x8EBFD028;
				kk++;
			}
			while( kk < NSEEDS ) {
				var s = seeds[kk];
				seeds[kk] = seeds[kk+(MAX-NSEEDS)] ^ (s >> 1);
				if( Std.cast(s) % 2 )
					seeds[kk] ^= 0x8EBFD028;
				kk++;
			}
			cur = 0;
		}
		var y = seeds[cur++];
		y ^= (y << 7) & 0x2B5B2500;
		y ^= (y << 15) & 0xDB8B0000;
		y ^= (y >> 16);
		return y;
	}

	public function random(max) {
		return this.int()%max;
	}

}