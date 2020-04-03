import Perso;
import Area;
import Math;
import Fruit;
import Level;
import Levels;
import Altimetre;
import Count;

function menu() {
	reset(0);
	mode = 0;
	count = 0;
	currentLevel = 0; //et pb de changement de vue(roll)

	camera._pos = new Vector(0, -20, 11);
	camera._target = new Vector(0, 0, 10);
	viewPositionChanged = 1.0;
	viewTargetChanged = 1.0;
	view = 2;
}

function pause_game() {
	mode = 3;
	alti.hide();
	speedCpt.hide();
}

function start_game() {
	mode = 1;
	lt = 100;
	rt = 100;
	alti = new Altimeter("alti1", 10, 200, perso.trs._tz, 100);
	speedCpt = new Count("speedCpt", 10, _height - 50, int(perso.speed * 100), "Kmh", 'Speed);
}

function reset(level) {
	perso.remove();
	if(level >= levels._length)
		level = 1;
	currentLevel = level;
	perso = levels[currentLevel].load();
	viewPositionChanged = 1.0;
	viewTargetChanged = 1.0;
	camera._pos = new Vector(perso.trs._tx + perso.direction._x * 10,
								perso.trs._ty + perso.direction._y * 10,
								5);
	camera._target = perso.trs._t;
	count = 0;
	mode = 2;
}

//**************************************************************************************
// Mouvements caméra
//**************************************************************************************

function replace_cam(pos, target) {
	if((perso.mode != 3 && perso.mode != 4))
		camera._pos = (camera._pos * viewPositionChanged)
				+ (perso.trs._t + pos) * (1 - viewPositionChanged);

	camera._target = (camera._target * viewTargetChanged)
			+ (perso.trs._t + target) * (1 - viewTargetChanged);

	if(view == 2) {
		camera._roll = perso.turnLevel;
	}

	if(viewPositionChanged > 0)
			viewPositionChanged -= 0.01;
	else
		viewPositionChanged = 0;
	if(viewTargetChanged > 0)
			viewTargetChanged -= 0.01;
	else
		viewTargetChanged = 0;
}

function replace_cam() {
	if(view == 0)
		replace_cam(new Vector(perso.direction._x * -20,
								perso.direction._y * -20,
								20),
					new Vector(0,0,0));
	else if(view == 1)
		replace_cam(new Vector(perso.direction._x * -30,
								perso.direction._y * -30,
								0),
					new Vector(0,0,0));
	else
		replace_cam(perso.persoUp * 3, perso.direction * 10);
}

function rotate_cam(rotX, rotY, rotZ, center) {
	manual = true;
	camera._pos -= center;
	var q = new Quaternion(rotX ,rotY ,rotZ);
	camera._pos = camera._pos * q.toMatrix();
	camera._pos += center;
	viewPositionChanged = 1.0;
}

//**************************************************************************************
// Boucle principale
//**************************************************************************************

function update_land() {
	land.time.x= time();
	land.time.y= cos(land.time.x) / 80.0;
	land.time.z= sin(land.time.x / 2.0) / 80.0;
}

function update_menu() {

	if(sKey.isPressed()) {
		reset(1);
		view = 0;
	}
	if(lKey.isPressed()) {
		currentLevel++;
		if(currentLevel >= levels._length)
			currentLevel = 0;
		reset(currentLevel);
	}
	if(up.isDown()) {
		camera._zoom *= 1.1;
	}
	if(down.isDown()) {
		camera._zoom /= 1.1;
	}
	if(left.isDown()) {
		rotate_cam(0,0,0.1,perso.trs._t);
	}
	if(right.isDown()) {
		rotate_cam(0,0,-0.1,perso.trs._t);
	}
	if(aKey.isDown()) {
		perso.mesh.rot(0,0,0.1);
	}

	replace_cam();
}

function update_game() {
	lt++;
	rt++;
	if(up.isPressed())
		if(down.isDown())
			perso.stop_up();
		else
			perso.start_down();
	if(up.isReleased()){
		perso.stop_down();
		if(down.isDown())
			perso.start_up();
	}
	if(up.isDown() && !down.isDown())
		perso.down(true);

	if(down.isPressed())
		if(up.isDown())
			perso.stop_down();
		else
			perso.start_up();
	if(down.isReleased()){
		perso.stop_up();
		if(up.isDown())
			perso.start_down();
	}
	if(down.isDown() && !up.isDown())
		perso.up(true);

	if(left.isDown()) {
		if(perso.left(true) && view != 2){
			if(left.time() < 1000 && rt > 80 && lt > 80)
				viewPositionChanged = 1.0;
		}

	}
	if(left.isReleased())
		lt = 0;

	if(right.isDown()) {
		if(perso.right(true) && view != 2){
			if(right.time() < 1000 && lt > 80 && rt > 80)
				viewPositionChanged = 1.0;
		}

	}
	if(right.isReleased())
		rt = 0;

	if(pKey.isPressed())
		pause_game();
	if(cKey.isPressed()) {
		camera._roll = 0;
		viewPositionChanged = 1.0;
		viewTargetChanged = 1.0;
		view++;
		if(view > 2)
			view = 0;
	}

	perso.update();
	alti.update(perso.trs._tz);
	speedCpt.update(int(perso.speed * 100));

	//update_land();

	replace_cam();
}

function update_intro() {
	count++;

	if(space.isPressed()) {
		viewPositionChanged = 1.0;
		viewTargetChanged = 1.0;
		start_game();
		return;
	}
	if(count < 40) {
		replace_cam(new Vector(perso.direction._x * 10, perso.direction._y * 10, 2),
					new Vector(0, 0, 0));
		return;
	}
	if(count < 160) {
		rotate_cam(0, 0, pi/40.0, perso.trs._t);
		return;
	}
	if(count >= 160){
		count = 0;
		viewPositionChanged = 1.0;
		viewTargetChanged = 1.0;
		start_game();
		return;
	}
}

function update_pause() {
	if(pKey.isPressed())
		start_game();
	if(rKey.isPressed())
		reset(currentLevel);
	if(mKey.isPressed())
		menu();
	if(nKey.isPressed())
		reset(currentLevel + 1);
	if(up.isDown()) {
		camera._zoom *= 1.1;
	}
	if(down.isDown()) {
		camera._zoom /= 1.1;
	}
	if(left.isDown()) {
		rotate_cam(0,0,0.1,perso.trs._t);
	}
	if(right.isDown()) {
		rotate_cam(0,0,-0.1,perso.trs._t);
	}
	if(aKey.isDown()) {
		perso.mesh.rot(0,0,0.1);
	}
}

function update() {
	update_land(); //pour faire bouger l'eau tout le temps...meme en pause
	//display(mode);
	display("current level: ", currentLevel);

	if(mode == 0)
		update_menu();
	else if(mode == 1)
		update_game();
	else if(mode == 2)
		update_intro();
	else
		update_pause();
}


land = new Mesh('Surf2);
surf_trs = new TRS(new Vector(0, 0, -1), null, new Vector(1,1,1));
land._color = 0xffffff;
land._pos = surf_trs;
land._alpha = 0.95;

land.c0 = new Object();
land.c0.x = pi;
land.c0.y = 1.0/2.0;
land.c0.z = 2.0*pi;
land.c0.w = 1.0/(2.0*pi);

land.c1 = new Object();
land.c1.x = 1.0;
land.c1.y = -1.0/2.0;
land.c1.z = 1.0/4.0;
land.c1.w = -1.0/720.0;

land.c2 = new Object();
land.c2.x = 1.0;
land.c2.y = -1.0/6.0;
land.c2.z = 1.0/120.0;
land.c2.w = -1.0/5040.0;

land.c3 = new Object();
land.c3.x = 2.0;
land.c3.y = 1.0/3.5;
land.c3.z = 1.0/4.0;
land.c3.w = 1.0/5.0;

land.time = new Object();
land.time.x = 0;
land.time.y = 0;
land.time.z = 0;
land.time.w = 3.0;

if(land.has_tag("TEX0"))
	land._shader = "m4x4 r0 , $VECT, #pos \n
	mul r1.x, r0.x, #c3.y\n
	add r1.x, r1.x, #time.x\n
	mad r1.x, r1.x, #c0.w, #c0.y\n
	expp r1.y, r1.x\n
	mad r1.x, r1.y, #c0.z, -#c0.x\n
	dst r2.xy, r1.x, r1.x\n
	mul r2.z, r2.y, r2.y\n
	mul r2.w, r2.y, r2.z\n
	mul r1, r2, r1.x\n
	dp4 r1.y, r1, #c2\n
	add r0.z, r0.z, r1.y\n
	mul r1.x, r0.y, #c3.z\n
	add r1.x, r1.x, #time.x\n
	mad r1.x, r1.x, #c0.w, #c0.y\n
	expp r1.y, r1.x\n
	mad r1.x, r1.y, #c0.z, -#c0.x\n
	dst r2.xy, r1.x, r1.x\n
	mul r2.z, r2.y, r2.y\n
	mul r2.w, r2.y, r2.z\n
	mul r1, r2, r1.x\n
	dp4 r1.y, r1, #c2\n
	add r1.y, r0.z, r1.y\n
	mul r0.z, r1.y, #c3.x\n
	m4x4 oPos, r0, #camera \n
	m3x3 r0 , $NORM, #pos \n
	add r1.y, r1.y, #time.w\n
	mul r1.y, r1.y, #c1.z \n
	add r0.z, r0.z, -r1.y\n
	dp3 r0, r0, #light\n
	max r0, r0, #zero \n
	mul r0, r0, #light_color \n
	add r0, r0, #ambient_color \n
	min r0, r0, #one \n
	mul oD0, r0, #color \n
	add oT0, $MAP0.xy, #time.yz \n";
else
	land._shader = "m4x4 r0 , $VECT, #pos \n
	mul r1.x, r0.x, #c3.y\n
	add r1.x, r1.x, #time.x\n
	mad r1.x, r1.x, #c0.w, #c0.y\n
	expp r1.y, r1.x\n
	mad r1.x, r1.y, #c0.z, -#c0.x\n
	dst r2.xy, r1.x, r1.x\n
	mul r2.z, r2.y, r2.y\n
	mul r2.w, r2.y, r2.z\n
	mul r1, r2, r1.x\n
	dp4 r1.y, r1, #c2\n
	add r0.z, r0.z, r1.y\n
	mul r1.x, r0.y, #c3.z\n
	add r1.x, r1.x, #time.x\n
	mad r1.x, r1.x, #c0.w, #c0.y\n
	expp r1.y, r1.x\n
	mad r1.x, r1.y, #c0.z, -#c0.x\n
	dst r2.xy, r1.x, r1.x\n
	mul r2.z, r2.y, r2.y\n
	mul r2.w, r2.y, r2.z\n
	mul r1, r2, r1.x\n
	dp4 r1.y, r1, #c2\n
	add r1.y, r0.z, r1.y\n
	mul r0.z, r1.y, #c3.x\n
	m4x4 oPos, r0, #camera \n
	m3x3 r0 , $NORM, #pos \n
	add r1.y, r1.y, #time.w\n
	rsq r1.y, r1.y\n
	mul r0.z, r0.z, r1.y\n
	dp3 r0, r0, #light\n
	max r0, r0, #zero \n
	mul r0, r0, #light_color \n
	add r0, r0, #ambient_color \n
	min r0, r0, #one \n
	mul oD0, r0, #color \n";

levels = generate_levels();

up = new Key("UP");
down = new Key("DOWN");
left = new Key("LEFT");
right = new Key("RIGHT");
up._repeat = -1;
down._repeat = -1;
left._repeat = -1;
right._repeat = -1;

space = new Key("SPACE");
rKey = new Key("R");
sKey = new Key("S");
mKey = new Key("M");
nKey = new Key("N");
aKey = new Key("A");
cKey = new Key("C");
lKey = new Key("L");
pKey = new Key("P");

skybox = new Mesh('SkyBox);
skybox.move(0, 0, -20);
skybox.scale(3);


//mode: 	0 - menu
//			1 - game
//			2 - video
//			3 - pause


menu();
setMain(update);
