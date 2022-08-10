#ifndef STRUCTS_H
#define STRUCTS_H

#include "definitions.h"

/*Definition of a label node struct*/
typedef struct LabelNode *labelPtr;
typedef struct LabelNode{
    char *name; /*label name*/
    unsigned int address; /*label definition address*/
    instruction labelInst; /*type of label instruction (enum defined at definitions.h)*/
}label;

/*Definition of a macro node struct*/
typedef struct MacroNode *macPtr;
typedef struct MacroNode{
    char *name; /*macro name*/
    char *data; /*macro content*/
}macro;

/** Represents a single data word. */
typedef struct data_word {
	/* The data content itself (a method for putting data into these field is defined) */
	unsigned long data;
} data_word;

typedef struct code_word {
	/*opcode for first word*/
	unsigned int opcode: 4;
	/*destination and source addressing methods for first word*/
	unsigned int src_addressing: 2;
	unsigned int dest_addressing: 2;
	/*source and destination register numbers in case we have registers as operands (second/third/fourth word)*/
	unsigned int src_register: 4;
	unsigned int dest_register: 4;
	/*the number in case we get an immediate addressing type operand, also used as field number with struct operand*/
	unsigned int immediateNum: 8;
	/*A,R,E (in every word)*/
	unsigned int ARE: 2;
	/*name of a used label to plug inside the translated code, and changed to address in second pass
	* used only if we have a label operand and is in second/third/fourth word*/
	char labelName[MAX_LABEL_NAME];
}code_word;

typedef struct codeImage{
	int lineNum;
	int content;
	char *labName;
}codeImg;

typedef struct inst_lookup {
	instruction name;
	char *value;
}inst_lookup_item;

#endif