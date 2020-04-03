import Level;

function generate_levels() {
	var levels = new List();
	levels.add(generate_level_0());
	levels.add(generate_level_1());
	levels.add(generate_level_2());
	levels.add(generate_level_3());
	return levels.toArray();
}

function generate_level_0() {
	var perso_data = new Perso_data("pouet", 0, 0, 10, 0, -1, 0, 0.4, 0.002, 300);
	var areas_data_list = new List();
	//name, type, points, decradius, bX, bY, tX, tY, size, slide
	areas_data_list.add(new Areas_data("Cliff", "cliff", 0, 0, -10, -10, 10, 10, 10, 10, 10, 0));

	var fruits_data_list = new List();

	return(new Level(perso_data, areas_data_list, fruits_data_list));
}

function generate_level_1() {
	var perso_data = new Perso_data("pouet", 0, 95, 50, 0, -1, 0, 0.4, 0.002, 300);
	var areas_data_list = new List();
	//name, points, decradius, bX, bY, tX, tY, height, slide
	areas_data_list.add(new Areas_data("Icy", "100 points", 100, 2, -10, -10, 10, 10, 10, 10, 20, 0.97));
	areas_data_list.add(new Areas_data("Normal", "200 points", 200, 2, -30, 45, -20, 55, 5, 5, 5, 0.9));
	areas_data_list.add(new Areas_data("Glue", "500 points", 500, 2, 23, -52, 27, -48, 2, 2, 2, 0.8));
	areas_data_list.add(new Areas_data("Cliff", "cliff", 0, 0, -500, 90, 500, 500, 500, 205, 50, 0));


	var fruits_data_list = new List();
	fruits_data_list.add(new Fruits_data("Banana", 0, 0, 30, 1));
	fruits_data_list.add(new Fruits_data("Apple", 0, -20, 15, 1));
	fruits_data_list.add(new Fruits_data("Banana", -100, 0, 10, 1));
	fruits_data_list.add(new Fruits_data("Banana", -99, -1, 12, 1));
	fruits_data_list.add(new Fruits_data("Banana", -98, -2, 14, 1));
	fruits_data_list.add(new Fruits_data("Banana", -97, -3, 16, 1));
	fruits_data_list.add(new Fruits_data("Banana", -95, -3, 18, 1));
	fruits_data_list.add(new Fruits_data("Banana", -94, -2, 20, 1));
	fruits_data_list.add(new Fruits_data("Banana", -93, -1, 22, 1));
	fruits_data_list.add(new Fruits_data("Banana", -92, 0, 24, 1));
	fruits_data_list.add(new Fruits_data("Banana", -93, 1, 26, 1));
	fruits_data_list.add(new Fruits_data("Banana", -94, 2, 28, 1));
	fruits_data_list.add(new Fruits_data("Banana", -95, 3, 30, 1));
	fruits_data_list.add(new Fruits_data("Banana", -97, 3, 32, 1));
	fruits_data_list.add(new Fruits_data("Banana", -98, 2, 34, 1));

	fruits_data_list.add(new Fruits_data("Orange", 0, 84, 47, 1));
	fruits_data_list.add(new Fruits_data("Orange", 0, 64, 45, 1));
	fruits_data_list.add(new Fruits_data("Orange", 20, 30, 40, 1));
	fruits_data_list.add(new Fruits_data("Orange", 40, -10, 36, 1));
	fruits_data_list.add(new Fruits_data("Orange", 60, -30, 32, 1));
	fruits_data_list.add(new Fruits_data("Orange", 70, -50, 28, 1));
	fruits_data_list.add(new Fruits_data("Orange", 72, -70, 26, 1));
	fruits_data_list.add(new Fruits_data("Orange", 70, -90, 22, 1));

	return(new Level(perso_data, areas_data_list, fruits_data_list));
}

function generate_level_2() {
	var perso_data = new Perso_data("pouet", 0, -95, 50, 0, 1, 0, 0.4, 0.002, 300);
	var areas_data_list = new List();
	//name, points, decradius, bX, bY, tX, tY, height, slide
	areas_data_list.add(new Areas_data("Icy", "100 points", 100, 2, -10, -10, 10, 10, 10, 10, 10, 0.97));
	areas_data_list.add(new Areas_data("Normal", "200 points", 200, 2, -30, 45, -20, 55, 5, 5, 5, 0.9));
	areas_data_list.add(new Areas_data("Glue", "500 points", 200, 2, 23, -52, 27, -48, 2, 2, 2, 0.8));
	areas_data_list.add(new Areas_data("Cliff", "cliff", 0, 0, -500, -500, 500, -90, 500, 205, 50, 0));


	var fruits_data_list = new List();
	fruits_data_list.add(new Fruits_data("Banana", 0, 0, 30, 1));
	fruits_data_list.add(new Fruits_data("Apple", 0, -20, 15, 1));

	return(new Level(perso_data, areas_data_list, fruits_data_list));
}

function generate_level_3() {
	var perso_data = new Perso_data("pouet", -95, 0, 50, 1, 0, 0, 0.4, 0.002, 300);
	var areas_data_list = new List();
	//name, points, decradius, bX, bY, tX, tY, height, slide
	areas_data_list.add(new Areas_data("Icy", "100 points", 100, 2, -10, -10, 10, 10, 10, 10, 10, 0.97));
	areas_data_list.add(new Areas_data("Normal", "200 points", 200, 2, -30, 45, -20, 55, 5, 5, 5, 0.9));
	areas_data_list.add(new Areas_data("Glue", "500 points", 200, 2, 23, -52, 27, -48, 2, 2, 2, 0.8));
	areas_data_list.add(new Areas_data("Cliff", "cliff", 0, 0, -500, -500, -90, 500, 205, 500, 50, 0));


	var fruits_data_list = new List();
	fruits_data_list.add(new Fruits_data("Banana", 0, 0, 30, 1));
	fruits_data_list.add(new Fruits_data("Apple", 0, -20, 15, 1));

	return(new Level(perso_data, areas_data_list, fruits_data_list));
}