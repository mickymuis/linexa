#include "linexa.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void printLabel( FILE* file, char label ) {
    fprintf( file, "\e[38;5;%dm\u2588\e[0m", (int)label, (int)label );
}

void print_help( char* argv0) {
    printf( "Linexa - Linearly Expanding (cellular) Automata\n \
usage: %s N -s state_labels -t transition_table [-i initial_sequence] [-o output] [-f format_table]\n \
\n \
\t N             \t Number of iterations. \n \
\t -l \n \
\t --labels      \t Specify a string of possible state labels. State labels can be exactly one ASCII character long.\n \
\t -t \n \
\t --ttable      \t Specify file that contains white-space separated table of transitions.\n \
\t -i \n \
\t --initial     \t Optional string of initial sequence.\n \
\t -o \n \
\t --output      \t Optional output file.\n \
\t -f \n \
\t --ftable      \t Optional file that contains LUT that translate each state lable into a character sequence.\n \
\t --help        \t This help.\n \
");
}

int
main( int argc, char** argv ) {

    bool has_N =false, has_labels =false, has_ttable =false, has_initial =false, has_output =false, has_ftable =false;
    unsigned int N =0;
    int index_labels =0, index_ttable =0, index_ftable =0, index_initial =0, index_output =0;

    for( int i = 1; i < argc; i++ ) {
        if( strncmp( argv[i], "-l", 2 ) == 0 || strncmp( argv[i], "--labels", 8 ) == 0 ) {
            has_labels =true;
            index_labels =++i;
        } else if( strncmp( argv[i], "-t", 2 ) == 0 || strncmp( argv[i], "--ttable", 8 ) == 0 ) {
            has_ttable =true;
            index_ttable =++i;
        } else if( strncmp( argv[i], "-i", 2 ) == 0 || strncmp( argv[i], "--initial", 9 ) == 0 ) {
            has_initial =true;
            index_initial =++i;
        } else if( strncmp( argv[i], "-o", 2 ) == 0 || strncmp( argv[i], "--output", 8 ) == 0 ) {
            has_output =true;
            index_output =++i;
        } else if( strncmp( argv[i], "-f", 2 ) == 0 || strncmp( argv[i], "--ftable", 8 ) == 0 ) {
            has_ftable =true;
            index_ftable =++i;
        } else if( strncmp( argv[i], "--help", 6 ) == 0 ) {
            print_help ( *argv );
            return 0;
        }
        else {
            N =atoi( argv[i] );
            if( !N || has_N ) {
                fprintf( stderr, "Invalid argument.\n" ); 
                return -1;
            }
            else has_N =true;
        }
    }
    if( !has_N || !has_labels || !has_ttable ) {
        fprintf( stderr, "Insufficient arguments.\n" );
        return -1;
    }

    struct linexa_t* linexa;

    FILE* ttable =fopen( argv[index_ttable], "r" );
    if( !ttable ) {
        fprintf( stderr, "Error opening %s - %s\n", argv[index_ttable], strerror(errno) );
        return -1;
    }

    char temp[MAX_STATESIZE+1];
    if( !fscanf( ttable, "%" MAX_STATESIZE_STR "s", temp ) ) {
        fprintf( stderr, "Unexpected format of transition table." );
        return -1;
    }
    int state_size = strlen( temp );
    rewind( ttable );
    //printf( "state size: %d\n", state_size );
    //free( temp );

    linexa =linexa_create( argv[index_labels], state_size ); 
    int size =linexa_readTransitionTable( linexa, ttable );
    if( size <= 0 ) {
        fprintf( stderr, "Error reading transition table.\n" );
        return -1;
    }

    // Print initial state
    if( strlen( argv[index_initial] ) < state_size ) {
        fprintf( stderr, "Error: intial state to small.\n" );
        return -1;
    }
    linexa_setState( linexa, argv[index_initial] );
    for( int i =0; i < state_size; i++ )
        printLabel( stdout, *(linexa->state+i) );

    for( int i =0; i < N; i++ ) {
        if( linexa_advance( linexa ) != 0 ) {
            fprintf( stderr, "Error: no transition for '%s'\n", linexa->state );
            return -1;
        }
        // Print newest extension
        printLabel( stdout, linexa_lastLabel( linexa ) );
    }

    fprintf( stdout, "\n" );
    
    linexa_free( linexa );

    return 0;
};
