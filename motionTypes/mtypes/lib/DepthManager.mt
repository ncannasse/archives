class DepthManager {

	var root_mc : MovieClip;
	var plans : Array<{ tbl : Array<MovieClip>, cur : int }>;

	function new( mc : MovieClip ) {
		root_mc = mc;
		plans = new Array();
	}

	function getMC() {
		return root_mc;
	}

	private function getPlan( pnb ) {
		var plan_data = plans[pnb];
		if( plan_data == null ) {
			plan_data = { tbl : new Array(), cur : 0 };
			plans[pnb] = plan_data;
		}
		return plan_data;
	}

	function compact( plan : int ) {
		var plan_data = plans[plan];
		var p = plan_data.tbl;
		var max = plan_data.cur;
		var i;
		var cur = 0;
		var base = plan * 1000;
		for(i=0;i<max;i++)
			if( p[i]._name != null ) {
				p[i].swapDepths(base+cur);
				p[cur] = p[i];
				cur++;
			}
		plan_data.cur = cur;
	}

	function attach( inst : String, plan : int ) : MovieClip {
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var d = plan_data.cur;
		if( d == 1000 ) {
			compact(plan);
			return attach(inst,plan);
		}
		var mc = Std.attachMC(root_mc,inst,d+plan*1000);
		//Log.trace("ATTACH " + inst + " @ " + (d+plan*1000));
		p[d] = mc;
		plan_data.cur++;
		return mc;
	}

	function empty( plan : int ) : MovieClip {
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var d = plan_data.cur;
		if( d == 1000 ) {
			compact(plan);
			return empty(plan);
		}
		var mc = Std.createEmptyMC(root_mc,d+plan*1000);
		//Log.trace("EMPTY @ " + (d+plan*1000));
		p[d] = mc;
		plan_data.cur++;
		return mc;
	}

	function reserve( mc : MovieClip, plan : int ) : int {
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var d = plan_data.cur;
		if( d == 1000 ) {
			compact(plan);
			return reserve(mc,plan);
		}
		p[d] = mc;
		plan_data.cur++;
		return d + plan * 1000;
	}

	function swap( mc : MovieClip, plan : int ) {
		var src_plan = Math.floor(mc.getDepth() / 1000);
		if( src_plan == plan )
			return;
		var plan_data = getPlan(src_plan);
		var p = plan_data.tbl;
		var max = plan_data.cur;
		var i;
		for(i=0;i<max;i++)
			if( p[i] == mc ) {
				p[i] = null;
				break;
			}
		mc.swapDepths( reserve(mc,plan) );
	}

	function under( mc : MovieClip ) {
		var d = mc.getDepth();
		var plan = Math.floor(d / 1000);
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var pd = d%1000;
		if( p[pd] == mc ) {
			p[pd] = null;
			p.unshift(mc);
			plan_data.cur++;
			compact(plan);
		}
	}

	function over( mc : MovieClip ) {
		var d = mc.getDepth();
		var plan = Math.floor(d / 1000);
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var pd = d%1000;
		if( p[pd] == mc ) {
			p[pd] = null;
			if( plan_data.cur == 1000 )
				compact(plan);
			d = plan_data.cur;
			plan_data.cur++;
			mc.swapDepths(d + plan * 1000);
			p[d] = mc;
		}
	}

	function clear( plan : int ) {
		var plan_data = getPlan(plan);
		var i;
		var p = plan_data.tbl;
		for(i=0;i<plan_data.cur;i++)
			p[i].removeMovieClip();
		plan_data.cur = 0;
	}

	function ysort( plan : int ) {
		var plan_data = getPlan(plan);
		var p = plan_data.tbl;
		var len = plan_data.cur;
		var i,j;
		var y = -99999999;
		for(i=0;i<len;i++) {
			var mc = p[i];
			var mcy = mc._y;
			if( mcy >= y )
				y = mcy;
			else {
				for(j=i;j>0;j--) {
					var mc2 = p[j-1];
					if( mc2._y > mcy ) {
						p[j] = mc2;
						mc.swapDepths(Std.cast(mc2));
					} else {
						p[j] = mc;
						break;
					}
				}
				if( j == 0 )
					p[0] = mc;
			}
		}
	}

	function destroy() {
		var i;
		for(i=0;i<plans.length;i++)
			clear(i);
	}

}