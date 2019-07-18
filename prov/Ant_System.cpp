#include <iostream>
#include <cstdlib>
#include <random>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Utilities/Graph.h"
using namespace std;

void update_gui(int width, Graph *g) {
	al_clear_to_color(al_map_rgb(102, 255, 255));
	int maxValue = g->getDimMax();
	for (int a = 0; a < g->getDim(); a++)
		for (int b = 0; b < g->getDim(); b++) {
//			if (g->getCells(a, b).pheromone > 0)
//				al_draw_filled_rectangle(((b * width)-width/2)+10, ((a * width)-width/2)+10, ((b * width)+width/2) +10, ((a * width)+width/2)+10,
//						al_map_rgb((g->getCells(a, b).pheromone/maxValue )*255, 0, 0));
			if (g->getCells(a, b).food)
				al_draw_filled_circle((b * width) + 10, (a * width) + 10,
						width / 2, al_map_rgb(0, 155, 0));
			else if (g->getCells(a, b).source)
				al_draw_filled_circle((b * width) + 10, (a * width) + 10,
						width / 2, al_map_rgb(51, 51, 204));
		}
	for (int f = 0; f < g->getAnts().size(); f++)
		if (g->getAnts()[f].isFood())
			al_draw_filled_circle((g->getAnts()[f].getY() * width) + 10,
					(g->getAnts()[f].getX() * width) + 10, width / 2,
					al_map_rgb(255, 15, 15));
		else
			al_draw_filled_circle((g->getAnts()[f].getY() * width) + 10,
					(g->getAnts()[f].getX() * width) + 10, width / 2,
					al_map_rgb(15, 15, 15));
	al_flip_display();
}

int main(int argc, char **argv) {
	int dimension = 100;
	int width = 9;
	Graph g(dimension);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0,
			g.getDimMax() - 1);
	srand(time(0));
	int randomFood1 = dist(rng);
//	int randomFood2 = dist(rng);
	int randomSource1 = randomFood1;
	while (abs(randomFood1 - randomSource1) < g.getDimMax() / 3)
		randomSource1 = dist(rng);
	g.intitializate_matrix();
	randomFood1 = 13;
//	randomFood2 = 87;
	randomSource1 = 9057;
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

	update_gui(width, &g);

	int epoch = 0;
	for (int a = 0; a < 7; a++)
		g.generate_ants();
	while (g.there_is_food() || (!g.there_is_food() && !g.getAnts().empty())) {
		cout << endl << endl << "EPOCA: " << epoch << endl;
		cout << "FOOD REMAINS---> " << g.getTotalFood() << " ANT REMAINED---->"
				<< g.getAnts().size() << endl << endl;
		if (epoch % 10 == 0)
			g.generate_ants();
//		if (epoch % 100 == 0)
//			g.decrease_pheromone();
		if (epoch % 100 == 0)
			g.printMatrix();
		update_gui(width, &g);
		g.update();
		epoch++;
	}
	al_destroy_display(display);
	cout << endl << endl << "END" << endl;
	return 0;
}
