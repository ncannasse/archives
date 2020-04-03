class Count {
	function Count(name, x, y, init, unit, res) {
		this.group = new Group(0.0004);
		this.name = name;
		this.count = new Text(this.group);
		this.count._font = res;
		this.xbase = x;
		this.count._y = y;
		this.unit = unit;
		this.count._text = string(init) + " " + this.unit;
		this.count._x = this.xbase - this.count._text._length * 14;
		this.count._color = 0xffffff;
	}

	function update(val) {
		this.count._text = string(val) + " " + this.unit;
		this.count._x = this.xbase - this.count._text._length * 14;
	}

	function show(){
		this.group._visible = true;
	}

	function hide(){
		this.group._visible = false;
	}


	function remove(){
		this.group.remove();
	}
}

class FlashCount extends Count {
	function FlashCount(name, x, y, init, unit, res) {
		this.lastVal = init;
		this.time = 20;
		super(name, x, y, init, unit, res);
	}

	function update(val) {
		this.time++;
		if( val != this.lastVal ) {
			this.time = 0;
			this.lastVal = val;
			super.update(val);
		}

		if( this.time < 20 )
			this.flash();
		else if( this.count._color != 0xffffffff )
			this.count._color = 0xffffffff;

	}

	function flash() {
		if( this.time % 2 == 0 )
			return;
		if( this.count._color == 0xffffffff )
			this.count._color = 0xffffff00;
		else
			this.count._color = 0xffffffff;
	}
}