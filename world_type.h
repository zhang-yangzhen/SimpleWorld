#ifndef WORLD_TYPE_H
#define WORLD_TYPE_H

#define MAXSPECIES 10 // Max number of species in the
#define MAXPROGRAM 40 // Max size of a species program
#define MAXCREATURES 50 // Max number of creatures in
                                      // the world
#define MAXHEIGHT 20 // Max height of the grid
#define MAXWIDTH 20  // Max width of the grid


typedef struct point_t {
    int r;
    int c;
};
/*
// Type: point_t
// ------------
// This type is used to represent a point in the grid.
// Its component r corresponds to the row number; its component
// c corresponds to the column number.
*/

enum direction_t { EAST, SOUTH, WEST, NORTH };
/*
// Type: direction_t
// ----------------
// This type is used to represent direction, which can take on
// one of the four values: East, South, West, and North.
*/

enum opcode_t {HOP, LEFT, RIGHT, INFECT, IFEMPTY, IFENEMY,
    IFSAME, IFWALL, GO };
/*
// Type: opcode_t
// -------------
// The type opcode_t is an enumeration of all of the legal
// command names.
*/

//const char *opName[] = {"hop", "left", "right", "infect",
//    "ifempty", "ifenemy", "ifsame", "ifwall", "go"};
// An array of strings representing the command name.

typedef struct instruction_t {
    enum opcode_t op;
    unsigned int address;
};
/*
// Type: instruction_t
// ------------------
// The type instruction_t is used to represent an instruction
// and consists of a pair of an operation code and an integer.
// For some operation code, the integer stores the address of
// the instruction it jumps to. The address is optional.
*/

typedef struct species_t {
    char name[32];
    unsigned int program_size;
    struct instruction_t program[MAXPROGRAM];
};
/*
// Type: species_t
// ------------------
// The type species_t is used to represent a species
// and consists of a string, an unsigned int, and an array
// of instruction_t. The string gives the name of the
// species. The unsigned int gives the number of instructions
// in the program of the species. The array stores all the
// instructions in the program according to their sequence.
*/

typedef struct creature_t {
    struct point_t location;
    enum direction_t direction;
    struct species_t *species;
    unsigned int programID;
};
/*
// Type: creature_t
// ------------------
// The type creature_t is used to represent a creature.
// It consists of a point_t, a direction_t, a pointer to
// species_t and an unsigned int. The point_t gives the location of
// the species. The direction_t gives the direction of the species.
// The pointer to species_t points to the species the creature belongs
// to. The programID gives the index of the instruction to be
// executed in the instruction_t array of the species.
*/

typedef struct grid_t {
    unsigned int height;
    unsigned int width;
    struct creature_t *squares[MAXHEIGHT][MAXWIDTH];
};
/*
// Type: grid_t
// ------------------
// The type grid_t consists of the height and the width of the grid
// and a two-dimensional array of pointers to creature_t. If there is
// a creature at the point (r, c) in the grid, then squares[r][c]
// stores a pointer to that creature. If point (r, c) is not occupied
// by any creature, then squares[r][c] is a NULL pointer.
*/

typedef struct world_t {
    unsigned int num_species;
    struct species_t species[MAXSPECIES];

    unsigned int num_creatures;
    struct creature_t creatures[MAXCREATURES];

    struct grid_t grid;
};
/*
// Type: world_t
// --------------
// This type consists of two unsigned ints, an array of species_t,
// an array of creature_t, and a grid_t object. The first unsigned
// int num_species specifies the number of species in the creature
// world-> The second unsigned int num_creatures specifies the number
// of creatures in the world-> All the species are stored in the array
// species and all the creatures are stored in the array creatures.
// The grid is given in the object grid.
*/

#endif

