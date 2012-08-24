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

int main( int argc, char* argv[] ) {

	/*memory variables*/
	unsigned short int* memory = ( unsigned short int* )malloc( sizeof( unsigned short int* ) * MEMSPACE );
	unsigned short int* registers = ( unsigned short int* )malloc( sizeof( unsigned short int* ) * NUM_REGISTERS );
	unsigned short int pc = 0;
	unsigned short int* stack = 0; /*stack memory will be dynamic*/
	int stackSize = 0;
	int numArgs[] = { 0, 2, 1, 1, 3, 3, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 0, 1, 1, 0 };

	/*file variables*/
	char* filename;
	int fdesc;
	int binSize;

	/*output messages*/
	char* illegalOpcode = "Illegal opcode encountered. Exiting.\n";
	char* illegalRegister = "Illegal register referenced. Exiting.\n";
	char* illegalLiteral = "Illegal literal referenced. Exiting.\n";
	char* stackError = "Stack error. Exiting\n";
	char* illegalMemory = "Illegal memory access. Exiting.\n";
	char* exitNormally = "Program exited normally.\n";
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
				int jumped = 0;
				
				/*get opcode and arguments*/
				opcode = memory[ pc ];
				args = ( unsigned short int* )malloc( sizeof( unsigned short int ) * numArgs[ opcode ] );
				for( i = 0; i < numArgs[ opcode ]; i++ ) {
					args[ i ] = memory[ pc + i + 1 ];
				}

				/*execute instruction*/
				switch( opcode ) {

					/*halt*/
					case 0:
						write( STDOUT_FILENO, exitNormally, strlen( exitNormally ) );
						end = 1;
						break;

					/*set*/
					case 1:
						if( args[ 0 ] < MAXINT || args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] < MAXINT ) {
							registers[ args[ 0 ] - MAXINT ] = args[ 1 ];
						} else {
							registers[ args[ 0 ] - MAXINT ] = registers[ args[ 1 ] - MAXINT ];
						}
						break;

					/*push*/
					case 2:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 0 ] < MAXINT ) {
							stack = realloc( stack, sizeof( unsigned short int ) * ++stackSize );
							stack[ stackSize - 1 ] = args[ 0 ];
						} else {
							stack = realloc( stack, sizeof( unsigned short int ) * ++stackSize );
							stack[ stackSize - 1 ] = registers[ args[ 0 ] - MAXINT ];
						}
						break;

					/*pop*/
					case 3:
						if( stackSize == 0 ) {
							write( STDERR_FILENO, stackError, strlen( stackError ) );
							end = 1;
						} else if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							registers[ args[ 0 ] - MAXINT ] = stack[ --stackSize ];
							/*stack = realloc( stack, sizeof( unsigned short int ) * --stackSize );*/
						}
						break;

					/*eq*/
					case 4:
						if( args[ 0 ] < MAXINT || args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							if( arg1 == arg2 ) {
								registers[ args[ 0 ] - MAXINT ] = 1;
							} else {
								registers[ args[ 0 ] - MAXINT ] = 0;
							}
						}
						break;
						

					/*gt*/
					case 5:
						if( args[ 0 ] < MAXINT || args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							if( arg1 > arg2 ) {
								registers[ args[ 0 ] - MAXINT ] = 1;
							} else {
								registers[ args[ 0 ] - MAXINT ] = 0;
							}
						}
						break;

						
					/*jump*/
					case 6:
						if( args[ 0 ] >= binSize / 2 ) {
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else {
							pc = args[ 0 ];
							jumped = 1;
							#ifdef DEBUG
							printf( "jumping to %d\n", args[ 0 ] );
							#endif
						}
						break;

					/*jump true*/
					case 7:
						if( args[ 1 ] >= binSize / 2 ) {
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else if( args[ 0 ] >= MAXINT + NUM_REGISTERS ){
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 0 ] < MAXINT ) {
								if( args[ 0 ] != 0 ) {
									pc = args[ 1 ];
									jumped = 1;
									#ifdef DEBUG
									printf( "jumping to %d\n", args[ 1 ] );
									#endif
								}
						} else {
								if( registers[ args[ 0 ] - MAXINT ] != 0 ) {
									pc = args[ 1 ];
									jumped = 1;
									#ifdef DEBUG
									printf( "jumping to %d\n", args[ 1 ] );
									#endif
								}
						}
						break;

					/*jump false*/
					case 8:
						if( args[ 1 ] >= binSize / 2 ) {
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else if( args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 0 ] < MAXINT ) {
								if( args[ 0 ] == 0 ) {
									pc = args[ 1 ];
									jumped = 1;
									#ifdef DEBUG
									printf( "jumping to %d\n", args[ 1 ] );
									#endif 
								}
						} else {
								if( registers[ args[ 0 ] - MAXINT ] == 0 ) {
									pc = args[ 1 ];
									jumped = 1;
									#ifdef DEBUG
									printf( "jumping to %d\n", args[ 1 ] );
									#endif
								}
						}
						break;

					/*add*/
					case 9:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalLiteral, strlen( illegalLiteral ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = arg1 + arg2;
							registers[ args[ 0 ] - MAXINT ] %= MAXINT;
						}
						break;

					/*mult*/
					case 10:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = arg1 * arg2;
							registers[ args[ 0 ] - MAXINT ] %= MAXINT;
						}
						break;

					/*mod*/
					case 11:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = arg1 % arg2;
							registers[ args[ 0 ] - MAXINT ] %= MAXINT;
						}
						break;

					/*and*/
					case 12:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = arg1 & arg2;
							
						}
						break;

					/*or*/
					case 13:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS || args[ 2 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							unsigned short int arg2;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							if( args[ 2 ] < MAXINT ) {
								arg2 = args[ 2 ];
							} else {
								arg2 = registers[ args[ 2 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = arg1 | arg2;
							
						}
						break;

					/*not*/
					case 14:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int arg1;
							if( args[ 1 ] < MAXINT ) {
								arg1 = args[ 1 ];
							} else {
								arg1 = registers[ args[ 1 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = ( ~arg1 );
							registers[ args[ 0 ] - MAXINT ] %= MAXINT;
						}
						break;

					/*rmem*/
					case 15:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS || args[ 0 ] < MAXINT ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else {
							unsigned short int target;
							if( args[ 1 ] < MAXINT ) {
								target = args[ 1 ];
							} else {
								target = registers[ args[ 1 ] - MAXINT ];
							}
							registers[ args[ 0 ] - MAXINT ] = memory[ target ];
						}
						break;

					/*wmem*/
					case 16:
						if( args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else if( args[ 1 ] >= MAXINT + NUM_REGISTERS ) {
							printf( "%d\n", args[ 1 ] );
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
							end = 1;
						} else {
							unsigned short int dest;
							unsigned short int value;
							if( args[ 0 ] < MAXINT ) {
								dest = args[ 0 ];
							} else {
								dest = registers[ args[ 0 ] - MAXINT ];
							}
							if( args[ 1 ] < MAXINT ) {
								value = args[ 1 ];
							} else {
								value = registers[ args[ 1 ] - MAXINT ];
							}
							memory[ dest ] = value;
						}
						break;

					/*call*/
					case 17:
						if( ( args[ 0 ] >= binSize / 2 && args[ 0 ] < MAXINT ) || args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							printf( "%d\n", args[ 0 ] );
							write( STDERR_FILENO, illegalMemory, strlen( illegalMemory ) );
							end = 1;
						} else {
							unsigned short int target;
							if( args[ 0 ] < MAXINT ) {
								target = args[ 0 ];
							} else {
								target = registers[ args[ 0 ] - MAXINT ];
							}
							stack = realloc( stack, sizeof( unsigned short int ) * ++stackSize );
							stack[ stackSize - 1 ] = pc + numArgs[ opcode ] + 1;
							pc = target;
							jumped = 1;
							#ifdef DEBUG
							printf( "jumping to %d\n", args[ 0 ] );
							#endif
							
						}
						break;

					/*ret*/
					case 18:
						
						pc = stack[ --stackSize ];
						/*stack = realloc( stack, sizeof( unsigned short int ) * --stackSize );*/
						jumped = 1;
						#ifdef DEBUG
						printf( "jumping to %d\n", pc );
						#endif
						break;

					/*out*/
					case 19:
						if( args[ 0 ] >= 9 && args[ 0 ] < 127 ) {
							buffer = realloc( buffer, sizeof( char ) * bufsize + 1 );
							buffer[ bufsize++ ] = args[ 0 ];
							if( args[ 0 ] == '\n' || bufsize == BUFLIMIT ) {
								write( STDOUT_FILENO, buffer, bufsize );
								free( buffer );
								buffer = 0;
								bufsize = 0;
							}
						}
						break;

					/*in*/
					case 20:
						if( args[ 0 ] < MAXINT || args[ 0 ] >= MAXINT + NUM_REGISTERS ) {
							write( STDERR_FILENO, illegalRegister, strlen( illegalRegister ) );
						} else {
							read( STDIN_FILENO, registers + ( args[ 0 ] - MAXINT ), 1 );
						}
						break;
					/*noop*/
					case 21:
						/*do nothing*/
						break;
					default:
						write( STDERR_FILENO, illegalOpcode, strlen( illegalOpcode ) );
						end = 1;
						break;
				}

				if( !jumped ) {

					/*increment program couner*/
					pc += numArgs[ opcode ] + 1;
					free( args );
					#ifdef DEBUG
					printf( "line %d\n", pc );
					#endif
				} 
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
