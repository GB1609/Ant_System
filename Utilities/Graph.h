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
#include <math.h>
#include "Ant.h"
enum Direction {
	UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, BORN
};

const int BEGIN_FOOD = 1000;
const int POSSIBLE_DIRECTION = 8;

struct Cell {
	vector<Ant> ants;
	vector<Direction> directions;
	int pheromone;
	bool source;
	bool food;
	Cell() {
		pheromone = 0;
		food = false;
		source = false;
	}
	void print() {
		cout << "PHEROMONE--->" << pheromone << endl;
		cout << "SOURCE--->" << source << endl;
		cout << "FOOD--->" << food << endl;
		cout << "ANTS--->" << ants.size() << endl;
		cout << "DIRECTIONS--->" << directions.size() << endl;
	}
};

class Graph {
private:
	Cell **cells;
	int dim;
	int max_num;
	int total_food;
	vector<int> sources;
	vector<int> foods;
	vector<int> foods_quantity;

public:
	Graph(int w) {
		dim = w;
		max_num = dim * dim;
		total_food = 0;
		cells = (Cell**) malloc(dim * sizeof(Cell*));
		for (int i = 0; i < dim; i++)
			cells[i] = (Cell*) malloc(dim * sizeof(Cell));
	}

	bool there_is_food() {
		return total_food > 0;
	}

	int getDim() {
		return dim;
	}

	int getDimMax() {
		return max_num;
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
		int r = right(a);
		int u = up(a);
		int d = down(a);
		int ul = up_left(a);
		int ur = up_right(a);
		int dl = down_left(a);
		int dr = down_right(a);
		return b == l || b == r || b == u || b == d || b == ul || b == ur
				|| b == dl || b == dr;
	}

	void intitializate_matrix() {
		for (int a = 0; a < max_num; a++) {
			pair<int, int> p = intToPair(a);
			int x = p.first;
			int y = p.second;
			cells[x][y] = Cell();
		}
	}

	void pheromone_update() {

	}

	void setFood(int a) {
		pair<int, int> p = intToPair(a);
		foods.push_back(a);
		foods_quantity.push_back(BEGIN_FOOD);
		total_food += BEGIN_FOOD;
		int x = p.first;
		int y = p.second;
		cells[x][y].food = true;
	}

	void setSource(int a) {
		pair<int, int> p = intToPair(a);
		sources.push_back(a);
		int x = p.first;
		int y = p.second;
		cells[x][y].source = true;
	}

	void generate_ants(int epoch) {
		for (int a = 0; a < sources.size(); a++) {
			pair<int, int> p = intToPair(sources[a]);
			int x = p.first;
			int y = p.second;
			cells[x][y].ants.push_back(Ant());
			cells[x][y].directions.push_back(BORN);
//			cout << "AGGIUNTA FORMICA IN " << sources[a] << endl;
		}
//		cout << "AGGIUNTA FORMICA nel turno " << epoch << endl;
	}

	int getPheromoneCell(int a) {
		if (a == -1)
			return -1;
		pair<int, int> p = intToPair(a);
		int x = p.first;
		int y = p.second;
		return cells[x][y].pheromone;
	}

	pair<int, int> max_value_pheromone(vector<int> &possible_move) {
		int m = -1;
		int position = -1;
		for (int a = 0; a < possible_move.size(); a++)
			if (m < possible_move[a]) {
				m = possible_move[a];
				position = a;
			}
		pair<int, int> p;
		p = make_pair(m, position);
		return p;
	}

	Direction getDirectionAnt(int a) //guarda se ce feromone attorno e prende quello con quantita maggiore (a caso altrimenti)
			{
		int l = getPheromoneCell(left(a));
		int r = getPheromoneCell(right(a));
		int u = getPheromoneCell(up(a));
		int d = getPheromoneCell(down(a));
		int ul = getPheromoneCell(up_left(a));
		int ur = getPheromoneCell(up_right(a));
		int dl = getPheromoneCell(down_left(a));
		int dr = getPheromoneCell(down_right(a));
		vector<int> possible_move { l, r, u, d, ul, ur, dl, dr };
		pair<int, int> max_pheromone = max_value_pheromone(possible_move);
		int toCheck = -1;
		if (max_pheromone.first == 0) //random
				{
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> dist(0,
					POSSIBLE_DIRECTION - 1);
			while (toCheck > -1) {
				int index = dist(rng);
				if (possible_move[index] > -1)
					toCheck = dist(rng);
			}
		} else
			toCheck = max_pheromone.second;
		switch (toCheck) {
		case 0:
			return LEFT;
		case 1:
			return RIGHT;
		case 2:
			return UP;
		case 3:
			return DOWN;
		case 4:
			return UP_LEFT;
		case 5:
			return UP_RIGHT;
		case 6:
			return DOWN_LEFT;
		case 7:
			return DOWN_RIGHT;
		default:
			return BORN;
		}
	}

	void update() {
		for (int a = 0; a < max_num; a++) {
			pair<int, int> p = intToPair(a);
			int x = p.first;
			int y = p.second;
		}
	}

	void printLocations() {
		cout << "STAMPA LOCALITA" << endl;
		for (int a = 0; a < dim; a++)
			for (int b = 0; b < dim; b++) {
				if (cells[a][b].food == true)
					cout << "cella [" << a << "," << b << "] ("
							<< pairToInt(a, b) << ") ---> CIBO" << endl;
				if (cells[a][b].source == true)
					cout << "cella [" << a << "," << b << "] ("
							<< pairToInt(a, b) << ") ---> NIDO" << endl;
			}
	}
};

#endif /* GRAPH_H_ */
