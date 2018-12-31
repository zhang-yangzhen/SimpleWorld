#ifndef SIMULATION_H
#define SIMULATION_H

#include "world_type.h"

int opType(char* op_name);
// EFFECTS: return the order of the command of name op_name in the command list;
// The function will return -1 if op_name is not a legal command name;
// For example, when op_name is "hop", the function will return 0;

int dirOrder(char* direction);
// EFFECTS: return the order of the direction of name direction in the direction list;
// The function will return -1 if direction is not a legal direction name;
// For example, when direction is "east", the function will return 0;

int initWorld(struct world_t *world, char *speciesFile, char *creaturesFile);
// MODIFIES: world
//
// EFFECTS: Initialize "world" given the species summary file
// "speciesFile" and the world description file
// "creaturesFile". This initializes all the components of
// "world". Returns true if initialization is successful.

void simulateCreature(struct creature_t *creature, struct grid_t *grid, int verbose);
// REQUIRES: creature is inside the grid.
//
// MODIFIES: creature, grid, cout.
//
// EFFECTS: Simulate one turn of "creature" and update the creature,
// the infected creature, and the grid if necessary.
// The creature programID is always updated. The function
// also prints to the stdout the procedure. If verbose is
// true, it prints more information.

void printGrid(const struct grid_t grid);
// MODIFIES: cout.
//
// EFFECTS: print a grid representation of the creature world->

struct point_t adjacentPoint(struct point_t pt, enum direction_t dir);
// EFFECTS: Returns a point that results from moving one square
// in the direction "dir" from the point "pt".

enum direction_t leftFrom(enum direction_t dir);
// EFFECTS: Returns the direction that results from turning
// left from the given direction "dir".

enum direction_t rightFrom(enum direction_t dir);
// EFFECTS: Returns the direction that results from turning
// right from the given direction "dir".

struct instruction_t getInstruction(const struct creature_t creature);
// EFFECTS: Returns the current instruction of "creature".

//creature_t *getCreature(const grid_t &grid, point_t location);
// REQUIRES: location is inside the grid.
//
// EFFECTS: Returns a pointer to the creature at "location" in "grid".

#endif
