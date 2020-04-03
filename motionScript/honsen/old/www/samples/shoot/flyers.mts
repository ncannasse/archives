class FlyingScore {
	function FlyingScore(group, x, y, points) {
		this.text = new Text(group);
		this.text._color = 0xffffff;
		this.text._font = 'Score;
		this.text._x = x - 15;
		this.text._y = y - 10;
		this.text._text = string(points);
	}

	function remove() {
		this.text.remove();
	}

	function update() {
		if( this.text._alpha > 0 ) {
			this.text._x += 4;
			this.text._y -= 3;
			this.text._alpha -= 0.05;
			return true;
		}
		else
			this.remove();
		return false;
	}
}

class FlyersEngine {
	function FlyersEngine() {
		this.group = new Group(0.0002);
		this.flyers = new List();
	}

	function remove() {
		this.flyers = null;
		this.group.remove();
	}

	function addFS(x, y, points) {
		if( y < -5 || y > 5 || x < camera._pos._x - 7 || x > camera._pos._x + 7 )
			return;
		var u = ((x - camera._pos._x + 7.0) / 14.0) * _width;
		var v = (1 - ((y + 5.0) / 10.0)) * _height;
		this.flyers.add(new FlyingScore(this.group, u, v, points));
	}

	function update() {
		var o = this.flyers;
		this.flyers.iter(fun(f){
			if( !f.update() )
				o.remove(f);
			});
	}
}