class Shoot {
	function Shoot(pos, speed, mesh, strength) {
		this.trs = new TRS(pos);
		this.speed = speed;
		this.mesh = mesh;
		if( this.mesh == null )
			print( "Erreur shoot !!!" );
		this.mesh._pos = new TRS(pos);
		this.strength = strength;
	}

	function update() {
		if( this.trs._tx < camera._pos._x - 10 || this.trs._tx > camera._pos._x + 10
			|| this.trs._ty < -6 || this.trs._ty > 6 ) {
				this.remove();
				return false;
		}
		this.trs._t += this.speed;
		this.mesh._pos = this.trs;
		return true;
	}

	function remove() {
		this.mesh.remove();
	}
}

function shotsUpdate(shotsList) {
	shotsList.iter(fun(s){
		if( !s.update() )
			shotsList.remove(s);
			});
}