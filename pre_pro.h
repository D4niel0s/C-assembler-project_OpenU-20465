#ifndef PRE_PRO_H
#define PRE_PRO_H

#include "lib_include.h"
#include "utils.h"
#include "structs.h"

boolean pre_pro(FILE *,FILE *);
char *getMacName(char *);
boolean isComment(char *);
boolean isMacro(macro [],char *,int);
boolean replaceMacro(FILE *, macro [], char *);

#endif