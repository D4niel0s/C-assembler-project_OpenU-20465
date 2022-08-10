#ifndef PRE_PRO_H
#define PRE_PRO_H

#include "lib_include.h"

boolean pre_pro(FILE *,FILE *);
int lineLen(char *);
boolean ignore(char *);
char *skipSpace(char *);
char *get1stW(char *);
char *getMacName(char *);
boolean isComment(char *);
boolean isMacro(macro *[],char *, int);
boolean replaceMacro(FILE *, macro *[], char *);
void addMacro(macro *(*[100]),char *);
void freeList(macro *(*list[100]), int);

#endif