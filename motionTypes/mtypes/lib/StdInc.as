class StdInc {

	static function init(mc) {
		// HACK HACK : suppress warnings from flash
		with( mc ) {
			function () {
				#include "std_inc.as"
			}.call(mc);
		}
	}

}