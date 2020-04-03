import Math;
import Area;
import Fruit;
import Count;

/*
modes:	0 - flyng
		1 - running
		2 - landing
		3 - crashing
		4 - swimming
		5 - slipping
		6 - nothing
*/

class Perso {
	function Perso(name, trs, direction, speed, weight, limit, areas, fruits) {
		this.name = name;
		this.trs = trs;
		this.direction = direction;
		this.speed = 0;
		this.speedMax = 2 * speed;
		this.speedMin = 0.1;
		this.weight = weight;
		this.mesh = new Mesh('Delta);
		//this.mesh.add_anim('Action);
		this.direction.normalize();
		this.trs._r = new Quaternion(0, 0, atan2(-this.direction._x, this.direction._y));
		this.mesh._pos = this.trs;

		this.shadow = new Mesh('Delta);
		this.strs = new TRS(this.trs._t, this.trs._r, new Vector(1, 1, 0));
		this.strs._tz += 0.1;
		this.shadow._pos = this.strs;
		this.shadow._color = 0x000000;
		this.shadow._alpha = 1.0 / (abs(this.trs._tz - this.strs._tz) / 2.0);
		this.shadow._texture = null;

		this.upLevel = 0; //remplacer l'ensemble par int
		this.turnLevel = 0; //remplacer l'ensemble par int
		this.updateUpLevel = true;
		this.updateTurnLevel = true;
		this.forceDown = false;
		this.UP = new Vector(0,0,1);
		this.persoUp = new Vector(0,0,1);
		this.G = new Vector(0,0,-1);
		this.limit = limit;
		this.forceUTurnL = false;
		this.forceUTurnR = false;
		this.areas = areas;
		this.landingArea = null;
		this.fruits = fruits;
		this.mode = 1;
		this.lastQuat = this.trs._r;
		this.lastUpLevel = this.upLevel;
		this.lastPersoUp = this.persoUp;
		this.param = new Array(3);
		this.param[0] = 0.01;
		this.param[1] = 8.0;
		this.param[2] = 10.0;
		this.anim = null;
		this.sAnim = null;

		this.score = 0;
		this.scoreCpt = new Count("score", _width - 150, 30, this.score, "Points", 'Score);
	}

	function remove() {
		this.mesh.remove();
		this.shadow.remove();

		this.areas.iter(fun(area){
			area.remove();
			});

		this.fruits.iter(fun(fruit){
			fruit.remove();
			});

		this.scoreCpt.remove();
	}

	function roll(side) {
		var axis = this.direction;
		axis.normalise();
		axis /= 100.0;
		if(side < 0) {
			this.turnLevel -= this.param[0];
			this.turn_mesh(axis._x, axis._y, axis._z);
		}
		else {
			this.turnLevel += this.param[0];
			this.turn_mesh(-axis._x, -axis._y, -axis._z);
		}
	}

	function turn_mesh(rotX, rotY, rotZ) {
		this.trs._r *= new Quaternion(rotX, rotY, rotZ);
		this.strs._r *= new Quaternion(rotX, rotY, rotZ);
		this.direction *= (new Quaternion(rotX, rotY, rotZ)).toMatrix();
		this.persoUp *= (new Quaternion(rotX, rotY, rotZ)).toMatrix(); //experimental...
	}

	function move() {
		var dep = ((this.direction  * this.speed) + (this.G * this.weight));
		this.trs._t += dep;
		this.strs._tx += dep._x;
		this.strs._ty += dep._y;
		this.shadow._alpha = 1.0 / (abs(this.trs._tz - this.strs._tz) / 2.0);

		if(this.trs._tx > this.limit) {
			if(this.direction._y < 0 && !this.forceUTurnR)
				this.forceUTurnL = true;
			else if(!this.forceUTurnL)
				this.forceUTurnR = true;
		}
		else if(this.trs._tx < -this.limit) {
			if(this.direction._y < 0 && !this.forceUTurnL)
				this.forceUTurnR = true;
			else if(!this.forceUTurnR)
				this.forceUTurnL = true;
		}
		else if(this.trs._ty > this.limit) {
			if(this.direction._x < 0 && !this.forceUTurnL)
				this.forceUTurnR = true;
			else if(!this.forceUTurnR)
				this.forceUTurnL = true;
			}
		else if(this.trs._ty < -this.limit) {
			if(this.direction._x < 0 && !this.forceUTurnR)
				this.forceUTurnL = true;
			else if(!this.forceUTurnL)
				this.forceUTurnR = true;
		}
		else {
			this.forceUTurnL = false;
			this.forceUTurnR = false;
		}
	}

	function start_up() {
		if(this.mode == 1) {
			if(this.anim)
				this.anim.remove();
			if(this.sAnim)
				this.sAnim.remove();
			this.anim = new Anim(this.mesh);
			this.anim._anim = 'Action;
			this.anim._speed = -0.5;
			this.anim._frame = 30;
			this.sAnim = new Anim(this.shadow);
			this.sAnim._anim = 'Action;
			this.sAnim._speed = -0.5;
			this.sAnim._frame = 30;
			this.speed = 0.1;
		}
	}

	function stop_up() {
		if(this.mode == 1) {
			if(this.anim)
				this.anim.remove();
			if(this.sAnim)
				this.sAnim.remove();
			this.anim = new Anim(this.mesh);
			this.anim._anim = 'Action;
			this.anim.stop();
			this.anim._frame = 30;
			this.sAnim = new Anim(this.shadow);
			this.sAnim._anim = 'Action;
			this.sAnim.stop();
			this.sAnim._frame = 30;
			this.speed = 0;
		}
	}

	function start_down() {
		if(this.mode == 1) {
			if(this.anim)
				this.anim.remove();
			if(this.sAnim)
				this.sAnim.remove();
			this.anim = new Anim(this.mesh);
			this.anim._anim = 'Action;
			this.anim._speed = 0.5;
			this.anim._frame = 30;
			this.sAnim = new Anim(this.shadow);
			this.sAnim._anim = 'Action;
			this.sAnim._speed = 0.5;
			this.sAnim._frame = 30;
			this.speed = 0.1;
		}
	}

	function stop_down() {
		if(this.mode == 1) {
			if(this.anim)
				this.anim.remove();
			if(this.sAnim)
				this.sAnim.remove();
			this.anim = new Anim(this.mesh);
			this.anim._anim = 'Action;
			this.anim.stop();
			this.anim._frame = 30;
			this.sAnim = new Anim(this.shadow);
			this.sAnim._anim = 'Action;
			this.sAnim.stop();
			this.sAnim._frame = 30;
			this.speed = 0;
		}
	}

	function up(user) {
		if(this.mode == 1) {
			if(this.speed < 0.6)
				this.speed *= 1.01;
				if(this.anim._speed > -2) {
					this.anim._speed *= 1.01;
					this.sAnim._speed *= 1.01;
				}
			var nextPos = this.trs._t - (this.direction * this.speed);
			if(abs(nextPos._x) < this.limit && abs(nextPos._y) < this.limit) {
				this.trs._t = nextPos;
				this.strs._t -= this.direction * this.speed;
			}
		}
		if(user && this.mode == 2) {
			this.speed *= this.landingArea.slideFactor;
		}
		if(!user || (this.mode == 0 && !(this.forceDown || this.forceUTurnL || this.forceUTurnR))) {
			if(this.upLevel < 1 && this.speed > this.speedMin) {
				var temp = this.param[0] * (abs(this.upLevel) + 1);
				this.upLevel += temp;
				var axis = vcross(this.direction, this.UP);
				axis.normalise();
				axis *= temp;
				this.turn_mesh(axis._x, axis._y, axis._z);
			}
			this.updateUpLevel = false;
		}
	}

	function down(user) {
		if(this.mode == 1) {
			if(this.speed < 0.6)
				this.speed *= 1.01;
				if(this.anim._speed < 2) {
					this.anim._speed *= 1.01;
					this.sAnim._speed *= 1.01;
				}
			var nextPos = this.trs._t + (this.direction * this.speed);
			if(abs(nextPos._x) < this.limit && abs(nextPos._y) < this.limit) {
				this.trs._t = nextPos;
				this.strs._t += this.direction * this.speed;
			}
		}
		if(!user || (this.mode == 0 && !(this.forceUTurnL || this.forceUTurnR))) {
			if(this.upLevel > -1) {
				var temp = this.param[0] * (abs(this.upLevel) + 1);
				this.upLevel -= temp;
				var axis = vcross(this.direction, this.UP);
				axis.normalise();
				axis *= temp;
				this.turn_mesh(-axis._x, -axis._y, -axis._z);
			}
			this.updateUpLevel = false;
		}
	}

	function left(user) {
		if(this.mode == 1) {
			var dep = vcross(this.direction, this.UP);
			var nextPos = this.trs._t + dep;
			if(abs(nextPos._x) < this.limit && abs(nextPos._y) < this.limit) {
				this.trs._t += dep;
				this.strs._t += dep;
			}
		}
		if(!user || (this.mode == 0 && !(this.forceUTurnL || this.forceUTurnR))) {
			this.turn_mesh(0, 0, -this.param[0]);
			if(this.turnLevel > -1)
				this.roll(-1);
			this.updateTurnLevel = false;
			if(user) {
				return true;
			}
		}
		return false;
	}

	function right(user) {
		if(this.mode == 1) {
			var dep = vcross(this.direction, this.UP) * -1;
			var nextPos = this.trs._t + dep;
			if(abs(nextPos._x) < this.limit && abs(nextPos._y) < this.limit) {
				this.trs._t += dep;
				this.strs._t += dep;
			}
		}
		if(!user || (this.mode == 0 && !(this.forceUTurnL || this.forceUTurnR))) {
			this.turn_mesh(0, 0, this.param[0]);
			if(this.turnLevel < 1)
				this.roll(1);
			this.updateTurnLevel = false;
			if(user) {
				return true;
			}
		}
		return false;
	}

	function check_colisions() {
		var dep = ((this.direction  * this.speed) + (this.G * this.weight));

		this.landingArea = null;
		var areasArray = this.areas.toArray();
		var i;
		for (i = 0; i < areasArray._length; i++) {
			if(areasArray[i].check_contact(this.trs._t, 0.7)) { //0.7 rayon de test
				this.landingArea = areasArray[i];
				break;
			}
		}

		var fruitsArray = this.fruits.toArray();
		var j;
		var steps = ceil(dep._length);//divisé par le rayon de colision mais il vaut 1 pour l'instant
		for(j = 0; j < steps; j++) {
			for(i = 0; i < fruitsArray._length; i++) {
				if(fruitsArray[i].check_contact(this.trs._t + this.persoUp + (dep / steps), 1)) { //1 rayon de test
					fruitsArray[i].at_contact(this);
					this.fruits.remove(fruitsArray[i]);
					break;
				}
			}
		}

		if(this.landingArea == null) {
			this.strs._tz = 1;
			if(this.mode == 1) {
				var dep = new Vector(0, 0, -2);
				this.trs._t += dep;
			}
			if(this.mode == 5) {
				var dep = new Vector(0, 0, -2);
				this.trs._t += dep;
				this.trs._r = this.lastQuat;
				this.upLevel = this.lastUpLevel;
				this.persoUp = this.lastPersoUp;
			}
			if(this.trs._tz < 1 || this.trs._tz + dep._z < 1) {
				while(this.upLevel != 0){
					this.update_upLevel();
				}
				this.weight = 0;
				this.direction = new Vector(0, 0, -0.001);
				this.mode = 3;//plouf
			}
			else {
				if(this.anim)
					this.anim.remove();
				if(this.sAnim)
					this.sAnim.remove();
				this.mode = 0;
			}
			return;
		}

		if(this.mode == 1)
			return;

		var areaHeight = this.landingArea.height;

		if(this.trs._tz - areaHeight < -1.5) {
			while(this.upLevel != 0){
				this.update_upLevel();
			}
			this.weight = 0;
			this.direction = new Vector(0, 0, -0.001);
			this.mode = 3;
			return;
		}

		if(this.trs._tz - areaHeight < -0.1
			|| this.trs._tz + dep._z - areaHeight < -1.5) {
			while(this.turnLevel != 0){
				this.update_turnLevel();
			}
			if(this.mode != 5) {
				this.lastQuat = this.trs._r;
				this.lastUpLevel = this.upLevel;
				this.lastPersoUp = this.persoUp;
			}
			this.trs._tz = areaHeight - 0.2;
			this.strs._tz = this.landingArea.height - 0.1;
			this.mode = 5;
			return;
		}

		this.strs._tz = this.landingArea.height + 0.1;

		if(this.trs._tz - areaHeight < 0.2
			|| this.trs._tz + dep._z - areaHeight < -0.1 ) { // pour s'assurer que
			//le prochain mouvement ne nous emmenera pas dans l'aire d'aterrissage

			if(this.mode != 2) {
				if(this.anim)
					this.anim.remove();
				if(this.sAnim)
					this.sAnim.remove();
				this.anim = new Anim(this.mesh);
				this.anim._anim = 'Action;
				this.anim._speed = this.speed * 4;
				this.anim._frame = 30;
				this.sAnim = new Anim(this.shadow);
				this.sAnim._anim = 'Action;
				this.sAnim._speed = this.speed * 4;
				this.sAnim._frame = 30;
				this.mode = 2;
			}

			this.trs._tz = areaHeight;
			while(this.turnLevel != 0){
				this.update_turnLevel();
			}
			while(this.upLevel != 0){
				this.update_upLevel();
			}
			area.at_contact();
			return;
		}
	}

	function update_upLevel() {
		if(this.upLevel > -this.param[0] && this.upLevel < this.param[0]) {
			this.upLevel = 0;
			this.direction._z = 0;
		}
		if(this.updateUpLevel && this.upLevel != 0) {
			if(this.upLevel < 0)
				this.up(false);
			else if(this.upLevel > 0)
				this.down(false);
			if(this.upLevel <= this.param[0])
				this.forceDown = false;
		}
		this.updateUpLevel = true;
	}

	function update_turnLevel() {
		if(this.turnLevel > -this.param[0] && this.turnLevel < this.param[0])
			this.turnLevel = 0;
		if(this.updateTurnLevel && this.turnLevel != 0) {
			if(this.turnLevel < 0)
				this.roll(1);
			else if(this.turnLevel > 0)
				this.roll(-1);
		}
		this.updateTurnLevel = true;
	}

	function update_flying() {
		if(this.speed > this.speedMax)
			this.speed = this.speedMax;
		if(this.speed < this.speedMin) {
			this.forceDown = true;
			this.speed = this.speedMin;
		}
		this.check_colisions();
		if(this.forceUTurnL)
			this.left(false);
		if(this.forceUTurnR)
			this.right(false);
		this.move();
		this.update_upLevel();
		this.update_turnLevel();
	}

	function update_running() {
		this.check_colisions();
	}

	function update_landing() {
		if(this.speed > 0.05) {
			this.speed *= this.landingArea.slideFactor;
			this.anim._speed = this.speed * 4;
			this.sAnim._speed = this.speed * 4;
			this.move();
			this.check_colisions();
		}
		else {
			if(this.anim)
				this.anim.remove();
			if(this.sAnim)
				this.sAnim.remove();
			this.anim = new Anim(this.mesh);
			this.anim._anim = 'Action;
			this.anim.stop();
			this.anim._frame = 30;
			this.sAnim = new Anim(this.shadow);
			this.sAnim._anim = 'Action;
			this.sAnim.stop();
			this.sAnim._frame = 30;
			this.speed = 0;

			var temp = new Vector(this.trs._tx, this.trs._ty, 0);
			temp -= this.landingArea._center;
			var dToCenter = temp._length;
			temp = floor(dToCenter / this.landingArea.decRadius);

			if(temp <= 2)
				this.score += this.landingArea.points / pow(2, temp);
			this.mode = 6;
		}
	}

	function update_crashing() {
		var dep = ((this.direction  * this.speed) + (this.G * this.weight));
		if(this.direction._z == 0)
			this.direction._z = -0.001;
		this.direction *= 1.1;
		if(this.trs._tz < 1 || this.trs._tz + dep._z < 1)
			this.shadow.remove();
		if(this.trs._tz < -10 || this.trs._tz + dep._z < -10){
			this.upLevel = 0;
			this.turnLevel = 0;
			this.speed = 0;
			this.mode = 4;//plouf
		}
		else
			this.move();
	}

	function update_swimming() {
		display("trop dur !");
	}

	function update_slipping() {
		if(this.persoUp._z > 0.1) {
			var axis = vcross(this.direction, this.UP);
			axis.normalise();
			axis *= -0.1;
			var q = new Quaternion(axis._x, axis._y, axis._z);
			this.persoUp *= (q.toMatrix());
			this.trs._r *= q;
		}
		if(this.speed > 0.05) {
			this.speed *= this.landingArea.slideFactor;
			this.move();
			this.check_colisions();
		}
		else {
			this.speed = 0;
			var temp = new Vector(this.trs._tx, this.trs._ty, 0);
			temp -= this.landingArea._center;
			var dToCenter = temp._length;
			temp = floor(dToCenter / this.landingArea.decRadius);
			if(temp <= 2)
				this.score += this.landingArea.points / pow(2, temp);
			this.mode = 6;
		}
	}

	function update() {
		//display(this.trs._t);
		//display(this.direction);
		//display(this.speed);

		this.mesh._pos = this.trs;
		this.shadow._pos = this.strs;

		this.scoreCpt.update(this.score);

		if(this.upLevel > 0)
			this.speed *= 1 - this.upLevel / this.param[1];
		if(this.upLevel < 0)
			this.speed *= 1 - this.upLevel / this.param[2];

		if(this.mode == 0) {
			display("flying...");
			this.update_flying();
			return;
		}

		if(this.mode == 1) {
			display("running...");
			this.update_running();
			return;
		}

		if(this.mode == 2) {
			display("landing...");
			this.update_landing();
			return;
		}

		if(this.mode == 3) {
			display("crashing...");
			this.update_crashing();
			return;
		}

		if(this.mode == 4) {
			display("swimming...");
			this.update_swimming();
			return;
		}

		if(this.mode == 5) {
			display("slipping...");
			this.update_slipping();
			return;
		}
	}
}