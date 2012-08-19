vm: vm.c
	gcc -ggdb -Wall -ansi -o bin/vm vm.c

disassembler: disassembler.c
	gcc -ggdb -Wall -ansi -o bin/disassembler disassembler.c
