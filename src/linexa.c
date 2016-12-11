#include "linexa.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TTABLE_GROW 4

struct linexa_t*
linexa_create( const char *labels, int stateSize ) {
    struct linexa_t* linexa =(struct linexa_t*)malloc( sizeof( struct linexa_t ) );
    
    linexa->stateSize =stateSize;
    linexa->state =(char*)malloc( stateSize * sizeof( char ) );
    memset( linexa->state, 0, stateSize );

    linexa->labelCount =strlen( labels );
    linexa->labels =(char*)malloc( linexa->labelCount * sizeof( char ) );
    strncpy( linexa->labels, labels, linexa->labelCount );

    linexa->transitionTable.entries =NULL;
    linexa->transitionTable.count =0;

    return linexa;
}

void
linexa_setState( struct linexa_t* linexa, const char* sequence ) {
    int offset =0;
    if( strlen( sequence ) > linexa->stateSize )
            offset =strlen( sequence ) - linexa->stateSize;
    strncpy( linexa->state, sequence + offset, linexa->stateSize );
}

int
linexa_readTransitionTable( struct linexa_t* linexa, FILE* file ) {
    
    if( linexa->transitionTable.entries )
        free( linexa->transitionTable.entries );

    linexa->transitionTable.count =0;

    int elems =0, i =0;
    struct ttable_entry_t* entries =(struct ttable_entry_t*)malloc( TTABLE_GROW * sizeof( struct ttable_entry_t ) );
    elems =TTABLE_GROW;

    while( !ferror( file ) && !feof( file ) ) {
        // grow if neccesary
        if( elems == i ) {
            entries =realloc( entries, (elems+=TTABLE_GROW)*sizeof( struct ttable_entry_t ) );
        }
        struct ttable_entry_t* entry = entries + i;
        int count;

        // input field
        count = fscanf( file, "%" MAX_STATESIZE_STR "s", &entry->input );
        if( count != 1 ) {
            if( count == EOF )
                break;
            return -1;
        }

        // output field
        if( fscanf( file, "%*c%c%*c", &entry->output ) != 1 ) {
            return -1;
        }

        //fprintf( stderr, "'%s' -> '%c'\n", entry->input, entry->output );

        i++;
    }
    linexa->transitionTable.count =i;
    linexa->transitionTable.entries =entries;
    return i;
}

int
linexa_advance( struct linexa_t* linexa ) {
    char label =0;

    for( int i =0; i < linexa->transitionTable.count; i++ ) {
        struct ttable_entry_t* entry =linexa->transitionTable.entries + i;
        if( strncmp( entry->input, linexa->state, linexa->stateSize) == 0 ) {
            label =entry->output;
            break;
        }
    }

    if( label == 0 ) {
        // No match
        return -1;
    }

    // move the current state one to the left
    memcpy( linexa->state, linexa->state + 1, linexa->stateSize - 1 );

    // add the new label
    linexa->state[linexa->stateSize-1] = label;

    return 0;
}

char
linexa_lastLabel( struct linexa_t* linexa ) {
    return linexa->state[linexa->stateSize -1];
}

void
linexa_free( struct linexa_t* linexa ) {

    free( linexa->state );
    free( linexa->labels );
    if( linexa->transitionTable.entries )
        free( linexa->transitionTable.entries );
    free( linexa );
}
