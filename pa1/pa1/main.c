/**********************************************************************
 * Copyright (c) 2021-2022
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* To avoid security error on Visual Studio */
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING FROM THIS LINE ******       */
#define MAX_NR_TOKENS    32    /* Maximum length of tokens in a command */
#define MAX_TOKEN_LEN    64    /* Maximum length of single token */
#define MAX_ASSEMBLY    256 /* Maximum length of assembly string */

typedef unsigned char bool;
#define true    1
#define false    0
/*          ****** DO NOT MODIFY ANYTHING UP TO THIS LINE ******      */
/*====================================================================*/



/***********************************************************************
 * translate()
 *
 * DESCRIPTION
 *   Translate assembly represented in @tokens[] into a MIPS instruction.
 *   This translate should support following 13 assembly commands
 *
 *    - add
 *    - addi
 *    - sub
 *    - and
 *    - andi
 *    - or
 *    - ori
 *    - nor
 *    - lw
 *    - sw
 *    - sll
 *    - srl
 *    - sra
 *    - beq
 *    - bne
 *
 * RETURN VALUE
 *   Return a 32-bit MIPS instruction
 *
 */
//res

typedef struct inst_struct {
    char* name;
    int opcode;
    int funct;
}inst_struct;

typedef struct register_struct {
    char* name;
    int number;
}register_struct;

inst_struct inst_list[MAX_NR_TOKENS] = {
    {"add", 0, 0x20},  //r포맷
    {"sub", 0, 0x22},  //r포맷
    {"and", 0, 0x24},  //r포맷
    {"or",  0, 0x25},  //r포맷
    {"nor", 0, 0x27},  //r포맷
    {"sll", 0, 0x00},  //r포맷
    {"srl", 0, 0x02},  //r포맷
    {"sra", 0, 0x03},  //r포맷
    {"addi", 0x08},  //i포맷
    {"andi", 0x0c},  //i포맷
    {"ori", 0x0d},  //i포맷
    {"lw", 0x23},  //i포맷
    {"sw", 0x2b},  //i포맷
    {"beq", 0x04},  //i포맷
    {"bne", 0x05},  //i포맷
};

register_struct register_list[MAX_NR_TOKENS] = {
    {"zero", 0}, {"at",1}, {"v0",2}, {"v1",3},
    {"a0",4}, {"a1",5}, {"a2",6}, {"a3",7},
    {"t0",8}, {"t1",9}, {"t2",10}, {"t3",11},
    {"t4",12}, {"t5",13}, {"t6",14}, {"t7",15},
    {"s0",16}, {"s1",17}, {"s2",18}, {"s3",19},
    {"s4",20}, {"s5",21}, {"s6",22}, {"s7",23},
    {"t8",24}, {"t9",25}, {"k1",26}, {"k2",27},
    {"gp",28}, {"sp",29}, {"fp",30}, {"ra",31},
};

int RegisterToNumber(char* tokens)
{
    int num = 0;
    for(int i=0; i<32; i++)
    {
        if(strcmp(tokens, register_list[i].name) == 0)
        {
            num = register_list[i].number;
        }
    }
    return num;
}

static unsigned int translate(int nr_tokens, char *tokens[])
{
    int num;
    int tok1, tok2, tok3;
    //타입에 따라 분류
    if((strcmp(tokens[0],"add") == 0) || (strcmp(tokens[0],"sub") == 0) || (strcmp(tokens[0],"and") == 0) || (strcmp(tokens[0],"or") == 0) || (strcmp(tokens[0],"nor") == 0) || (strcmp(tokens[0],"sll") == 0) || (strcmp(tokens[0],"srl") == 0) || (strcmp(tokens[0],"sra") == 0))//r타입
    {
        for(int i=0; i<5; i++)// add,sub,and, or, nor
        {
            if(strcmp(tokens[0],inst_list[i].name) == 0)
            {
                tok1 = RegisterToNumber(tokens[1]);
                tok2 = RegisterToNumber(tokens[2]);
                tok3 = RegisterToNumber(tokens[3]);
                 //필요없는 연산 최소화하기 위해 00000shamt는 안해도 됨 어차피 시프트하면 0으로 채워짐 , 비트마스크
                num = ((inst_list[i].opcode << 26) | (tok2 << 21) | (tok3 << 16) | (tok1 << 11) | inst_list[i].funct);
                return num;
            }
        }
        
        for(int i=5; i<8; i++)// sll, srl, sra
        {
            if(strcmp(tokens[0],inst_list[i].name) == 0)
            {
                tok1 = RegisterToNumber(tokens[1]);
                tok2 = RegisterToNumber(tokens[2]);
                if(strncmp(tokens[3], "0x", 2)==0)//16진수 들어올 때
                {
                    num = ((inst_list[i].opcode << 21) | (tok2 << 16) | (tok1 << 11) | (strtol(tokens[3], NULL, 16) << 6) | inst_list[i].funct);
                    return num;
                }
                else//10진수들어올때
                {
                    num = ((inst_list[i].opcode << 21) | (tok2 << 16) | (tok1 << 11) | (strtol(tokens[3], NULL, 10) << 6) | inst_list[i].funct);
                    return num;
                }
            }
        }
    }
    else //i타입
    {
        for(int i=8; i<16; i++)
        {
            if(strcmp(tokens[0],inst_list[i].name) == 0)
            {
                tok1 = RegisterToNumber(tokens[1]);
                tok2 = RegisterToNumber(tokens[2]);
                if(strncmp(tokens[3], "0x", 2)==0)//16진수 양수
                {
                    num = ((inst_list[i].opcode << 26) | (tok2 << 21) | (tok1 << 16) | (strtol(tokens[3], NULL, 16)));
                    return num;
                }
                else if(strncmp(tokens[3], "-0x", 3)==0)//16진수 음수
                {
                    num = ((inst_list[i].opcode << 26) | (tok2 << 21) | (tok1 << 16) | ((strtol(tokens[3], NULL, 16)) & 65535));
                    return num;
                }
                else if((strtol(tokens[3], NULL, 10)) > 0)//10진수 양수
                {
                    num = ((inst_list[i].opcode << 26) | (tok2 << 21) | (tok1 << 16) | (strtol(tokens[3], NULL, 10)));
                    return num;
                }
                else//10진수 음수
                {
                    num = ((inst_list[i].opcode << 26) | (tok2 << 21) | (tok1 << 16) | ((strtol(tokens[3], NULL, 10)) & 65535));
                    return num;
                }
            }
        }
    }
    return 0x02324020;
}



/***********************************************************************
 * parse_command()
 *
 * DESCRIPTION
 *   Parse @assembly, and put each assembly token into @tokens[] and the number
 *   of tokes into @nr_tokens. You may use this implemention or your own
 *   from PA0.
 *
 *   A assembly token is defined as a string without any whitespace (i.e., space
 *   and tab in this programming assignment). For exmaple,
 *     command = "  add t1   t2 s0 "
 *
 *   then, nr_tokens = 4, and tokens is
 *     tokens[0] = "add"
 *     tokens[1] = "t0"
 *     tokens[2] = "t1"
 *     tokens[3] = "s0"
 *
 *   You can assume that the characters in the input string are all lowercase
 *   for testing.
 *
 *
 * RETURN VALUE
 *   Return 0 after filling in @nr_tokens and @tokens[] properly
 *
 */
static int parse_command(char *assembly, int *nr_tokens, char *tokens[])
{
    char *curr = assembly;
    int token_started = false;
    *nr_tokens = 0;

    while (*curr != '\0') {
        if (isspace(*curr)) {
            *curr = '\0';
            token_started = false;
        } else {
            if (!token_started) {
                tokens[*nr_tokens] = curr;
                *nr_tokens += 1;
                token_started = true;
            }
        }
        curr++;
    }

    return 0;
}


/*====================================================================*/
/*          ****** DO NOT MODIFY ANYTHING BELOW THIS LINE ******      */

/***********************************************************************
 * The main function of this program.
 */
int main(int argc, char * const argv[])
{
    char assembly[MAX_ASSEMBLY] = { '\0' };
    FILE *input = stdin;

    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "No input file %s\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (input == stdin) {
        printf("*********************************************************\n");
        printf("*          >> SCE212 MIPS translator  v0.10 <<          *\n");
        printf("*                                                       *\n");
        printf("*                                       .---.           *\n");
        printf("*                           .--------.  |___|           *\n");
        printf("*                           |.------.|  |=. |           *\n");
        printf("*                           || >>_  ||  |-- |           *\n");
        printf("*                           |'------'|  |   |           *\n");
        printf("*                           ')______('~~|___|           *\n");
        printf("*                                                       *\n");
        printf("*                                 Spring 2022           *\n");
        printf("*********************************************************\n\n");
        printf(">> ");
    }

    while (fgets(assembly, sizeof(assembly), input)) {
        char *tokens[MAX_NR_TOKENS] = { NULL };
        int nr_tokens = 0;
        unsigned int machine_code;

        for (size_t i = 0; i < strlen(assembly); i++) {
            assembly[i] = tolower(assembly[i]);
        }

        if (parse_command(assembly, &nr_tokens, tokens) < 0)
            continue;

        machine_code = translate(nr_tokens, tokens);

        fprintf(stderr, "0x%08x\n", machine_code);

        if (input == stdin) printf(">> ");
    }

    if (input != stdin) fclose(input);

    return EXIT_SUCCESS;
}
