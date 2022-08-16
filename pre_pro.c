#ifndef PRE_PRO_C
#define PRE_PRO_C

#include "pre_pro.h"

/*we get a line with a bigger size than max size, to check if we dont have too long lines*/
#define LINE_CHECK_LEN 100

int main(){
    return 1;
}

boolean pre_pro(FILE *in,FILE *out){

    macro *macList[MEM_CAP] = { NULL };

    int lineNum = 0;
    int macroCounter = 0;
    boolean macFlag = false;

    char *buf = (char *)(malloc(LINE_CHECK_LEN));
    char *word = (char *)(malloc(MAX_LINE));

    while(fgets(buf,LINE_CHECK_LEN,in) != NULL){
        lineNum++;
        if(strlen(buf) <= MAX_LINE){
            if(!ignore(buf)){
                strcpy(word, get1stW(buf));
                
                if(isMacro(macList,word,macroCounter) ){
                    replaceMacro(out, macList, word);
                }else if(strcmp(word,"macro") == 0){
                    if(!isReserved(word)){
                        strcpy(word, getMacName(buf));
                        macroCounter++;
                        macList[macroCounter-1] = (macro *)malloc(sizeof(macro));
                        strcpy(macList[macroCounter-1] -> name , word);

                        macFlag = true;
                        while(macFlag){
                            fgets(buf,LINE_CHECK_LEN,in);

                            if(strlen(buf) > MAX_LINE){
                                printf("error in line %d: line is too long\n",lineNum);
                                freeList((macro ***)&macList,macroCounter);
                                free(buf);
                                free(word);
                                return false;
                            }

                            if(ignore(buf)){
                                fprintf(out,"%s",buf);
                                continue;
                            }
                            strcpy(word,get1stW(buf));

                            if(strcmp(word,"endmacro") == 0){
                                macFlag = false;
                            }else{
                                strcat(macList[macroCounter-1]->data, skipSpace(buf));
                            }
                        }
                    }
                    else{
                        printf("error in line %d: macro name cannot be a reserved word\n", lineNum);
                        freeList((macro ***)&macList,macroCounter);
                        free(buf);
                        free(word);
                        return false;
                    }

                }else{
                    fprintf(out,"%s",buf);
                }

            }else{
                fprintf(out,"%s",buf);
            }

        }else{
            printf("error in line %d: line is too long\n",lineNum);
            freeList((macro ***)&macList,macroCounter);
            free(buf);
            free(word);
            return false;
        }
    }
    freeList((macro ***)&macList,macroCounter);
    free(buf);
    free(word);
    return true;
}


/*gets a macro's name from the format "macro <macName>" */
char *getMacName(char *line){
    line = strtok(line, " ");
    return strtok(NULL, " \n\0");
}

/*Function to check wether a macro already exists with input name*/
boolean isMacro(macro *list[],char *nameCheck,int len){
    int i;
    for(i=0;i<len;i++){
        if(strcmp((list)[i]->name, nameCheck) == 0){
            return true;
        }
    }
    return false;
}

boolean replaceMacro(FILE *outf, macro *list[], char *nameCheck){
    int i;
    for(i=0;i<100;i++){
        if(strcmp(list[i]->name, nameCheck) == 0){
            break;
        }
    }
    if(strcmp(list[i]->name, nameCheck) == 0){
        fprintf(outf,"%s",list[i]->data);
        return true;
    }else{
        return false;
    }
}

void freeList(macro *(*list[100]), int len){
    int i;
    for(i=0;i<len;i++){
        free((*list)[i]);
    }
}

#endif
