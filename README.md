# C assembler project - Open University semester 2022b 

* This project was made as a final project to the C lab course at the open university (course No.20465).
* This project got a final grade of 100 points.
* This project is an assembler for an imaginary assembly language on an imaginary machine.

## Compiling and running the project
 
 **To compile** this project, use the included makefile. type the command:
 > make
 
 In the terminal at the current directory. (should include all project files)
 
 **To run** this project, type in the following command in your terminal:
 > ./assembler file1 file2
 
* You may enter as many files as you like.
 
* You must only type the name of the file, without it's format.
There must exist a file with this name in the current directory. (.as file extension)

**Note:** This project compies using the GCC compiler, and is written in c90 standard, this project is meant to run on a linux environment.

## Imaginary machine specs

 * 8 registers. (r0,r1, ... ,r7) (PSW included)
 * Each word is 10 bits long.
 * Memory of 256 words.
 * Supports whole numbers. (Includes negative numbers)
 * Uses 2's complement in arithmetics.
 * Supports characters. (Represented by their ASCII value)
 
 The base of the machine code is 32, meaning there are 32 symbols in our machine code.
 
 This table represents the symbol for each decimal value above it:
 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 |20|21|22|23|24|25|26|27|28|29|30|31|
 |--|----|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|--|--|--|--|--|--|--|--|--|--|--|--|
 | ! | @ | # | $ | % | ^ | & | * | < | > | a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v |
 
 ## Structure of a code word
 
 |Bits 9-6 | Bits 5-4 | Bits 3-2 | Bits 1-0|
 |-------|----------------------|----------------------|------|
 |Opcode | Addressing type No.1 | Addressing type No.2 | A,R,E|
 
 * Each command has it's unique opcode:
    |Command name| Opcode (decimal)| Description|
    |-----|----|----------------------------------------------------------------|
    | mov | 0 |  Copies value from argument 1 to argument 2.|
    | cmp | 1 |  Compares two arguments, answer stored in PSW.|
    | add | 2 |  Adds value of two arguments, stores answer in 2nd argument.|
    | sub | 3 |  Subtracts value of two arguments, stores answer in 2nd argument.|
    | not | 4 |  Invert all operand bits.|
    | clr | 5 |  Clear all operand bits.|
    | lea | 6 |  Stores the address of argument 1 inside argumet 2.|
    | inc | 7 |  Increments operand by one.|
    | dec | 8 |  Decrements operand by one.|
    | jmp | 9 |  Program will run at the address of the operand.|
    | bne | 10 | Program will run at address of operand if compare flag in PSW is 0.|
    | get | 11 | Gets character from standard input.|
    | prn | 12 | Puts character in standard output.|
    | jsr | 13 | Calls to start a sub-routine at the address of the operand.|
    | rts | 14 | Return to main flow from sub-routine.|
    | hlt | 15 | Stop the program run.|
 --------------------------
 * Addressing types of arguments:
    | Name | Number (decimal) | description |
    |------|------------------|-------------|
    |Immediate | 0 | The operand is a number. (Numbers are whole, and can contain a plus or a minus) (A number in an instruction call must start with a #)|
    | Direct | 1 | The operand is a label.|
    | Struct | 2 | The operand is a label (struct type), and a field number (1 or 2).|
    | Register | 3 | The operand is a register. |
    
 -------------------------
 * A,R,E field describes the type of encoding for the CPU:
    | Symbol | Meaning | Use cases |
    |--------|---------|-----------|
    | A | Absolute encoding | used for immediate addressing arguments, as well as commands and registers.
    | R | Relocatable encoding | used for labels from the source file.
    | E | External encoding | used for labels from other files.
    
 ## The assembly language
 
 As mentioned before, this is an imaginary language.
 
 This language has two types of sentences:
 
  * Commands. (All commands listed above)
  * Instructions:
      * `.data` - Stores an array of **whole** numbers. (Supports negative numbers)
      * `.string` - Stores a string. (starts and ends with quotation marks)
      * `.struct` - Stores a struct with two fields, 1st field is a number, 2nd field is a string (Rules for both are the same as above)
      * `.entry` - Marks a certain label as entry, meaning it can be used in other files when imported.
      * `.extern` - Marks a certain label as extern, meaning it came from another file.
  * Lines that start with `;` are comments and will be ignored
  * There is support for blank lines

## The output


The program outputs files as following:

* `file.am` - contains file contents after macro expansion.
* `file.ob` - contains machine code according to the imaginary machine and the project's instructions. (Described below)
* `file.ext` - contains all addresses of uses of external lables. (External labels must be declared as extern)
* `file.ent` - contains the addresses of entry label's definitions. (Entry labels must be declared as entry)

**Note:** the `.ext` and `.ent` files will not be generated if there are no extern or entry labels. (Accordingly)

-------------------------------

The `.ob` file is written in the following format:

> {Code Encoded Length} {Data Encoded Length}
> 
> {Address 1} {Encoding of first word}
> 
> {Address 2} {Encoding of second word}

**Notes:** 
 * First address (Accorfing to the project's instructions) is always 100.
 * The length of encoded Code and Data is the number of words that the encoding of each takes up in memory.
 * The object file contains encoding of commands first, and data afterwards. (Separation of Data and Code)

-------------------------------

The `.ext` file is written in the following format:

> {Label 1} {Use address 1}
>
> {Label 1} {Use address 2}

**Notes:**
 * For each label, all uses will be written.
 * If labels are marked with extern and not used, the `.ext` file will be empty

-------------------------------

The `.ent` file is written in the following format:

> {Label 1} {Declaration address}
> 
> {Label 2} {Declaration address}

Each label has it's declaration address; where it's declared.
