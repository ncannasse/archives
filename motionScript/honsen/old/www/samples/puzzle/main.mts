import Count;
import Game;
import Math;

function start() {
	camera._pos = (new Vector(6, 12, 6)) * pow(1.2, int(sqrt(progression)));
	var temp = int(3 + sqrt(progression));
	camera._target = new Vector(temp - 1, temp, 0);
	light._direction = new Vector(0, 1, 2);

	textGroup = new Group(0.001);
	message = new Text(textGroup);
	message._x = _width / 2 - 85;
	message._y = _height / 2 + 20;
	message._color = 0xb82edb;
	message._font = 'Message;

	ge = new GameEngine(1 + progression, temp);
	finished = false;
	setMain(update);
}

function reset() {
	textGroup.remove();
	ge.remove();
	start();
}

function end_game(wl) {
	finished = true;
	if( wl ) {
		message._text = "  WIN  ";
		progression++;
		score += ge.moves * 100;
	}
	else {
		message._text = "LOOSE";
		progression = 1;
		score = 0;
	}

	endTime = 0;
	setMain(update_end);
}

function update() {
	if( up.isPressed() )
		if( ge.up() )
			end_game(true);

	if( down.isPressed() )
		if( ge.down() )
			end_game(true);

	if( left.isPressed() )
		if( ge.left() )
			end_game(true);

	if( right.isPressed() )
		if( ge.right() )
			end_game(true);

	if( space.isPressed() )
		ge.select();

	if( ge.moves <= 0 && !finished )
		end_game(false);
	ge.update();
}

function update_end() {
	endTime++;
	if( endTime < 100 ){
		ge.update();
		scoreDraw.update(score);
	}
	else
		reset();
}

up = new Key("UP");
down = new Key("DOWN");
left = new Key("LEFT");
right = new Key("RIGHT");
space = new Key("SPACE");
space.repeat = 500;


pKey = new Key("P");
mKey = new Key("M");
dKey = new Key("D");

bgGroup = new Group(0.99);
bg = new Sprite(bgGroup);
bg._bitmap = 'Bg;
bg._x = 0;
bg._y = 0;
bg._width = _width;
bg._height = _height;
bg._tu = 0;
bg._tu2 = 1;
bg._tv = 0;
bg._tv2 = 1;

score = 0;
scoreDraw = new FlashCount("Score", _width - 8, _height, 0, "pts", 'Count);
progression = 1;

start();