# ANT_SYSTEM
    
## Overview
This program is an implementation of ant_system model, a cellular automaton that simulates the behavior of an ant colony in search of food with use of **MPI**, is a communication protocol for computers..


In the initial state there is a number of ants located at a point of origin, called a **nest**.
Ants start to move in a random way until one of them find the **food**.


Once the food is found the ant returns towards the nest releasing a substance called pheromone during the journey. The remains ants

The remaining ants will be attracted to this pheromone and move towards it. In this way they will be able to find food easily and return to the nest. The greater the number of neighboring ants that propagate pheromone the greater the distance it will expand.

## Implementation technologies
There are provided both sequential and parallel implementation of the Cellular Automata, and both of them was developed by using the **C++**

GUI was implemented with **ALLEGRO** library.

## Parallel implementation

The parallel implementation consists in a separation of problem in n sub_problem 
each of which is assigned to a different process.

The division was carried out in rows, dividing the matrix by mirroring the world equally between the processes.

<p align="center">
  <img src="https://github.com/GB1609/AntSystem/blob/master/readme_img/subdivision.jpg" width="320">
</p>

Each process will manage the updating of the elements present in the competence line, or the cells (with the value of the pheromone) and the ants.

Every Process receive informations about cells and ants from the process next to him, both right and left.
This is due to the fact that every process to manage the ants that are near the edges needs to know the data of the edges on its left and on its right. Since we are not facing a problem with the toroidal world, the first process will send exclusively to the one on its right, while the process to the left only.

### Details of implementation

At beginning the process 0 initiliazate GUI, with source and food location.

Every Process must manage:
1. A vector of ants in its portion
    * move them randomly if they haven't food
    * move them in direction of source if they have food
    * expand pheromone in a neighborhood starting from its position
1. A vector of cells in its portion
1. The expansion of pheromone in its portion
1. Send to its neighbours the ants that leave its portions of matrix
1. Receive from its neighbours 
1. Send to Process 0 all data for gui display

Le comunicazioni avvengono tramite l'ausilio di 3 datatype:
1. MPI_Datatype (*MPI_STRUCT*) ant_mpi: to send and receive the ants that leave or enter in a process
1. MPI_Datatype (*MPI_Type_contiguous*) single_row_to_send_mpi: to send a single row of matrix to the limiting process)
1. MPI_Datatype (*MPI_Type_contiguous*) all_data_row: to send all sub_matrix to process 0 for gui.

## GUI implementation
The gui was implemented with Allegro, a a cross-platform library. 
The implementation is very simple, each cell is colored based on the amount of pheromone in it:
1. The higher the pheromone, the higher the shade of red
1. The lower it is the less it's red.
1. The food cell is colored green
1. The source cell is colored blue
1. Every cell with an ant without food is colored black
1. Every cell with an ant with food is colored yellow

<p align="center">
  <img src="https://github.com/GB1609/AntSystem/blob/master/readme_img/image0.png" width="500">
</p>



## Results (ONE PROCESS VS MORE)


## Conclusion



## Technologies

* ALLEGRO https://liballeg.org/git.html
* MPI
