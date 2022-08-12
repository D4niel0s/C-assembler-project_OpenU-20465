#ifndef PRE_PRO_H
#define PRE_PRO_H

#include "lib_include.h"

boolean pre_pro(FILE *,FILE *);
char *getMacName(char *);
boolean isComment(char *);
boolean isMacro(macro *[],char *, int);
boolean replaceMacro(FILE *, macro *[], char *);
void freeList(macro **list[], int);

#endif