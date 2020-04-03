class Count {
	function Count(name, x, y, init, unit, res) {
		this.group = new Group(0.0004);
		this.name = name;
		this.count = new Text(this.group);
		this.count._font = res;
		this.count._x = x;
		this.count._y = y;
		this.count._text = string(init) + " " + unit;
		this.count._color = 0xffffff;
		this.unit = unit;
	}

	function update(val) {
		this.count._text = string(val) + " " + this.unit;
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