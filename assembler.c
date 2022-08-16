#ifndef ASSEMBLER_MAIN
#define ASSEMBLER_MAIN

#include "assembler.h"

int main(int argc, char **argv){
    /*all possible FILE pointers*/
    FILE *ASfile;
    FILE *AMfile;
    FILE *EXTfile;
    FILE *ENTfile;
    FILE *OBfile;

    char fileName[MAX_LINE] = {' '};

    /*label table initialization*/
    label labelTab[MEM_CAP] = { {"\0",0,false} };

    /*entry and extern lists initialization*/
    extNode extList[MEM_CAP] = { {"\0",{0},0} };
    entNode entList[MEM_CAP] = { {"\0"} };

    /*code and data image initialization*/
    codeImg codeImage[MEM_CAP] = { {0,0,"\0"} };
    dataImg dataImage[MEM_CAP] = { {0,0} };

    int i;

    for(i=0;i<argc;i++){
        
    }
}

#endif