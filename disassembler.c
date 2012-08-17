#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define WORDSIZE ( 2 )
#define NUM_REGISTERS ( 8 )
#define MAX_INT ( 32767 )

/*
halt: 0
  stop execution and terminate the program
*/

/*set register <a> to the value of <b>*/
int set( unsigned short int a, const unsigned short int b );

/*push <a> onto the stack*/
int push( const unsigned short int a );

/*remove the top element from the stack and write it into <a>; empty stack = error*/
int pop( unsigned short int a );

/*set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise*/
int eq( unsigned short int a, const unsigned short int b, const unsigned short int c );
/*
set: 1 a b
  set register <a> to the value of <b>
push: 2 a
  push <a> onto the stack
pop: 3 a
  remove the top element from the stack and write it into <a>; empty stack = error
eq: 4 a b c
  set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise
gt: 5 a b c
  set <a> to 1 if <b> is greater than <c>; set it to 0 otherwise
jmp: 6 a
  jump to <a>
jt: 7 a b
  if <a> is nonzero, jump to <b>
jf: 8 a b
  if <a> is zero, jump to <b>
add: 9 a b c
  assign into <a> the sum of <b> and <c> (modulo 32768)
mult: 10 a b c
  store into <a> the product of <b> and <c> (modulo 32768)
mod: 11 a b c
  store into <a> the remainder of <b> divided by <c>
and: 12 a b c
  stores into <a> the bitwise and of <b> and <c>
or: 13 a b c
  stores into <a> the bitwise or of <b> and <c>
not: 14 a b
  stores 15-bit bitwise inverse of <b> in <a>
rmem: 15 a b
  read memory at address <b> and write it to <a>
wmem: 16 a b
  write the value from <b> into memory at address <a>
call: 17 a
  write the address of the next instruction to the stack and jump to <a>
ret: 18
  remove the top element from the stack and jump to it; empty stack = halt
out: 19 a
  write the character represented by ascii code <a> to the terminal
in: 20 a
  read a character from the terminal and write its ascii code to <a>; it can be assumed that once input starts, it will continue until a newline is encountered; this means that you can safely read whole lines from the keyboard and trust that they will be fully read
noop: 21
*/

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
		if( numRead == WORDSIZE ) {
			fprintf( outfile, "%4d: ", ++line );
			fprintf( outfile, "opcode = %d ", opcode );
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
		} else {
			stop = 1;
		}
	}

	return 0;
}
