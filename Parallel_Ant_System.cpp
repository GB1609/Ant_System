#include <iostream>
#include <cstdlib>
#include <random>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <vector>
#include <iterator>
#include <mpi/mpi.h>

using namespace std;

const int BEGIN_ANT = 50;
const int dimension = 100;
const int width_gui = 9;
const int BEGIN_FOOD = 2000;
const int POSSIBLE_DIRECTION = 8;
const int AROUND = 3;
bool gui_on = true;

struct Ant {
	bool food;
	int mySource;
	int pheromone;
	int currentPosition;
	int x;
	int y;
	Ant() {
		food = false;
	}
	;
	Ant(int pos, int toI, int x, int y) {
		pheromone = toI;
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

int left(int n, int max_num) {
	int toCheck = n - 1;
	if (toCheck > -1 && toCheck % dimension != dimension - 1)
		return n - 1;
	return -1;
}

int right(int n, int max_num) {
	int toCheck = n + 1;
	if (toCheck < max_num && toCheck % dimension != 0)
		return n + 1;
	return -1;
}

int down(int n, int max_num) {
	if (n + dimension < max_num)
		return n + dimension;
	return -1;
}

int up(int n, int max_num) {
	if (n - dimension > -1)
		return n - dimension;
	return -1;
}

int up_left(int n, int max_num) {
	int toCheck = up(n, max_num) - 1;
	if (toCheck > -1 && (toCheck % dimension) != dimension - 1)
		return toCheck;
	return -1;
}

int up_right(int n, int max_num) {
	int toCheck = up(n, max_num) + 1;
	if (toCheck > -1 && (toCheck % dimension) != 0)
		return toCheck;
	return -1;
}

int down_right(int n, int max_num) {
	int toCheck = down(n, max_num) + 1;
	if (toCheck < max_num && (toCheck % dimension) != 0)
		return toCheck;
	return -1;
}

int down_left(int n, int max_num) {
	int toCheck = down(n, max_num) - 1;
	if (toCheck < max_num && (toCheck % dimension) != dimension - 1)
		return toCheck;
	return -1;
}

bool not_in(vector<int> &around, int p) {
	for (int a = 0; a < around.size(); a++)
		if (p == around[a])
			return true;
	return false;
}

pair<int, int> intToPair(int num) {
	pair<int, int> p;
	int x = num / dimension;
	int y = num % dimension;
	p = make_pair(x, y);
	return p;
}

int pairToInt(int row, int col) {
	return row * dimension + col;
}

int euclidianDistance(int a, int b) {
	pair<int, int> p1 = intToPair(a);
	pair<int, int> p2 = intToPair(b);
	return sqrt(
			(p1.first - p2.first) * (p1.first - p2.first)
					+ (p1.second - p2.second) * (p1.second - p2.second));
}

void update_gui(int width, int matrix[dimension][dimension],
		vector<Ant> all_ants, int source, int food) {
	const ALLEGRO_COLOR food_color = al_map_rgb(0, 155, 0);
	const ALLEGRO_COLOR ant_color = al_map_rgb(0, 0, 0);
	const ALLEGRO_COLOR cell_without_pheromone = al_map_rgb(102, 255, 255);
	const ALLEGRO_COLOR source_color = al_map_rgb(51, 51, 204);
	const ALLEGRO_COLOR ant_with_food = al_map_rgb(255, 200, 15);

	int maxValue = dimension * dimension;
	int limit_draw = maxValue * 0.2;
	for (int a = 0; a < dimension; a++)
		for (int b = 0; b < dimension; b++) {
			if (matrix[a][b] > limit_draw) {
				int vc = (matrix[a][b] * 255) / maxValue;
				al_draw_filled_rectangle((b * width) + 10, (a * width) + 10,
						(b * width) + width + 10, (a * width) + width + 10,
						al_map_rgb(vc, 0, 0));
			} else
				al_draw_filled_rectangle((b * width) + 10, (a * width) + 10,
						(b * width) + width + 10, (a * width) + width + 10,
						cell_without_pheromone);
		}

	for (int f = 0; f < all_ants.size(); f++) {
		if (all_ants[f].food)
			al_draw_filled_circle((all_ants[f].y * width) + width / 2 + 10,
					(all_ants[f].x * width) + width / 2 + 10, width / 2,
					ant_with_food);
		else
			al_draw_filled_circle((all_ants[f].y * width) + 10 + width / 2,
					(all_ants[f].x * width) + 10 + width / 2, width / 2,
					ant_color);
	}

	int b = food % dimension;
	int a = food / dimension;

	al_draw_filled_circle((b * width) + width / 2 + 10,
			(a * width) + width / 2 + 10, width / 2, food_color);
	b = source % dimension;
	a = source / dimension;
	al_draw_filled_circle((b * width) + width / 2 + 10,
			(a * width) + 10 + width / 2, width / 2, source_color);
	al_flip_display();
}

void decrease_pheromone(int *data, int w) {
	for (int a = 0; a < w; a++)
		if (data[a] > 0)
			data[a]--;
}

int find_direction_without_food(Ant *ant) //guarda se ce feromone attorno e prende quello con quantita maggiore (a caso altrimenti)
		{

	int a = ant->currentPosition;
	int toCheck = -1;
	int left_pos = left(a, dimension * dimension);
//		int l = getPheromoneCell(left_pos);
	int right_pos = right(a, dimension * dimension);
//		int r = getPheromoneCell(right_pos);
	int up_pos = up(a, dimension * dimension);
//		int u = getPheromoneCell(up_pos);
	int down_pos = down(a, dimension * dimension);
//		int d = getPheromoneCell(down_pos);
	int upLeft_pos = up_left(a, dimension * dimension);
//		int ul = getPheromoneCell(upLeft_pos);
	int upRight_pos = up_right(a, dimension * dimension);
//		int ur = getPheromoneCell(upRight_pos);
	int downLeft_pos = down_left(a, dimension * dimension);
//		int dl = getPheromoneCell(downLeft_pos);
	int downRight_pos = down_right(a, dimension * dimension);
//		int dr = getPheromoneCell(downRight_pos);
//		vector<int> possible_move { l, r, u, d, ul, ur, dl, dr };
	vector<int> possible_move { left_pos, right_pos, up_pos, down_pos,
			upLeft_pos, upRight_pos, downLeft_pos, downRight_pos };

//		pair<int, int> max_pheromone = max_value_pheromone(possible_move);
//	if (max_pheromone.first < 1) //random
//			{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0,
			POSSIBLE_DIRECTION - 1);
	while (toCheck == -1) {
		int index = dist(rng);
		if (possible_move[index] > -1)
			toCheck = possible_move[index];
	}

//	}
//		else

//			toCheck = max_pheromone.second;
//		switch (toCheck) {
//		case 0:
//			return left_pos;
//		case 1:
//			return right_pos;
//		case 2:
//			return up_pos;
//		case 3:
//			return down_pos;
//		case 4:
//			return upLeft_pos;
//		case 5:
//			return upRight_pos;
//		case 6:
//			return downLeft_pos;
//		case 7:
//			return downRight_pos;
//		default:
//			cerr << "SEND NEW DIRECTION" << endl;
//			return -1;
//		}
	return toCheck;
}

int come_back_with_food(Ant *ant, int source) {
	int a = ant->currentPosition;
	int possible_move[] = { left(a, dimension * dimension), right(a,
			dimension * dimension), up(a, dimension * dimension), down(a,
			dimension * dimension), up_left(a, dimension * dimension), up_right(
			a, dimension * dimension), down_left(a, dimension * dimension),
			down_right(a, dimension * dimension) };
	int toCheck = -1;
	int max = -1;
	int min = INT_MAX;
	int eD = -1;
	for (int d = 0; d < POSSIBLE_DIRECTION; d++) {
		int valutate = possible_move[d];
		if (valutate > 0 && euclidianDistance(valutate, source) < min) {
			min = euclidianDistance(valutate, source);
			eD = valutate;
		}
	}
	return eD;
}

int count_ant_around(int pos, vector<Ant> ants, vector<int> &around, int num) {
	if (pos < 0)
		return 0;
	int cont = 0;
	num--;
//		for (int a = 0; a < ants.size(); a++)
//			if (ants[a].food && not_in(around,ants[a].currentPosition)
//					&& (ants[a].currentPosition == left(pos)
//							|| ants[a].currentPosition == right(pos)
//							|| ants[a].currentPosition == down(pos)
//							|| up(pos) == ants[a].currentPosition
//							|| up_right(pos) == ants[a].currentPosition
//							|| up_left(pos) == ants[a].currentPosition
//							|| down_left(pos) == ants[a].currentPosition
//							|| down_right(pos) == ants[a].currentPosition))
//					around.push_back(ants[a].currentPosition);
//		if (num > 0) {
//			count_ant_around(left(pos), around, num);
//			count_ant_around(right(pos), around, num);
//			count_ant_around(up(pos), around, num);
//			count_ant_around(down(pos), around, num);
//		}
	return around.size();
}

void propagate(int *cells, int *left_received, int *right_received,
		int *left_send, int *right_send, int pos, int v, Direction d, int min,
		int max, int rank, int fraction) {
	int cP = pos;
	int dec;
	int l;
	bool go = true;
	int val = v;
	while (go) {
		dec = val * 0.2;
		if (d == RIGHT)
			l = right(cP, dimension * dimension);
		if (d == LEFT)
			l = left(cP, dimension * dimension);
		if (d == UP)
			l = up(cP, dimension * dimension);
		if (d == DOWN)
			l = down(cP, dimension * dimension);
		if (d == DOWN_LEFT)
			l = down_left(cP, dimension * dimension);
		if (d == DOWN_RIGHT)
			l = down_right(cP, dimension * dimension);
		if (d == UP_LEFT)
			l = up_left(cP, dimension * dimension);
		if (d == UP_RIGHT)
			l = up_right(cP, dimension * dimension);
		if (l > min - 1 && l < max) {
			val = val - dec;
			cP = l;
			int to_update = cP - (rank * fraction * dimension);
			if (cells[to_update] == 0) {
				cells[to_update] = val;
				go = false;
			} else if (cells[to_update] < val)
				cells[to_update] = val;
		} else
			go = false;
	}
}

void propagate_pheromone(Ant *ant, int *cells, int *cell_left, int *cell_right,
		int fraction, int min, int max, int rank) {

	int a = ant->currentPosition;
	int to_update = a - (rank * (fraction * dimension));
	int toIncreasePheromon = ant->pheromone;
	vector<int> around { a };
//		int ant_around = count_ant_around(a, around, AROUND) + 1; //TODO

	int ant_around = 1;

	if (cells[to_update] < toIncreasePheromon)
		cells[to_update] = toIncreasePheromon;

	while (ant_around > 0) {
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, LEFT, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, RIGHT, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, UP, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, DOWN, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, DOWN_LEFT, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, DOWN_RIGHT, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, UP_LEFT, min, max, rank, fraction); //TODO
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, UP_RIGHT, min, max, rank, fraction); //TODO
		ant_around--;
	}

//	cout << "MATRIX OF PROCESS---->" << rank << endl << endl;
//	int cont = 0;
//	for (int a = 0; a < dimension * fraction; a++) {
//		cout << cells[a] << " - ";
//		cont++;
//		if (cont == dimension) {
//			cont = 0;
//			cout << endl;
//		}
//	}
	if (ant->pheromone > 1)
		ant->pheromone--;
}

void update(vector<Ant> &my_ants, vector<Ant> &ant_to_send_left,
		vector<Ant> &ant_to_send_right, int *my_cell, int *cell_left,
		int *cell_right, int &FOOD, int my_rank, int fraction,
		int proc_destination, int destination, int source, int min, int max) {

	vector<Ant>::iterator f = my_ants.begin();
	decrease_pheromone(my_cell, dimension * fraction);
	while (f != my_ants.end()) {
		if (f->currentPosition == destination) {
			f->food = true;
			FOOD--;
		}
		if (f->food || FOOD == 0) {
			if (f->currentPosition == source) {
				f = my_ants.erase(f);
				f--;
			} else {
				propagate_pheromone(&*f, my_cell, cell_left, cell_right,
						fraction, min, max, my_rank);
				int newPosition = come_back_with_food(&*f, source);
				pair<int, int> p = intToPair(newPosition);
				int x = p.first;
				int y = p.second;
				f->currentPosition = newPosition;
				f->x = x;
				f->y = y;
				if (f->currentPosition < min) {
					ant_to_send_left.push_back(*f);
					f = my_ants.erase(f);
					f--;
					cerr << "ENTRO1" << endl;
				}
				if (f->currentPosition >= max) {
					ant_to_send_right.push_back(*f);
					f = my_ants.erase(f);
					f--;
					cerr << "ENTRO2" << endl;
				}
			}
		} else {
			int newPositon = find_direction_without_food(&*f);
			pair<int, int> p = intToPair(newPositon);
			int x = p.first;
			int y = p.second;
			f->currentPosition = newPositon;
			f->x = x;
			f->y = y;
			if (f->currentPosition < min) {
				ant_to_send_left.push_back(*f);
				f = my_ants.erase(f);
				f--;
			}
			if (f->currentPosition >= max) {
				ant_to_send_right.push_back(*f);
				f = my_ants.erase(f);
				f--;
			}
		}
		f++;
	}
}

int main(int argc, char **argv) {
	ALLEGRO_DISPLAY *disp;

	if (gui_on)
		al_init();

	int process, rank_process;

	MPI_Init(NULL, NULL);
	double begin_execution = MPI_Wtime();

	MPI_Comm_size(MPI_COMM_WORLD, &process);
	if (dimension % process != 0) {
		cout << "MI SCOCCIAVA GESTIRE LA DIVISIONE ERRATA" << endl;
		MPI_Finalize();
		return 0;
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);

	const int BLOCKROWS = (dimension / process);
	const int BLOCKCOLS = dimension;
	int my_data[dimension][dimension]; //generale

	int randomFood1, randomSource1;
	vector<Ant> all_ants;
	int epoch = 0;
	int FOOD = 1500;
	if (rank_process == 0) {
		if (gui_on)
			disp = al_create_display(dimension * width_gui + 25,
					dimension * width_gui + 25);
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0,
				(dimension * dimension) - 1);
		srand(time(0));

		for (int i = 0; i < dimension; i++)
			for (int b = 0; b < dimension; b++)
				my_data[i][b] = 0;

		randomFood1 = dist(rng);
		randomSource1 = randomFood1;
		while (abs(randomFood1 - randomSource1) < (dimension * dimension) / 3)
			randomSource1 = dist(rng);
		randomFood1 = 113;
		randomSource1 = 9976;
	}
	randomFood1 = 113;
	randomSource1 = 9976;

	int prc_source = ((randomSource1 / dimension) / BLOCKROWS);
	int prc_food = ((randomFood1 / dimension) / BLOCKROWS);

	vector<Ant> my_ants;

	int processCurrentGeneration[BLOCKROWS * BLOCKCOLS];
	int left_data[BLOCKCOLS];
	int right_data[BLOCKCOLS];
	int min = rank_process * BLOCKROWS * BLOCKCOLS;
	int max = (rank_process + 1) * BLOCKROWS * BLOCKCOLS;

	vector<Ant> left_ants;
	vector<Ant> right_ants;

	for (int i = 0; i < BLOCKROWS * BLOCKCOLS; i++) {

		processCurrentGeneration[i] = 0;

		if (i < BLOCKCOLS) {
			left_data[i] = 0;
			right_data[i] = 0;
		}
	}

	MPI_Status status;
	MPI_Request request;

	MPI_Comm linearArrayTopology;

	int dimensions = 2, left, right, reorder = false;
	int toroidal[dimensions], topologyDimensions[dimensions],
			coords[dimensions];

	topologyDimensions[0] = process;
	topologyDimensions[1] = 1;
	toroidal[0] = 0, toroidal[1] = 0;
	MPI_Cart_create(MPI_COMM_WORLD, dimensions, topologyDimensions, toroidal,
			reorder, &linearArrayTopology);
	MPI_Cart_shift(linearArrayTopology, 0, 1, &left, &right);
	MPI_Cart_coords(linearArrayTopology, rank_process, dimensions, coords);

	if (rank_process == prc_source) {
		for (int z = 0; z < BEGIN_ANT; z++)
			my_ants.push_back(
					Ant(randomSource1, (dimension * dimension),
							randomSource1 / dimension,
							randomSource1 % dimension));
	}

	MPI_Datatype ant_mpi;
	MPI_Datatype type[6] = { MPI_C_BOOL, MPI_INT, MPI_INT, MPI_INT, MPI_INT,
	MPI_INT };
	int blocklen[6] = { 1, 1, 1, 1, 1, 1 };
	MPI_Aint displ[6] = { 0, 1 * sizeof(bool), 2 * sizeof(int), 3 * sizeof(int),
			4 * sizeof(int), 5 * sizeof(int) };
	MPI_Type_struct(6, blocklen, displ, type, &ant_mpi);
	MPI_Type_commit(&ant_mpi);

	MPI_Datatype rowtype;
	MPI_Type_contiguous(BLOCKROWS * BLOCKCOLS, MPI_INT, &rowtype);
	MPI_Type_commit(&rowtype);

	MPI_Datatype singleNeighbourdRowType;
	MPI_Type_contiguous(BLOCKCOLS, MPI_INT, &singleNeighbourdRowType);
	MPI_Type_commit(&singleNeighbourdRowType);
	if (rank_process == prc_food)
		MPI_Bcast(&FOOD, 1, MPI_INT, rank_process, linearArrayTopology);
	MPI_Request ra[3];

	while (FOOD > 0) {
		epoch++;
		if (rank_process == prc_source && epoch % 2 == 0)
			my_ants.push_back(
					Ant(randomSource1, (dimension * dimension),
							randomSource1 / dimension,
							randomSource1 % dimension));
		left_ants.clear();
		right_ants.clear();

		update(my_ants, left_ants, right_ants, processCurrentGeneration,
				left_data, right_data, FOOD, rank_process, BLOCKROWS, prc_food,
				randomFood1, randomSource1, min, max);

		if (rank_process == prc_food)
			MPI_Bcast(&FOOD, 1, MPI_INT, rank_process, linearArrayTopology);

		int dim = my_ants.size();

		if (gui_on && rank_process != 0) {
			MPI_Isend(&dim, 1, MPI_INT, 0, 47, linearArrayTopology, &ra[0]);
			MPI_Isend(&my_ants[0], my_ants.size(), ant_mpi, 0, 99,
					linearArrayTopology, &ra[1]);
			MPI_Isend(&processCurrentGeneration, 1, rowtype, 0, 12,
					linearArrayTopology, &ra[2]);

		}

		int dimleft = left_ants.size();
		int dimright = right_ants.size();

		if (left > -1 && left < process) {
			MPI_Send(&dimleft, 1, MPI_INT, left, 88, linearArrayTopology);
			MPI_Send(&left_ants[0], dimleft, ant_mpi, left, 95,
					linearArrayTopology);
		}
		if (right > -1 && right < process) {
			MPI_Send(&dimright, 1, MPI_INT, right, 89, linearArrayTopology);
			MPI_Send(&right_ants[0], dimright, ant_mpi, right, 91,
					linearArrayTopology);
		}
		if (right > -1 && right < process) {
			MPI_Recv(&dimright, 1, MPI_INT, right, 88, linearArrayTopology,
					&status);
			vector<Ant> mod_r(dimright);

			MPI_Recv(&mod_r[0], dimright, ant_mpi, right, 95,
					linearArrayTopology,
					MPI_STATUS_IGNORE);

			for (int j = 0; j < dimright; j++) {
				my_ants.push_back(mod_r[j]);
			}
		}
		if (left > -1 && left < process) {
			MPI_Recv(&dimleft, 1, MPI_INT, left, 89, linearArrayTopology,
					&status);

			vector<Ant> mod_l(dimleft);

			MPI_Recv(&mod_l[0], dimleft, ant_mpi, left, 91, linearArrayTopology,
					&status);

			for (int j = 0; j < dimleft; j++)
				my_ants.push_back(mod_l[j]);
		}

		if (gui_on && rank_process != 0) {
			for (int i = 0; i < 3; i++)
				MPI_Wait(&ra[i], &status);

		}
		if (gui_on && rank_process == 0) {
			all_ants.clear();
			for (int i = 1; i < process; i++) {
				MPI_Recv(&dim, 1, MPI_INT, i, 47, linearArrayTopology, &status);
				vector<Ant> mod(dim);
				MPI_Recv(&mod[0], dim, ant_mpi, i, 99, linearArrayTopology,
						&status);
				MPI_Recv(&my_data[i * BLOCKROWS], 1, rowtype, i, 12,
						linearArrayTopology, &status);
				for (int j = 0; j < dim; j++)
					all_ants.push_back(mod[j]);
			}

			for (int i = 0; i < my_ants.size(); i++)
				all_ants.push_back(my_ants[i]);
			for (int i = 0; i < BLOCKROWS * BLOCKCOLS; i++) {
				int curr_row = (i / BLOCKCOLS) + (BLOCKROWS * rank_process);
				int curr_col = (i % BLOCKCOLS) + (BLOCKROWS * rank_process);
				my_data[curr_row][curr_col] = processCurrentGeneration[i];
			}

			update_gui(width_gui, my_data, all_ants, randomSource1,
					randomFood1);
		}
	}
	if (rank_process == 0 && gui_on)
		al_destroy_display(disp);
	double end_execution = MPI_Wtime();
	cout << "TEMPO IMPIEGATO-----_> " << end_execution - begin_execution
			<< " PROCESSO " << rank_process << endl;
	MPI_Finalize();
	return 0;
}

