//
// Created by carson on 11/15/21.
//

#include "assembler.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

char *ASM_ERROR_UNKNOWN_INSTRUCTION = "Unknown instruction";
char *ASM_ERROR_ARG_REQUIRED = "Argument Required";
char *ASM_ERROR_BAD_LABEL = "Bad Label";
char *ASM_ERROR_OUT_OF_RANGE = "Number is out of range";

//=========================================================
//  All the instructions available on the LMSM architecture
//=========================================================
const char *INSTRUCTIONS[25] =
        {"ADD", "SUB", "LDA", "STA", "BRA", "BRZ", "BRP", "INP", "OUT", "HLT", "COB", "DAT",
         "LDI",
         "CALL", "RET",
         "SPUSH", "SPUSHI", "SPOP", "SDUP", "SADD", "SSUB", "SMAX", "SMIN", "SMUL", "SDIV"
        };
const int INSTRUCTION_COUNT = 25;

//===================================================================
//  All the instructions that require an arg on the LMSM architecture
//===================================================================
const char *ARG_INSTRUCTIONS[11] =
        {"ADD", "SUB", "LDA", "STA", "BRA", "BRZ", "BRP", "DAT",
         "LDI",
         "CALL",
         "SPUSHI"
        };
const int ARG_INSTRUCTION_COUNT = 11;

//======================================================
// Constructors/Destructors
//======================================================

instruction * asm_make_instruction(char* type, char *label, char *label_reference, int value, instruction * predecessor) {
    instruction *new_instruction = calloc(1, sizeof(instruction));
    new_instruction->instruction = type;
    new_instruction->label = label;
    new_instruction->label_reference = label_reference;
    new_instruction->value = value;
    new_instruction->next = NULL;
    if (predecessor != NULL) {
        predecessor->next = new_instruction;
        new_instruction->offset = predecessor->offset + predecessor->slots;
    } else {
        new_instruction->offset = 0;
    }

    if(strcmp(new_instruction->instruction, "SPUSHI")==0){
        new_instruction->slots = 2;
    }
    else if(strcmp(new_instruction->instruction, "CALL")==0){
        new_instruction->slots = 3;
    }
    else{new_instruction->slots = 1;}

    return new_instruction;
}

void asm_delete_instruction(instruction *instruction) {
    if (instruction == NULL) {
        return;
    }
    asm_delete_instruction(instruction->next);
    free(instruction);
}

compilation_result * asm_make_compilation_result() {
    compilation_result *result = calloc(1, sizeof(compilation_result));
    return result;
}

void asm_delete_compilation_result(compilation_result *result) {
    asm_delete_instruction(result->root);
    free(result);
}

//======================================================
// Helpers
//======================================================
int asm_is_instruction(char * token) {
    for (int i = 0; i < INSTRUCTION_COUNT; ++i) {
        if (strcmp(token, INSTRUCTIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int asm_instruction_requires_arg(char * token) {
    for (int i = 0; i < ARG_INSTRUCTION_COUNT; ++i) {
        if (strcmp(token, ARG_INSTRUCTIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int asm_is_num(char * token){
    if (*token == '-') { // allow a leading negative
        token++;
    }
    while (*token != '\0') {
        if (*token < '0' || '9' < *token) {
            return 0;
        }
        token++;
    }
    return 1;
}

int asm_find_label(instruction *root, char *label) {
    // TODO - scan the linked list for the given label, return -1 if not found
    return -1;
}


//======================================================
// Assembly Parsing/Scanning
//======================================================

void asm_parse_src(compilation_result * result, char * original_src){

    // copy over so strtok can mutate
    char * src = calloc(strlen(original_src) + 1, sizeof(char));
    strcat(src, original_src);

    instruction * last_instruction = NULL;
    instruction * current_instruction = NULL;

    //TODO - generate a linked list of instructions and store the first into
    //       the result->root
    //
    //       generate the following errors as appropriate:
    //
    //       ASM_ERROR_UNKNOWN_INSTRUCTION - when an unknown instruction is encountered
    //       ASM_ERROR_ARG_REQUIRED        - when an instruction does not have a proper argument passed to it
    //       ASM_ERROR_OUT_OF_RANGE        - when a number argument is out of range (-999 to 999)
    //
    //       store the error in result->error

    char* token;
    char* label = NULL;
    char* label_reference = NULL;
    char* get_instruction = NULL;
    instruction * predecessor = NULL;
    int value = 0;

    char delimit[] = " \n";

    token = strtok(src,delimit);


    while(token != NULL) {

        //parsing label
        if(asm_is_instruction(token) == 0){
            label = token;
            token = strtok(NULL, delimit);
        }
        //parsing instruction
        if(asm_is_instruction(token) == 1){
            get_instruction = token;
            token = strtok(NULL, delimit);
        }
        //parsing value
        /*if(asm_is_num(token) == 0){
            value = atoi(token);
            //get range error
            if (value < -999 || value > 999){
                result->error = ASM_ERROR_OUT_OF_RANGE;
                return;
            }
            token = strtok(NULL, delimit);
        }*/
        //parsing label reference
        /*if(asm_is_instruction(token) == 1){
            label_reference = token;
            token = strtok(NULL, delimit);
        }*/



        if (result->root == NULL) {
            if(get_instruction == NULL){result->error = ASM_ERROR_UNKNOWN_INSTRUCTION;return;}
            else if(strcmp(get_instruction,"BRA") == 0 && asm_instruction_requires_arg(label_reference) == 0){result->error = ASM_ERROR_ARG_REQUIRED;return;}
            else{result->root = asm_make_instruction(get_instruction, label, label_reference, value, predecessor);}
        }
        //add next instruction to linked list
        else {

            instruction *node = result->root;
            while (node->next != NULL) {
                node = node->next;
            }
            if (node->next == NULL) {
                if(get_instruction == NULL){result->error = ASM_ERROR_UNKNOWN_INSTRUCTION;return;}
                else if(strcmp(get_instruction,"BRA") == 0 && asm_instruction_requires_arg(label_reference) == 0){result->error = ASM_ERROR_ARG_REQUIRED;return;}
                else{node->next = asm_make_instruction(get_instruction, label, label_reference, value, predecessor);}

            }
        }
    }

}







//======================================================
// Machine Code Generation
//======================================================

void asm_gen_code_for_instruction(compilation_result  * result, instruction *instruction) {

    //TODO - generate the machine code for the given instruction
    //
    // note that some instructions will take up multiple slots
    //
    // note that if the instruction has a label reference rather than a raw number reference
    // you will need to look it up with `asm_find_label` and, if the label does not exist,
    // report the error as ASM_ERROR_BAD_LABEL


    int value_for_instruction = instruction->value;
    if (strcmp("ADD", instruction->instruction) == 0) {
        result->code[instruction->offset] = 100 + value_for_instruction;
    }else if (strcmp("SUB", instruction->instruction) == 0) {
        result->code[instruction->offset] = 200 + value_for_instruction;
    } else if (strcmp("STA", instruction->instruction) == 0) {
        result->code[instruction->offset] = 300 + value_for_instruction;
    }else if (strcmp("LDI", instruction->instruction) == 0) {
        result->code[instruction->offset] = 400 + value_for_instruction;
    }else if (strcmp("LDA", instruction->instruction) == 0) {
        result->code[instruction->offset] = 500 + value_for_instruction;
    }else if (strcmp("BRA", instruction->instruction) == 0) {
        result->code[instruction->offset] = 600 + value_for_instruction;
    }else if (strcmp("BRZ", instruction->instruction) == 0) {
        result->code[instruction->offset] = 700 + value_for_instruction;
    }else if (strcmp("BRP", instruction->instruction) == 0) {
        result->code[instruction->offset] = 800 + value_for_instruction;
    }else if (strcmp("INP", instruction->instruction) == 0) {
        result->code[instruction->offset] = 901;
    }else if (strcmp("OUT", instruction->instruction) == 0) {
        result->code[instruction->offset] = 902;
    }else if (strcmp("HLT", instruction->instruction) == 0 || strcmp("COB", instruction->instruction) == 0) {
        result->code[instruction->offset] = 000;
    }else {
        result->code[instruction->offset] = 0;
    }

}

void asm_gen_code(compilation_result * result) {
    instruction * current = result->root;
    while (current != NULL) {
        asm_gen_code_for_instruction(result, current);
        current = current->next;
    }
}

//======================================================
// Main API
//======================================================

compilation_result * asm_assemble(char *src) {
    compilation_result * result = asm_make_compilation_result();
    asm_parse_src(result, src);
    asm_gen_code(result);
    return result;
}
