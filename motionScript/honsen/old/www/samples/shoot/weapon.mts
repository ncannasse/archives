import Shots;

class Weapon {
	function Weapon(speed) {
		this.speed = speed;
	}
}

class BasicWeapon extends Weapon {
	function BasicWeapon(yspeed) {
		super(yspeed);
	}

	function shoot(x, y, shotsList) {
		shotsList.add(new Shoot(new Vector(x, y, 0), new Vector(0, this.speed, 0), new Mesh('Ball), 1));
	}
}

class DualWeapon extends Weapon {
	function DualWeapon(yspeed) {
		super(yspeed);
	}

	function shoot(x, y, shotsList) {
		shotsList.add(new Shoot(new Vector(x + 0.2, y, 0), new Vector(0, this.speed, 0), new Mesh('Ball), 1));
		shotsList.add(new Shoot(new Vector(x - 0.2, y, 0), new Vector(0, this.speed, 0), new Mesh('Ball), 1));
	}
}

class VWeapon extends Weapon {
	function VWeapon(yspeed) {
		super(yspeed);
	}

	function shoot(x, y, shotsList) {
		shotsList.add(new Shoot(new Vector(x, y, 0), new Vector(0, this.speed, 0), new Mesh('Ball), 1));
		shotsList.add(new Shoot(new Vector(x, y, 0), new Vector(this.speed, this.speed, 0), new Mesh('Ball), 1));
		shotsList.add(new Shoot(new Vector(x, y, 0), new Vector(-this.speed, this.speed, 0), new Mesh('Ball), 1));
	}
}

function randomWeapon(yspeed) {
	var r = random(3);
	if( r == 0 )
		return new BasicWeapon(yspeed);
	else if( r == 1 )
		return new DualWeapon(yspeed);
	else if( r == 2 )
		return new VWeapon(yspeed);
}