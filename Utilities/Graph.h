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
#include <iterator>
enum Direction {
	UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, BORN
};
#include "Ant.h"

const int BEGIN_FOOD = 2000;
const int POSSIBLE_DIRECTION = 8;

struct Cell {
	int pheromone;
	bool source;
	bool food;
	Cell() {
		pheromone = 0;
		food = false;
		source = false;
	}
};

class Graph {
private:
	vector<Ant> ants;
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

	int euclidianDistance(int a, int b) {
		pair<int, int> p1 = intToPair(a);
		pair<int, int> p2 = intToPair(b);
		return sqrt(
				(p1.first - p2.first) * (p1.first - p2.first)
						+ (p1.second - p2.second) * (p1.second - p2.second));
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

	void generate_ants() {
		for (int a = 0; a < sources.size(); a++) {
			pair<int, int> p = intToPair(sources[a]);
			ants.push_back(Ant(sources[a], max_num, p.first, p.second));
		}
	}

	int getPheromoneCell(int a) {
		if (a < 0 || a > max_num)
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

	Direction find_direction_without_food(Ant *ant) //guarda se ce feromone attorno e prende quello con quantita maggiore (a caso altrimenti)
			{
		int a = ant->getCurrentPosition();
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
		if (max_pheromone.first < 1) //random
				{
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution < std::mt19937::result_type
					> dist(0, POSSIBLE_DIRECTION - 1);
			while (toCheck == -1) {
				int index = dist(rng);
				if (possible_move[index] > -1)
					toCheck = index;
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
			cerr << "SEND NEW DIRECTION" << endl;
			return BORN;
		}
	}

	int come_back_with_food(Ant *ant) {
		int a = ant->getCurrentPosition();
		int possible_move[] = { left(a), right(a), up(a), down(a), up_left(a),
				up_right(a), down_left(a), down_right(a) };
		int toCheck = -1;
		int max = max_num;
		for (int d = 0; d < POSSIBLE_DIRECTION; d++)
			if (possible_move[d] == ant->getMySource()) {
				toCheck = d;
				break;
			} else if (getPheromoneCell(possible_move[d])
					<= ant->getToIncreasePheromon()
					&& getPheromoneCell(possible_move[d]) > max
					&& getPheromoneCell(possible_move[d]) > 0) {
				toCheck = d;
				max = getPheromoneCell(possible_move[d]);
			}
		if (toCheck == -1)
			return -1;
		return possible_move[toCheck];
	}

	void decrease_food(int pos) {
		for (int a = 0; a < foods.size(); a++)
			if (foods[a] == pos) {
				foods_quantity[a]--;
				total_food--;
				if (foods_quantity[a] <= 0) {
					foods.erase(foods.begin() + a);
					foods_quantity.erase(foods_quantity.begin() + a);
					pair<int, int> p = intToPair(pos);
					int x = p.first;
					int y = p.second;
					cells[x][y].food = false;
					for (int f = 0; f < ants.size(); f++)
						ants[f].setFood(false);
					cout << endl << endl << "MATRICE ALLA FINE" << endl;
					printMatrix();
					cout << endl;
					reset_pheromone();
					break;
				}
			}
	}

	void propagate(int pos, int v, Direction d) {
		int cP = pos;
		bool go = true;
		int val = v;
		while (go) {
			int dec = v * 0.2;
			int l;
			if (d == RIGHT)
				l = right(cP);
			if (d == LEFT)
				l = left(cP);
			if (d == UP)
				l = up(cP);
			if (d == DOWN)
				l = down(cP);
			if (d == DOWN_LEFT)
				l = down_left(cP);
			if (d == DOWN_RIGHT)
				l = down_right(cP);
			if (d == UP_LEFT)
				l = up_left(cP);
			if (d == UP_RIGHT)
				l = up_right(cP);
			if (l > 0 || l < max_num) {
				pair<int, int> p = intToPair(l);
				int x = p.first;
				int y = p.second;
				if (cells[x][y].pheromone < val - dec)
					cells[x][y].pheromone = val - dec;
				val = val - dec;
				cP = l;
				if (getPheromoneCell(cP) < 1)
					go = false;
			} else
				go = false;
		}
	}

	void propagate_pheromone(Ant *ant) {
		int a = ant->getCurrentPosition();
		pair<int, int> p = intToPair(a);
		int x = p.first;
		int y = p.second;
		int toIncreasePheromon = ant->getToIncreasePheromon();
		if (cells[x][y].pheromone < toIncreasePheromon) {
			cells[x][y].pheromone = toIncreasePheromon;
			propagate(a, toIncreasePheromon, LEFT);
			propagate(a, toIncreasePheromon, RIGHT);
			propagate(a, toIncreasePheromon, UP);
			propagate(a, toIncreasePheromon, DOWN);
			propagate(a, toIncreasePheromon, DOWN_LEFT);
			propagate(a, toIncreasePheromon, DOWN_RIGHT);
			propagate(a, toIncreasePheromon, UP_RIGHT);
			propagate(a, toIncreasePheromon, UP_LEFT);
		}
		ant->decreaseToIncreasePheromon();
	}
	void decrease_pheromone() {
		for (int a = 0; a < dim; a++)
			for (int b = 0; b < dim; b++)
				if (cells[a][b].pheromone > 0)
					cells[a][b].pheromone -= 1;
	}
	void reset_pheromone() {
		for (int a = 0; a < dim; a++)
			for (int b = 0; b < dim; b++)
				cells[a][b].pheromone = 0;
	}

	void update() {
		vector<Ant>::iterator f=ants.begin();
		while (f !=ants.end()) {
			if (f->isFood()) {
				propagate_pheromone(&*f);
				int newPosition = come_back_with_food(&*f);
				if (getPheromoneCell(newPosition) == -1 || newPosition == -1) {
					Direction come_to_souce = f->getLastDirection();
					newPosition = ant_new_location(f->getCurrentPosition(),
							come_to_souce);
					f->remove_move();
				}
				pair<int, int> p = intToPair(newPosition);
				int x = p.first;
				int y = p.second;
				f->setCurrentPosition(newPosition, x, y);
				f->update_pass_to_come_back();
				if (cells[x][y].source) {
					cells[x][y].pheromone = 1;
					f=ants.erase(f);
					f--;
				}
			} else {
				Direction d = find_direction_without_food(&*f);
				f->addMove(d);
				int newPositon = ant_new_location(f->getCurrentPosition(),
						d);
				if (newPositon < 0)
					cerr << "WRONG WRONG WRONG" << endl;
				pair<int, int> p = intToPair(newPositon);
				int x = p.first;
				int y = p.second;
				f->setCurrentPosition(newPositon, x, y);
				f->update_pass_to_find();
				if (cells[x][y].food) {
					f->setFood(true);
					decrease_food(newPositon);
				}
			}
			f++;
		}
	}

	int ant_new_location(int pos, Direction d) {
		switch (d) {
		case LEFT:
			return left(pos);
		case RIGHT:
			return right(pos);
		case DOWN:
			return down(pos);
		case UP:
			return up(pos);
		case DOWN_RIGHT:
			return down_right(pos);
		case UP_RIGHT:
			return up_right(pos);
		case UP_LEFT:
			return up_left(pos);
		case DOWN_LEFT:
			return down_left(pos);
		case BORN:
			return -1;
		default:
			cerr << "ERRORE IN NEW LOCATION" << endl;
			return -1;
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

	void printMatrix() {
		for (int a = 0; a < dim; a++) {
			cout << "|";
			for (int b = 0; b < dim; b++) {
				if (cells[a][b].food)
					cout << " - " << "F" << " - ";
				else if (cells[a][b].source)
					cout << " - S - ";
				else
					cout << " - " << cells[a][b].pheromone << " - ";
			}
			cout << "|" << endl;
		}
	}

	const vector<Ant>& getAnts() const {
		return ants;
	}

	int getTotalFood() const {
		return total_food;
	}

	Cell getCells(int x, int y) const {
		return cells[x][y];
	}
}
;

#endif /* GRAPH_H_ */
