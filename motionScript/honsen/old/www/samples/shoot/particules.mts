class Particule {
	function Particule(x, y) {
		var s = (random(20) + 1 ) / 10.0
		this.trs = new TRS(new Vector(x, y, 0), null, new Vector(s, s, s));
		var x = ((random(101) + 50) / 100.0) * (random(3) - 1);
		var y = ((random(101) + 50) / 100.0) * (random(3) - 1);
		var z = ((random(101) + 50) / 100.0) * (random(3) - 1);
		this.speed = new Vector(x, y, z);

		if( this.speed._length < 0.5 ) // evite d'avoir une particule sans mouvement
			this.speed = new Vector(1, 0, 0);

		var rx = (random(101) - 50) / 100.0;
		var ry = (random(101) - 50) / 100.0;
		var rz = (random(101) - 50) / 100.0;
		this.rot = new Quaternion(rx, ry, rz);

		this.mesh = new Mesh('Particule, null, true);
		if( this.mesh == null )
			print( "Erreur particule !!!" );
		this.mesh._color = 0xffff00;
		this.mesh._pos = this.trs;
	}

	function remove() {
		if( this.mesh.remove_special(true) != true )
			print("NO");
	}

	function update() {
		this.trs._t += this.speed;
		this.trs._r *= this.rot;
		this.mesh._pos = this.trs;
	}
}

class ParticuleEngine {
	function ParticuleEngine() {
		this.particules = new List();
		this.count = 0;
	}

	function remove() {
		this.particules.iter(fun(p) { p.remove() } );
		this.particules = new List();
		this.count = 0;
	}

	function add(x, y) {
		if( this.particules.add(new Particule(x, y)) == null )
			print("raahh");
		else
			this.count++;
		if( this.particules._length > 100 ) { // permet de garder un nombre raisonnable de particules, les plus vieilles etant éliminées
			var part = this.particules.pop();
			part.remove();
			this.count--;
		}
	}

	function add_multi(x, y, nb) {
		var i = 0;
		for( i = 0; i < nb; i++ )
			this.add(x, y);
	}

	function update() {
		if( this.count != this.particules._length )
			print("oulah");

		this.particules.iter(fun(p) { p.update(); } );
	}
}