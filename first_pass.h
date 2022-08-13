#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "lib_include.h"

boolean first_pass(FILE *infile, label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[]);
int isLabelDef(char *);
char *rmColon(char *);
boolean isalnumStr(char *);
boolean isLegalLabel(char *);
boolean isReserved(char *);
opcode getCommandNum(char *);
reg getRegNum(char *);
instruction getInstType(char *);
boolean isDefinedLabel(char *p, label list[]);
int getOperandNum(opcode );
boolean isNumber(char *);
int getAdressingType(char *);
boolean checkAddrWithOpcode(opcode , addressing_type, addressing_type);
boolean writeToCodeImage(opcode,char *,char *,addressing_type ,addressing_type, codeImg [], int *);
boolean CodeToWords(char *, codeImg[], int *);
boolean dataToWords(char *line, dataImg dataImage[], int *DC);
boolean translateStruct(char *line, dataImg dataImage[],int *DC);
boolean translateString(char *line, dataImg dataImage[], int *DC);
boolean translateData(char *input, dataImg dataImage[], int *DC);
boolean isLegalNumberWithSpaces(char *check);

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

#endif