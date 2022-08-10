#ifndef PRE_PRO_C
#define PRE_PRO_C

#include "pre_pro.h"

int main(){
    FILE *input = fopen("infile.txt","r+");
    FILE *output = fopen("outfile.txt","w+");
    pre_pro(input,output);
    fclose(input);
    fclose(output);
    return 1;
}

boolean pre_pro(FILE *in,FILE *out){

    macro *macList[100] = { NULL };

    int macroCounter = 0;
    boolean macFlag = false;

    char *buf = (char *)(malloc(100));
    char *word = (char *)(malloc(MAX_LINE));
    printf("CREATED VARS\n");
    while( fgets(buf,100,in) != NULL){
        printf("GOT LINE, %s\n",buf);
        if(strlen(buf) <= MAX_LINE){
            printf("LINE OK\n");
            if(!ignore(buf)){
                printf("LINE NOT IGNORED\n");
                printf("word is IM HERE, %s\n",word);
                printf("getting 1st word\n");
                strcpy(word, get1stW(buf));
                printf("GOT 1st WORD, %s\n",word);
                if( isMacro(macList,word,macroCounter) ){
                    printf("IS A MACRO, %s\n",word);
                    replaceMacro(out, macList, word);
                    printf("REPLACED!\n");
                }else if(strcmp(word,"macro") == 0){
                    strcpy(word, getMacName(buf));
                    printf("NEW MACRO DEFINITION!, %s\n", word);
                    macroCounter++;
                    macList[macroCounter-1] = (macro *)malloc(sizeof(macro));
                    macList[macroCounter-1] -> name = (char *)malloc(MAX_LABEL_NAME);
                    macList[macroCounter-1] -> data = (char *)malloc(MAX_LINE*10);
                    strcpy(macList[macroCounter-1] -> name , word);
                    printf("ADDED NEW MACRO TO LIST! name's, %s\n",macList[macroCounter-1]->name);
                    macFlag = true;
                    while(macFlag){
                        fgets(buf,100,in);
                        printf("got another line!, %s\n",buf);
                        if(strlen(buf) > MAX_LINE){
                            printf("%s","*****Error in pre-proccessing: line is too long*****");
                            freeList(&macList,macroCounter);
                            free(buf);
                            free(word);
                            return false;
                        }
                        if(ignore(buf)){
                            fprintf(out,"%s",buf);
                            continue;
                        }
                        printf("NOT IGNORESSSSSS\n");
                        strcpy(word,get1stW(buf));
                        printf("1st WORD IS, %s\n",word);
                        if(strcmp(word,"endmacro") == 0){
                            macFlag = false;
                            printf("macflag - false\n");
                        }else{
                            strcat(macList[macroCounter-1]->data, buf);
                            printf("added content\n");
                        }
                    }

                }else{
                    printf("IN ELSE\n");
                    fprintf(out,"%s",buf);
                    printf("printed\n");
                }
            }else{
                if(!isComment(buf))
                fprintf(out,"%s",buf);
            }
        }else{
            printf("%s","*****Error in pre-proccessing: line is too long*****");
            freeList(&macList,macroCounter);
            free(buf);
            free(word);
            return false;
        }
    }
    freeList(&macList,macroCounter);
    free(buf);
    free(word);
    return true;
}


/*gets a macro's name from the format "macro <macName>" */
char *getMacName(char *line){
    line = strtok(line, " ");
    return strtok(NULL, " \n\0");
}

boolean isComment(char *buf){
    return *buf == ';';
}

/*Function to check wether a macro already exists with input name*/
boolean isMacro(macro *list[],char *nameCheck,int len){
    int i;
    for(i=0;i<len;i++){
        printf("inside of ismacro, name is:%ss\n",list[i]->name);
        printf("inside of ismacro, name to compare is:%ss\n",nameCheck);
        if(strcmp((list)[i]->name, nameCheck) == 0){
            return true;
        }
    }
    printf("ISAFTER LOOP\n");
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