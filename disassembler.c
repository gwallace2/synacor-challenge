#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WORDSIZE ( 2 )
#define NUM_REGISTERS ( 8 )
#define MAX_INT ( 32767 )
#define NUM_OPS ( 22 )

int main( int argc, char* argv[] ) {

	/*interpret each instruction of the binary*/
	int stop = 0;
	FILE* outfile = fopen( "out.asm", "w" );
	unsigned short int opcode;
	unsigned short int* args;
	int numArgs[] = { 0, 2, 1, 1, 3, 3, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 0, 1, 1, 0 };
	char* ops[] = { "halt", "set", "push", "pop", "eq", "gt", "jmp", "jt", "jf", "add", "mult", "mod", "and", "or", "not", "rmem", "wmem", "call",
			"ret", "out", "in", "noop" };
	int i;
	int line = 0;
	
	while( !stop ) {

		/*read the opcode*/
		int numRead = read( STDIN_FILENO, &opcode, WORDSIZE );
		if( numRead == WORDSIZE && opcode < 22 ) {
			fprintf( outfile, "%4d: ", ++line );
			fprintf( outfile, "%s ", ops[ opcode ] );
			args = ( unsigned short int* )malloc( sizeof( unsigned short int ) * numArgs[ opcode ] );
			for( i = 0; i < numArgs[ opcode ]; i++ ) {
				char* string;
				read( STDIN_FILENO, args + i, WORDSIZE );
				if( opcode == 19 ) {
					if( *( args + i ) == '\n' ) {
						fprintf( outfile, "'\\n' ", *( args + i ) );
					} else {
						fprintf( outfile, "'%c' ", *( args + i ) );
					}
				} else {
					fprintf( outfile, "%d ", *( args + i ) );
				}
			}
			free( args );
			fprintf( outfile, "\n" );
		} else if ( opcode >= 22 ) {
			int cont;
			char response;
			fprintf( outfile, "%4d: ", ++line );
			fprintf( outfile, ";%d\n", opcode );
		}  else {
			stop = 1;
		}
	}
	fclose( outfile );
	return 0;
}
