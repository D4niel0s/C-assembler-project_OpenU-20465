# C assembler project - Open University semester 2022b 

* This project was made as a final project to the C lab course at the open university (course No.20465).
* This project is an assembler for an imaginary assembly language on an imaginary machine.

## Compiling and running the project
 
 **To compile** this project, use the included makefile. type the command:
 > make
 
 In the terminal at the current directory. (should include all project files)
 
 **To run** this project, type in the following command in your terminal:
 > ./assembler file1 file2
 
* You may enter as many files as you like.
 
* You must only type the name of the file, without it's format.
There must exist a file with this name in the current directory.

**Note:** this project compies using the GCC compiler, and is written in c90 standard, this project is meant to run on a linux environment.

### Output

The program outputs files as following:

* `file.am` - contains file contents after macro expansion.
* `file.ob` - contains machine code according to the imaginary machine and the project's instructions. (Described below)
* `file.ext` - contains all addresses of uses of external lables. (External labels must be declared as extern)
* `file.ent` - contains the addresses of entry label's definitions. (Entry labels must be declared as entry)

**Note:** the `.ext` and `.ent` files will not be generated if there are no extern or entry labels. (Accordingly)


## Imaginary machine specs

 * 8 registers. (r0,r1, ... ,r7) (PSW included)
 * Each word is 10 bits long.
 * Memory of 256 words.
 * Supports whole numbers. (Includes negative numbers)
 * Uses 2's complement in arithmetics.
 * Supports characters. (Represented by their ASCII value)
 
 ### Structure of a code word
 
 |Bits 9-6 | Bits 5-4 | Bits 3-2 | Bits 1-0|
 |-------|----------------------|----------------------|------|
 |Opcode | Addressing type No.1 | Addressing type No.2 | A,R,E|
 
 * Each command has it's unique opcode:
    |Command name| Opcode (decimal)| Description|
    |-----|----|----------------------------------------------------------------|
    | mov | 0 |  copies value from argument 1 to argument 2.|
    | cmp | 1 |  compares two arguments, answer stored in PSW.|
    | add | 2 |  adds value of two arguments, stores answer in 2nd argument.|
    | sub | 3 |  subtracts value of two arguments, stores answer in 2nd argument.|
    | not | 4 |  invert all operand bits.|
    | clr | 5 |  clear all operand bits.|
    | lea | 6 |  stores the address of argument 1 inside argumet 2.|
    | inc | 7 |  increments operand by one.|
    | dec | 8 |  decrements operand by one.|
    | jmp | 9 |  program will run at the address of the operand.|
    | bne | 10 | program will run at address of operand if compare flag in PSW is 0.|
    | get | 11 | gets character from standard input.|
    | prn | 12 | puts character in standard output.|
    | jsr | 13 | calls to start a sub-routine at the address of the operand.|
    | rts | 14 | return to main flow from sub-routine.|
    | hlt | 15 | stop the program run.|
 --------------------------
 * Addressing types of arguments:
    * Immediate - the operand is a number. (Numbers are whole, and can contain a plus or a minus) (A number in an instruction call must start with a #)
    * Direct - the operand is a label.
    * Register - the operand is a register. 
    * Struct - the operand is a label (struct type), and a field number (1 or 2).
 -------------------------
 * A,R,E field describes the type of encoding for the CPU:
    * A - Absolute encoding; used for immediate addressing arguments, as well as commands and registers.
    * R - Relocatable encoding; used for labels from the source file.
    * E - External encoding; used for labels from other files.
    
 
