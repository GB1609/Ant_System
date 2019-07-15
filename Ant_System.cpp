#include <iostream>
using namespace std;
#include "Utilities/Graph.h"
#include <math.h>

void initializateG(Graph g) {
	for (int a = 0; a < g.getDimMax(); a++)
		for (int b = 0; b < g.getDimMax(); b++)
			if (g.isNear(a, b))
				cout << a << " e " << b << " sono vicini" << endl;
//			else
//				cout << a << " e " << b << " non sono vicini" << endl;
//				g.setAdjacents(a, b);
}

int main(int argc, char **argv) {
	Graph g(5);
	initializateG(g);
	return 0;
}
