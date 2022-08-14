#ifndef FIRST_PASS_C
#define FIRST_PASS_C

#include "first_pass.h"

int main(){    
    FILE *in = fopen("infile.txt","r+");
    label labelTab[100] = { {"\0",0,false} };
    codeImg codeImage[256] = { {0,0,"\0"} };
    dataImg dataImage[256] = { {0,0} };
    extNode extList[100] = { {"\0"} };
    entNode entList[100] = { {"\0"} };
    int i;

    if(!first_pass(in, labelTab, codeImage, dataImage, extList, entList)){
        printf("ERROR");
        return 0;
    }

    printf("label tab:\n");
    for(i=0;i<7;i++){
        printf("name is: %s\n", labelTab[i].name);
    }
    printf("*end of label*\n\n");

    printf("codeImage:\n");
    for(i=0;i<30;i++){
        if(codeImage[i].labName[0] != '\0'){
            printf("lab is: %s\n", codeImage[i].labName);
        }else{
        printf("content is: %d\n", codeImage[i].content);
        }
    }
    printf("*end of code image*\n\n");

    printf("dataImage:\n");
    for(i=0;i<30;i++){
        printf("content is: %d\n", dataImage[i].content);
    }
    printf("*end of data image*\n\n");
    
    printf("externals:\n");
    for(i=0;i<5;i++){
        printf("name is: %s\n", extList[i].name);
    }
    printf("*end of externals*\n\n");
    
    printf("entries:\n");
    for(i=0;i<5;i++){
        printf("name is: %s\n", entList[i].name);
    }
    printf("*end of entries*\n\n");
}

boolean first_pass(FILE *infile, label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[]){
    char buf[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};
    char cpy[MAX_LINE] = {' '};
    char currentLabName[MAX_LABEL_NAME+1] = {' '};
    char *token;

    boolean labelDef  = false; /*flag for a line with a label definition*/

    int labelCounter = 0;
    int externCounter = 0;
    int entryCounter = 0;

    int IC = 0; /*instruction counter*/
    int DC = 0; /*data counter*/

    opcode currentOpcode;
    instruction currentInst;

    while(fgets(buf,MAX_LINE,infile) != NULL){
        if(!ignore(buf)){ /*we translate a line only if it should not be ignored (not blank, not comment)*/

            strcpy(buf,skipSpace(buf));
            strcpy(word,get1stW(buf));
            
            if(isLabelDef(word) == 1){ /*return values for isLabelDef listed below, checks if a word is a legal label declaration*/
                if(!isDefinedLabel(word,labelTab)){ /*check if a label with this name has already been declared somewhere else*/
                    labelDef = true;
                    strcpy(currentLabName, strtok(word,":")); /*if a label is ok, then when we tokenize it with a colon we should get the label name*/

                    /*we do not initialize labe's name and don't increment label counter because the label might be ignored due to .extern or .entry instruction*/
                }else{
                    printf("********error in first pass, label declared twice: ,%s********",word);
                    return false;
                }
            }else if(isLabelDef(word) == 2){
                printf("********error in first pass, illegal label name: %s********",word);
                return false;
            }
            
            /*if a label is declared in this line, we have a line that looks like this:
            LABEL: <value>
            so we need to skip to the start of the value*/
            
            
            if(labelDef){
                token = strtok(buf, " ");
                token = strtok(NULL, "\0");
                if(token == NULL){
                    printf("error, emptry string after label");
                    return false;
                }
            }else{
                token = buf;
            }
            strcpy(word,get1stW(token));
            
            if(token[strlen(token)-1] == '\n'){
                token[strlen(token)-1] = '\0';
            }
            
            currentOpcode = getCommandNum(word);
            currentInst = getInstType(word);

            if(currentInst != NONE_INST){
                if(currentInst == EXTERN_INST || currentInst == ENTRY_INST){ /*we have a .extern or .entry instruction*/
                    if(labelDef){ /*according to the maman's instructions, if a label is defined and inside the label is a .extern or .entry instruction, we ignore the label*/
                        printf("*WARNING: label definition includes .extern or .entry, label %s ignored*",currentLabName);
                    }
                    /*skip to after the .extern/.entry instruction*/
                    strcpy(token,skipWord(token));
                    strcpy(token,skipSpace(token));

                    /*empty line after .extern or .entry call*/
                    if(buf[0] == '\0' || buf[0] == '\n'){
                        printf("no arguments for .extern or .entry");
                        return false;
                    }
                    strcpy(word,token); /*copying buf to word to tokenize word without changing buf*/
                    strcpy(currentLabName, strtok(word," ")); /*get 1st word after .extern or .entry call*/
                    token = strtok(NULL," ");

                    if(token != NULL){ /*if we have another token after one argument, it's an error*/
                        printf("too many arguments for .extern/ .entry instruction");
                        return false;
                    }
                    if(!isLegalLabel(currentLabName)){ /*if we get an illegal label name inside the instruction it's an error*/
                        printf("illegal label name in .extern/ .entry instruction");
                        return false;
                    }
                    switch(currentInst){
                        case ENTRY_INST:
                            strcpy(entryList[entryCounter].name, currentLabName); /*store label name inside of entry list*/
                            entryCounter++;
                            break;
                        case EXTERN_INST:
                            strcpy(externList[externCounter].name, currentLabName); /*store label name inside of extern list*/
                            externCounter++;
                            break;
                    }
                }else{ /*the instruction is of type .data, .string or .struct*/
                    if(labelDef){
                        strcpy(labelTab[labelCounter].name,currentLabName); /*initializing name field of label*/
                        labelTab[labelCounter].address == DC;
                        labelTab[labelCounter].dataFlag == true; /*is a data label, data flag is true*/
                        labelCounter++;
                    }
                    if(!dataToWords(token, dataImage, &DC)){ /*translate data into data image array*/
                        printf("failed to write to data");
                        return false;
                    }
                }
                
            }else if(currentOpcode != NONE_OP){
                if(labelDef){
                    strcpy(labelTab[labelCounter].name,currentLabName); /*initializing name field of label*/
                    labelTab[labelCounter].address == IC + 100; /*setting address of label to 100 + instructioons so far (according to maman's instructions)*/
                    labelTab[labelCounter].dataFlag == false; /*not a data label, data flag is false*/
                    labelCounter++;
                }
                if(!CodeToWords(token, codeImage, &IC)){ /*translate buffer to code into code image*/
                    printf("failed to write to code");
                    return false;
                } 
            }else{
                printf("********invalid expression: %s********",word);
                return false;
            }
            
            labelDef = false;
            strcpy(currentLabName,"\0");
            currentInst = NONE_INST;
            currentOpcode = NONE_OP;
        }
    }
    return true;
}

/*checks for a string p if a label with this name exists already*/
boolean isDefinedLabel(char *p, label list[]){
    int i;
    for(i=0;i<100;i++){
        if(strcmp(list[i].name, p) == 0){
            return true;
        }
    }
    return false;
}

/*returns if a given string represents a legal label declaration*/
/*0 = not a label declaration*/
/*1 = legal label name and label decalaration*/
/*2 = label declaration and illegal label name*/
int isLabelDef(char *p){
    char cpy[MAX_LINE] = {' '};
    /*if the last character of a string is a colon, it is eligible to be a label definition*/
    /*we then need to check if a label is legal or not (alpha-numeric, starts with a letter etc.)*/
    if(p[strlen(p)-1] == ':'){
        strcpy(cpy,p);        
        strcpy(cpy, strtok(cpy,":"));
        if(isLegalLabel(cpy)){
        return 1;
        }
        return 2;
    } 
    return 0;
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
    code_word words[5] = {0,0,0,0,0,0,0,"\0"};

    strcpy(Cop1,op1);
    strcpy(Cop2,op2);

    /*storing first word: operation translation*/
    words[i].opcode = (unsigned int)thisOp;

    if(addr1 == NONE_ADDR){
        words[i].src_addressing = 0; /*NONE_ADDR is -1 and will be interprated as 11, but we need 0*/
    }else{
        words[i].src_addressing = (unsigned int)addr1;
    }
    if(addr2 == NONE_ADDR){
        words[i].dest_addressing = 0; /*NONE_ADDR is -1 and will be interprated as 11, but we need 0*/
    }{
        words[i].dest_addressing = (unsigned int)addr2;
    }

    words[i].ARE = A_FLAG; /*in the function word A,R,E field is always absolute*/

    ans += words[i].opcode;
    ans <<= SIZEOF_ADDRESSING; /*shift to the left by the number of bits that the source addressing field takes*/
    ans += words[i].src_addressing;
    ans <<= SIZEOF_ADDRESSING; /*shift to the left by the number of bits that the destination addressing field takes*/
    ans += words[i].dest_addressing;
    ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
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
            words[i].ARE = A_FLAG; /*ARE is absolute in this case*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
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
            words[i].ARE = A_FLAG; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            i++;
            (*IC)++;
            break;

        /*meaning we have a register name*/
        case REGISTER_ADDR:
            words[i].src_register = getRegNum(Cop1);
            words[i].ARE = A_FLAG;
            if(addr2 == REGISTER_ADDR){
                doubleRegFlag = true;
                words[i].dest_register = getRegNum(Cop2);

                ans += words[i].src_register;
                ans <<= SIZEOF_REGISTER; /*shift to the left by the number of bits that the destination register field takes*/
                ans += words[i].dest_register;
                ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
            }else{
                ans += words[i].src_register;
                ans <<= SIZEOF_REGISTER + SIZEOF_ARE; /*shift to the left by the number of bits that the destination register field takes plus the ARE field (since dest register is zero/none)*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
            }
            (*IC)++;
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
            words[i].ARE = A_FLAG; /*ARE is zero in this case*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
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
            words[i].ARE = A_FLAG; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            (*IC)++;
            break;

        case REGISTER_ADDR:
            if(doubleRegFlag == true){ /*we already stored this register number before*/
                break;
            }else{ /*no 1st register, only 2nd register, so 1st register is 0 ans 2nd register is the register number*/
                words[i].dest_register = getRegNum(op2);
                words[i].ARE = A_FLAG; /*ARE is zero here*/

                ans += words[i].dest_register;
                ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
                (*IC)++;
                break;
            }
        case NONE_ADDR:
            break;
    }
    return true;
}

/*takes string input, if it's ok, translates it according to the maman's instructions to the codeImage array*/
boolean CodeToWords(char *line, codeImg codeImage[], int *IC){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '}; /*used to store the first word of the input, but also used as a copy of the input (since we want to alter it and not lose cpy)*/
    char *cpyStr;
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
    
    cpyStr = strtok(cpy," ");
    cpyStr = strtok(NULL,"\0");

    if(cpyStr != NULL){ /*if this is null, we got an operation with zero operands and shouldnt copy*/
        strcpy(word,cpyStr); /*copying cpy to word (to use strtok without altering origin)*/
    }
    /*this next part extracts the operands (if there are any), checks the validity of the line construction (commas,num of operands etc.) and gets their addressing types*/
    if(operandNum == 2){

        if(cpy[0] == '\n' || cpy[0] == '\0'){
            printf("not enough operands!");
            return false;
        }

        if(strstr(cpyStr, ",,") != NULL){ /*if we have two commas in a row it's an error*/
            printf("ERROR: too many commas between operands");
            return false;
        }
        if(strchr(cpyStr,',') == NULL){ /*if we don't have commas at all, that is also an error*/
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
            if(cpyStr[0] == ','){
                printf("label name cannot contain a comma!");
                return false;
            }
            strcpy(op1,strtok(cpyStr,","));
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
            if(cpyStr[0] == ','){ /*strtok will ignore this comma*/
                printf("label name cannot contain a comma!");
                return false;
            }
            if(cpyStr[strlen(cpy)-1] == ','){ /*strtok will ignore this comma*/
                printf("label name cannot contain a comma!");
                return false;
            }
            strcpy(op1,strtok(cpyStr,",")); /*now in op1 we have either something like this: <op1> or like this: <op1><space>*/
            if(isspace(op1[strlen(op1)-1])){
                i=0;
                while(!isspace(op1[i])){
                    i++; /*getting to the first whitespace and deleting everything after it*/
                }
                op1[i] = '\0';
            }
            token = strtok(NULL," ");
            if(skipSpace(token) == '\n' || skipSpace(token) == '\0'){
                printf("not enough operands!");
                return false;
            }
            strcpy(op2,token); /*no spaces after op1, so we only need to take the other operand from the string*/
        }

        /*in this scenario, we have mover 3 times with strtok,
        * meaning that we have a string looking like this: <op1> , <op2>*/
        if(counter == 3){
            strcpy(op1, strtok(cpyStr," ")); /*beacause we have a spac after op1*/
            strtok(NULL," "); /*beacause we have another token (comma) if we tokenize using space*/
            strcpy(op2,strtok(NULL," ")); /*last token is op2*/
        }

        /*we now have the two operands in the corressponding variables, now we take their addressing types/methods*/
        addr1 = getAdressingType(op1);
        addr2 = getAdressingType(op2);

    }else if(operandNum == 1){

        if(cpyStr[0] == '\n' || cpyStr[0] == '\0'){
            printf("not enough operands!");
            return false;
        }

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
        strcpy(word,cpyStr);
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
        if(strchr(cpyStr,',') != NULL){
            printf("illegal label name");
            return false;
        }

        /*all ok, get operand and addressing type*/
        strcpy(op2,get1stW(cpyStr));
        addr2 = getAdressingType(op2);
    }else{
        /*we already skipped all spaces, now we either have an operand (illegal) or the end of the string (NULL)*/
        if(cpyStr != NULL){
            printf("too many operands, command %s doesn't recieve operands",word);
            return false;
        }
    }

    /*we now have the operands, and their addressing types, now we need to check the validity of addressing types with the operation*/
    if(checkAddrWithOpcode(thisOpcode,addr1,addr2)){
        /*addressing types ok, now we need to translate the line and write it to the code Image using out function*/
        if(writeToCodeImage(thisOpcode,op1,op2,addr1,addr2,codeImage,IC)){
            return true;
        }
        return false;
    }else{
        printf("addressing types not allowed!");
        return false;
    }

}

/*writes data instruction operands to data image*/
/*gets a string representing .data instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateData(char *input, dataImg dataImage[], int *DC){
    char cpy[MAX_LINE] = {' '};
    char *token;

    strcpy(cpy,input);

    /*no double commas*/
    if(strstr(cpy,",,") != NULL){
        printf("multiple commas between opernads!");
        return false;
    }
    /*no comma at start*/
    if(cpy[0] == ','){
        printf("number cannot contain comma!");
        return false;
    }
    /*no comma at end*/
    if(cpy[strlen(cpy)-1] == ','){
        printf("number cannot contain comma!");
        return false;
    }

    token = strtok(cpy, " ,");
    while(token != NULL){ /*foe evry token between commas, we check if it's a legal number (that may include spaces)*/
        if(isNumber(token)){
            if(atoi(token) > 1024){ /*we can store up to 1024 in 10 bits*/
                printf("number is too big to be stored!");
                return false;
            }else{
            dataImage[(*DC)].content = atoi(token); /*number ok, we store it*/
            }
        }else{
            printf("illegal number in data instruction!");
            return false;
        }
        token = strtok(NULL, " ,");
        (*DC)++;
    }
    return true;
}

/*writes string instruction operands to data image*/
/*gets a string representing .string instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateString(char *line, dataImg dataImage[], int *DC){
    char cpy[MAX_LINE] = {' '};
    int i = 1;
    boolean endOfString = false;

    strcpy(cpy,line);

    if(cpy[0] != '"'){ /*a string has to start with a quotation mark*/
        printf("a string must start with a quotation mark!");
        return false;
    }

    /*for each charracter in the string, we check if it closes the string, or if it's another character to add*/
    /*if it closes the string, we should add a "NULL terminator" in our translation (the value zero), and stop the loop*/
    for(i=1;i<strlen(cpy);i++){
        if(cpy[i] == '"'){
            endOfString = true;
            dataImage[(*DC)].content = 0;
            i++;
            (*DC)++;
            break;
        }else{
            dataImage[(*DC)].content = (int)cpy[i];
            (*DC)++;
        }
    }

    /*if we went thourgh the entire string, and did not close out string, that's an error*/
    if(!endOfString){
        printf("a string must end with a quotation mark");
        return false;
    }

    for(;i<strlen(cpy);i++){
        if(! isspace(cpy[i]) ){
            printf("too many arguments for .string instruction");
            return false;
        }
    }
}

/*writes struct operands to data image*/
/*gets a string representing .struct instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateStruct(char *line, dataImg dataImage[],int *DC){
    char cpy[MAX_LINE] = {' '};
    char *token;

    strcpy(cpy,line);

    /*no double commas*/
    if(strstr(cpy,",,") != NULL){
        printf("multiple commas between opernads!");
        return false;
    }
    /*there are commas in the input*/
    if(strchr(cpy,',') == NULL){
        printf("no comma between operands!");
        return false;
    }
    /*no comma at start*/
    if(cpy[0] == ','){
        printf("number cannot contain comma!");
        return false;
    }
    /*no comma at end*/
    if(cpy[strlen(cpy)-1] == ','){
        printf("number cannot contain comma!");
        return false;
    }

    token = strtok(cpy," ,");
    if(token == NULL){
        printf("no arguments for struct instruction");
        return false;
    }
    if(isNumber(token)){
        dataImage[(*DC)].content = atoi(token);
        (*DC)++;
    }else{
        printf("number in struct instruction is illegal");
        return false;
    }
    
    token = strtok(NULL,"\0");
    strcpy(token, skipSpace(token));
    if(*token == ','){
        token++;
        strcpy(token, skipSpace(token));
    }

    translateString(token, dataImage, DC);
}

/*gets a string representing a data instruction (.data/.string/.struct) ,and translates it to binary code inside of the data Image as requested in maman's instructions */
boolean dataToWords(char *line, dataImg dataImage[], int *DC){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};
    char *token;

    instruction currentInst;
    int numCounter = 0;

    strcpy(cpy,line);
    strcpy(word,get1stW(cpy));

    currentInst = getInstType(word); /*getting instruction type*/

    /*skiping after the instruction, to the start of the value*/
    strcpy(word, skipWord(cpy));
    strcpy(word, skipSpace(word));
    
    /*according to the type of data, we invoke the according function*/
    switch(currentInst){
        case DATA_INST:
            if(translateData(word, dataImage, DC)){
                return true;
            }
            break;
        case STRING_INST:
            if(translateString(word, dataImage, DC)){
                return true;
            }
            break;
        case STRUCT_INST:
            if(translateStruct(word, dataImage, DC)){
                return true;
            }
            break;
    }
    return false;
}

#endif