#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    int round;
    int verbose = 0;
    if (argc < 4) {            //ERROR 1
        printf("Error: Missing arguments!\n");
        printf("Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]\n");
        return 0;
    }
    round = atoi(argv[3]);
    if (round < 0) {        //ERROR 2
        printf("Error: Number of simulation rounds is negative!\n");
        return 0;
    }
    if (argc > 4) {
        if (strcmp(argv[4],"verbose") == 0 || strcmp(argv[4],"v") == 0) {
            verbose = 1;
        }
        else {
            verbose = 0;
        }
    }

    //initialize the data of a new world
    struct world_t *world = (struct world_t*)malloc(sizeof(struct world_t));
    world->num_species = 0;
    world->num_creatures = 0;
    int i, j;
    //world->grid.squares=new creature_t[20*20];
    for (i = 0; i < MAXHEIGHT; i++) {
        for (j = 0;j < MAXWIDTH; j++) {
            world->grid.squares[i][j] = NULL;
        }
    }
    if (initWorld(world, argv[1], argv[2])) {
        printf("Initial state\n");
        printGrid(world->grid);

        for (i = 0; i < round; i++) {
            printf("Round %d\n", i+1);
            for (j = 0; j < world->num_creatures; j++) {
                simulateCreature(&world->creatures[j], &world->grid, verbose);
                if (verbose) {
                    printGrid(world->grid);
                }
            }
            if (!verbose) {
                printGrid(world->grid);
            }
        }
    }
    return 0;
}
