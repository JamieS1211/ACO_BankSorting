//
// Created by Jamie on 30/10/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "API.h"

/**
 * A function to print the current status of the network to console
 * @param network
 */
void printNetwork(Network network) {
    for (int nodeIndex = 0; nodeIndex < network.nodeCount; nodeIndex++) {
        printf("Node: %i\n", network.nodes[nodeIndex].nodeID);

        for (int pathIndex = 0; pathIndex < network.nodes[nodeIndex].pathCount; pathIndex++) {

            printf("Path: %i Destination node:%i Cost:%f Pheromone:%f\n",
                   pathIndex,
                   network.nodes[nodeIndex].paths[pathIndex].destinationNodeID,
                   network.nodes[nodeIndex].paths[pathIndex].cost,
                   network.nodes[nodeIndex].paths[pathIndex].pheromone
            );
        }
    }
}

/**
 * A function to simulate the ant moving through the network
 * @param antPointer
 * @param network
 */
void simulateAnt(Ant *antPointer, Network network) {
    while(1) {
        Node currentNode = network.nodes[antPointer->nodePath[antPointer->pathLength - 1]];

        int validPathCount = 0;
        Path *validPaths = malloc(validPathCount * sizeof(Path));

        for (int pathIndex = 0; pathIndex < currentNode.pathCount; pathIndex++) {

             bool alreadyVisited = false;
             for (int visitedPathIndex = 0; visitedPathIndex < antPointer->pathLength; visitedPathIndex++) {
                 if (currentNode.paths[pathIndex].destinationNodeID == antPointer->nodePath[visitedPathIndex]) {
                     alreadyVisited = true;
                     break;
                 }
             }

            if (!alreadyVisited) {
                validPathCount++;
                validPaths = realloc(validPaths, validPathCount * sizeof(Path));
                validPaths[validPathCount - 1] = currentNode.paths[pathIndex];
            }
        }

        if (validPathCount > 0) {
            antDecision(antPointer, validPaths, validPathCount);
        } else {
            break;
        }


        free(validPaths);
    }

}

/**
 * A function that probabilistically moves the ant along one of the given paths
 * @param antPointer
 * @param pathArray
 * @param pathArraySize
 */
void antDecision(Ant *antPointer, Path *pathArray, int pathArraySize) {
    float totalPheromone = 0;

    for (int pathIndex = 0; pathIndex < pathArraySize; pathIndex++) {
        totalPheromone += pathArray[pathIndex].pheromone;
    }

    float randomValue = (rand() / (float) RAND_MAX) * totalPheromone;


    float tempPheromoneValue = 0;
    for (int pathIndex = 0; pathIndex < pathArraySize; pathIndex++) {
        tempPheromoneValue += pathArray[pathIndex].pheromone;

        if (tempPheromoneValue > randomValue) {
            antPointer->pathLength++;
            antPointer->nodePath = realloc(antPointer->nodePath, antPointer->pathLength * sizeof(int));
            antPointer->nodePath[antPointer->pathLength - 1] = pathArray[pathIndex].destinationNodeID;

            break;
        }
    }
}

/**
 * A function that places pheromone in the network along the path of the given ant
 * @param networkPointer
 * @param ant
 * @param pheromoneQuantity
 */
void placePheromone(Network *networkPointer, Ant ant, float pheromoneQuantity) {
    for (int antNodeIndex = 0; antNodeIndex < ant.pathLength - 1; antNodeIndex++) {
        int currentNode = ant.nodePath[antNodeIndex];
        int nextNode = ant.nodePath[antNodeIndex + 1];

        for (int pathIndex = 0; pathIndex < networkPointer->nodes[currentNode].pathCount; pathIndex++) {
            if (networkPointer->nodes[currentNode].paths[pathIndex].destinationNodeID == nextNode) {
                networkPointer->nodes[currentNode].paths[pathIndex].pheromone += pheromoneQuantity;
                break;
            }
        }
    }
}

/**
 * A function that modifies the pheromone throughout the network by a given scaling factor
 * @param networkPointer
 * @param evaporationRate
 */
void evaporatePheromone(Network *networkPointer, float evaporationRate) {
    for (int nodeIndex = 0; nodeIndex < networkPointer->nodeCount; nodeIndex++) {
        for (int pathIndex = 0; pathIndex < networkPointer->nodes[nodeIndex].pathCount; pathIndex++) {
            networkPointer->nodes[nodeIndex].paths[pathIndex].pheromone *= evaporationRate;
        }
    }
}


