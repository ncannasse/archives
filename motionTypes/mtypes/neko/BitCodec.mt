class BitCodec {
	var nbits : int;
	var bits : int;
	var out_buf : StringBuf;
	var in_buf : String;
	var in_pos : int;
	var crc : int;

	static function initInvChars() {
		var i;
		var a = new Array();
		for(i=0;i<64;i++)
			a[ENCODE.at(i)] = i;
		return a;
	}
	static var ENCODE = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_"
	static var DECODE = initInvChars();

	function new() {
		setData("");
	}

	function crcStr() {
		return 
			ENCODE.charAt(crc&63) + 
			ENCODE.charAt((crc>>6)&63) + 
			ENCODE.charAt((crc>>12)&63) + 
			ENCODE.charAt((crc>>18)&63);
	}

	function setData(d) {
		in_buf = d;
		in_pos = 0;
		nbits = 0;
		bits = 0;
		crc = 0;
		out_buf = new StringBuf();
	}

	function read(n) {
		while(nbits < n) {
			var c = DECODE[in_buf.at(in_pos++)];
			if( c == null )
				Std.throw("eof");
			crc ^= c;
			crc &= 0xFFFFFF;
			crc *= c;
			nbits += 6;
			bits <<= 6;
			bits |= c;
		}
		nbits -= n;
		return (bits >> nbits) & ((1 << n) - 1);
	}

	function nextPart() {
		nbits = 0;
	}

	function toString() {
		if( nbits > 0 )
			write(6-nbits,0);
		return out_buf.toString();
	}

	function write(n,b) {
		nbits += n;
		bits <<= n;
		bits |= b;
		while(nbits >= 6) {
			nbits -= 6;
			var k = (bits >> nbits) & 63;
			crc ^= k;
			crc &= 0xFFFFFF;
			crc *= k;
			out_buf.addChar(ENCODE.at(k));
		}
	}

}