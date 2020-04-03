class Altimeter {
	function Altimeter(name, x, y, init, max) {
		this.group = new Group(0.0004);
		this.name = name;
		this.indicator = new Sprite(this.group);
		this.indicator._bitmap = 'Alti;
		this.indicator._width = _width / 20.0;
		this.indicator._height = _height / 5.0;
		this.indicator._tu = 0;
		this.indicator._tv = 0;
		this.indicator._tu2 = 0.15;
		this.indicator._tv2 = 1;
		this.indicator._x = x;
		this.indicator._y = y;

		this.indicatorsMovable = new Group(0.0003);
		this.movable = new Sprite(this.indicatorsMovable);
		this.movable._bitmap = 'Alti;
		this.movable._width = _width / 30.0;
		this.movable._height = _width / 40.0;
		this.movable._tu = 0.23;
		this.movable._tv = 0.45;
		this.movable._tu2 = 0.35;
		this.movable._tv2 = 0.53;
		this.movable._x = x + _width / 40.0;

		this.max = max;
		this.base = this.indicator._y + this.indicator._height - this.movable._height;
		this.step = this.indicator._height * 1.0 / this.max;
		this.movable._y = this.base - (init + 15) * this.step;
	}

	function update(val) {
		this.movable._y = this.base - (val + 15) * this.step;
	}

	function show(){
		this.group._visible = true;
		this.indicatorsMovable._visible = true;
	}

	function hide(){
		this.group._visible = false;
		this.indicatorsMovable._visible = false;
	}
}