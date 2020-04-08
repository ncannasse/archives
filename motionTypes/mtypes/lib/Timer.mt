class Timer {

    static var wantedFPS = 32;
	static var maxDeltaTime = 0.5;
    static var oldTime = Std.getTimer();
	static var tmod_factor = 0.95;
	static var calc_tmod = 1;
    static var tmod = 1;
    static var deltaT = 1;
	static var frameCount = 0;

	static function update() {
		frameCount++;
		var newTime = Std.getTimer();
		deltaT = (newTime - oldTime) / 1000.0;
		oldTime = newTime;
		if( deltaT < maxDeltaTime )
			calc_tmod = calc_tmod * tmod_factor + (1 - tmod_factor) * deltaT * wantedFPS;
		else
			deltaT = 1 / wantedFPS;
		tmod = calc_tmod;
	}

	static function fps():float {
	  return wantedFPS/tmod ;
	}

}
