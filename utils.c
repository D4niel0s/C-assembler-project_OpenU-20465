#ifndef UTILS_C
#define UTILS_C

#include "utils.h"

/*boolean function to determine wether you should ignore a line or not*/
boolean ignore(char *line){
    char cpy[MAX_LINE] = {' '};
    strcpy(cpy, skipSpace(line));
    if(*cpy == '\0' || *cpy == '\n' || *cpy == COMMENT_CHAR){
        return true;
    }
    return false;
}

/*gets a string with spaces in it's beginning and returns a string without them*/
char *skipSpace(char *p){
    if(p==NULL){return p;}
    while(isspace(*p)){
        p++;
    }
    return p;
}

/*gets a string and skips a single word until a space*/
char *skipWord(char *p){
    if(p==NULL){return p;}
    while(!isspace(*p)){
        p++;
    }
    return p;
}


/*a function that returns the 1st word of a string (also a string)*/
char *get1stW(char *p){
    char q[MAX_LINE] = {' '};
    if(p == NULL){
        return NULL;
    }
    else{
        strcpy(q,p);
        return strtok(q," \t\n\0");
    }
}


#endif