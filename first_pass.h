#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "lib_include.h"

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

boolean first_pass(FILE *);

#endif