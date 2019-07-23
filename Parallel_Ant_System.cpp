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
const int width_gui = 3;
const int BEGIN_FOOD = 750;
const int POSSIBLE_DIRECTION = 8;
const int AROUND = 2;
const int RF1=dimension+2;
const int RS1=dimension*dimension -(dimension)-1;
const bool gui_on = false;

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

int position_left(int n, int max_num) {
	int toCheck = n - 1;
	if (toCheck > -1 && toCheck % dimension != dimension - 1)
		return n - 1;
	return -1;
}

int position_right(int n, int max_num) {
	int toCheck = n + 1;
	if (toCheck < max_num && toCheck % dimension != 0)
		return n + 1;
	return -1;
}

int position_down(int n, int max_num) {
	if (n + dimension < max_num)
		return n + dimension;
	return -1;
}

int position_up(int n, int max_num) {
	if (n - dimension > -1)
		return n - dimension;
	return -1;
}

int up_left(int n, int max_num) {
	int toCheck = position_up(n, max_num) - 1;
	if (toCheck > -1 && (toCheck % dimension) != dimension - 1)
		return toCheck;
	return -1;
}

int up_right(int n, int max_num) {
	int toCheck = position_up(n, max_num) + 1;
	if (toCheck > -1 && (toCheck % dimension) != 0)
		return toCheck;
	return -1;
}

int down_right(int n, int max_num) {
	int toCheck = position_down(n, max_num) + 1;
	if (toCheck < max_num && (toCheck % dimension) != 0)
		return toCheck;
	return -1;
}

int down_left(int n, int max_num) {
	int toCheck = position_down(n, max_num) - 1;
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
	int limit_draw = maxValue * 0.3;
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

int search_pheromone(Ant *ant, int *cells, int fraction, int min, int max,
		int rank, int *left_received, int *right_received) {

	int m = INT_MAX;

	int r = ant->currentPosition;
	int count = -1;
	int toReturn = -1;
	int to_verify;
	int left = position_left(r, dimension * dimension);
	while (left >= 0) {
		count++;
		to_verify = left - (rank * fraction * dimension);
		if (min <= left && left < max && cells[to_verify] > 0) {
			if (count < min) {
				m = count;
				toReturn = left;
			}
			break;
		}
		left = position_left(left, dimension * dimension);
	}

	count = -1;
	int right = position_right(r, dimension * dimension);
	while (right >= 0) {
		count++;
		to_verify = right - (rank * fraction * dimension);
		if (min <= right && right < max && cells[to_verify] > 0) {
			if (count < min) {
				m = count;
				toReturn = right;
			}
			break;
		}
		right = position_right(right, dimension * dimension);
	}

	count = -1;
	int down = position_down(r, dimension * dimension);
	while (down >= 0) {
		count++;
		to_verify = down - (rank * fraction * dimension);
		if (min <= down && down < max && cells[to_verify] > 0) {
			if (count < min) {
				m = count;
				toReturn = down;
			}
			break;
		}
		down = position_down(down, dimension * dimension);
	}
	return toReturn;

	count = -1;
	int up = position_up(r, dimension * dimension);
	while (up >= 0) {
		count++;
		to_verify = up - (rank * fraction * dimension);
		if (min <= up && down < up && cells[to_verify] > 0) {
			if (count < min) {
				m = count;
				toReturn = up;
			}
			break;
		}
		up = position_down(up, dimension * dimension);
	}
	return toReturn;
}

int find_direction_without_food(Ant *ant, int *cells, int fraction, int min,
		int max, int rank, int *left_received, int *right_received, int epoch) //guarda se ce feromone attorno e prende quello con quantita maggiore (a caso altrimenti)
		{
	int a = ant->currentPosition;
	int to_return = -1;
	if (epoch % 500 == 0)
		to_return = search_pheromone(ant, cells, fraction, min, max, rank,
				left_received, right_received);
	if (to_return == -1) {
		int left_pos = position_left(a, dimension * dimension);
		int right_pos = position_right(a, dimension * dimension);
		int up_pos = position_up(a, dimension * dimension);
		int down_pos = position_down(a, dimension * dimension);
		int upLeft_pos = up_left(a, dimension * dimension);
		int upRight_pos = up_right(a, dimension * dimension);
		int downLeft_pos = down_left(a, dimension * dimension);
		int downRight_pos = down_right(a, dimension * dimension);
		vector<int> possible_move { left_pos, right_pos, up_pos, down_pos,
				upLeft_pos, upRight_pos, downLeft_pos, downRight_pos };
		int m = 0;
		to_return = -1;
		for (int a = 0; a < POSSIBLE_DIRECTION; a++) {
			if (possible_move[a] >= min && possible_move[a] < max
					&& m
							< cells[possible_move[a]
									- (fraction * dimension * rank)]) {
				m = cells[possible_move[a] - (rank * fraction * dimension)];
				to_return = possible_move[a];
			} else if (possible_move[a] >= min - dimension
					&& possible_move[a] < min)
				if (left_received[possible_move[a] - (min - dimension)] > m) {
					m = left_received[possible_move[a] - (min - dimension)];
					to_return = possible_move[a];
				} else if (possible_move[a] >= max
						&& possible_move[a] < max + dimension)
					if (right_received[possible_move[a] - max] > m) {
						m = left_received[possible_move[a] - max];
						to_return = possible_move[a];
					}
		}
		if (to_return == -1) //random
				{
			std::random_device dev;
			std::mt19937 rng(dev());
			std::uniform_int_distribution<std::mt19937::result_type> dist(0,
					POSSIBLE_DIRECTION - 1);
			while (to_return == -1) {
				int index = dist(rng);
				if (possible_move[index] > -1)
					to_return = possible_move[index];
			}
		}
	}
	return to_return;
}

int come_back_with_food(Ant *ant, int source) {
	int a = ant->currentPosition;
	int possible_move[] =
			{ position_left(a, dimension * dimension), position_right(a,
					dimension * dimension), position_up(a,
					dimension * dimension), position_down(a,
					dimension * dimension), up_left(a, dimension * dimension),
					up_right(a, dimension * dimension), down_left(a,
							dimension * dimension), down_right(a,
							dimension * dimension) };
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
	num--;
	for (int a = 0; a < ants.size(); a++)
		if (ants[a].food && not_in(around, ants[a].currentPosition)
				&& (ants[a].currentPosition
						== position_left(pos, dimension * dimension)
						|| ants[a].currentPosition
								== position_right(pos, dimension * dimension)
						|| ants[a].currentPosition
								== position_down(pos, dimension * dimension)
						|| position_up(pos, dimension * dimension)
								== ants[a].currentPosition
						|| up_right(pos, dimension * dimension)
								== ants[a].currentPosition
						|| up_left(pos, dimension * dimension)
								== ants[a].currentPosition
						|| down_left(pos, dimension * dimension)
								== ants[a].currentPosition
						|| down_right(pos, dimension * dimension)
								== ants[a].currentPosition))
			around.push_back(ants[a].currentPosition);
	if (num > 0) {
		count_ant_around(position_left(pos, dimension * dimension), ants,
				around, num);
		count_ant_around(position_right(pos, dimension * dimension), ants,
				around, num);
		count_ant_around(position_up(pos, dimension * dimension), ants, around,
				num);
		count_ant_around(position_down(pos, dimension * dimension), ants,
				around, num);
	}
	return around.size();
}

void propagate(int *cells, int *left_received, int *right_received,
		int *left_send, int *right_send, int pos, int v, Direction d, int min,
		int max, int rank, int fraction) {
	int cP = pos;
	int dec;
	int l;
	int val = v;
	while (true) {
		dec = val * 0.2;
		if (d == RIGHT)
			l = position_right(cP, dimension * dimension);
		if (d == LEFT)
			l = position_left(cP, dimension * dimension);
		if (d == UP)
			l = position_up(cP, dimension * dimension);
		if (d == DOWN)
			l = position_down(cP, dimension * dimension);
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
				return;
			} else if (cells[to_update] < val)
				cells[to_update] = val;
		} else
			return;
	}
}

void propagate_pheromone(Ant *ant, int *cells, int *cell_left, int *cell_right,
		int fraction, int min, int max, int rank, vector<Ant> ants) {

	int a = ant->currentPosition;
	int to_update = a - (rank * fraction * dimension);
	int toIncreasePheromon = ant->pheromone;
	vector<int> around { a };
	int ant_around = count_ant_around(a, ants, around, AROUND);

	if (cells[to_update] < toIncreasePheromon)
		cells[to_update] = toIncreasePheromon;

	while (ant_around > 0) {
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, LEFT, min, max, rank, fraction);
		propagate(cells, cell_left, cell_right, cell_left, cell_right, a,
				toIncreasePheromon, RIGHT, min, max, rank, fraction);
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

	if (ant->pheromone > 1)
		ant->pheromone--;
}

void update(vector<Ant> &my_ants, vector<Ant> &ant_to_send_left,
		vector<Ant> &ant_to_send_right, int *left_received, int *right_received,
		int *my_cell, int *cell_left, int *cell_right, int &FOOD, int my_rank,
		int fraction, int proc_destination, int destination, int source,
		int min, int max, int epoch) {

	vector<Ant>::iterator f = my_ants.begin();
	while (f != my_ants.end()) {
		if (f->currentPosition == destination) {
			f->food = true;
			FOOD--;
		}
		if (f->food || FOOD == 0) {
			propagate_pheromone(&*f, my_cell, cell_left, cell_right, fraction,
					min, max, my_rank, my_ants);
			if (f->currentPosition == source) {
				f = my_ants.erase(f);
				f--;
			} else {
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
				}
				if (f->currentPosition >= max) {
					ant_to_send_right.push_back(*f);
					f = my_ants.erase(f);
					f--;
				}
			}
		} else {
			int newPositon = find_direction_without_food(&*f, my_cell, fraction,
					min, max, my_rank, left_received, right_received,epoch);
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
//		if (epoch % 2000)
//			decrease_pheromone(my_cell, dimension * fraction);
}
int main(int argc, char **argv) {
	ALLEGRO_DISPLAY *disp;

	int FOOD = BEGIN_FOOD;

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
		randomFood1 = RF1;
		randomSource1 = RS1;
	}
	randomFood1 = RF1;
	randomSource1 = RS1;

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
	int left_cell_to_send[dimension];
	int right_cell_to_send[dimension];
	int left_cell_received[dimension];
	int right_cell_received[dimension];
	for (int i = 0; i < dimension; i++) {
		left_cell_to_send[i] = 0;
		right_cell_to_send[i] = 0;
		left_cell_received[i] = 0;
		right_cell_received[i] = 0;
	}

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

	MPI_Datatype all_data_row;
	MPI_Type_contiguous(BLOCKROWS * BLOCKCOLS, MPI_INT, &all_data_row);
	MPI_Type_commit(&all_data_row);

	MPI_Datatype single_row_to_send_mpi;
	MPI_Type_contiguous(BLOCKCOLS, MPI_INT, &single_row_to_send_mpi);
	MPI_Type_commit(&single_row_to_send_mpi);
	MPI_Request ra[3];

	while (FOOD > 0) {
		if (rank_process == prc_source && epoch % 5 == 0)
			my_ants.push_back(
					Ant(randomSource1, (dimension * dimension),
							randomSource1 / dimension,
							randomSource1 % dimension));
		left_ants.clear();
		right_ants.clear();

		update(my_ants, left_ants, right_ants, left_cell_received,
				right_cell_received, processCurrentGeneration, left_data,
				right_data, FOOD, rank_process, BLOCKROWS, prc_food,
				randomFood1, randomSource1, min, max, epoch);

		if (rank_process == prc_food)
			for (int a = 0; a < process; a++)
				MPI_Send(&FOOD, 1, MPI_INT, a, 10, linearArrayTopology);

		if (rank_process != prc_food)
			MPI_Recv(&FOOD, 1, MPI_INT, prc_food, 10, linearArrayTopology,
					&status);

		int dim = my_ants.size();

		if (gui_on && rank_process != 0) {
			MPI_Isend(&dim, 1, MPI_INT, 0, 47, linearArrayTopology, &ra[0]);
			MPI_Isend(&my_ants[0], my_ants.size(), ant_mpi, 0, 99,
					linearArrayTopology, &ra[1]);
			MPI_Isend(&processCurrentGeneration, 1, all_data_row, 0, 12,
					linearArrayTopology, &ra[2]);
		}

		int dimleft = left_ants.size();
		int dimright = right_ants.size();

		if (left > -1 && left < process) {
			MPI_Send(&dimleft, 1, MPI_INT, left, 88, linearArrayTopology);
			MPI_Send(&left_ants[0], dimleft, ant_mpi, left, 95,
					linearArrayTopology);
			for (int a = 0; a < dimension; a++)
				left_cell_to_send[a] = processCurrentGeneration[a];
			MPI_Send(&left_cell_to_send[0], 1, single_row_to_send_mpi, left, 76,
					linearArrayTopology);
		}
		if (right > -1 && right < process) {
			MPI_Send(&dimright, 1, MPI_INT, right, 89, linearArrayTopology);
			MPI_Send(&right_ants[0], dimright, ant_mpi, right, 91,
					linearArrayTopology);
			for (int a = dimension; a > 0; a--)
				right_cell_to_send[dimension - a] =
						processCurrentGeneration[(dimension * BLOCKROWS) - a];
			MPI_Send(&right_cell_to_send[0], 1, single_row_to_send_mpi, right,
					75, linearArrayTopology);
		}
		if (right > -1 && right < process) {
			MPI_Recv(&dimright, 1, MPI_INT, right, 88, linearArrayTopology,
					&status);
			vector<Ant> mod_r(dimright);

			MPI_Recv(&mod_r[0], dimright, ant_mpi, right, 95,
					linearArrayTopology,
					MPI_STATUS_IGNORE);

			MPI_Recv(&right_cell_received[0], 1, single_row_to_send_mpi, right,
					76, linearArrayTopology,
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

			MPI_Recv(&left_cell_received[0], 1, single_row_to_send_mpi, left,
					75, linearArrayTopology,
					MPI_STATUS_IGNORE);

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
				MPI_Recv(&my_data[i * BLOCKROWS], 1, all_data_row, i, 12,
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
		epoch++;
	}
	if (rank_process == 0 && gui_on)
		al_destroy_display(disp);
	double end_execution = MPI_Wtime();
	cout << "TEMPO IMPIEGATO-----_> " << end_execution - begin_execution
			<< " PROCESSO " << rank_process << endl;
	MPI_Finalize();
	return 0;
}

