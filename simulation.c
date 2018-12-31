#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "simulation.h"
#include "world_type.h"

char *op_names[] = {"hop", "left", "right", "infect", "ifempty", "ifenemy", "ifsame", "ifwall", "go"};
char *dir_name[] = {"east", "south", "west", "north"};
char *dir_short_name[] = {"e", "s", "w", "n"};

int opType(char *op_name) {
    int i;
    for (i = 0; i < 9; i++) {
        if (strcmp(op_name, op_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int dirOrder(char *direction) {
    int i;
    for (i = 0; i < 4; i++) {
        if (strcmp(direction, dir_name[i]) == 0) {
            return i;
        }
    }
    return -1;
}

char *directName(int order) {
    return dir_name[order];
}

int initWorld(struct world_t *world, char *species_file, char *creatures_file) {
    FILE *in_species = fopen(species_file, "r");
    if (!in_species) {         //ERROR 3
        printf("Error: Cannot open file %s!\n", species_file);
        return 0;
    }

    char creature_dir[32];
    char specie[32];
    fscanf(in_species, "%s", creature_dir);
    while (!feof(in_species)) {
        fscanf(in_species, "%s\n", specie);
        if (strcmp(specie, "") != 0) {
            if (world->num_species == MAXSPECIES) {          //ERROR 4
                printf("Error: Too many species!\n");
                printf("Maximal number of species is %d.\n", MAXSPECIES);
                return 0;
            }
            strcpy(world->species[world->num_species].name, specie);
            world->num_species++;
        }
    }
    fclose(in_species);

    //input instructions of all species
    char line[255];
    FILE *in_creatures;
    int i;
    for (i = 0; i < world->num_species; i++) {
        char f_creature[64];
        strcpy(f_creature, creature_dir);
        strcat(f_creature, "/");
        strcat(f_creature, world->species[i].name);
        in_creatures = fopen(f_creature, "r");
        if (!in_creatures) {               //ERROR 3
            printf("Error: Cannot open file %s!\n", f_creature);
            fclose(in_creatures);
            return 0;
        }
        world->species[i].program_size = 0;
        char operation[32];
        while (!feof(in_creatures)) {
            fscanf(in_creatures, "%s", operation);
            if (strcmp(operation, "") != 0) {
                int step;
                if (world->species[i].program_size >= MAXPROGRAM) {              //ERROR 5
                    printf("Too many instructions for species %s!\n", world->species[i].name);
                    printf("Maximal number of instructions is %d.\n", MAXPROGRAM);
                    fclose(in_creatures);
                    return 0;
                }
                if (0 <= opType(operation) && opType(operation) <= 3) {
                    world->species[i].program[world->species[i].program_size].op = (enum opcode_t)(opType(operation));
                    world->species[i].program_size++;
                    fgets(line, 255, (FILE*)in_creatures);
                }
                else if (4 <= opType(operation) && opType(operation) <= 8) {
                    fscanf(in_creatures, "%d", &step);
                    if (0 <= step && step <= MAXPROGRAM) {
                        world->species[i].program[world->species[i].program_size].op = (enum opcode_t)(opType(operation));
                        world->species[i].program[world->species[i].program_size].address = step;
                        world->species[i].program_size++;
                    }
                    fgets(line, 255, (FILE*)in_creatures);
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        fclose(in_creatures);
    }

    //input data of the world
    FILE *in_world;
    int width, height;
    in_world = fopen(creatures_file, "r");
    if (!in_world) {               //ERROR 3
        printf("Error: Cannot open file %s!\n", creatures_file);
        return 0;
    }
    fscanf(in_world, "%d", &height);
    if (height > MAXHEIGHT || height < 1) {      //ERROR 10
        printf("Error: The grid height is illegal!\n");
        return 0;
    }
    fscanf(in_world, "%d", &width);
    if (width > MAXWIDTH || width < 1) {         //ERROR 11
        printf("Error: The grid width is illegal!\n");
        return 0;
    }
    world->grid.height = height;
    world->grid.width = width;

    //input the data of the creatures in the world
    while (!feof(in_world)) {
        fscanf(in_world, "%s", specie);
        if (world->num_creatures == MAXCREATURES) {              //ERROR 7
            printf("Error: Too many creatures!\n");
            printf("Maximal number of creatures is %d.\n", MAXCREATURES);
            return 0;
        }
        char direction[8];
        int x, y, specie_order = -1;
        fscanf(in_world, "%s", direction);
        fscanf(in_world, "%d", &x);
        fscanf(in_world, "%d\n", &y);
        if (x < 0 || x >= world->grid.height || y < 0 || y >= world->grid.width) {             //ERROR 12
            printf("Error: Creature (%s %s %d %d) is out of bound!\n", specie, direction, x, y);
            printf("The grid size is %d-by-%d.\n", world->grid.height, world->grid.width);
            return 0;
        }
        if (dirOrder(direction) == -1) {          //ERROR 9
            printf("Error: Direction %s is not recognized!\n", direction);
            return 0;
        }
        if (world->grid.squares[x][y] != NULL) {             //ERROR 13
            printf("Error: Creature (%s %s %d %d) overlaps with creature (%s %s %d %d)!\n", specie, direction, x, y, world->grid.squares[x][y]->species->name, dir_name[world->grid.squares[x][y]->direction], world->grid.squares[x][y]->location.r, world->grid.squares[x][y]->location.c);
            return 0;
        }
        world->creatures[world->num_creatures].direction = (enum direction_t)(dirOrder(direction));
        world->creatures[world->num_creatures].location.r = x;
        world->creatures[world->num_creatures].location.c = y;
        world->creatures[world->num_creatures].programID = 1;
        for (i = 0; i < world->num_species; i++) {
            if (strcmp(specie, world->species[i].name) == 0) {
                specie_order = i;
            }
        }
        if (specie_order == -1) {               //ERROR 8
            printf("Error: Species %s not found!\n", specie);
            return 0;
        }
        world->creatures[world->num_creatures].species = &world->species[specie_order];
        world->grid.squares[x][y] = &world->creatures[world->num_creatures];
        world->num_creatures++;
    }
    //printGrid(world->grid);
    return 1;
}

void printGrid(const struct grid_t grid) {
    int i, j;
    for (i = 0; i < grid.height; i++) {
        for (j = 0; j < grid.width; j++) {
            if (grid.squares[i][j] == NULL) {
                printf("____ ");
            }
            else {
                char specie[32], direction[32];
                strcpy(specie, grid.squares[i][j]->species->name);
                strcpy(direction, dir_short_name[grid.squares[i][j]->direction]);
                printf("%c%c_%s ", specie[0], specie[1], direction);
            }
        }
        printf("\n");
    }
}

struct point_t adjacentPoint(struct point_t pt, enum direction_t dir) {
    struct point_t ad_pt;
    switch (dir) {
        case EAST: {
            ad_pt.c = pt.c+1;
            ad_pt.r = pt.r;
            break;
        }
        case WEST: {
            ad_pt.c = pt.c-1;
            ad_pt.r = pt.r;
            break;
        }
        case SOUTH: {
            ad_pt.c = pt.c;
            ad_pt.r = pt.r+1;
            break;
        }
        case NORTH: {
            ad_pt.c = pt.c;
            ad_pt.r = pt.r-1;
            break;
        }
    }
    return ad_pt;
}

enum direction_t leftFrom(enum direction_t dir) {
    switch (dir) {
        case EAST: {
            return NORTH;
            break;
        }
        case WEST: {
            return SOUTH;
            break;
        }
        case SOUTH: {
            return EAST;
            break;
        }
        case NORTH: {
            return WEST;
            break;
        }
    }
}

enum direction_t rightFrom(enum direction_t dir) {
    switch (dir) {
        case EAST: {
            return SOUTH;
            break;
        }
        case WEST: {
            return NORTH;
            break;
        }
        case SOUTH: {
            return WEST;
            break;
        }
        case NORTH: {
            return EAST;
            break;
        }
    }
}

struct instruction_t getInstruction(const struct creature_t creature) {
    return creature.species->program[creature.programID - 1];
}

void simulateCreature(struct creature_t *creature, struct grid_t *grid, int verbose) {
    if (verbose)
        printf("Creature (%s %s %d %d) takes action:\n", creature->species->name, dir_name[creature->direction], creature->location.r, creature->location.c);
    else
        printf("Creature (%s %s %d %d) takes action:", creature->species->name, dir_name[creature->direction], creature->location.r, creature->location.c);

    struct instruction_t operation = getInstruction(*creature);
    int f_round = 0;
    while (!f_round) {
        switch (operation.op) {
            case HOP: {
                struct point_t p0 = creature->location;
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c >= 0 && p.c < grid->width && p.r >= 0 && p.r < grid->height) {
                    if (grid->squares[p.r][p.c] == NULL) {
                        creature->location=p;
                    }
                }
                grid->squares[p0.r][p0.c] = NULL;
                grid->squares[creature->location.r][creature->location.c] = creature;
                creature->programID++;
                f_round = 1;
                if (verbose) {
                    printf("Instruction %d: hop\n", creature->programID - 1);
                }
                else {
                    printf("hop\n");
                }
                break;
            }
            case LEFT: {
                creature->direction = leftFrom(creature->direction);
                creature->programID++;
                f_round = 1;
                if (verbose) {
                    printf("Instruction %d: left\n", creature->programID - 1);
                }
                else {
                    printf("left\n");
                }
                break;
            }
            case RIGHT: {
                creature->direction = rightFrom(creature->direction);
                creature->programID++;
                f_round = 1;
                if (verbose) {
                    printf("Instruction %d: right\n", creature->programID - 1);
                }
                else {
                    printf("right\n");
                }
                break;
            }
            case INFECT: {
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c >= 0 && p.c < grid->width && p.r >= 0 && p.r < grid->height) {
                    if (grid->squares[p.r][p.c] != NULL) {
                        if (grid->squares[p.r][p.c]->species->name != creature->species->name) {
                            grid->squares[p.r][p.c]->programID = 1;
                            grid->squares[p.r][p.c]->species = creature->species;
                        }
                    }
                }
                creature->programID++;
                f_round = 1;
                if (verbose) {
                    printf("Instruction %d: infect\n", creature->programID - 1);
                }
                else {
                    printf("infect\n");
                }
                break;
            }
            case IFEMPTY: {
                int ID0 = creature->programID;
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c >= 0 && p.c < grid->width && p.r >= 0 && p.r < grid->height) {
                    if (grid->squares[p.r][p.c] == NULL) {
                        creature->programID = operation.address;
                    }
                    else {
                        creature->programID++;
                    }
                }
                else {
                    creature->programID++;
                }
                if (verbose) {
                    printf("Instruction %d: ifempty %d\n", ID0, operation.address);
                }
                break;
            }
            case IFENEMY: {
                int ID0 = creature->programID;
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c >= 0 && p.c < grid->width && p.r >= 0 && p.r < grid->height) {
                    if (grid->squares[p.r][p.c] != NULL) {
                        if (grid->squares[p.r][p.c]->species->name != creature->species->name) {
                            creature->programID = operation.address;
                        }
                        else {
                            creature->programID++;
                        }
                    }
                    else creature->programID++;
                }
                else creature->programID++;
                if (verbose) {
                    printf("Instruction %d: ifenemy %d\n", ID0, operation.address);
                }
                break;
            }
            case IFSAME: {
                int ID0 = creature->programID;
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c >= 0 && p.c < grid->width && p.r >= 0 && p.r < grid->height) {
                    if (grid->squares[p.r][p.c] != NULL) {
                        if (grid->squares[p.r][p.c]->species->name == creature->species->name) {
                            creature->programID = operation.address;
                        }
                        else {
                            creature->programID++;
                        }
                    }
                    else {
                        creature->programID++;
                    }
                }
                else {
                    creature->programID++;
                }
                if (verbose) {
                    printf("Instruction %d: ifsame %d\n", ID0, operation.address);
                }
                break;
            }
            case IFWALL: {
                int ID0 = creature->programID;
                struct point_t p = adjacentPoint(creature->location, creature->direction);
                if (p.c < 0 || p.c >= grid->width || p.r < 0 || p.r >= grid->height) {
                    creature->programID = operation.address;
                }
                else {
                    creature->programID++;
                }
                if (verbose) {
                    printf("Instruction %d: ifwall %d\n", ID0 ,operation.address);
                }
                break;
            }
            case GO: {
                int ID0 = creature->programID;
                creature->programID = operation.address;
                if (verbose) {
                    printf("Instruction %d: go %d\n", ID0, operation.address);
                }
                break;
            }
        }
        operation = getInstruction(*creature);
    }
}


