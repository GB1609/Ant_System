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

struct Cell
{
    int ant_size;
    vector<Ant> ants;
    vector<int> directions;
    int pheromone;
    bool source;
    bool food;
};

class Graph
{
  private:
    bool **adjacents;
    Cell **cells;
    int dim;
    int max_num;

  public:
    Graph (int w)
    {
      dim = w;
      max_num = dim * dim;
      adjacents = (bool **) malloc (max_num * sizeof(bool*));
      cells = (Cell**) malloc (max_num * sizeof(Cell*));
      for (int i = 0; i < dim; i++)
      {
	adjacents[i] = (bool *) malloc (max_num * sizeof(bool));
	cells[i] = (Cell *) malloc (max_num * sizeof(Cell));
      }
    }
    void
    addCell ()
    {

    }
    
    bool**
    getAdjacents () const
    {
      return adjacents;
    }
    
    void
    setAdjacents (int a, int b) const
    {
      adjacents[a][b] = true;
    }

    int
    getDim ()
    {
      return dim;
    }

    int
    getDimMax ()
    {
      return max_num;
    }

    void
    printGAdj ()
    {
      for (int a = 0; a < dim; a++)
      {
	for (int b = 0; b < dim; b++)
	  cout << adjacents[a][b] << " - ";
	cout << endl;
      }
    }

    int
    pairToInt (int row, int col)
    {
      return row * dim + col;
    }

    pair<int, int>
    intToPair (int num)
    {
      pair<int, int> p;
      int x = num / dim;
      int y = num % dim;
      p = make_pair (x, y);
      return p;
    }
    int
    left (int n)
    {
      if (n - 1 > -1) return n - 1;
      return -1;
    }

    int
    right (int n)
    {
      if (n + 1 < max_num) return n + 1;
      return -1;
    }

    int
    down (int n)
    {
      if (n + dim < max_num) return n + 1;
      return -1;
    }

    int
    up (int n)
    {
      if (n - dim >= 0) return n + 1;
      return -1;
    }

    bool
    isNear (int a, int b)
    {
      int l = left (a);
      int r = right (a);
      int u = up (a);
      int d = down (a);
      return b == l || b == r || b == u || b == d;
    }
};

#endif /* GRAPH_H_ */
