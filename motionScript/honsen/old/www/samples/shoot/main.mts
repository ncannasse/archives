import Math;
import Scrolling;
import Shuttle;
import Rock;
import Shots;
import Particules;
import Indicator;
import Count;
import Flyers;

light._direction = new Vector(0, 0, -1);
light._color = 0xffffd0;
light._ambient = 0x707088;

function stop() {
	endTime = 0;
	setMain(update_end);
}

function stop_end() {
	function iter_remove_shots(s, o) {
		s.remove();
		o.remove(s);
	}
	axelayShots.iter(fun(s) { s.remove(); } );
	axelayShots = new List();
	enemiesShots.iter(fun(s) { s.remove(); } );
	enemiesShots = new List();

	pe.remove();
	se.remove();
	re.remove();
	ee.remove();
	fe.remove();
	li.remove();
	sc.remove();

	reset();
}

function reset() {
	camera._pos = new Vector(0, 0.00001, -13);
	camera._target = new Vector(0, 0, 0);

	axelayShots = new List();
	enemiesShots = new List();

	pe = new ParticuleEngine();
	re = new RockEngine(pe);
	ee = new EnemyEngine(pe, enemiesShots);
	se = new ScrollEngine();
	//speed, depth, alpha, image
	se.add(new Scrolling(0.01, 0.999, 1, 'Scroll));
	se.add(new Scrolling(0.2, 0.998, 0.999, 'Scroll2));
	se.add(new Scrolling(0.4, 0.997, 0.999, 'Scroll3));

	fe = new FlyersEngine();

	level = 1;

	group = new Group(0.0001);//splash screen
	splash = new Sprite(group);
	splash._bitmap = 'Splash;
	splash._width = _width;
	splash._height = _height;
	splash._tu = 0;
	splash._tu2 = 1;
	splash._tv = 0;
	splash._tv2 = 1;

	spacep = false;
	beginTime = 0;
	setMain(update_begin);
}

function update_begin() {
	if( spacep ) {
		beginTime++;
		splash._tv += 0.01;
		splash._tv2 -= 0.01;
		splash._alpha -= 0.025;
	}
	if( space.isPressed() && !spacep ) {
		//position(x,y,z)
		axelay = new Axelay(0, -3, 0);
		li = new LifeIndicator(370, 240, axelay.life, 5);
		sc = new FlashCount("Score", 392, 30, 0, "pts", 'Score);
		spacep = true;
		beginTime = 0;
	}
	if( beginTime > 40 ) {
		group.remove();
		setMain(update);
	}
	se.update();
}

function update_end() {
	endTime++;
	if( endTime > 40 ) // temps pendant lequel le jeu continu à tourner apres la mort
		if( respawn ) {
			axelay.respawn(camera._pos._x, -3, 0);
			setMain(update);
		}
		else {
			axelay.remove();
			stop_end();
		}
	li.update(axelay.life);
	pe.update();
	re.update(axelay, axelayShots, fe, false);
	ee.update(axelay, axelayShots, fe, false);
	se.update();
	shotsUpdate(axelayShots);
	shotsUpdate(enemiesShots);
}

function update() {
	if( up.isDown() ) {
		if( se.getSpeed() < 5 ) {
			se.changeSpeed(0.07);
			re.speed_up();
			ee.speed_up();
		}
		axelay.up();
	}
	if( up.isReleased() ) {
		axelay.up_released();
	}


	if( down.isDown() ) {
		if( se.getSpeed() > 0.5 ) {
			se.changeSpeed(-0.05);
			re.speed_down();
			ee.speed_down();
		}
		axelay.down();
	}
	if( down.isReleased() ) {
		axelay.down_released();
	}


	if( left.isDown() ) {
		if( axelay.cap >= 0 ) {
			se.left();
		}
		axelay.left();
	}
	if( left.isReleased() ) {
		axelay.left_released();
	}


	if( right.isDown() ) {
		if( axelay.cap <= 0 ) {
			se.right();
		}
		axelay.right();
	}
	if( right.isReleased() ) {
		axelay.right_released();
	}

	if( (pKey.isPressed() || (axelay.score > level * level * 1000)) && level < 9 ) {
		level++;
		re.level_up();
		ee.level_up();
	}
	if( mKey.isPressed() && level > 1 ) {
		level--;
		re.level_down();
		ee.level_down();
	}

	if( space.isPressed() ) {
		axelay.shoot(axelayShots);
	}

	if( !axelay.update(enemiesShots, pe) ) {
		respawn = (axelay.saves > 0);
		stop();
	}
	li.update(axelay.life);
	pe.update();
	re.update(axelay, axelayShots, fe, true);
	ee.update(axelay, axelayShots, fe, true);
	se.update();
	sc.update(axelay.score);
	fe.update();
	shotsUpdate(axelayShots);
	shotsUpdate(enemiesShots);

	var totalCount = ee.count + re.count + pe.count + axelayShots._length + enemiesShots._length + 1;
	if( totalCount != gc_mesh_count() || pe.count != mesh_count() || dKey.isPressed() ) {
		print("game particules count" + pe.count);
		print("engine particules count" + mesh_count());
		print("game total count" + totalCount);
		print("engine total count" + gc_mesh_count());
		print(re.count);
	}
}

up = new Key("UP");
up._repeat = -1;
down = new Key("DOWN");
down._repeat = -1;
left = new Key("LEFT");
left._repeat = -1;
right = new Key("RIGHT");
right._repeat = -1;
space = new Key("SPACE");
space.repeat = 500;


pKey = new Key("P");
mKey = new Key("M");
dKey = new Key("D");

reset();