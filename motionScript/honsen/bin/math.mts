primitive sqrt(x) = std_sqrt;
primitive abs(x) = std_abs;
primitive cos(x) = std_cos;
primitive sin(x) = std_sin;
primitive tan(x) = std_tan;
primitive log(x) = std_log;
primitive exp(x) = std_exp;
primitive pow(x,y) = std_pow;
primitive static PI = std_get_pi;

primitive ceil(x) = std_ceil;
primitive floor(x) = std_floor;
primitive round(x) = std_round;
primitive acos(x) = std_acos;
primitive asin(x) = std_asin;
primitive atan(x) = std_atan;
primitive atan2(x,y) = std_atan2;

function min(x,y) {
	if( x < y )
		return x;
	return y;
}

function max(x,y) {
	if( x > y )
		return x;
	return y;
}
