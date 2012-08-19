#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define WORDSIZE ( 16 )
#define MEMSPACE ( 32768 )
#define MAXINT ( 32768 )
#define NUM_REGISTERS ( 8 )
#define BUFLIMIT ( 1024 )

/*set register <a> to the value of <b>*/
int set( unsigned short int* reg, unsigned short int value );

int main( int argc, char* argv[] ) {

	/*memory variables*/
	unsigned short int* memory = ( unsigned short int* )malloc( sizeof( unsigned short int* ) * MEMSPACE );
	unsigned short int* registers = ( unsigned short int* )malloc( sizeof( unsigned short int* ) * NUM_REGISTERS );
	unsigned short int pc = 0;
	unsigned short int stack; /*stack memory will be dynamic*/
	int numArgs[] = { 0, 2, 1, 1, 3, 3, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 0, 1, 1, 0 };

	/*file variables*/
	char* filename;
	int fdesc;
	int binSize;
	
	if( argc == 2 ) {

		/*read binary into memory*/
		int numRead;
		filename = argv[ 1 ];
		fdesc = open( filename, O_RDONLY );
		binSize = lseek( fdesc, 0, SEEK_END );
		lseek( fdesc, 0, SEEK_SET );
		numRead = read( fdesc, memory, binSize );
		if( numRead == binSize || numRead == 0 ) {

			/*loop control variables*/
			int end = 0;
			int i;

			/*instruction data*/
			unsigned short int opcode;
			unsigned short int* args;

			/*buffered output variables*/
			char* buffer = 0;
			int bufsize = 0;

			/*loop through and interpret each instruction*/
			while( !end ) {
				
				/*get opcode and arguments*/
				opcode = memory[ pc ];
				args = ( unsigned short int* )malloc( sizeof( unsigned short int ) * numArgs[ opcode ] );
				for( i = 0; i < numArgs[ opcode ]; i++ ) {
					args[ i ] = memory[ pc + i + 1 ];
				}

				/*execute instruction*/
				switch( opcode ) {
					case 0:
						write( STDOUT_FILENO, "Program exited normally.\n", strlen( "Program exited normally.\n" ) );
						end = 1;
						break;
					case 19:
						buffer = realloc( buffer, sizeof( char ) * bufsize + 1 );
						buffer[ bufsize++ ] = args[ 0 ];
						if( args[ 0 ] == '\n' || bufsize == BUFLIMIT ) {
							write( STDOUT_FILENO, buffer, strlen( buffer ) );
							free( buffer );
							buffer = 0;
						}
						break;
					case 21:
						break;
					default:
						end = 1;
						break;
				}

				/*increment program couner*/
				pc += numArgs[ opcode ] + 1;
				free( args );
			}
		} else {
			perror( "error reading binary" );
		}
		close( fdesc );
	} else {
		write( STDERR_FILENO, argv[ 0 ], strlen( argv[ 0 ] ) );
		write( STDERR_FILENO, " binary\n", strlen( " binary\n" ) );
		
	}
	return 0;
}

int set( unsigned short int* reg, const unsigned short int value ) {
	*reg = value;
}
