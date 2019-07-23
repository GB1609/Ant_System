# ANT_SYSTEM
    
## Overview
This program is an implementation of ant_system model, a cellular automaton that simulates the behavior of an ant colony in search of food with use of **MPI**.


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


Each process will manage the updating of the elements present in the competence line, or the cells (with the value of the pheromone) and the ants.




Come accennato prima, in ogni versione, il partizionamento viene effettuato in modo equo fra
i vari processi. Il processo master si occupa, solamente all’inizio, di configurare l’ambiente
iniziale della simulazione e successivamente di spedire una partizione dei dati ad ogni
processo, a questo punto, ogni processo effettuerà le varie operazioni di aggiornamento sulla
propria partizione ad esclusione delle celle che si trovano sul bordo. Questo è fondamentale
poiché essendo lo stato di ogni cella strettamente dipendente da quello delle celle del
vicinato, le celle sul bordo hanno bisogno di conoscere lo stato di celle che appartengono ad
un altro processo, per tale ragione vengono gestiti separatamente dal resto. Infatti, all’inizio
della computazione di ogni step ogni processo riceve in modo ​ non bloccante ​ i bordi di cui
necessita dai processi vicini, effettua l’aggiornamento delle celle centrali e successivamente
verifica che i bordi necessari siano stati effettivamente ricevuti, altrimenti rimane in attesa di
essi. Non appena ogni processo termina il proprio aggiornamento spedisce i nuovi dati al
master che si occuperà di effettuare il rendering delle informazioni.

## Results



## Technologies

** ALLEGRO **
** MPI **
