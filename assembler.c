#ifndef ASSEMBLER_MAIN
#define ASSEMBLER_MAIN

#include "assembler.h"

int main(int argc, char **argv){
    label labelTab[MAX_LABELS_COUNT];  
    codeImg codeImage[500];
    /*codeImg codeImage[500]; data image*/
    extNode extList[MAX_LABELS_COUNT];
    entNode entList[MAX_LABELS_COUNT];

    pre_pro(infile,afterM);
    first_pass(infile,labelTab,codeImage,dataImage,extList,entList);
    second_pass(infile,extfile,entfile,binfile,labelTab,codeImage,dataImage,extList,entList);
}

#endif