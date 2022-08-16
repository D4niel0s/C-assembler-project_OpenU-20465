#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "lib_include.h"


#define TOTAL_COMMAND_NUM 16
#define TOTAL_REGISTER_NUM 8
#define TOTAL_INSTRUCTION_NUM 5


/*all function prototypes - first pass*/

/*main logic of first pass:
* translates every line (that needs to be translated) into code/data image,
* stores lables to label table, 
* stores extern/entry defined labels to according arrays, 
* updates IC and DC to correspond with instruction and data count*/
boolean first_pass(FILE *infile, label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[], int * IC,int *DC);

/*check if a string represents a legal number / is it all constructed of alpha-numeral characters*/
boolean isNumber(char *);
boolean isalnumStr(char *);

/*getting instruction/register/command number of a given word (used to understand specifications about our translation)*/
instruction getInstType(char *);
reg getRegNum(char *);
opcode getCommandNum(char *);

/*check legality of a word (legal for label, not reserved etc.)*/
int isLabelDef(char *);
boolean isLegalLabel(char *);
boolean isReserved(char *);
boolean isDefinedLabel(char *p, label list[]);

/*getting various information from opcode and operands (operand number, addressing types etc.)*/
int getOperandNum(opcode);
int getAdressingType(char *);
boolean checkAddrWithOpcode(opcode , addressing_type, addressing_type);

/*parsing, translating, and writing to code image*/
boolean CodeToWords(char *, codeImg[], int *, int);
boolean writeToCodeImage(opcode,char *,char *,addressing_type ,addressing_type, codeImg [], int *,int);

/*parsing, translating, and writing to data image*/
boolean dataToWords(char *, dataImg [], int *, int);
boolean translateStruct(char *, dataImg [],int *, int);
boolean translateString(char *, dataImg [], int *, int);
boolean translateData(char *, dataImg [], int *, int);

/*end of function prototypes*/

/*static data structures, used to find number of command/register/instruction*/
static char *commandArray[16] = {
	"mov",
	"cmp",
	"add",
	"sub",
	"not",
	"clr",
	"lea",
	"inc",
	"dec",
	"jmp",
	"bne",
	"get",
	"prn",
	"jsr",
	"rts",
	"hlt"
};

static char *registerArray[8] = {
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7"
};

static char *instArray[5] = {
	".data",
	".string",
	".struct",
	".extern",
	".entry"
};
/*end of static data structures*/

#endif