#ifndef FIRST_PASS_C
#define FIRST_PASS_C

#include "first_pass.h"


boolean first_pass(FILE *infile, FILE *extFile, FILE *entFile, FILE *binOut){

    int IC = 100;
    int DC = 0;
    label *labelTab[100] = { NULL };
    codeImg commandImage[500] = {[0 ... 499] = {0,0,NULL}};
    int currentLabIndex = -1;
    char currentLabName[MAX_LABEL_NAME+1] = {' '};

    int labelCounter = 0;
    boolean labelDef  = false;

    char *buf = (char *)(malloc(MAX_LINE));
    char *word = (char *)(malloc(MAX_LINE));

    opcode currentOpcode;
    instruction currentInst;
    reg currentReg;

    while(fgets(buf,MAX_LINE,infile) != NULL){
        if(!ignore(buf)){
            strcpy(word,get1stW(buf));
            
            if(isLabelDef(word)){
                    if(isDefinedLabel(word,labelTab) != 2){
                        labelDef = true;
                        strcpy(currentLabName,word);
                    }else{
                        printf("********error in first pass, label declared twice: ,%s********",word);
                        return false;
                    }
                }else{
                    printf("********error in first pass, illegal label name: %s********",word);
                    return false;
                }
            
            /*if a label is declared in this line, we have a line that looks like this:
            LABEL: <value>
            so we need to skip to the start of the value*/
            if(labelDef){
                strcpy(buf,skipWord(buf));
                strcpy(buf,skipSpace(buf));
            }
            /*mov r1,r2*/
            /*.data r1,r2*/
            strcpy(word,get1stW(buf));

            currentOpcode = getCommandNum(word);
            currentInst = getInstType(word);

            if(currentInst != NONE_INST){
                if(currentInst == EXTERN_INST || currentInst == ENTRY_INST){
                    if(labelDef){
                        printf("*WARNING: label definition includes .extern or .entry, label %s ignored*",currentLabName);
                    }
                }else{ /*the instruction is of type .data, .string or .struct*/
                    convertData();
                }
                
            }else if(currentOpcode != NONE_OP){
                checkLegalCodeLine(buf);
                convertCodeToBin(buf);
            }else{
                printf("********invalid expression: %s********",word);
                return false;
            }
            
            labelDef = false;
            strcpy(currentLabName,"");
            currentInst = NONE_INST;
            currentOpcode = NONE_OP;
            currentReg = NONE_REG;
            currentLabIndex = -1;
        }
    }
}

/*returns if a given string represents a legal label declaration*/
boolean isLabelDef(char *p){
    /*if the last character of a string is a colon, it is eligible to be a label definition*/
    /*we then need to check if a label is legal or not (alpha-numeric, starts with a letter etc.)*/
    if(p[strlen(p)-1] == ':' && isLegalLabel(p)){
        return true;
    }
    return false;
}

/*recieves a string and returns it without the colon, used to get the name of the label out of it's declaration*/
char *rmColon(char *p){
    char cpy[MAX_LINE] ={' '};
    strcpy(cpy,p);
    return strtok(cpy,":");
}

/*checks if a string consists only out of alpha-numeric characters*/
boolean isalnumStr(char *p){
    int i;
    for(i=0;i<strlen(p);i++){
        /*if a character inside the string is not alpha-numeric, we return false*/
        if(!isalnum(p[i])){
            return false;
        }
    }
    return true;
}

/*checks for a given string, if it fits for a label name (alpha-numeric,starts with a letter, no longer than 30 characters, not reserved)*/
boolean isLegalLabel(char *labCheck){
    if(isalpha(labCheck[0]) && !isReserved(labCheck) && isalnumStr(labCheck) && strlen(labCheck)<=MAX_LABEL_NAME){
        return true;
    }
    return false;
}

/*checks for a given string, if it represents a reserved word (eg: command,register, etc.)*/
boolean isReserved(char *p){
    if(getCommandNum(p) == NONE_OP && getRegNum(p) == NONE_REG && getInstType(p) == NONE_INST){
        return false;
    }
    return true;
}

/*gets the command num (opcode) of a given string, if not found, returns NONE_OP*/
opcode getCommandNum(char *p){
    int i;
    for(i=0;i<16;i++){
        if(strcmp(commandArray[i],p) == 0){
            return i;
        }
    }
    return NONE_OP;
}

/*gets the register number of a given string, if not found, returns NONE_REG*/
reg getRegNum(char *p){
    int i;
    for(i=0;i<8;i++){
        if(strcmp(registerArray[i],p) == 0){
            return i;
        }
    }
    return NONE_REG;
}

/*gets the instruction type of a given string (eg: .data, .string etc.), if not found, returns NONE_INST*/
instruction getInstType(char *p){
    int i;
    for(i=0;i<5;i++){
        if(strcmp(instArray[i],p) == 0){
            return i;
        }
    }
    return NONE_INST;
}

/*checks for a given string and returns:
* 0: if a label with this name does not exist
* 1: if a label with this name exists but does not have an address yet
* 2: if a label with this name exists and has an address*/
int isDefinedLabel(char *p, label *list[]){
    int i;
    for(i=0;i<100;i++){
        if(list[i] == NULL){
            return 0; /*every time we encounter a new label we add it to the end of the list, so if we reach a NULL value, it means we checked all labels*/
        }
        if(strcmp(list[i]->name,p) == 0){
            if(list[i]->address == UNASSIGNED){
                return 1;
            }
            else{
                return 2;
            }
        }
    }
    return 0;
}

/*for a goven string, returns the index of the label corresponding to this name, if not found, returns -1*/
int getLabelIndex(char *p, label *list[]){
    int i;
    for(i=0;i<100;i++){
        if(list[i] == NULL){
            return -1; /*iterated through the whole list and not found*/
        }
        if(strcmp(list[i]->name,p) == 0){
            return i;
        }
    }
    return -1; /*iterated through the whole list and not found*/
}

/*gets an operation and returns the amount of operands the operation takes, if not a legal operation, returns -1*/
int getOperandNum(opcode operation){
    switch(operation){
        /*two operand group*/
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP: 
            return 2;

        /*one operand group*/
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case GET_OP:
        case PRN_OP:
        case JSR_OP:
            return 1;

        /*0 operand group*/
        case RTS_OP:
        case HLT_OP:
            return 0;

        /*default case for a non-operation input*/
        default: 
            return -1;
    }
}

/*checks if a given string is a legal number, legal numbers can start with either a plus or a minus, and can only contain digits (no decimal point etc.)*/
boolean isNumber(char *numCheck){
    int i = 0;
    if(numCheck[0] == '+' || numCheck[0] == '-'){ /*if a number starts with a plus or a minus its ok so we start from index 1*/
        i = 1;
    }
    for(;i<strlen(numCheck);i++){
        if(!isdigit(numCheck[i])){
            return false; /*found a non digit character inside the number*/
        }
    }
    return true; /*iterated through the whole string and evrything is OK*/
}

/*gets a string representing an operand, and returns it's addressing type according to the addressing type instructions*/
int getAdressingType(char *operand){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};

    strcpy(cpy,operand);

    if(cpy[0] == '#'){
        if(strchr(cpy,'#') != strrchr(cpy,'#')){ /*if we have two hastags, strtok won't recognize it, so we check if we have hashtags at two differrent locations*/
            return NONE_ADDR;
        }
       strcpy(word, strtok(cpy,"#"));
       /*word is now the token after '#', we need to check if it's a legal number, and there are no characters after it*/
       if(isNumber(word)){
            return IMMEDIATE_ADDR;
       }else{
            return NONE_ADDR;
       }
    }
    if(isLegalLabel(cpy)){ /*if the operand is a legal label, we assume it is a label (the character '.' makes a string not an ok label name, so we know it is not a struct)*/
        return DIRECT_ADDR;
    }

    if(getRegNum(cpy) != NONE_REG){ /*is a register*/
        return REGISTER_ADDR;
    }

    if(strchr(cpy,'.') != NULL && cpy[0] != '.'){ /*includes a point and does not start with a point (starting with a point makes strtok skip it)*/
        strcpy(word,strtok(cpy,"."));
        if(isLegalLabel(word)){ /*if the first part is an ok label name its ok*/
            strcpy(word,strtok(NULL," \t\n\0")); /*go to the next token by space (the next value after the point)*/
            if(strlen(word) == 1 && (word[0] == '1' || word[0] == '2')){ /*if the value after the label is 1 or 2 its ok and represents a struct*/
                return STRUCT_ADDR;
            }
        }
    }
    return NONE_ADDR;
}

boolean checkAddrWithOpcode(opcode operation, addressing_type addr1, addressing_type addr2){
    int operandNum = getOperandNum(operation);

    switch(operation){
        /*two operand group*/
        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
            if((addr1 != NONE_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;

        case CMP_OP:
            if((addr1 != NONE_ADDR) && (addr2 != NONE_ADDR)){
               return true; 
            }
            return false;

        case LEA_OP:
            if((addr1 == DIRECT_ADDR || addr1 == STRUCT_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;
            
        /*one operand group*/
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case GET_OP:
        case JSR_OP:
            if((addr1 == NONE_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;

        case PRN_OP:
            if((addr1 == NONE_ADDR) && (addr2 != NONE_ADDR)){
               return true; 
            }
            return false;
            
        /*0 operand group*/
        case RTS_OP:
        case HLT_OP:
            if((addr1 == NONE_ADDR) && (addr2 == NONE_ADDR)){
               return true; 
            }
            return false;

        /*default case for a non-operation input*/
        default: 
            return false;
    }
}

/*writes to code image the translation of a single operation line*/
/*this function assumes that we got addressing types that match the operation (we run this function in translateCode after validation)*/
boolean writeToCodeImage(opcode thisOp,char *op1,char *op2,addressing_type addr1,addressing_type addr2, codeImg image[], int *IC){
    char Cop1[MAX_LINE] = {' '};
    char Cop2[MAX_LINE] = {' '};

    boolean doubleRegFlag = false; /*used in case we have two register operands*/
    int i = 0; /*used for accessing the i element of our code words array*/
    unsigned int ans = 0;
    /*output is maximum of 5 binary words, so we declare and initialize an array of 5 code_word elements*/
    code_word words[5] = {
        [0] = {0,0,0,0,0,0,0,{' '}},
        [1] = {0,0,0,0,0,0,0,{' '}},
        [2] = {0,0,0,0,0,0,0,{' '}},
        [3] = {0,0,0,0,0,0,0,{' '}},
        [4] = {0,0,0,0,0,0,0,{' '}},
    };

    strcpy(Cop1,op1);
    strcpy(Cop2,op2);

    /*storing first word: operation translation*/
    words[i].opcode = (unsigned int)thisOp;
    words[i].src_addressing = (unsigned int)addr1;
    words[i].dest_addressing = (unsigned int)addr2;
    words[i].ARE = 0;

    ans += words[i].opcode;
    ans <<= 2; /*shift to the left by the number of bits that the source addressing method takes*/
    ans += words[i].src_addressing;
    ans <<= 2; /*shift to the left by the number of bits that the destination addressing method takes*/
    ans += words[i].dest_addressing;
    ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
    ans+= words[i].ARE;

    image[(*IC)].content = ans;
    (*IC)++;
    ans = 0;

    i++; /*incrementing i to go to next word*/
    switch(addr1){
        /*meaning we got a number formatted like this: #<numValue>*/
        case IMMEDIATE_ADDR:
            Cop1[0] = ' '; /*replacing hashtag with space*/
            strcpy(Cop1,skipSpace(Cop1)); /*skipping that space to leave us with only the number*/
            /*we can store a value between -128 and 255 with 8 bits*/
            if(atoi(Cop1) > 255 || atoi(Cop1) < -128){
                printf("number is too big!"); /*if we got an illegal value, we exit*/
                return false; 
            }
            words[i].immediateNum = (unsigned)atoi(Cop1); /*getting number and storing it inside it's field*/
            words[i].ARE = 0; /*ARE is zero in this case*/

            ans += words[i].immediateNum;
            ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            break;

        /*meaning we got a label name in op1*/
        case DIRECT_ADDR:
            strcpy(image[(*IC)].labName, Cop1);
            (*IC)++;
            i++; /*incrementing i to go to next cell*/
            break;

        /*meaning we have a label name followed by a point and a field number (1/2)*/
        case STRUCT_ADDR:
            strcpy(words[i].labelName,strtok(Cop1,".")); /*copying label name to the labl name field*/
            strcpy(image[(*IC)].labName, words[i].labelName);
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            words[i].immediateNum = strcmp(strtok(NULL," "),"1") == 0? 1 : 2; /*getting field number*/
            words[i].ARE = 0; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            i++;
            (*IC)++;
            break;

        /*meaning we have a register name*/
        case REGISTER_ADDR:
            words[i].src_register = getRegNum(Cop1);
            words[i].ARE = 0;
            if(addr2 == REGISTER_ADDR){
                doubleRegFlag = true;
                words[i].dest_register = getRegNum(Cop2);

                ans += words[i].src_register;
                ans <<= 4; /*shift to the left by the number of bits that the destination register field takes*/
                ans += words[i].dest_register;
                ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
            }else{
                ans += words[i].src_register;
                ans <<= 6; /*shift to the left by the number of bits that the destination register field takes plus the ARE field (since dest register is zero/none)*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
            }
            i++; /*incrementing i to go to next cell*/
            break;

        case NONE_ADDR:
            break;
    }
    ans = 0;
    switch(addr2){
        case IMMEDIATE_ADDR:
            Cop2[0] = ' '; /*replacing hashtag with space*/
            strcpy(Cop2,skipSpace(Cop2)); /*skipping that space to leave us with only the number*/
            /*we can store a value between -128 and 255 with 8 bits*/
            if(atoi(Cop2) > 255 || atoi(Cop2) < -128){
                printf("number is too big!"); /*if we got an illegal value, we exit*/
                return false; 
            }
            words[i].immediateNum = (unsigned)atoi(Cop2); /*getting number and storing it inside it's field*/
            words[i].ARE = 0; /*ARE is zero in this case*/

            ans += words[i].immediateNum;
            ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            (*IC)++;
            break;

        case DIRECT_ADDR:
            strcpy(image[(*IC)].labName, Cop2);
            (*IC)++;
            break;

        case STRUCT_ADDR:
            strcpy(words[i].labelName,strtok(Cop2,".")); /*copying label name to the labl name field*/
            strcpy(image[(*IC)].labName,words[i].labelName);
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            words[i].immediateNum = strcmp(strtok(NULL," "),"1") == 0? 1 : 2; /*getting field number*/
            words[i].ARE = 0; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= 2; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            (*IC)++;
            break;

        case REGISTER_ADDR:
            if(doubleRegFlag == true){ /*we already stored this register number before*/
                break;
            }else{
                words[i].dest_register = getRegNum(op2);
                words[i].ARE = 0;

                ans += words[i].dest_register;
                ans <<= 2;
                ans += words[i].ARE;
                image[(*IC)].content = ans;
                break;
            }
        case NONE_ADDR:
            break;
    }
    return true;
}

boolean CodeToWords(char *line, codeImg codeImage[], int *IC){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '}; /*used to store the first word of the input, but also used as a copy of the input (since we want to alter it and not lose cpy)*/
    char *token; /*used to count how many times can we use strtok on the string (also used to save pointers)*/

    /*an operand of type struct is potentially the longest operand, which is a label+2 characters, so we make two operands (arrays) of that length*/
    char op1[MAX_LABEL_NAME+2] = {' '};
    char op2[MAX_LABEL_NAME+2] = {' '};

    int counter = 0; /*counter variable (how many times can we use strtok)*/
    int i; /*loop variable*/
    int operandNum; /*amount of operands the operation recieves*/
    opcode thisOpcode = NONE_OP; /*opcode*/
    addressing_type addr1 = NONE_ADDR; /*addressing type 1*/
    addressing_type addr2 = NONE_ADDR; /*addressing type*/

    strcpy(cpy,line);
    strcpy(word,get1stW(cpy));

    thisOpcode = getCommandNum(word);
    operandNum = getOperandNum(thisOpcode);

    if(thisOpcode == NONE_OP){
        printf("illegal function name!!!");
        return false;
    }
    
    strcpy(cpy,skipWord(cpy));
    strcpy(cpy,skipSpace(cpy));
    strcpy(word,cpy); /*copying cpy to word (to use strtok without altering origin)*/

    /*this next part extracts the operands (if there are any), checks the validity of the line construction (commas,num of operands etc.) and gets their addressing types*/
    if(operandNum == 2){
        if(strstr(cpy, ",,") != NULL){ /*if we have two commas in a row it's an error*/
            printf("ERROR: too many commas between operands");
            return false;
        }
        if(strchr(cpy,',') == NULL){ /*if we don't have commas at all, that is also an error*/
            printf("no comma between operands");
            return false;
        }

        /*this while loop count how many times can we tokenize the string using space.
        * in other words, how many words separated by spaces do we have
        * if count will be one, we have something like this: <op1>,<op2> (no spaces)
        * if count will be two, we have somthing like this: <op1>, <op2> or like this <op1> ,<op2> (one space between)
        * if count will be three, we have something like this: <op1> , <op2> (space from each side)*/
        token = strtok(word," ");
        while(token != NULL){
            counter++;
            token = strtok(NULL," ");
        }

        /*we got too many words. (eg: <op1> , <op2>  <extraWord>)*/
        if(counter>3){
            printf("error: too many arguments for command (extra word)");
            return false;
        }

        /*in this scenario, we have moved 1 time with strtok, 
        * meaning that we have a string looking like this: <op1>,<op2> */
        if(counter == 1){
            if(cpy[0] == ','){
                printf("label name cannot contain a comma!");
                return false;
            }
            strcpy(op1,strtok(cpy,","));
            token = strtok(NULL, " ");
            if(token == NULL){
                printf("not enough operands!");
                return false;
            }
            strcpy(op2,token);
        }

        /*in this scenario, we have moved 2 times with strtok,
        * meaning that we have a string looking like this: <op1> ,<op2>
        * or like this: <op1>, <op2> */
        if(counter == 2){
            if(cpy[0] == ','){
                printf("label name cannot contain a comma!");
                return false;
            }
            strcpy(op1,strtok(cpy,",")); /*now in op1 we have either something like this: <op1> or like this: <op1><space>*/
            if(isspace(op1[strlen(op1)-1])){
                i=0;
                while(!isspace(op1[i])){
                    i++; /*getting to the first whitespace and deleting everything after it*/
                }
                op1[i] = '\0';
            }
            strcpy(op2,strtok(NULL," ")); /*no spaces after op1, so we only need to take the other operand from the string*/
        }

        /*in this scenario, we have mover 3 times with strtok,
        * meaning that we have a string looking like this: <op1> , <op2>*/
        if(counter == 3){
            strcpy(op1, strtok(cpy," ")); /*beacause we have a spac after op1*/
            strtok(NULL," "); /*beacause we have another token (comma) if we tokenize using space*/
            strcpy(op2,strtok(NULL," ")); /*last token is op2*/
        }

        /*we now have the two operands in the corressponding variables, now we take their addressing types/methods*/
        addr1 = getAdressingType(op1);
        addr2 = getAdressingType(op2);

    }else if(operandNum == 1){
        /*this counts the amount of times we can tokenize the string with spaces.
        * in other words, how many words separated by spaces do we have.
        * if this is greater than one, we have too many words or too many arguments.*/
        token = strtok(word," ");
        while(token != NULL){
            counter++;
            token = strtok(NULL," ");
        }

        if(counter > 1){
            printf("too many arguments!");
            return false;
        }

        /*checking again with commas*/
        counter = 0;
        strcpy(word,cpy);
        token = strtok(word,",");
        while(token != NULL){
            counter++;
            token = strtok(NULL," ");
        }

        if(counter > 1){
            printf("too many arguments!");
            return false;
        }

        /*if the string includes commas, it is an illegal label name*/
        if(strchr(cpy,',') != NULL){
            printf("illegal label name");
            return false;
        }

        /*all ok, get operand and addressing type*/
        strcpy(op2,get1stW(cpy));
        addr2 = getAdressingType(op2);

    }else{
        /*we already skipped all spaces, now we either have an operand (illegal) or the end of the string (NULL terminator)*/
        if(cpy[0] != '\0'){
            printf("too many operands, command %s doesn't recieve operands",word);
            return false;
        }
    }

    /*we now have the operands, and their addressing types, now we need to check the validity of addressing types with the operation*/
    if(checkAddrWithOpcode(thisOpcode,addr1,addr2)){
        /*addressing types ok, now we need to translate the line and write it to the code Image using out function*/
        writeToCodeImage(thisOpcode,op1,op2,addr1,addr2,codeImage,IC);
    }else{
        printf("addressing types not allowed!");
        return false;
    }

}
#endif