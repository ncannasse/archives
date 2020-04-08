class SoundManager {

	var root_mc : MovieClip;
	var depth : int;
	var channels : Array<SoundManagerChannel>;

	var fade_from : SoundManagerChannel, fade_to : SoundManagerChannel;
	var fade_start : float, fade_end : float;
	var fade_pos : float, fade_len : float;
	var last_time : float;

	function new( mc : MovieClip, base_depth : int ) {
		root_mc = mc;
		depth = base_depth;
		channels = new Array();
		fade_pos = -1;
	}

	private static function do_stop_sound( k, s ) {
		s.stop();
		s.flag_playing = false;
	}

	private function on_sound_completed() {
		Std.cast(this).flag_playing = false;
	}

	function destroy() {
		var i;
		for(i=0;i<channels.length;i++) {
			var c = channels[i];
			c.sounds.iter(do_stop_sound);
			c.mc.removeMovieClip();
		}
		channels = [];
	}

	private function getChannel( chan : int ) {
		var c = channels[chan];
		if( c == null ) {
			var mc = Std.createEmptyMC(root_mc,depth++);
			c = { mc : mc, sounds : new Hash(), vol : 100, vol_ctrl : new Sound(mc), nb : chan, enabled : true };
			channels[chan] = c;
		}
		return c;
	}

	private function getSound( name : String, chan : int ) : Sound {
		var c = getChannel(chan);
		var s = c.sounds.get(name);
		if( s == null ) {
			s = new Sound(c.mc);
			s.attachSound(name);
			s.onSoundComplete = on_sound_completed;
			s.flag_playing = false;
			c.sounds.set(name,s);
		}
		return s;
	}

	function playSound( name : String, chan : int ) {
		var s = getSound(name,chan);
		s.start(0,1);
		s.flag_playing = true;
	}

	function play( name : String ) {
		playSound(name,0);
	}

	function loop( name : String, chan : int ) {
		var s = getSound(name,chan);
		s.start(0,0xFFFF);
		s.flag_playing = true;
	}

	function stopSound( name : String, chan : int ) {
		var s = getSound(name,chan);
		s.stop();
		s.flag_playing = false;
	}

	function fade( chan_from : int, chan_to : int, length : int ) {
		if( fade_pos != -1 ) {
			setVolume(fade_to.nb,fade_end);
			this.stop(fade_from.nb);
			setVolume(fade_from.nb,fade_end);
		}
		fade_from = getChannel(chan_from);
		fade_to = getChannel(chan_to);
		fade_start = fade_to.vol;
		fade_end = fade_from.vol;
		fade_pos = 0;
		fade_len = length;
		last_time = Std.getTimer();
	}

	function main() {
		if( fade_pos != -1 ) {
			var last_fade = false;
			var t = Std.getTimer();
			fade_pos += ((last_time - t) / 1000) / fade_len;
			last_time = t;
			if( fade_pos >= 1 ) {
				fade_pos = 1;
				last_fade = true;
			}
			var volume = (fade_end - fade_start) * fade_pos + fade_start;
			setVolume(fade_to.nb,volume);
			setVolume(fade_from.nb,fade_end-volume);
			if( last_fade ) {
				fade_pos = -1;
				this.stop(fade_from.nb);
				setVolume(fade_from.nb,fade_end);
			}
		}
	}

	function enable( chan : int, flag : bool ) {
		var c = getChannel(chan);
		c.enabled = flag;
		if( c.enabled )
			c.vol_ctrl.setVolume(c.vol);
		else
			c.vol_ctrl.setVolume(0);
	}

	function stop( chan : int ) {
		var c = getChannel(chan);
		c.sounds.iter(do_stop_sound);
	}

	function isPlaying( name : String, chan : int ) {
		var s = getSound(name,chan);
		return s.flag_playing;
	}

	function setVolume( chan : int, volume : float ) {
		var c = getChannel(chan);
		c.vol = volume;
		if( c.enabled )
			c.vol_ctrl.setVolume(c.vol);
	}

}