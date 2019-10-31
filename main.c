#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "ACO_API/API.h"
#include "bagAPI.h"


int main() {
    //Set the random seed based on current time (prevent same results every time)
    srand((unsigned int) time(0));

    //Load data file
    FILE *dataFile;
    dataFile = fopen("BankProblem.txt", "r");

    /*
     * File structure (repeating for n bags)
     *
     * 1   -security van capacity: %i
     * n+2 - bag %i:
     * n+3 -  weight: %f
     * n+4 -  value: %i
     *
     */

    //Setup data file variables
    int vanCapacity;
    int bagCount = 0;
    int bagID;
    Bag *bags = malloc(bagCount * sizeof(Bag));

    //Read data file contents
    fscanf(dataFile, "security van capacity: %i", &vanCapacity);

    while(fscanf(dataFile, " bag %i:", &bagID) != EOF) {
        bagCount++;
        bags = realloc(bags, bagCount * sizeof(Bag));
        fscanf(dataFile, "  weight: %f", &bags[bagCount - 1].weight);
        fscanf(dataFile, "  value: %i", &bags[bagCount - 1].value);
    }

    //Close data file
    fclose(dataFile);



    //Find fitness benchmark (compare packing efficiency of solution to average value to weight density)
    float totalWeight = 0;
    int totalValue = 0;
    for (int bagID = 0; bagID < bagCount; bagID++) {
        totalWeight += bags[bagID].weight;
        totalValue += bags[bagID].value;
    }
    float averageValueDensity = totalValue / totalWeight;
    float estimatedValue = averageValueDensity * vanCapacity;





    int testsToRun = 100;
    float cumulativeBestValues = 0;
    for (int test = 0; test < testsToRun; test++) {
        //Initialise network
        Network network;
        network.nodeCount = (bagCount * 2) + 2; //In or out stats for bag plus start and end node
        network.nodes = malloc(network.nodeCount * sizeof(Node));

        for (int nodeIndex = 0; nodeIndex < network.nodeCount; nodeIndex++) {
            /*
             * First and last node considered as start and end respectively
             * All other nodes come in pairs representing the two states (in van / not in van) of the bag
             * All nodes should be connected ONLY to bags not yet sorted (higher bag index)
             *
             * Bag index given by: ((nodeIndex + nodeIndex % 2) / 2) - 1
             * Note this marks start and end nodes with bag indexes however this is to be ignored
             */

            network.nodes[nodeIndex].nodeID = nodeIndex;

            if (nodeIndex == 0) {
                //START node - To in or out state of first bag
                network.nodes[nodeIndex].pathCount = 2;
            } else if (nodeIndex == network.nodeCount - 1) {
                //END node - Can't go anywher
                network.nodes[nodeIndex].pathCount = 0;
            } else if ((nodeIndex + nodeIndex % 2) / 2 == bagCount) {
                //Nodes for last bag - To end node
                network.nodes[nodeIndex].pathCount = 1;
            } else {
                //Nodes for all but last bags - To either in or out state of next bag
                network.nodes[nodeIndex].pathCount = 2;
            }

            network.nodes[nodeIndex].paths = malloc(network.nodes[nodeIndex].pathCount * sizeof(Path));

            for (int pathIndex = 0; pathIndex < network.nodes[nodeIndex].pathCount; pathIndex++) {
                network.nodes[nodeIndex].paths[pathIndex].destinationNodeID =
                        nodeIndex + nodeIndex % 2 + (pathIndex + 1);
                network.nodes[nodeIndex].paths[pathIndex].cost = 1;
                network.nodes[nodeIndex].paths[pathIndex].pheromone = rand() / (float) RAND_MAX;
            }
        }


        //Run ant simulation
        int p = 10000; //Population size
        int g = 100; //Generation size
        float e = 0.6; //Evaporation rate (multiplier between each run)
        float m = 0.0001; //Amount of pheromone deposited according to fitness

        float bestFitness = 0;
        float bestAntWeight = 0;
        int bestAntValue = 0;

        for (int generation = 0; generation < p / g; generation++) {
            Ant *ants = malloc(g * sizeof(Ant));
            float *fitnesses = malloc(g * sizeof(float));

            //Setup generation
            for (int antIndex = 0; antIndex < g; antIndex++) {
                ants[antIndex].pathLength = 1;
                ants[antIndex].nodePath = malloc(ants[antIndex].pathLength * sizeof(int));
                ants[antIndex].nodePath[0] = 0;

                //Simulate ant movement
                simulateAnt(&ants[antIndex], network);



                // Calculate fitness
                float loadedWeight = 0;
                int loadedValue = 0;
                int loadedBags = 0;
                for (int bagID = 0; bagID < bagCount; bagID++) {
                    if (ants[antIndex].nodePath[bagID] % 2 == 0) {
                        loadedWeight += bags[bagID].weight;
                        loadedValue += bags[bagID].value;
                        loadedBags++;
                    }
                }

                fitnesses[antIndex] = (loadedValue / estimatedValue);

                if (loadedWeight > vanCapacity) {
                    fitnesses[antIndex] *= vanCapacity / totalWeight;
                }

                fitnesses[antIndex] *= fitnesses[antIndex];
                fitnesses[antIndex] *= fitnesses[antIndex];
                fitnesses[antIndex] *= fitnesses[antIndex];





                if (fitnesses[antIndex] > bestFitness) {
                    bestFitness = fitnesses[antIndex];
                    bestAntWeight = loadedWeight;
                    bestAntValue = loadedValue;
                }
            }


            for (int antIndex = 0; antIndex < g; antIndex++) {
                //Increase pheromone
                placePheromone(&network, ants[antIndex], m * fitnesses[antIndex]);
                free(ants[antIndex].nodePath);
            }
            free(ants);
            free(fitnesses);



            //Evaporate pheromone
            evaporatePheromone(&network, e);
        }

        printf("Best Ant \t fitness: %g \t loadedWeight: %g \t loadedValue: %i\n", bestFitness, bestAntWeight,
               bestAntValue);

        cumulativeBestValues += bestAntValue;
    }

    printf("Average best ant value: %g\n", cumulativeBestValues / testsToRun);
    return 0;
}