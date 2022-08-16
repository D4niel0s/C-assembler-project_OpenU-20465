assembler.o: assembler.c assembler.h 
	gcc -ansi -Wall -pedantic -c assembler.c
	
second_pass.o: second_pass.c second_pass.h utils.h structs.h
	gcc -ansi -Wall -pedantic -c second_pass.c

first_pass.o: first_pass.c first_pass.h utils.h structs.h
	gcc -ansi -Wall -pedantic -c first_pass.c

pre_pro.o: pre_pro.c pre_pro.h definitions.h utils.h structs.h 
	gcc -ansi -Wall -pedantic -c pre_pro.c

utils.o: utils.c utils.h definitions.h structs.h
	gcc -ansi -Wall -pedantic -c utils.c