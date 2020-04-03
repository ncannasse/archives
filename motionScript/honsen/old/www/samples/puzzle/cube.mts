import Math;

pi2 = pi / 2.0;

class Cube {
	function Cube(x, y, mv, top) {
		this.x = x;
		this.y = y;
		this.top = 1;
		this.front = 2;
		this.side = 4;
		this.mesh = new Mesh('Dice);
		this.trs = new TRS(new Vector(x * 2, y * 2, 1));
		if( top == 0 ) {
			this.hroll(random(4));
			this.vroll(random(4));
			this.sroll(random(4));
		}
		else if( top == 1 ) {
			this.sroll(random(4));;
		}
		else if( top == 2 ) {
			this.vroll(3);
			this.sroll(random(4));;
		}
		else if( top == 3 ) {
			this.hroll(1);
			this.sroll(random(4));;
		}
		else if( top == 4 ) {
			this.hroll(3);
			this.sroll(random(4));;
		}
		else if( top == 5 ) {
			this.vroll(1);
			this.sroll(random(4));;
		}
		else if( top == 6 ) {
			this.vroll(2);
			this.sroll(random(4));;
		}
		this.mesh._pos = this.trs;

		this.mr = null;
		this.lmr = null;
		this.llmr = null;
		this.moveCoef = 0;
		this.moveVect = null;

		this.used = false;
		this.movable = mv;
		this.normal_color();
	}

	function remove() {
		this.mesh.remove();
	}

	function update() {
		if( this.moveCoef > 0 ) {
			this.llmr = new Vector(this.lmr._x, this.lmr._y, 0);
			this.lmr = vlerp(this.mr, this.lmr, this.moveCoef);
			this.moveCoef -= 0.025;
			if( this.moveCoef < 0.8 )
				this.moveCoef -= 0.05;
			this.mesh.move(-(this.trs._tx + this.moveVect._x), -(this.trs._ty + this.moveVect._y), 0);
			this.mesh.rot(this.lmr._x - this.llmr._x, this.lmr._y - this.llmr._y, 0);
			this.mesh.move(this.trs._tx + this.moveVect._x, this.trs._ty + this.moveVect._y, 0);
			if( this.moveCoef <= 0.4 ) {
				this.moveCoef = 0;
				this.mesh.move(-(this.trs._tx + this.moveVect._x), -(this.trs._ty + this.moveVect._y), 0);
				this.mesh.rot(this.mr._x - this.lmr._x, this.mr._y - this.lmr._y, 0);
				this.mesh.move(this.trs._tx + this.moveVect._x, this.trs._ty + this.moveVect._y, 0);
				this.trs._t += this.moveVect * 2;
			}
		}
		else
			this.moveCoef = 0;
	}

	function move(mx, my, wait) {
		if( !this.movable )
			return false;
		if( my == 1 )
			return this.down(wait);
		if( my == -1 )
			return this.up(wait);
		if( mx == 1 )
			return this.right(wait);
		if( mx == -1 )
			return this.left(wait);
		return false;
	}

	function up(wait) {
		if( this.moveCoef == 0 || !wait){
			this.y--;
			this.mr = new Vector(pi2, 0, 0);
			this.lmr = new Vector(0,0,0);
			this.moveVect = new Vector(0,-1,0);
			if( wait )
				this.moveCoef = 1;
			else
				this.moveCoef = 0.1;
			var temp = this.front;
			this.front = 7 - this.top;
			this.top = temp;
			return true;
		}
		return false;
	}

	function down(wait) {
		if( this.moveCoef == 0 ){
			this.y++;
			this.mr = new Vector(-pi2, 0, 0);
			this.lmr = new Vector(0,0,0);
			if( wait )
				this.moveCoef = 1;
			else
				this.moveCoef = 0.1;
			this.moveVect = new Vector(0,1,0);
			var temp = this.top;
			this.top = 7 - this.front;
			this.front = temp;
			return true;
		}
		return false;
	}

	function left(wait) {
		if( this.moveCoef == 0 ){
			this.x--;
			this.mr = new Vector(0, -pi2, 0);
			this.lmr = new Vector(0,0,0);
			if( wait )
				this.moveCoef = 1;
			else
				this.moveCoef = 0.1;
			this.moveVect = new Vector(-1,0,0);
			var temp = this.side;
			this.side = 7 - this.top;
			this.top = temp;
			return true;
		}
		return false;
	}

	function right(wait) {
		if( this.moveCoef == 0 ){
			this.x++;
			this.mr = new Vector(0, pi2, 0);
			this.lmr = new Vector(0,0,0);
			if( wait )
				this.moveCoef = 1;
			else
				this.moveCoef = 0.1;
			this.moveVect = new Vector(1,0,0);
			var temp = this.top;
			this.top = 7 - this.side;
			this.side = temp;
			return true;
		}
		return false;
	}

	function hroll(n) {
		while( n != 0 ) {
			this.trs._r *= new Quaternion(0,pi2,0);
			var temp = this.top;
			this.top = 7 - this.side;
			this.side = temp;
			n--;
		}
	}

	function vroll(n) {
		while( n != 0 ) {
			this.trs._r *= new Quaternion(pi2,0,0);
			var temp = this.front;
			this.front = 7 - this.top;
			this.top = temp;
			n--;
		}
	}

	function sroll(n) {
		while( n != 0 ) {
			this.trs._r *= new Quaternion(0,0,-pi2);
			var temp = this.front;
			this.front = 7 - this.side;
			this.side = temp;
			n--;
		}
	}

	function set_color(c) {
		this.mesh._color = c;
	}

	function select_color() {
		if( this.movable )
			this.set_color(0xff0000);
		else
			this.set_color(0xff00ff);
	}

	function on_color() {
			if( this.movable )
				this.set_color(0x00ff00);
			else
				this.set_color(0xff00ff);
	}

	function normal_color() {
			if( this.movable )
				this.set_color(0xffffff);
			else
				this.set_color(0xffff00);
	}

	function toString() {
		return "Dé (" + string(this.x) + ", " + string(this.y) + ") top " + string(this.top) ;
	}
}