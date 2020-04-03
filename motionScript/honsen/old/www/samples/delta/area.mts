import Math;

class Area {
	function Area(name, points, decRadius, bottomX, bottomY,
					topX, topY, height, slideFactor, mesh) {
		this.name = name;
		this.points = points;
		this.decRadius = decRadius;
		this.bottomX =bottomX;
		this.bottomY = bottomY;
		this.topX = topX;
		this.topY = topY;
		this.height = height;
		this.slideFactor =  slideFactor;
		this.mesh = mesh;
	}

	function distance_to(pos) { //fonctionne si le rectangle est aligné sur les axes ce qui est toujours le cas d'une "area"
		var newPos = new Vector(abs(pos._x - this._center._x) - abs(this.topX - this.bottomX) / 2.0,
								abs(pos._y - this._center._y) - abs(this.topY - this.bottomY) / 2.0,
								0);
		if(newPos._x < 0 && newPos._y < 0)
			if(newPos._x > newPos._y)
				return -newPos._x;
			else
				return -newPos._y;
		if(newPos._x >= 0 && newPos._y < 0)
			return newPos._x;
		if(newPos._x < 0 && newPos._y >= 0)
			return newPos._y;

		return sqrt(newPos._x * newPos._x + newPos._y * newPos._y);

	}

	function check_contact(pos, radius) {
		if((pos._x >= this.bottomX && pos._x <= this.topX)
			&& (pos._y >= this.bottomY && pos._y <= this.topY))
			return true;
		else
			return this.distance_to(pos) < radius;
	}

	function at_contact() {
		display("contact avec ", this.name);
	}

	function get_center() {
		return new Vector((this.bottomX + this.topX) / 2.0, (this.bottomY + this.topY) / 2.0, 0);
	}

	function remove() {
		this.mesh.remove();
	}
}

function create_area(type, name, points, decRadius, bottomX, bottomY,
						topX, topY, size, slideFactor) {
	var mesh;
	if(type == "Normal")
		mesh = new Mesh('Normal_area);
	else if(type == "Icy")
		mesh = new Mesh('Icy_area);
	else if(type == "Glue")
		mesh = new Mesh('Glue_area);
	else if(type == "Cliff")
		mesh = new Mesh('Cliff);
	else {
		print("invalid area type");
		return null;
	}
	var trs = new TRS(new Vector((topX + bottomX) / 2.0, (bottomY + topY) / 2.0, 0),
						null,
						size);
	mesh._pos = trs;
	return new Area(name, points, decRadius, bottomX, bottomY,
					topX, topY, size._z, slideFactor, mesh);
}