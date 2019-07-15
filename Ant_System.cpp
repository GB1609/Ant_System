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
	while (abs(randomFood1 - randomSource1) < g.getDimMax() / 3) {
		cout << "searching " << randomFood1 << " ----------------"
				<< randomSource1 << "   DIFF :::" << randomFood1 - randomSource1
				<< endl;
		randomSource1 = dist(rng);
	}
	cout << "LOCATIONS   " << randomFood1 << " ----------------"
			<< randomSource1 << endl;
	g.intitializate_matrix();
	g.setFood(randomFood1);
	g.setSource(randomSource1);
	g.printLocations();

	int epoch = 0;

	while (g.there_is_food() && epoch < 100000) {
		if (epoch % 3 == 0)
			g.generate_ants();
		cout << "EPOCA: " << epoch << endl << endl;
		g.printMatrix();
		g.update();
		epoch++;
	}
	cout<<endl<<endl<<"END"<<endl;
	return 0;
}
