import Math;
import Particules;

class Rock {
	function Rock(level, xMod) {
		//genere un bloc au hasard (forme, position, vitesse, rotation)
		var z = (random(21) - 1) * 5; // -5 <= z <= 95
		//if( z > 100 - (5 * level) ) // pour augmenter la proportion de meteors actifs avec les niveaux...
		//	z = 0; //gniark
		var xtemp = int(z / 2.0 + 7);
		var x = ((random(xtemp) * 2) - (xtemp - 1)) + xMod;
		var y = (z + 15) / 2.0; // 5 <= y <= 55
		this.limit = -y;
		var zoom = (random(40) + 1) / 60.0;
		this.trs = new TRS(new Vector(x, y, z), null, new Vector(zoom, zoom, zoom));

		var shape = random(3);
		if( shape == 0 )
			this.mesh = new Mesh('Shape1);
		else if( shape == 1 )
			this.mesh = new Mesh('Shape2);
		else if( shape == 2 )
			this.mesh = new Mesh('Shape3);

		if( this.mesh == null )
			print( "Erreur rock !!!" );

		this.mesh._pos = this.trs;
		this.mesh._color = 0xbe7f3c;
		if( z == 0 )
			this.mesh._color = 0xff0000;
		if( z < 0 ) {
			this.mesh._color = 0x00ff00;
			this.mesh._alpha = 0.8;
		}

		this.speed = new Vector(0, -((random(50) + 11) / 1000.0 * xtemp), 0);

		var rx = random(51) / 100.0;
		var ry = random(51) / 100.0;
		var rz = random(51) / 100.0;
		this.rot = new Quaternion(rx, ry, rz);

		this.life = random(level) + 2;
		this.points = 20 * this.life;
	}

	function remove() {
		this.mesh.remove();
	}

	function move() {
		this.trs._t += this.speed;
		this.trs._r *= this.rot;
		this.mesh._pos = this.trs;
	}

	function check_contact(axelay) {
		var stepNB = ceil(abs(this.speed._y / this.trs._sx));
		var step = this.speed._y / stepNB;
		var i = 0;
		var temp = this.trs._t;
		for( i = 0; i < stepNB; i++ ) {
			var test = ((temp._x - axelay.trs._tx) * (temp._x - axelay.trs._tx))
						+ ((temp._y - axelay.trs._ty) * (temp._y - axelay.trs._ty));
			if( test < (this.trs._sx + 1) * (this.trs._sx + 1) ) {
				this.life = 0; // détruit automatiquement
				axelay.rock_colision();
				return true;
			}
			temp += step;
		}
		return false;
	}

	function check_shots_contact(axelayShots) {
		var arr = axelayShots.toArray();
		var i = 0;
		for(i = 0; i < arr._length; i++) {
			var shoot = arr[i];
			var stepNB = ceil((shoot.speed / this.trs._sx)._length);
			var step = shoot.speed / stepNB;
			var j = 0;
			var temp = shoot.trs._t;
			for( j = 0; j < stepNB; j++ ) {
				var test = ((temp._x - this.trs._tx) * (temp._x - this.trs._tx))
							+ ((temp._y - this.trs._ty) * (temp._y - this.trs._ty));
				if( test < (this.trs._sx * this.trs._sx) ) {
					this.life -= shoot.strength;
					axelayShots.remove(shoot);
					shoot.remove();
					return true;
				}
				temp += step;
			}
		}
		return false;
	}
}

class RockEngine {
	function RockEngine(pe) {
		this.level = 1;
		this.time = 0;
		this.activeRocks = new List();
		this.collisionRocks = new List();
		this.nextCreate = random(this.level * 40) + (10 - this.level) * 4;
		this.pe = pe;
		this.count = 0;
	}

	function remove() {
		this.activeRocks.iter(fun(i) { i.remove(); } );
		this.activeRocks = new List();
		this.collisionRocks.iter(fun(i) { i.remove(); } );
		this.collisionRocks = new List();
		this.count = 0;
	}

	function update(axelay, axelayShots, fe, colisions) {
		this.time++;
		var peTemp = this.pe;
		var countTemp = this.count;
		function iter_move(r, o) {
			if( r.trs._ty < r.limit ) {
				r.remove();
				o.remove(r);
				countTemp--;
				return;
			}
			if( r.trs._tz == 0 && colisions) {
				if( r.check_contact(axelay) && r.life <= 0 ) {
					peTemp.add_multi(r.trs._tx, r.trs._ty, 10);
					r.remove();
					o.remove(r);
					countTemp--;
					return;
				}
				if( r.check_shots_contact(axelayShots) && r.life <= 0 ) {
					axelay.score += r.points;
					fe.addFS(r.trs._tx, r.trs._ty, r.points);
					peTemp.add_multi(r.trs._tx, r.trs._ty, 10);
					r.remove();
					o.remove(r);
					countTemp--;
					return;
				}
			}
			r.move();
		}

		var o = this.activeRocks;
		this.activeRocks.iter(fun(r) { iter_move(r, o); } );
		o = this.collisionRocks;
		this.collisionRocks.iter(fun(r) { iter_move(r, o); } );

		this.count = countTemp;

		if( this.time == this.nextCreate ) {
			this.time = 0;
			this.nextCreate = random(10 - this.level * 4) + (10 - this.level) * 4;
			var i = 0;
			var stop = random(this.level) + 1;
			for( i = 0; i < stop; i++ ) {
				if(	this.activeRocks._length + this.collisionRocks._length < 60
					&& this.activeRocks._length < 20 ) {
					var temp = new Rock(this.level, axelay.trs._tx);
					this.count++;
					if( temp.trs._tz == 0 )
						this.collisionRocks.add(temp);
					else
						this.activeRocks.add(temp);
				}
			}
		}
	}

	function change_speed(m) {
		function iter_speed(r, o) {
			r.speed *= m;
		}
		this.activeRocks.iter(fun(r){ r.speed *= m; } );
		this.collisionRocks.iter(fun(r){ r.speed *= m; } );
	}

	function speed_up() {
		this.change_speed(1.01);
	}

	function speed_down() {
		this.change_speed(0.99);
	}

	function level_up() {
		if( this.level < 9 )
			this.level++;
	}

	function level_down() {
		if( this.level > 1 )
			this.level--;
	}
}