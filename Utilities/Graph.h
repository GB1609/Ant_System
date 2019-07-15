/*
 * Graph.h
 *
 *  Created on: 4 lug 2019
 *      Author: gb1609
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
using namespace std;
#include <vector>
#include <utility>
#include "Ant.h"

struct Cell {
	int ant_size;
	vector<Ant> ants;
	vector<int> directions;
	int pheromone;
	bool source;
	bool food;
};

class Graph {
private:
	bool **adjacents;
	Cell **cells;
	int dim;
	int max_num;

public:
	Graph(int w) {
		dim = w;
		max_num = dim * dim;
		adjacents = (bool**) malloc(dim * sizeof(bool*));
		cells = (Cell**) malloc(dim * sizeof(Cell*));
		for (int i = 0; i < dim; i++) {
			adjacents[i] = (bool*) malloc(dim * sizeof(bool));
			cells[i] = (Cell*) malloc(dim * sizeof(Cell));
		}
	}

	bool**
	getAdjacents() const {
		return adjacents;
	}

	void setAdjacents(int a, int b) const {
		adjacents[a][b] = true;
	}

	int getDim() {
		return dim;
	}

	int getDimMax() {
		return max_num;
	}

	void printGAdj() {
		for (int a = 0; a < dim; a++) {
			for (int b = 0; b < dim; b++)
				cout << adjacents[a][b] << " - ";
			cout << endl;
		}
	}

	int pairToInt(int row, int col) {
		return row * dim + col;
	}

	pair<int, int> intToPair(int num) {
		pair<int, int> p;
		int x = num / dim;
		int y = num % dim;
		p = make_pair(x, y);
		return p;
	}
	int left(int n) {
		int toCheck = n - 1;
		if (toCheck > -1 && toCheck % dim != dim - 1)
			return n - 1;
		return -1;
	}

	int right(int n) {
		int toCheck = n + 1;
		if (toCheck < max_num && toCheck % dim != 0)
			return n + 1;
		return -1;
	}

	int down(int n) {
		if (n + dim < max_num)
			return n + dim;
		return -1;
	}

	int up(int n) {
		if (n - dim > -1)
			return n - dim;
		return -1;
	}

	int up_left(int n) {
		int toCheck = up(n) - 1;
		if (toCheck > -1 && (toCheck % dim) != dim - 1)
			return toCheck;
		return -1;
	}

	int up_right(int n) {
		int toCheck = up(n) + 1;
		if (toCheck > -1 && (toCheck % dim) != 0)
			return toCheck;
		return -1;
	}

	int down_right(int n) {
		int toCheck = down(n) + 1;
		if (toCheck < max_num && (toCheck % dim) != 0)
			return toCheck;
		return -1;
	}

	int down_left(int n) {
		int toCheck = down(n) - 1;
		if (toCheck < max_num && (toCheck % dim) != dim - 1)
			return toCheck;
		return -1;
	}

	bool isNear(int a, int b) {
		if (a == b)
			return false;
		int l = left(a);
//		cout << "LEFT:" << l << endl;
		int r = right(a);
//		cout << "RIGHT:" << r << endl;
		int u = up(a);
//		cout << "UP:" << u << endl;
		int d = down(a);
//		cout << "DOWN:" << d << endl;
		int ul = up_left(a);
//		cout << "UP-LEFT:" << ul << endl;
		int ur = up_right(a);
//		cout << "UP_RIGHT:" << ur << endl;
		int dl = down_left(a);
//		cout << "DOWN_LEFT:" << dl << endl;
		int dr = down_right(a);
//		cout << "DOWN_RIGHT:" << dr << endl;
		return b == l || b == r || b == u || b == d || b == ul || b == ur
				|| b == dl || b == dr;
	}
	void pheromone_update() {

	}
};

#endif /* GRAPH_H_ */
