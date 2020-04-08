class BitCodec {
	var error_flag : bool;
	var nbits : int;
	var bits : int;
	var data : String;
	var in_pos : int;
	var crc : int;

	function new() {
		setData("");
	}

	function crcStr() {
		return c64(crc&63) + c64((crc>>6)&63) + c64((crc>>12)&63) + c64((crc>>18)&63);
	}

	function setData(d) {
		error_flag = false;
		data = d;
		in_pos = 0;
		nbits = 0;
		bits = 0;
		crc = 0;
	}

	function read(n) {
		while(nbits < n) {
			var c = d64(data.charAt(in_pos++));
			if( in_pos > data.length || c == null ) {
				error_flag = true;
				return -1;
			}
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

	function hasError() {
		return error_flag;
	}

	function toString() {
		if( nbits > 0 )
			write(6-nbits,0);
		return data;
	}

	function write(n,b) {
		nbits += n;
		bits <<= n;
		bits |= b;
		var k;
		while(nbits >= 6) {
			nbits -= 6;
			k = (bits >> nbits) & 63;
			crc ^= k;
			crc &= 0xFFFFFF;
			crc *= k;
			data += c64(k);
		}
	}

	static function ord(code) {
		return code.charCodeAt(0);
	}

	static function chr(code) {
		return String.fromCharCode(code);
	}

	static function d64(code) {
		var chars = "$azAZ_"; // anti obfu
		if( code >= chars.charAt(1) && code <= chars.charAt(2) )
			return ord(code) - ord(chars.charAt(1));
		if( code >= chars.charAt(3) && code <= chars.charAt(4) )
			return ord(code) - ord(chars.charAt(3)) + 26;
		if( code >= "0" && code <= "9" )
			return ord(code) - ord("0") + 52;
		if( code == "-")
			return 62;
		if( code == chars.charAt(5) )
			return 63;
		return null;
	}

	static function c64(code) {
		var chars = "$aA_"; // anti obfu
		if( code < 0 )
			return "?";
		if( code < 26 )
			return chr(code+ord(chars.charAt(1)));
		if( code < 52 )
			return chr((code-26)+ord(chars.charAt(2)));
		if( code < 62 )
			return chr((code-52)+ord("0"));
		if( code == 62 )
			return "-";
		if( code == 63 )
			return chars.charAt(3);
		return "?";
	}

}