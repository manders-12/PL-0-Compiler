/*
Michael Anderson
Professor Aedo
COP 3402
10/7/2022
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

int base(int *stack, int BP, int L);
void print_instruction(int PC, instruction IR);
void print_stack(int PC, int BP, int SP, int *stack);

void execute(int trace_flag, instruction *code)
{
	if (trace_flag)
	{
		printf("VM Exectution:\n");
		printf("\t\t\t\tPC\tBP\tSP\tstack\n");
		printf("Initial Values:\t\t\t0\t0\t-1\n");
	}

	int *stack = malloc(ARRAY_SIZE*sizeof(int)); //declaring stack of size ARRAY_SIZE (default is 50)
	for (int i = 0; i < ARRAY_SIZE; i++) stack[i] = 0; // sets all stack values to 0 initially
	int bp = 0; // base pointer initialized to 0
	int sp = -1; // stack pointer initialized to -1
	int pc = 0; // program counter initialized to 0
	instruction IR; // declaring IR
	int halt = 0;

	while(halt == 0){
        //Fetch Cycle
        IR = code[pc]; // IR <- code[PC]
        pc++; // PC <- PC + 1

        if (trace_flag) print_instruction(pc, IR);// prints the instruction if trace_flag is true


        //Execute Cycle
        switch(IR.op){
            case 1 : // LIT
                sp++;
                stack[sp] = IR.m;
                break;

            case 2 : // OPR
                switch (IR.m){
                    case 1 : // ADD
                        sp--;
                        stack[sp] = stack[sp] + stack[sp+1];
                        break;
                    case 2 : // SUB
                        sp--;
                        stack[sp] = stack[sp] - stack[sp+1];
                        break;
                    case 3 : // MUL
                        sp--;
                        stack[sp] = stack[sp] * stack[sp+1];
                        break;
                    case 4 : // DIV
                        sp--;
                        stack[sp] = stack[sp] / stack[sp+1];
                        break;
                    case 5 : // EQL
                        sp--;
                        stack[sp] = stack[sp] == stack[sp+1];
                        break;
                    case 6 : // NEQ
                        sp--;
                        stack[sp] = stack[sp] != stack[sp+1];
                        break;
                    case 7 : // LSS
                        sp--;
                        stack[sp] = stack[sp] < stack[sp+1];
                        break;
                    case 8 : // LEQ
                        sp--;
                        stack[sp] = stack[sp] <= stack[sp+1];
                        break;
                    case 9 : // GTR
                        sp--;
                        stack[sp] = stack[sp] > stack[sp+1];
                        break;
                    case 10 : // GEQ
                        sp--;
                        stack[sp] = stack[sp] >= stack[sp+1];
                        break;
                }
                break;

            case 3 : // LOD
                sp++;
                stack[sp] = stack[base(stack, bp, IR.l)+IR.m];
                break;

            case 4 : // STO
                stack[base(stack, bp, IR.l)+IR.m] = stack[sp];
                sp--;
                break;

            case 5 : // CAL
                stack[sp+1] = base(stack, bp, IR.l);
                stack[sp+2] = bp;
                stack[sp+3] = pc;
                bp = sp+1;
                pc = IR.m;
                break;

            case 6 : // RTN
                sp = bp-1;
                bp = stack[sp+2];
                pc = stack[sp+3];
                break;

            case 7 : // INC
                sp = sp+IR.m;
                break;

            case 8 : // JMP
                pc = IR.m;
                break;

            case 9 : // JPC
                if (stack[sp]==0){
                    pc = IR.m;
                }
                sp--;
                break;

            case 10 : // SYS
                switch(IR.m){
                    case 1 : // WRT
                        printf("\nOutput : %d", stack[sp]);
                        printf("\n\t\t\t\t");
                        sp--;
                        break;
                    case 2 : // RED
                        sp++;
                        printf("\nInput : ");
                        scanf("%d", &stack[sp]);
                        printf("\t\t\t\t");
                        break;
                    case 3 : // HLT
                        halt = 1;
                        break;
                }
                break;
        }
        if (trace_flag) print_stack(pc, bp, sp, stack); // prints the stack if trace_flag is true
	}


}

int base(int *stack, int BP, int L)
{
	while (L > 0)
	{
		BP = stack[BP];
		L--;
	}
	return BP;
}


void print_stack(int PC, int BP, int SP, int *stack)
{
	int i;
	printf("%d\t%d\t%d\t", PC, BP, SP);
	for (i = 0; i <= SP; i++)
		printf("%d ", stack[i]);
	printf("\n");
}

void print_instruction(int PC, instruction IR)
{
	char opname[4];

	switch (IR.op)
	{
		case LIT : strcpy(opname, "LIT"); break;
		case OPR :
			switch (IR.m)
			{
				case ADD : strcpy(opname, "ADD"); break;
				case SUB : strcpy(opname, "SUB"); break;
				case MUL : strcpy(opname, "MUL"); break;
				case DIV : strcpy(opname, "DIV"); break;
				case EQL : strcpy(opname, "EQL"); break;
				case NEQ : strcpy(opname, "NEQ"); break;
				case LSS : strcpy(opname, "LSS"); break;
				case LEQ : strcpy(opname, "LEQ"); break;
				case GTR : strcpy(opname, "GTR"); break;
				case GEQ : strcpy(opname, "GEQ"); break;
				default : strcpy(opname, "err"); break;
			}
			break;
		case LOD : strcpy(opname, "LOD"); break;
		case STO : strcpy(opname, "STO"); break;
		case CAL : strcpy(opname, "CAL"); break;
		case RTN : strcpy(opname, "RTN"); break;
		case INC : strcpy(opname, "INC"); break;
		case JMP : strcpy(opname, "JMP"); break;
		case JPC : strcpy(opname, "JPC"); break;
		case SYS :
			switch (IR.m)
			{
				case WRT : strcpy(opname, "WRT"); break;
				case RED : strcpy(opname, "RED"); break;
				case HLT : strcpy(opname, "HLT"); break;
				default : strcpy(opname, "err"); break;
			}
			break;
		default : strcpy(opname, "err"); break;
	}

	printf("%d\t%s\t%d\t%d\t", PC - 1, opname, IR.l, IR.m);
}
