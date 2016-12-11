#ifndef LINEXA_H
#define LINEXA_H
#include <stdio.h>

#define MAX_STATESIZE 16
#define MAX_STATESIZE_STR "16"

struct ttable_entry_t {
    char input[MAX_STATESIZE+1];
    char output;
};

struct ttable_t {
    struct ttable_entry_t *entries;
    int count;
};

struct linexa_t {
    int labelCount;
    char *labels;
    int stateSize;
    char *state;
    struct ttable_t transitionTable;
};

struct linexa_t*
linexa_create( const char *labels, int stateSize );

void
linexa_setState( struct linexa_t*, const char* sequence );

int
linexa_readTransitionTable( struct linexa_t*, FILE* );

int
linexa_advance( struct linexa_t* );

char
linexa_lastLabel( struct linexa_t* );

void
linexa_free( struct linexa_t* );

#endif
