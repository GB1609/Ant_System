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

struct Ant {
	bool food;
	int mySource;
	int toIncreasePheromon;
	int currentPosition;
	int x;
	int y;
	Ant(int pos, int toI, int x, int y) {
		toIncreasePheromon = toI;
		food = false;
		currentPosition = pos;
		mySource = pos;
		this->x = x;
		this->y = y;
	}
};

enum Direction {
	UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, BORN
};

const int BEGIN_FOOD = 2000;
const int POSSIBLE_DIRECTION = 8;
const int AROUND = 3;

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
	int epoch;

public:
	Graph(int w) {
		dim = w;
		max_num = dim * dim;
		total_food = 0;
		cells = (Cell**) malloc(dim * sizeof(Cell*));
		for (int i = 0; i < dim; i++)
			cells[i] = (Cell*) malloc(dim * sizeof(Cell));
		epoch = 1;
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
		if (ants.size() < 200)
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

	int search_pheromone(int pos) {
		int min = INT_MAX;

		int r = pos;
		int count = -1;
		int toReturn = -1;
		while (left(r) >= 0) {
			count++;
			if (getPheromoneCell(left(r)) > 0) {
				if (count < min) {
					min = count;
					toReturn = left(r);
				}
				break;
			}
			r = left(r);
		}

		r = pos;
		count = -1;
		while (right(r) >= 0) {
			count++;
			if (getPheromoneCell(right(r)) > 0) {
				if (count < min) {
					min = count;
					toReturn = right(r);
				}
				break;
			}
			r = right(r);
		}

		r = pos;
		count = -1;
		while (up(r) >= 0) {
			count++;
			if (getPheromoneCell(up(r)) > 0) {
				if (count < min) {
					min = count;
					toReturn = up(r);
				}
				break;
			}
			r = up(r);
		}

		r = pos;
		count = -1;
		while (down(r) >= 0) {
			count++;
			if (getPheromoneCell(down(r)) > 0) {
				if (count < min) {
					min = count;
					toReturn = down(r);
				}
				break;
			}
			r = down(r);
		}

		return toReturn;
	}

	int find_direction_without_food(Ant *ant) //guarda se ce feromone attorno e prende quello con quantita maggiore (a caso altrimenti)
			{
		int a = ant->currentPosition;
		if (epoch % 500 == 0) {
			int sp = search_pheromone(a);
			if (sp > 0)
				return sp;
		}
		int toCheck = -1;
		int left_pos = left(a);
		int l = getPheromoneCell(left_pos);
		int right_pos = right(a);
		int r = getPheromoneCell(right_pos);
		int up_pos = up(a);
		int u = getPheromoneCell(up_pos);
		int down_pos = down(a);
		int d = getPheromoneCell(down_pos);
		int upLeft_pos = up_left(a);
		int ul = getPheromoneCell(upLeft_pos);
		int upRight_pos = up_right(a);
		int ur = getPheromoneCell(upRight_pos);
		int downLeft_pos = down_left(a);
		int dl = getPheromoneCell(downLeft_pos);
		int downRight_pos = down_right(a);
		int dr = getPheromoneCell(downRight_pos);
		vector<int> possible_move { l, r, u, d, ul, ur, dl, dr };
		pair<int, int> max_pheromone = max_value_pheromone(possible_move);
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
			return left_pos;
		case 1:
			return right_pos;
		case 2:
			return up_pos;
		case 3:
			return down_pos;
		case 4:
			return upLeft_pos;
		case 5:
			return upRight_pos;
		case 6:
			return downLeft_pos;
		case 7:
			return downRight_pos;
		default:
			cerr << "SEND NEW DIRECTION" << endl;
			return -1;
		}
	}

	int come_back_with_food(Ant *ant) {
		int a = ant->currentPosition;
		int possible_move[] = { left(a), right(a), up(a), down(a), up_left(a),
				up_right(a), down_left(a), down_right(a) };
		int toCheck = -1;
		int max = -1;
		int min = INT_MAX;
		int eD = -1;
		for (int d = 0; d < POSSIBLE_DIRECTION; d++) {
			int valutate = possible_move[d];
			if (valutate > 0
					&& euclidianDistance(valutate, ant->mySource) < min) {
				min = euclidianDistance(valutate, ant->mySource);
				eD = valutate;
			}
		}
		return eD;
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
					for (int f = 0; f < ants.size(); f++) {
						ants[f].food = false;
						ants[f].toIncreasePheromon = false;
					}
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
		int dec;
		int l;
		bool go = true;
		int val = v;
		while (go) {
			dec = val * 0.1;
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
			if (l > -1) {
				val = val - dec;
				cP = l;
				pair<int, int> p = intToPair(cP);
				int x = p.first;
				int y = p.second;
				if (cells[x][y].pheromone == 0) {
					cells[x][y].pheromone = val;
					go = false;
				} else if (cells[x][y].pheromone < val)
					cells[x][y].pheromone = val;
			} else
				go = false;
		}
	}

	int count_ant_around(int pos, int posOriginal, int num) {
		if (pos < 0)
			return 0;
		int cont = 0;
		num--;
		for (int a = 0; a < ants.size(); a++)
			if (ants[a].food && ants[a].currentPosition != posOriginal
					&& (ants[a].currentPosition == left(pos)
							|| ants[a].currentPosition == right(pos)
							|| ants[a].currentPosition == down(pos)
							|| up(pos) == ants[a].currentPosition
							|| up_right(pos) == ants[a].currentPosition
							|| up_left(pos) == ants[a].currentPosition
							|| down_left(pos) == ants[a].currentPosition
							|| down_right(pos) == ants[a].currentPosition)) {
				cont++;
			}
		if (num > 0) {
			cont += count_ant_around(left(pos), posOriginal, num);
			cont += count_ant_around(right(pos), posOriginal, num);
			cont += count_ant_around(up(pos), posOriginal, num);
			cont += count_ant_around(down(pos), posOriginal, num);
		}
		return cont;
	}

	void propagate_pheromone(Ant *ant) {

		int a = ant->currentPosition;
		pair<int, int> p = intToPair(a);
		int x = p.first;
		int y = p.second;
		int toIncreasePheromon = ant->toIncreasePheromon;
		int ant_around = count_ant_around(a, a, AROUND + 1) + 1;

		if (cells[x][y].pheromone < toIncreasePheromon)
			cells[x][y].pheromone = toIncreasePheromon;

		while (ant_around > 0) {
			propagate(a, toIncreasePheromon, LEFT);
			propagate(a, toIncreasePheromon, RIGHT);
			propagate(a, toIncreasePheromon, UP);
			propagate(a, toIncreasePheromon, DOWN);
			propagate(a, toIncreasePheromon, DOWN_LEFT);
			propagate(a, toIncreasePheromon, DOWN_RIGHT);
			propagate(a, toIncreasePheromon, UP_RIGHT);
			propagate(a, toIncreasePheromon, UP_LEFT);
			ant_around--;
		}

		if (ant->toIncreasePheromon > 1)
			ant->toIncreasePheromon--;
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
		vector<Ant>::iterator f = ants.begin();
		while (f != ants.end()) {
			if (f->food) {
				propagate_pheromone(&*f);
				int newPosition = come_back_with_food(&*f);
				if (newPosition < 0)
					cerr << "ERrOR come back" << endl;
				pair<int, int> p = intToPair(newPosition);
				int x = p.first;
				int y = p.second;
				f->currentPosition = newPosition;
				f->x = x;
				f->y = y;
				if (cells[x][y].source) {
					cells[x][y].pheromone = 1;
					f = ants.erase(f);
					f--;
				}
			} else {
				int newPositon = find_direction_without_food(&*f);
				if (newPositon < 0)
					cerr << "WRONG WRONG WRONG" << endl;
				pair<int, int> p = intToPair(newPositon);
				int x = p.first;
				int y = p.second;
				f->currentPosition = newPositon;
				f->x = x;
				f->y = y;
				if (cells[x][y].food) {
					f->food = true;
					decrease_food(newPositon);
				}
			}
			f++;
		}
		epoch++;
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
