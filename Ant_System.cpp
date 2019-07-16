#include <iostream>
#include <cstdlib>
#include <random>

#include "Utilities/Graph.h"
using namespace std;

int main(int argc, char **argv) {
	int dimension = 10;
	Graph g(dimension);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0,
			g.getDimMax() - 1);
	srand(time(0));
	int randomFood1 = dist(rng);
	int randomSource1 = randomFood1;
	while (abs(randomFood1 - randomSource1) < g.getDimMax() / 3)
		randomSource1 = dist(rng);
	g.intitializate_matrix();
	randomFood1 = 354;
	randomSource1 = 2120;
	g.setFood(randomFood1);
	g.setSource(randomSource1);
	g.printLocations();
	int epoch = 0;
	g.generate_ants();
	while (g.there_is_food()) {
		cout << endl << endl << "EPOCA: " << epoch << endl;
		cout << "FOOD REMAINS---> " << g.getTotalFood() << " ANT REMAINED---->"
				<< g.getAnts().size() << endl << endl;
		if (epoch % 10 == 0)
			g.generate_ants();
		if (epoch % 1000 == 0)
			g.decrease_pheromone();
		if (epoch % 10000 == 0)
			g.printMatrix();
		g.update();
		epoch++;
	}
	g.printMatrix();

	cout << endl << endl << "END";
	return 0;
}
