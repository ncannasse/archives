import Fruit;
import Area;
import Perso;

class Perso_data {
	function Perso_data(nom, px, py, pz, dx, dy, dz, speed, weight, limit) {
		this.name = name;
		this.trs = new TRS(new Vector(px, py, pz));
		this.direction = new Vector(dx, dy, dz);
		this.speed = speed;
		this.weight = weight;
		this.limit = limit;
	}
}

class Areas_data {
	function Areas_data(type, name, points, decRadius, bottomX, bottomY,
						topX, topY, sizeX, sizeY, sizeZ, slideFactor) {
		this.type = type;
		this.name = name;
		this.points = points;
		this.decRadius = decRadius;
		this.bottomX = bottomX;
		this.bottomY = bottomY;
		this.topX = topX;
		this.topY = topY;
		this.size = new Vector(sizeX, sizeY, sizeZ);
		this.slideFactor = slideFactor;
	}
}

class Fruits_data {
	function Fruits_data(type, px, py, pz, size) {
		this.type = type;
		this.pos = new Vector(px, py, pz);
		this.size = size;
	}
}

class Level {
	function Level(perso_data, areas_data_list, fruits_data_list) {
		this.perso_data = perso_data;
		this.areas_data_list = areas_data_list;
		this.fruits_data_list = fruits_data_list;
	}

	function load() {
		var areas = new List();
		var areas_data_tab = this.areas_data_list.toArray();
		var fruits_data_tab = this.fruits_data_list.toArray();
		var i;
		var t;
		for(i=0; i < areas_data_tab._length; i++) {
			t = areas_data_tab[i];
			areas.add(create_area(t.type, t.name, t.points, t.decRadius, t.bottomX, t.bottomY,
									t.topX, t.topY, t.size, t.slideFactor));
		}

		var fruits = new List();
		for(i=0; i < fruits_data_tab._length; i++) {
			t = fruits_data_tab[i];
			fruits.add(create_fruit(t.type, t.pos, t.size, areas));
		}
		t = this.perso_data;
		return new Perso(t.name, new TRS(t.trs), t.direction, t.speed, t.weight, t.limit, areas, fruits);
	}
}