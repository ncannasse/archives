class Indicator {
	function Indicator(name, x, y, init, max, mtu, mtv, mtu2, mtv2, width, height) {
		this.name = name;
		this.indicatorGroup = new Group(0.0002);

		this.indicator = new Sprite(this.indicatorGroup);
		this.indicator._bitmap = 'Indic;
		this.indicator._width = width;
		this.indicator._height = height;
		this.indicator._tu = 0.18;
		this.indicator._tv = 0.17;
		this.indicator._tu2 = 0.30;
		this.indicator._tv2 = 0.76;
		this.indicator._x = x;
		this.indicator._y = y;

		this.indicatorsMovable = new Group(0.0003);
		this.movable = new Sprite(this.indicatorsMovable);
		this.movable._bitmap = 'Indic;
		this.movable._width = width - this.indicator._width / 16;
		this.temp = (this.indicator._height - this.indicator._height / 32)/ max;
		this.movable._height =  this.temp * init;
		this.movable._tu = mtu;
		this.movable._tv = mtv;
		this.movable._tu2 = mtu2;
		this.movable._tv2 = mtv2;
		this.movable._x = x + this.indicator._width / 32;
		this.movable._y = this.indicator._y + this.indicator._height - this.movable._height;
		this.max = max;

		this.val = init;
		this.lastVal = init;
		this.mod = 0;
	}

	function remove() {
		this.indicatorGroup.remove();
		this.indicatorsMovable.remove();
	}

	function update(val) {
		if( val != this.lastVal ) {
			this.lastVal = val;
			this.mod = 1.0;
		}

		if( this.mod > 0 ) {
			this.val = this.val * this.mod + this.lastVal * (1.0 - this.mod);
			this.movable._height =  this.temp * this.val;
			this.movable._y = this.indicator._y + this.indicator._height - this.movable._height;
			this.mod -= 0.1;
		}
	}
}

class LifeIndicator extends Indicator {
	function LifeIndicator(x, y, init, max) {
		super("life indicator", x, y, init, max, 0.9, 0.05,	0.95, 0.10, 20, 50);
	}
}