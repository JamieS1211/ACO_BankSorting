//
// Created by Jamie on 30/10/2019.
//

#ifndef ACO_BANKSORTING_API_H
#define ACO_BANKSORTING_API_H

typedef struct ant {
    int pathLength;
    int *nodePath;
} Ant;

typedef struct path {
    int destinationNodeID;
    float cost;
    float pheromone;
} Path;

typedef struct node {
    int nodeID;
    int pathCount;
    Path *paths;
} Node;

typedef struct network {
    int nodeCount;
    Node *nodes;
} Network;

/**
 * A function to print the current status of the network to console
 * @param network
 */
void printNetwork(Network network);

/**
 * A function to simulate the ant moving through the network
 * @param antPointer
 * @param network
 */
void simulateAnt(Ant *antPointer, Network network);

/**
 * A function that probabilistically moves the ant along one of the given paths
 * @param antPointer
 * @param pathArray
 * @param pathArraySize
 */
void antDecision(Ant *antPointer, Path *pathArray, int pathArraySize);

/**
 * A function that places pheromone in the network along the path of the given ant
 * @param networkPointer
 * @param ant
 * @param pheromoneQuantity
 */
void placePheromone(Network *networkPointer, Ant ant, float pheromoneQuantity);

/**
 * A function that modifies the pheromone throughout the network by a given scaling factor
 * @param networkPointer
 * @param evaporationRate
 */
void evaporatePheromone(Network *networkPointer, float evaporationRate);




#endif //ACO_BANKSORTING_API_H
