#include <iostream>
#include <cstdlib>
#include <random>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Utilities/Graph.h"
using namespace std;

const int BEGIN_ANT = 20;

void update_gui(int width, Graph *g) {
	const ALLEGRO_COLOR food_color = al_map_rgb(0, 155, 0);
	const ALLEGRO_COLOR ant_color = al_map_rgb(0, 0, 0);
	const ALLEGRO_COLOR cell_without_pheromone = al_map_rgb(102, 255, 255);
	const ALLEGRO_COLOR source_color = al_map_rgb(51, 51, 204);
	const ALLEGRO_COLOR ant_with_food = al_map_rgb(255, 200, 15);
	int maxValue = g->getDimMax();
	int limit_draw = maxValue * 0.2;
	for (int a = 0; a < g->getDim(); a++)
		for (int b = 0; b < g->getDim(); b++) {
			if (g->getCells(a, b).pheromone > limit_draw) {
				int vc = (g->getCells(a, b).pheromone * 255) / maxValue;
				al_draw_filled_rectangle((b * width) + 10, (a * width) + 10,
						(b * width) + width + 10, (a * width) + width + 10,
						al_map_rgb(vc, 0, 0));
			} else
				al_draw_filled_rectangle((b * width) + 10, (a * width) + 10,
						(b * width) + width + 10, (a * width) + width + 10,
						cell_without_pheromone);
			if (g->getCells(a, b).food)
				al_draw_filled_circle((b * width) + width / 2 + 10,
						(a * width) + width / 2 + 10, width / 2, food_color);
			else if (g->getCells(a, b).source)
				al_draw_filled_circle((b * width) + width / 2 + 10,
						(a * width) + 10 + width / 2, width / 2, source_color);
		}
	for (int f = 0; f < g->getAnts().size(); f++)
		if (g->getAnts()[f].food)
			al_draw_filled_circle((g->getAnts()[f].y * width) + width / 2 + 10,
					(g->getAnts()[f].x * width) + width / 2 + 10, width / 2,
					ant_with_food);
		else
			al_draw_filled_circle((g->getAnts()[f].y * width) + 10 + width / 2,
					(g->getAnts()[f].x * width) + 10 + width / 2, width / 2,
					ant_color);
	al_flip_display();
}

int main(int argc, char **argv) {
	int dimension = 100;
	int width = 10;
	Graph g(dimension);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0,
			g.getDimMax() - 1);
	srand(time(0));
	int randomFood1 = dist(rng);
	int randomFood2 = dist(rng);
	int randomSource1 = randomFood1;
	while (abs(randomFood1 - randomSource1) < g.getDimMax() / 3)
		randomSource1 = dist(rng);
	g.intitializate_matrix();
	randomFood1 = 9756;
//	randomFood2 = 5462;
	randomSource1 = 98;
	g.setFood(randomFood1);
//	g.setFood(randomFood2);
	g.setSource(randomSource1);
	g.printLocations();

	ALLEGRO_DISPLAY *display = NULL;
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	display = al_create_display((dimension * width) + 25,
			(dimension * width) + 25);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	int epoch = 0;
	for (int a = 0; a < BEGIN_ANT; a++)
		g.generate_ants();
	while (g.there_is_food()) {
		cout << endl << endl << "EPOCA: " << epoch << endl;
		cout << "FOOD REMAINS---> " << g.getTotalFood() << " ANT REMAINED---->"
				<< g.getAnts().size() << endl << endl;
		if (epoch % 5 == 0)
			g.generate_ants();
		if (epoch % 2 == 0)
			g.decrease_pheromone();
		if (epoch % 1000 == 0)
			g.printMatrix();
		update_gui(width, &g);
		g.update();
		epoch++;
	}
	al_destroy_display(display);
	cout << endl << endl << "END" << endl;
	return 0;
}
