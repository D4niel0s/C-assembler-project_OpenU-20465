#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "lib_include.h"
#include "pre_pro.h"
#include "first_pass.h"
#include "second_pass.h"
#include "utils.h"
#include "structs.h"

int main(int argc, char **argv);

/*generates a file name using origin and a given extension*/
void generateFileName(char *origin, extension);

#endif