class Scrolling {
	function Scrolling(speed, z, alpha, res) {
		this.group = new Group(z);
		this.scroll = new Sprite(this.group);
		this.scroll._bitmap = res;
		this.scroll._x = 0;
		this.scroll._y = 0;
		this.scroll._width = _width;
		this.scroll._height = _height;
		this.scroll._tu = 0.1;
		this.scroll._tu2 = 0.9;
		this.scroll._tv = 0.7;
		this.scroll._tv2 = 1;
		this.scroll._alpha = alpha;
		this.speed = 1;
		this.speedFactor = speed;
	}

	function remove() {
		this.group.remove();
	}

	function update() {
		this.scroll._tv -= 0.01 * this.speed * this.speedFactor;
		this.scroll._tv2 -= 0.01 * this.speed * this.speedFactor;
	}

	function left() {
		this.scroll._tu -= 0.001 * this.speed * this.speedFactor;
		this.scroll._tu2 -= 0.001 * this.speed * this.speedFactor;
	}

	function right() {
		this.scroll._tu += 0.001 * this.speed * this.speedFactor;
		this.scroll._tu2 += 0.001 * this.speed * this.speedFactor;
	}
}

class ScrollEngine {
	function ScrollEngine() {
		this.list = new List();
		this.speed = 0;
	}

	function remove() {
		this.list.iter(fun(i){ i.remove(); } );
		this.list = new List();
	}

	function add(s) {
		this.list.add(s);
		//assume same speed for each plan
		this.speed = s.speed;
	}

	function changeSpeed(m) {
		this.list.iter(fun(s){ s.speed += m; } );
		this.speed += m;
	}

	function getSpeed() {
		return this.speed;
	}

	function left() {
		this.list.iter(fun(s){ s.left(); } );
	}

	function right() {
		this.list.iter(fun(s){ s.right(); } );
	}

	function update() {
		this.list.iter(fun(s){ s.update(); } );
	}
}