import Math;
import Weapon;

//*******************************************
//		generic Shuttle
//*******************************************

class Shuttle {
	function Shuttle(name, pos, mesh, weapon, life) {
		this.name = name;
		//rotate the mesh to have the up down
		this.trs = new TRS(pos, new Quaternion(0, pi, 0), new Vector(0.5, 0.5, 0.5));
		this.mesh = mesh;

		if( this.mesh == null )
			print( "Erreur shuttle !!!" );

		this.mesh._texture = null;
		this.mesh._pos = this.trs;
		this.cap = 0;
		this.updateCap = false;
		this.dep = 0;
		this.updateDep = false;

		this.weapon = weapon;
		this.life = life;
	}

	function remove() {
		this.mesh.remove();
	}

// update functions *************************

	function update() {
		this.update_cap();
		this.update_dep();
		this.mesh._pos = this.trs;
	}

	function update_cap() {
		if( this.updateCap && abs(this.cap) > 0.001 ) {
			var temp = this.cap;
			if( this.cap < 0 ) {
				temp += 0.02;
				if( temp <= 0 )
					this.move_left(0.02, false, 0.2);
				else
					this.move_left(-this.cap, false, 0.2);
			}
			else {
				temp -= 0.02;
				if( temp >= 0 )
					this.move_right(-0.02, false, 0.2);
				else
					this.move_right(-this.cap, false, 0.2);
			}
		}
	}

	function update_dep() {
		if( this.updateDep && abs(this.dep) > 0.001 ) {
			var temp = this.dep;
			if( this.dep < 0 ) {
				temp += 0.01;
				if( temp <= 0 )
					this.move_up(0.01);
				else
					this.move_up(-this.dep);
			}
			else {
				temp -= 0.02;
				if( temp >= 0 )
					this.move_down(-0.02);
				else
					this.move_down(-this.dep);
			}
		}
	}

//*******************************************

	function roll(r) {
		this.trs._r *= new Quaternion(0, r, 0);
		this.cap += r;
	}

	function translate(t) {
		this.trs._t += new Vector(0, t, 0);
		this.dep += t;
	}

// real move functions **********************

	function move_left(m, move, dep) {
		if( this.cap < 0.5 )
			this.roll(m);
		if( move && this.cap >= 0 )
			this.trs._tx -= dep;
	}

	function move_right(m, move, dep) {
		if( this.cap > -0.5 )
			this.roll(m);
		if( move && this.cap <= 0 )
			this.trs._tx += dep;
	}

	function move_up(m) {
		if( this.dep < 1 )
			this.translate(m);
	}

	function move_down(m) {
		if( this.dep > -0.7 )
			this.translate(m);
	}

//*******************************************

	function shoot(shotsList) {
		this.weapon.shoot(this.trs._tx, this.trs._ty, shotsList);
	}

// function usable from main ****************

	function left() {
		this.updateCap = false;
		var temp = 0.01;
		if( this.cap < 0 )
			temp += 0.06;
		this.move_left(temp, true, 0.2);
	}

	function left_released() {
		this.updateCap = true;
	}

	function right() {
		this.updateCap = false;
		var temp = -0.01;
		if( this.cap > 0 )
			temp -= 0.06;
		this.move_right(temp, true, 0.2);
	}

	function right_released() {
		this.updateCap = true;
	}

	function up() {
		this.updateDep = false;
		var temp = 0.02;
		if( this.dep < 0 )
			temp += 0.01;
		this.move_up(temp);
	}

	function up_released() {
		this.updateDep = true;
	}

	function down() {
		this.updateDep = false;
		var temp = -0.01;
		if( this.dep > 0 )
			temp -= 0.02;
		this.move_down(temp);
	}

	function down_released() {
		this.updateDep = true;
	}

//*******************************************

	function check_shots_contact(shots) {
		var arr = shots.toArray();
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
					shots.remove(shoot);
					shoot.remove();
					return true;
				}
				temp += step;
			}
		}
		return false;
	}

}


//*******************************************
//		Axelay
//*******************************************

class Axelay extends Shuttle {
	function Axelay(x, y, z) {
		this.score = 0;
		this.saves = 3;
		this.cptGroup = new Group(0.0002);
		this.picto = new Sprite(this.cptGroup); //doit etre initialisée
		this.picto._bitmap = 'AxelayPicto;
		this.picto._x = 4;
		this.picto._y = _height - 24;
		this.picto._width = 20;
		this.picto._height = 20;
		this.cpt = new Text(this.cptGroup);
		this.cpt._color = 0xffffff;
		this.cpt._font = 'Saves;
		this.cpt._text = "X " + string(this.saves);
		this.cpt._x = 28;
		this.cpt._y = _height - 6;
		this.invincibility = 0;
		super("Axelay", new Vector(x, y, z), new Mesh('Delta),
				new VWeapon(0.4), 5);
	}

	function remove() {
		this.cptGroup.remove();
	}

	function respawn(x, y, z) {
		this.mesh = new Mesh('Delta);
		this.trs._t = new Vector(x, y, z);
		this.mesh._texture = null;
		this.mesh._pos = this.trs;
		this.cap = 0;
		this.updateCap = false;
		this.dep = 0;
		this.updateDep = false;
		this.life = 5;
		this.trs._r = new Quaternion(0, pi, 0);
		this.invincibility = 120;
	}

	function rock_colision() {
		if( this.invincibility == 0 )
			this.life = 0;
	}

	function enemy_colision() {
		if( this.invincibility == 0 )
			this.life = 0;
	}

	function left() {
		super.left();
		if( this.cap >= 0 && abs(this.trs._tx - camera._pos._x) > 5 ) {
			camera._pos += new Vector(-0.2, 0, 0);
			camera._target += new Vector(-0.2, 0, 0);
		}
	}

	function right() {
		super.right();
		if( this.cap <= 0 && abs(this.trs._tx - camera._pos._x) > 5 ) {
			camera._pos += new Vector(0.2, 0, 0);
			camera._target += new Vector(0.2, 0, 0);
		}
	}

	function update(enemiesShots, pe) {
		if( this.invincibility > 0 ) {
			this.invincibility--;
			if( this.invincibility % 2 == 0 )
				if( this.mesh._alpha == 1 )
					this.mesh._alpha = 0.2;
				else
					this.mesh._alpha = 1;
			if( this.invincibility == 0 )
				this.mesh._alpha = 1;
		}
		if( this.invincibility == 0 ) {
			this.check_shots_contact(enemiesShots);
			if( this.life <= 0 ) {
				this.life = 0;
				pe.add_multi(this.trs._tx, this.trs._ty, 20);
				this.saves_down();
				super.remove();
				return false;
			}
		}
		super.update();
		return true;
	}

	function change_saves(m) {
		this.saves += m;
		this.cpt._text = "X" + string(this.saves);
	}

	function saves_up() {
		this.change_saves(1);
	}

	function saves_down() {
		this.change_saves(-1);
	}

	function change_life(m) {
		this.life += m;
		if( this.life > 5 )
			this.life = 5;
	}
}


//*******************************************
//		Enemy
//*******************************************

class Enemy extends Shuttle {
	function Enemy(name, mesh, xMod, life, weapon, shotsList) {
		var z = (random(4) - 1) * 5 + 2.5; // -2.5 <= z <= 12.5
		var x = random(13) - 6 + xMod;
		this.speed = new Vector(0, (random(20) + 4) / 40.0, 0);
		this.time = 0;
		this.shotTime = random(31) + 10;
		this.shotsList = shotsList;
		this.points = (random(5) + 1) * 100;
		this.bonus = 0;
		super(name, new Vector(x, -10, z), mesh, weapon, life);
	}

	function update(updateTime) {
		if( updateTime )
			this.time++;
		if( this.time == this.shotTime && this.bonus == 0 ) {
			this.time = 0;
			this.shotTime = random(31) + 10;
			this.shoot(this.shotsList);
		}
		this.trs._t += this.speed;
		super.update();
	}

	function check_contact(axelay) {
		var stepNB = ceil(abs(this.speed._y)); // division par rayon, ici 1
		var step = this.speed._y / stepNB;
		var i = 0;
		var temp = this.trs._t;
		for( i = 0; i < stepNB; i++ ) {
			var test = ((temp._x - axelay.trs._tx) * (temp._x - axelay.trs._tx))
						+ ((temp._y - axelay.trs._ty) * (temp._y - axelay.trs._ty));
			if( test < (this.trs._sx + 1) * (this.trs._sx + 1) ) { // +1 correspond au rayon de axelay
				this.life = 0; // détruit automatiquement
				if( this.bonus == 0 )
					axelay.enemy_colision();
				else if( this.bonus == 4 )
					axelay.saves_up();
				else if( this.bonus == 7 )
					axelay.change_life(2);
				return true;
			}
			temp += step;
		}
		return false;
	}

	function transform() {
		this.bonus = random(10);
		if( this.bonus == 4 ) {
			this.mesh.remove();
			this.mesh = new Mesh('LifeBonus);
			super.update();
			return true;
		}
		else if( this.bonus == 7 ) {
			this.mesh.remove();
			this.mesh = new Mesh('HealthBonus);
			super.update();
			return true;
		}
		this.bonus = 0;
		return false;
	}
}


//*******************************************
//		Enemies types
//*******************************************

class CommonEnemy extends Enemy {
	function CommonEnemy(xMod, shotsList) {
		super("Common", new Mesh('Common), xMod, 1,
				randomWeapon(-0.4), shotsList);
	}
}

class CommonEnemyM extends Enemy {
	function CommonEnemyM(xMod, shotsList) {
		super("CommonM", new Mesh('Common), xMod, 1,
				randomWeapon(-0.4), shotsList);
		this.turnTime = 0;
	}

	function update(updateTime) {
		if( updateTime && this.bonus == 0 ) {
			this.turnTime+=0.05;
			var cosTime = cos(this.turnTime);
			if( cosTime < -0.85 ) {
				this.updateCap = false;
				this.move_left(0.01, true, 0.05);
			}
			else if( cosTime > 0.85 ) {
				this.updateCap = false;
				this.move_right(-0.01, true, 0.05);
			}
			else
				this.updateCap = true;
		}
		super.update(updateTime);
	}
}

//*******************************************
//		EnemyGenerator
//*******************************************

class EnemyEngine {
	function EnemyEngine(pe, shotsList) {
		this.level = 1;
		this.time = 0;
		this.activeEnemies = new List();
		this.enemies = new List();
		this.nextCreate = random(this.level * 40) + (10 - this.level) * 4;
		this.pe = pe;
		this.shotsList = shotsList;
		this.count = 0;
	}

	function remove() {
		this.enemies.iter(fun(e){ e.remove(); } );
		this.enemies = new List();
		this.activeEnemies.iter(fun(e){ e.remove(); } );
		this.activeEnemies = new List();
		this.count = 0;
	}

	function update(axelay, axelayShots, fe, colisions) {
		this.time++;
		var peTemp = this.pe;
		var aeTemp = this.activeEnemies;
		var countTemp = this.count;

		function iter_move(e, o) {
			if( e.trs._ty < -6 ) {
				e.remove();
				o.remove(e);
				countTemp--;
				return;
			}

			if( colisions && e.check_contact(axelay) && e.life <= 0 ) {
				if( e.bonus == 0 )
					peTemp.add_multi(e.trs._tx, e.trs._ty, 10);
				e.remove();
				o.remove(e);
				countTemp--;
				return;
			}

			if( e.bonus == 0 && colisions && e.check_shots_contact(axelayShots) && e.life <= 0 ) {
				axelay.score += e.points;
				fe.addFS(e.trs._tx, e.trs._ty, e.points);
				peTemp.add_multi(e.trs._tx, e.trs._ty, 10);
				if( !e.transform() ) {
					e.remove();
					o.remove(e);
					countTemp--;
				}
				return;
			}
			e.update(true);
		}

		function iter_approach(e, o) {
			if( e.trs._ty > 10 ) {
				e.trs._t = new Vector(e.trs._tx, e.trs._ty, 0);
				e.trs._r *= new Quaternion(0, 0, pi);
				o.remove(e);
				aeTemp.add(e);
				e.speed = new Vector(0, -(random(20) + 4) / 150.0, 0);
				return;
			}
			e.update(false);
		}

		var o = this.activeEnemies;
		this.activeEnemies.iter(fun(e){ iter_move(e, o); } );
		o = this.enemies;
		this.enemies.iter(fun(e){ iter_approach(e, o); } );

		this.count = countTemp;

		if( this.time == this.nextCreate ) {
			this.time = 0;
			this.nextCreate = random(10 - this.level * 40) + (10 - this.level) * 40;
			var i = 0;
			var stop = random(this.level) + 1;
			for( i = 0; i < stop; i++ ) {
				if(	this.activeEnemies._length < 5 ) {
					var temp ;
					var enemyRandom = random(2); // random sur le nombre de type d'enemis dispo
					if( enemyRandom == 0 ) {
						temp = new CommonEnemy(axelay.trs._tx, this.shotsList);
						this.count++;
					}
					else if( enemyRandom == 1 ){
						temp = new CommonEnemyM(axelay.trs._tx, this.shotsList);
						this.count++;
					}
					else
						return;
					this.enemies.add(temp);
				}
			}
		}
	}

	function level_up() {
		if( this.level < 9 )
			this.level++;
	}

	function level_down() {
		if( this.level > 1 )
			this.level--;
	}

	function change_speed(m) {
		this.enemies.iter(fun(e){ e.speed *= m; } );
		this.activeEnemies.iter(fun(e){ e.speed *= m; } );
	}

	function speed_up() {
		this.change_speed(1.01);
	}

	function speed_down() {
		this.change_speed(0.99);
	}
}