class RandSeed {

	var a : Array<int>;
	var next : Array<int>;
	var seed : int;
	var init : bool;
	var rndtime : int, rndtime1 : int, rndtime2 : int, rndtime3 : int;

	function new() {
		init = false;
		seed = 1;
	}

	function setSeed(n) {
		seed = n;
		init = false;
	}

	function doInit() {
		a = new Array();
		next = new Array();
		a[0] = seed;
		next[0] = 1;
		var i;
		for(i=1;i<9689;i++) {
			a[i] = int(69069 * a[i-1] + 1);
			next[i] = i + 1;
		}
		next[9688] = 0;
		rndtime = 9688;
		rndtime1 = 9688 - 157;
		rndtime2 = 9688 - 314;
		rndtime3 = 9688 - 471;
		init = true;
	}

	function random(max) {
		if( !init ) 
			doInit();
		rndtime = next[rndtime];
		rndtime1 = next[rndtime1];
		rndtime2 = next[rndtime2];
		rndtime3 = next[rndtime3];
		a[rndtime] ^= a[rndtime1] ^ a[rndtime2] ^ a[rndtime3];
		return (a[rndtime] & 0x7FFFFFFF) % max;
	}

}