import Math;

class Fruit {
	function Fruit(name, size, mesh, shadow, areas, trs) {
		this.name = name;
		this.size = size;
		this.mesh = mesh;
		this.shadow = shadow;

		this.mesh._pos = trs;

		var landingArea = null;
		var areasArray = areas.toArray();
		var i;
		for (i = 0; i < areasArray._length; i++) {
			if(areasArray[i].check_contact(trs._t, size)) {
				landingArea = areasArray[i];
				break;
			}
		}

		this.shadow._texture = null;
		this.shadow._color = 0x000000;
		trs._sz = 0;
		if(landingArea == null)
			trs._tz = 0.5;
		else
			trs._tz = landingArea.height + 0.1;
		this.shadow._alpha = 1.0 / (abs(mesh._pos.pos()._z - trs._tz) / 2.0);
		this.shadow._pos = trs;
	}

	function check_contact(pos, radius) {
		return ((this.mesh._pos.pos() - pos)._length < (this.size + radius));
	}

	function at_contact() {
		print ("ramasssé fruit inconnu");
	}

	function remove() {
		this.mesh.remove();
		this.shadow.remove();
	}
}

class Banana extends Fruit{
	function Banana(pos, size, areas) {
		var mesh = new Mesh('Banana);
		var trs = new TRS(pos, null, new Vector(size, size, size));
		var shadow = new Mesh('Banana);
		super("Banana", size, mesh, shadow, areas, trs);
	}

	function at_contact(perso) {
		super.remove();
		perso.speed += 0.2;
		perso.score += 20;
	}
}

class Orange extends Fruit{
	function Orange(pos, size, areas) {
		var mesh = new Mesh('Apple);
		var trs = new TRS(pos, null, new Vector(size, size, size));
		var shadow = new Mesh('Banana);
		super("Orange", size, mesh, shadow, areas, trs);
	}

	function at_contact(perso) {
		super.remove();
		perso.score += 10;
	}
}

class Apple extends Fruit{
	function Apple(pos, size, areas) {
		var mesh = new Mesh('Apple);
		var trs = new TRS(pos, null, new Vector(size, size, size));
		var shadow = new Mesh('Apple);
		super("Apple", size, mesh, shadow, areas, trs);
	}

	function at_contact(perso) {
		super.remove();
		perso.weight += 0.002;
		perso.score += 50;
	}
}

function create_fruit(type, pos, size, areas) {
	if(type == "Banana")
		return new Banana(pos, size, areas);
	else if(type == "Apple")
		return new Apple(pos, size, areas);
	else if(type == "Orange")
		return new Orange(pos, size, areas);
	else{
		print("invalid fruit type");
		return null;
	}
}