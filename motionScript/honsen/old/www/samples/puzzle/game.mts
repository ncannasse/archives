import Cube;
import Math;

class GameEngine {
	function GameEngine(lvl, size) {
		this.width = size;
		this.height = size;
		this.levelTable = new Array(this.width, this.height);
		this.dalleTable = new Array(this.width, this.height);

		this.sd = false;
		this.cx = size/2;
		this.cy = size/2;
		this.dices = lvl;
		this.moves = 10 * (this.dices - 1);

		this.staticTop = 0;

		generate_level_shuffle(this.dices);

		while( this.check_win() ) {
			var i = 0;
			var j = 0;
			this.staticTop = 0;
			for( i = 0; i < this.width; i++ )
				for( j = 0; j < this.height; j++ ){
					this.levelTable[i][j].remove();
					this.dalleTable[i][j].remove();
				}
			generate_level_shuffle(this.dices);
		}

		this.movesGroup = new Group(0.001);
		this.movesDraw = new Text(this.movesGroup);
		this.movesDraw._text = "X " + string(this.moves);
		this.movesDraw._x = _width / 2;
		this.movesDraw._y = 30;
		this.movesDraw._color = 0xeeeeee;
		this.movesDraw._font = 'Count;

		this.selectedDice = null;
	}

	function remove() {
		this.movesGroup.remove();
		var i = 0;
		var j = 0;
		for( i = 0; i < this.width; i++ )
			for( j = 0; j < this.height; j++ ){
				this.levelTable[i][j].remove();
				this.dalleTable[i][j].remove();
			}
	}

	function generate_level(dices) {
		var i = 0;
		var j = 0;
		for( i = 0; i < this.width; i++ )
			for( j = 0; j < this.height; j++ ) {
				this.dalleTable[i][j] = new Mesh('Dalle);
				this.dalleTable[i][j]._pos = new TRS(new Vector(i * 2, j * 2, -0.1));
			}

		var gen = dices;
		while( gen != 0 ) {
			var x = random(this.width);
			var y = random(this.height);
			var mv = true;
			if( random(100/this.dices) == 0 )
				mv = false;
			if( this.levelTable[x][y] == null ) {
				if( mv )
					this.levelTable[x][y] = new Cube(x, y, mv, 0);
				else {
					if( this.staticTop == 0 ) {
						this.levelTable[x][y] = new Cube(x, y, mv, 0);
						this.staticTop = this.levelTable[x][y].top;
					}
					else
						this.levelTable[x][y] = new Cube(x, y, mv, this.staticTop);
				}
				this.selectedDice = this.levelTable[x][y];
				gen--;
			}
		}
	}

	function generate_level_shuffle(dices) {
		var i = 0;
		var j = 0;
		for( i = 0; i < this.width; i++ )
			for( j = 0; j < this.height; j++ ) {
				this.dalleTable[i][j] = new Mesh('Dalle);
				this.dalleTable[i][j]._pos = new TRS(new Vector(i * 2, j * 2, -0.1));
			}


		var shufflesCount = 2 * this.dices + random(5 * this.dices);
		var top = random(6) + 1;
		var gen = dices;
		var x = 1;
		var y = 1;
		var tempList = new List();
		var fCount = 0;
		while( gen != 0 ) {
			var mv = true;
			if( random(100/this.dices) == 0 && fCount < dices - 1 ) {
				mv = false;
				fCount++;
			}
			this.levelTable[x][y] = new Cube(x, y, mv, top);
			if( mv )
				tempList.add(this.levelTable[x][y]);
			gen--;
			x++;
			if( x >= this.width - 1 ) {
				x = 1;
				y++;
			}
		}

		var tempArray = tempList.toArray();
		this.moves = shufflesCount;

		while( shufflesCount != 0 ) {
			var n = random(tempArray._length);
			var dir = random(4);
			var mx = 1;
			var my = 0;
			if( dir == 0 ) {
				mx = -1;
			}
			else if( dir == 1 ) {
				mx = 0;
				my = 1;
			}
			else if( dir == 2 ) {
				mx = 0;
				my = -1;
			}
			if( this.move_special(tempArray[n].x, tempArray[n].y, mx, my) ) {
				tempArray[n].update();
				shufflesCount--;
			}
		}

		this.selectedDice = tempArray[0];
	}

	function check_neightboors(dice, top, toScan) {
		var i = 1;
		var j = 1;
		var x = dice.x;
		var y = dice.y;

		for( i = -1; i <= 1; i++ )
			for( j = -1; j <= 1; j++ ){
				if( abs(i) != abs(j)
					&& x + j >= 0 && x + j < this.width
					&& y + i >= 0 && y + i < this.height
					&& this.levelTable[x+j][y+i].top == top
					&& !this.levelTable[x+j][y+i].used ) {
						this.levelTable[x+j][y+i].used = true;
						toScan.add(this.levelTable[x+j][y+i]);
					}
				}
	}

	function check_win() {
		var i = 0;
		var j = 0;
		for( i = 0; i < this.height; i++ )
			for( j = 0; j < this.width; j++ )
				this.levelTable[j][i].used = false;

		var toScan = new List();
		this.selectedDice.used = true;
		toScan.add(this.selectedDice);
		var scanned = 0;
		while( toScan._length != 0 ) {
			var dice = toScan.pop();
			this.check_neightboors(dice, this.selectedDice.top, toScan);
			scanned++;
		}
		return scanned == this.dices;
	}

	function move(mx, my) {
		this.levelTable[this.cx][this.cy].normal_color();
		this.dalleTable[this.cx][this.cy].set_color(0xffffff);

		var nx = this.cx + mx;
		var ny = this.cy + my;
		if( nx < 0 || nx >= this.width
			|| ny < 0 || ny >= this.height )
			return false;

		if( this.sd ) {
			if( this.levelTable[nx][ny] == null && this.selectedDice.move(mx, my, true) ) {
				this.levelTable[this.cx][this.cy] = null;
				this.levelTable[nx][ny] = this.selectedDice;
				this.cx = nx;
				this.cy = ny;
				this.moves--;
				this.movesDraw._text = "X " + string(this.moves);
				return this.check_win();
			}
		}
		else {
			this.cx = nx;
			this.cy = ny;
		}
		return false;
	}

	function move_special(vx, vy, mx, my) {
		var nx = vx + mx;
		var ny = vy + my;
		if( nx < 0 || nx >= this.width
			|| ny < 0 || ny >= this.height )
			return false;

		if( this.levelTable[nx][ny] == null && this.levelTable[vx][vy].move(mx, my, false) ) {
			this.levelTable[nx][ny] = this.levelTable[vx][vy];
			this.levelTable[vx][vy] = null;
			return true;
		}
		return false;
	}

	function up() {
		return this.move(0, -1);
	}

	function down() {
		return this.move(0, 1);
	}

	function right() {
		return this.move(1, 0);
	}

	function left() {
		return this.move(-1, 0);
	}

	function select() {
		if( this.sd ) {
			if( this.selectedDice.moveCoef == 0 ) {
				this.sd = false;
				this.selectedDice = null;
			}
		}
		else {
			if( this.levelTable[this.cx][this.cy] == null
				|| !this.levelTable[this.cx][this.cy].movable )
				return;
			this.sd = true;
			this.selectedDice = this.levelTable[this.cx][this.cy];
			this.dalleTable[this.cx][this.cy].set_color(0xff0000);
		}
	}

	function update() {
		this.selectedDice.update();
		if( this.sd )
			this.levelTable[this.cx][this.cy].select_color();
		else {
			this.levelTable[this.cx][this.cy].on_color();
			if( this.levelTable[this.cx][this.cy].movable )
				this.dalleTable[this.cx][this.cy].set_color(0x00ff00);
			else
				this.dalleTable[this.cx][this.cy].set_color(0xff00ff);
		}
	}

	function toString() {
		var i = 0;
		for( i = 0; i < this.width; i++ )
			print(this.levelTable[i]);
	}
}