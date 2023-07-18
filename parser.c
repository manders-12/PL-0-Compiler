/* Michael Anderson
 * Professor Aedo
 * COP3402 Fall 2022
 * Homework 3
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

lexeme *tokens;
int token_index = 0;
symbol *table;
int table_index = 0;
instruction *code;
int code_index = 0;

int error = 0;
int level;

void emit(int op, int l, int m);
void add_symbol(int kind, char name[], int value, int level, int address);
void mark();
int multiple_declaration_check(char name[]);
int find_symbol(char name[], int kind);

void print_parser_error(int error_code, int case_code);
void print_assembly_code();
void print_symbol_table();

//my functions
void program();
void block();
void declarations();
void constt();
void var(int );
void proc();
void statement();
void ident();
void call();
void begin();
void funcIf();
void funcWhile();
void read();
void write();
void def();
void rtn();

void condition();
void expression();
void term();
void factor();

instruction *parse(int code_flag, int table_flag, lexeme *list)
{
    tokens = list;
    table = malloc(ARRAY_SIZE * sizeof(symbol));
    code = malloc(ARRAY_SIZE * sizeof(instruction));

	program();
    if (code_flag && error == 0) print_assembly_code();
	if (table_flag && error == 0) print_symbol_table();
	if (error == 0) return code;
	else return NULL;
}

void program() {
    level = -1;

    block();
    if(error != -1 && tokens[token_index].type != period){
        print_parser_error(1, 0);
        error = 1;
    }

    if (error != -1){
        for(int i = 0; i < code_index; i++){
            if(code[i].op == CAL){
                if (code[i].m == -1){
                    error = 1;
                    print_parser_error(21, 0);
                }
                else {
                    code[i].m = table[code[i].m].address;
                }
            }
        }
    }



    if(error != -1 && code[code_index-1].op != SYS) emit(SYS, 0, HLT);
}

void block() { // BLOCK ::= DECLARATIONS STATEMENT
    level++;
    if(error != -1) declarations();
    if(error != -1) statement();
    mark();
    level--;
}

void declarations(){ // DECLARATIONS ::= {CONST|VAR|PROC}
    int numVars = 0;
    if (error != -1){
        do{
            if(tokens[token_index].type == keyword_const) constt(); //CONST
            else if (tokens[token_index].type == keyword_var){
                var(numVars); // VAR
                numVars++;
            }
            else if (tokens[token_index].type == keyword_procedure) proc(); // PROC
        } while ((tokens[token_index].type == keyword_const || tokens[token_index].type == keyword_var || tokens[token_index].type == keyword_procedure)&& error != -1);
    }
    if (error != -1) emit(INC, 0, 3+numVars);
}

void constt(){ //CONST
    token_index++;
    int negative = 1;
    int tempError = 0;
    symbol tempSym;
    tempSym.address = 0;
    tempSym.level = level;
    tempSym.kind = 1;

    if(tokens[token_index].type == identifier || tokens[token_index].type == assignment_symbol) {
        if (tokens[token_index].type == identifier){
            strcpy(tempSym.name, tokens[token_index].identifier_name);
            if (multiple_declaration_check(tempSym.name) != -1){
                error = 1;
                print_parser_error(3, 0);
            }
            token_index++;
        }
        else {
            error = 1;
            strcpy(tempSym.name, "null");
            print_parser_error(2, 1);
        }

    }
    else {
        tempError = 1;
        error = -1;
        print_parser_error(2, 1);
    }

    if(!tempError && (tokens[token_index].type == assignment_symbol || tokens[token_index].type == minus || tokens[token_index].type == number)){
        if(tokens[token_index].type == assignment_symbol) token_index++;
        else {
            error = 1;
            print_parser_error(4, 1);
        }

        if(tokens[token_index].type == minus){
            token_index++;
            negative = -1;
        }
    }
    else if (!tempError){
        tempError = 1;
        error = -1;
        print_parser_error(4, 1);
    }

    if(!tempError && (tokens[token_index].type == number || tokens[token_index].type == semicolon)){
        if (tokens[token_index].type == number){
            tempSym.value = negative*tokens[token_index].number_value;
            token_index++;
        }
        else {
            tempSym.value = 0;
            error = 1;
            print_parser_error(5, 0);
        }
    }
    else if (!tempError){
        tempError = 1;
        error = -1;
        print_parser_error(5, 0);
    }

    if(!tempError &&
       (tokens[token_index].type == semicolon || tokens[token_index].type == keyword_const ||
         tokens[token_index].type == keyword_var || tokens[token_index].type == keyword_procedure ||
         tokens[token_index].type == identifier || tokens[token_index].type == keyword_call ||
         tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_if ||
         tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_read ||
         tokens[token_index].type == keyword_write || tokens[token_index].type == keyword_def ||
         tokens[token_index].type == keyword_return || tokens[token_index].type == period ||
         tokens[token_index].type == right_curly_brace)) {
        if (tokens[token_index].type == semicolon) token_index++;
        else {
            error = 1;
            print_parser_error(6, 1);
        }
    }
    else if (!tempError) {
        tempError = 1;
        error = -1;
        print_parser_error(6, 1);
    }

    if(!tempError) add_symbol(tempSym.kind, tempSym.name, tempSym.value, tempSym.level, tempSym.address);
}

void var(int numVars){
    token_index++;
    symbol tempSym;
    tempSym.kind = 2;
    tempSym.address = 3 + numVars;
    tempSym.value = 0;
    tempSym.level = level;

    if (tokens[token_index].type == identifier || tokens[token_index].type == semicolon){
        if(tokens[token_index].type == identifier){
            strcpy(tempSym.name, tokens[token_index].identifier_name);
            if(multiple_declaration_check(tempSym.name) != -1){
                error = 1;
                print_parser_error(3, 0);
            }
            token_index++;
        }
        else{
            error = 1;
            strcpy(tempSym.name, "null");
            print_parser_error(2, 2);
        }
    }
    else {
        error = -1;
        print_parser_error(2, 2);
    }

    if(error != -1 && (tokens[token_index].type == semicolon || tokens[token_index].type == keyword_const ||
         tokens[token_index].type == keyword_var || tokens[token_index].type == keyword_procedure ||
         tokens[token_index].type == identifier || tokens[token_index].type == keyword_call ||
         tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_if ||
         tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_read ||
         tokens[token_index].type == keyword_write || tokens[token_index].type == keyword_def ||
         tokens[token_index].type == keyword_return || tokens[token_index].type == period ||
         tokens[token_index].type == right_curly_brace)) {
        if (tokens[token_index].type == semicolon) token_index++;
        else {
            error = 1;
            print_parser_error(6, 2);
        }
    }
    else if (error != -1){
        error = -1;
        print_parser_error(6, 2);
    }

    if(error != -1) add_symbol(tempSym.kind, tempSym.name, tempSym.value, tempSym.level, tempSym.address);
}

void proc(){
    token_index++;
    symbol tempSym;
    tempSym.value = 0;
    tempSym.kind = 3;
    tempSym.level = level;
    tempSym.address = -1;

    if (tokens[token_index].type == identifier || tokens[token_index].type == semicolon){
        if(tokens[token_index].type == identifier){
            strcpy(tempSym.name, tokens[token_index].identifier_name);
            if(multiple_declaration_check(tempSym.name) != -1){
                error = 1;
                print_parser_error(3, 0);
            }
            token_index++;
        }
        else{
            error = 1;
            strcpy(tempSym.name, "null");
            print_parser_error(2, 3);
        }
    }
    else {
        error = -1;
        print_parser_error(2, 3);
    }

    if(error != -1 && (tokens[token_index].type == semicolon || tokens[token_index].type == keyword_const ||
         tokens[token_index].type == keyword_var || tokens[token_index].type == keyword_procedure ||
         tokens[token_index].type == identifier || tokens[token_index].type == keyword_call ||
         tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_if ||
         tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_read ||
         tokens[token_index].type == keyword_write || tokens[token_index].type == keyword_def ||
         tokens[token_index].type == keyword_return || tokens[token_index].type == period ||
         tokens[token_index].type == right_curly_brace)) {
        if (tokens[token_index].type == semicolon) token_index++;
        else {
            error = 1;
            print_parser_error(6, 3);
        }
    }
    else if (error != -1){
        error = -1;
        print_parser_error(6, 3);
    }

    if(error != -1) add_symbol(tempSym.kind, tempSym.name, tempSym.value, tempSym.level, tempSym.address);
}

void statement(){
    if (error == -1) return;
    else if(tokens[token_index].type == identifier) ident();
    else if (tokens[token_index].type == keyword_call) call();
    else if (tokens[token_index].type == keyword_begin) begin();
    else if (tokens[token_index].type == keyword_if) funcIf();
    else if (tokens[token_index].type == keyword_while) funcWhile();
    else if (tokens[token_index].type == keyword_read) read();
    else if (tokens[token_index].type == keyword_write) write();
    else if (tokens[token_index].type == keyword_def) def();
    else if (tokens[token_index].type == keyword_return) rtn();
}

void ident(){
    int tempError;
    int symbolIndex = find_symbol(tokens[token_index].identifier_name, 2);
    int l = -1;
    int m = -1;
    if (symbolIndex != -1){
        l = level - table[symbolIndex].level;
        m = table[symbolIndex].address;
    }
    else if (find_symbol(tokens[token_index].identifier_name, 1) != -1 || find_symbol(tokens[token_index].identifier_name, 3) != -1){
        error = 1;
        print_parser_error(7, 0);
    }
    else {
        error = 1;
        print_parser_error(8, 1);
    }
    token_index++;

    if(tokens[token_index].type == assignment_symbol || tokens[token_index].type == identifier || tokens[token_index].type == number || tokens[token_index].type == left_parenthesis){
        if(tokens[token_index].type == assignment_symbol) token_index++;
        else {
            error = 1;
            print_parser_error(4, 2);
        }
    }
    else {
        error = -1;
        print_parser_error(4, 2);
    }

    if (error != -1) expression();
    if(error != -1) emit(STO, l, m);
}

void call(){
    token_index++;
    int l = -1;
    int m = -1;

    if(tokens[token_index].type == identifier || tokens[token_index].type == period ||
       tokens[token_index].type == right_curly_brace || tokens[token_index].type == semicolon ||
       tokens[token_index].type == keyword_end){

        if (tokens[token_index].type == identifier){
            int index1 = find_symbol(tokens[token_index].identifier_name, 1);
            int index2 = find_symbol(tokens[token_index].identifier_name, 2);
            int index3 = find_symbol(tokens[token_index].identifier_name, 3);
            if(index3 != -1){
                l = level - table[index3].level;
                m = index3;
            }
            else if (index1 != -1 || index2 != -1){
                error = 1;
                print_parser_error(9, 0);
            }
            else {
                error = 1;
                print_parser_error(8, 2);
            }
            emit(CAL, l, m);
            token_index++;
        }
        else {
            error = 1;
            print_parser_error(2, 4);
        }
    }
    else {
        error = -1;
        print_parser_error(2, 4);
    }
}

void begin(){
    do{
        token_index++;
        statement();
    } while (tokens[token_index].type == semicolon && error != -1);

    if(error != -1){
        if(tokens[token_index].type == keyword_end) token_index++;
        else if (tokens[token_index].type == identifier || tokens[token_index].type == keyword_call ||
                tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_if ||
                tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_read ||
                tokens[token_index].type == keyword_write || tokens[token_index].type == keyword_def ||
                tokens[token_index].type == keyword_return){
            error = -1;
            print_parser_error(6, 4);
        }
        else if (tokens[token_index].type == period || tokens[token_index].type == right_curly_brace || tokens[token_index].type == semicolon){
            error = 1;
            print_parser_error(10, 0);
        }
        else {
            error = -1;
            print_parser_error(10, 0);
        }
    }
}

void funcIf(){
    token_index++;

    condition();
    int tempIndex = code_index;
    if(error != -1){
        emit(JPC, 0, 0);

        if (tokens[token_index].type == keyword_then || tokens[token_index].type == period ||
            tokens[token_index].type == right_curly_brace || tokens[token_index].type == semicolon ||
            tokens[token_index].type == keyword_end || tokens[token_index].type == identifier ||
            tokens[token_index].type == keyword_call || tokens[token_index].type == keyword_begin ||
            tokens[token_index].type == keyword_if || tokens[token_index].type == keyword_while ||
            tokens[token_index].type == keyword_read || tokens[token_index].type == keyword_write ||
            tokens[token_index].type == keyword_def || tokens[token_index].type == keyword_return) {
            if (tokens[token_index].type == keyword_then) token_index++;

            else {
                error = 1;
                print_parser_error(11, 0);
            }

            statement();
            code[tempIndex].m = code_index;
        }
        else {
            error = -1;
            print_parser_error(11, 0);
        }
    }
}

void funcWhile(){
    token_index++;
    int jmpIndex = code_index;
    condition();
    int tempIndex = code_index;

    if(error != -1){
        emit(JPC, level, 0);
        if (tokens[token_index].type == keyword_do || tokens[token_index].type == period ||
            tokens[token_index].type == right_curly_brace || tokens[token_index].type == semicolon ||
            tokens[token_index].type == keyword_end || tokens[token_index].type == identifier ||
            tokens[token_index].type == keyword_call || tokens[token_index].type == keyword_begin ||
            tokens[token_index].type == keyword_if || tokens[token_index].type == keyword_while ||
            tokens[token_index].type == keyword_read || tokens[token_index].type == keyword_write ||
            tokens[token_index].type == keyword_def || tokens[token_index].type == keyword_return) {
            if(tokens[token_index].type == keyword_do) token_index++;
            else {
                error = 1;
                print_parser_error(12, 0);
            }

            statement();
            emit(JMP, 0, jmpIndex);
            code[tempIndex].m = code_index;
        }
        else {
            error = -1;
            print_parser_error(12, 0);
        }
    }
}

void read(){
    token_index++;
    int tempError = 0;

    if(tokens[token_index].type == identifier || tokens[token_index].type == period ||
       tokens[token_index].type == right_curly_brace || tokens[token_index].type== semicolon ||
       tokens[token_index].type == keyword_end){
        int l = -1;
        int m = -1;
        if (tokens[token_index].type == identifier){
            int tempIndex1 = find_symbol(tokens[token_index].identifier_name, 1);
            int tempIndex2 = find_symbol(tokens[token_index].identifier_name, 2);
            int tempIndex3 = find_symbol(tokens[token_index].identifier_name, 3);
            if(tempIndex2 != -1) {
                l = level - table[tempIndex2].level;
                m = table[tempIndex2].address;
            }
            else if (tempIndex1 != -1 || tempIndex3 != -1){
                error = 1;
                print_parser_error(13, 0);
            }
            else {
                error = 1;
                print_parser_error(8, 3);
            }
            token_index++;
        }
        else {
            tempError = 1;
            error = 1;
            print_parser_error(2, 5);
        }

        emit(SYS, 0, RED);
        emit(STO, l, m);
    }
    else {
        tempError = 1;
        error = -1;
        print_parser_error(2, 5);
    }
}

void write(){
    token_index++;
    expression();
    if(error != -1){
        emit(SYS, 0, WRT);
    }
}

void def(){
    token_index++;
    int tempError = 0;
    int tempIndex3;
    int hold;

    if(tokens[token_index].type == identifier || tokens[token_index].type == left_curly_brace) {
        if (tokens[token_index].type == identifier){
            int tempIndex1 = find_symbol(tokens[token_index].identifier_name, 1);
            int tempIndex2 = find_symbol(tokens[token_index].identifier_name, 2);
            tempIndex3 = find_symbol(tokens[token_index].identifier_name, 3);
            if(tempIndex3 != -1){
                if (table[tempIndex3].level != level){
                    tempError = 1;
                    error = 1;
                    print_parser_error(22, 0);
                }
                else if (table[tempIndex3].address != -1){
                    tempError = 1;
                    error = 1;
                    print_parser_error(23, 0);
                }
            }
            else if (tempIndex2 != -1 || tempIndex1 != -1){
                tempError = 1;
                error = 1;
                print_parser_error(14, 0);
            }
            else {
                tempError = 1;
                error = 1;
                print_parser_error(8, 4);
            }

            token_index++;
        }
        else {
            tempError = 1;
            error = 1;
            print_parser_error(2, 6);
        }

        //token_index++;
        if(tokens[token_index].type == left_curly_brace || tokens[token_index].type == keyword_const ||
           tokens[token_index].type == keyword_var || tokens[token_index].type == keyword_procedure ||
           tokens[token_index].type == identifier || tokens[token_index].type == keyword_call ||
           tokens[token_index].type == keyword_begin || tokens[token_index].type == keyword_if ||
           tokens[token_index].type == keyword_while || tokens[token_index].type == keyword_read ||
           tokens[token_index].type == keyword_write || tokens[token_index].type == keyword_def ||
           tokens[token_index].type == keyword_return || tokens[token_index].type == right_curly_brace){
            hold = code_index;
            emit(JMP, 0, -1);
            if(!tempError) table[tempIndex3].address = code_index;
            if (tokens[token_index].type == left_curly_brace) token_index++;
            else {
                error = 1;
                print_parser_error(15, 0);
            }

            block();
            if(error != -1){
                if (code[code_index-1].op != RTN) emit(RTN, 0, 0);
                code[hold].m = code_index;

                if(tokens[token_index].type == right_curly_brace || tokens[token_index].type == period || tokens[token_index].type == semicolon || tokens[token_index].type == keyword_end){
                    if (tokens[token_index].type == right_curly_brace) token_index++;
                    else {
                        error = 1;
                        print_parser_error(16, 0);
                    }
                }
                else {
                    error = -1;
                    print_parser_error(16, 0);
                }
            }
        }
        else if (error != -1){
            error = -1;
            print_parser_error(15, 0);
        }
    }
    else if (error != -1){
        error = -1;
        print_parser_error(2, 6);
    }
}

void rtn(){
    if (error != -1){
        if (level == 0) emit(SYS, 0, HLT);
        else emit(RTN, 0, 0);
    }
    token_index++;
}

void condition(){
    expression();
    int tempError = 0;
    token_type tempRelation;

    if (tokens[token_index].type == equal_to || tokens[token_index].type == not_equal_to ||
        tokens[token_index].type == less_than || tokens[token_index].type == less_than_or_equal_to ||
        tokens[token_index].type == greater_than || tokens[token_index].type == greater_than_or_equal_to){
       tempRelation = tokens[token_index].type;
       token_index++;
    }
    else if (tokens[token_index].type == identifier || tokens[token_index].type == number || tokens[token_index].type == left_parenthesis){
        tempError = 1;
        error = 1;
        print_parser_error(17, 0);
    }
    else{
        tempError = 1;
        error = -1;
        print_parser_error(17, 0);
    }

    if(error != -1) expression();
    if(error != -1){
        if(tempRelation == equal_to) emit(OPR, 0, EQL);
        else if (tempRelation == not_equal_to) emit(OPR, 0, NEQ);
        else if (tempRelation == less_than) emit(OPR, 0, LSS);
        else if (tempRelation == less_than_or_equal_to) emit(OPR, 0, LEQ);
        else if (tempRelation == greater_than) emit(OPR, 0, GTR);
        else if (tempRelation == greater_than_or_equal_to) emit(OPR, 0, GEQ);
        else emit(OPR, 0, -1);
    }
}

void expression(){
    //printf("\nexpression");
    term();
    while((tokens[token_index].type == plus || tokens[token_index].type == minus) && error != -1){
        token_type hold = tokens[token_index].type;
        token_index++;
        term();
        if(hold == minus) emit(OPR, 0, SUB);
        else emit(OPR, 0, ADD);
    }
}

void term(){
    //printf("\nterm");
    factor();
    while((tokens[token_index].type == times || tokens[token_index].type == division) && error != -1){
        token_type hold = tokens[token_index].type;
        token_index++;
        factor();
        if(hold == times) emit(OPR, 0, MUL);
        else emit(OPR, 0, DIV);
    }
}

void factor(){
    //printf("\nfactor");
    if(tokens[token_index].type == identifier){
        int index1 = find_symbol(tokens[token_index].identifier_name, 1);
        int index2 = find_symbol(tokens[token_index].identifier_name, 2);

        if(index1 == -1 && index2 == -1){
            if (find_symbol(tokens[token_index].identifier_name, 3) != -1){
                error = 1;
                print_parser_error(18, 0);
            }
            else {
                error = 1;
                print_parser_error(8, 5);
            }
        }
        else if (index1 != -1 && index2 != -1){
            if (table[index1].level > table[index2].level) emit(LIT, 0, table[index1].value);
            else emit(LOD, level-table[index2].level, table[index2].address);
        }
        else if (index1 != -1){
            emit(LIT, 0, table[index1].value);
        }
        else emit(LOD, level-table[index2].level, table[index2].address);
        token_index++;
    }

    else if (tokens[token_index].type == number){
        emit(LIT, 0, tokens[token_index].number_value);
        token_index++;
    }

    else if (tokens[token_index].type == left_parenthesis){
        token_index++;
        expression();
        if (tokens[token_index].type == right_parenthesis ||
            tokens[token_index].type == times || tokens[token_index].type == division ||
            tokens[token_index].type == plus || tokens[token_index].type == minus ||
            tokens[token_index].type == period || tokens[token_index].type == right_curly_brace ||
            tokens[token_index].type == semicolon || tokens[token_index].type == keyword_end ||
            tokens[token_index].type == equal_to || tokens[token_index].type == not_equal_to ||
            tokens[token_index].type == less_than || tokens[token_index].type == less_than_or_equal_to ||
            tokens[token_index].type == greater_than || tokens[token_index].type == greater_than_or_equal_to ||
            tokens[token_index].type == keyword_then || tokens[token_index].type == keyword_do){

            if(tokens[token_index].type == right_parenthesis) token_index++;
            else {
                error = 1;
                print_parser_error(19, 0);
            }
        }
        else {
            error = -1;
            print_parser_error(19, 0);
        }
    }

    else if (tokens[token_index].type == times || tokens[token_index].type == division ||
            tokens[token_index].type == plus || tokens[token_index].type == minus ||
            tokens[token_index].type == period || tokens[token_index].type == right_curly_brace ||
            tokens[token_index].type == semicolon || tokens[token_index].type == keyword_end ||
            tokens[token_index].type == equal_to || tokens[token_index].type == not_equal_to ||
            tokens[token_index].type == less_than || tokens[token_index].type == less_than_or_equal_to ||
            tokens[token_index].type == greater_than || tokens[token_index].type == greater_than_or_equal_to ||
            tokens[token_index].type == keyword_then || tokens[token_index].type == keyword_do){
        error = 1;
        print_parser_error(20, 0);
    }

    else {
        error = -1;
        print_parser_error(20, 0);
    }
}

void emit(int op, int l, int m)
{
	code[code_index].op = op;
	code[code_index].l = l;
	code[code_index].m = m;
	code_index++;
	/*
	print_assembly_code();
	print_symbol_table();
	*/
}

void add_symbol(int kind, char name[], int value, int level, int address)
{
	table[table_index].kind = kind;
	strcpy(table[table_index].name, name);
	table[table_index].value = value;
	table[table_index].level = level;
	table[table_index].address = address;
	table[table_index].mark = 0;
	table_index++;
}

void mark()
{
	int i;
	for (i = table_index - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

int multiple_declaration_check(char name[])
{
	int i;
	for (i = 0; i < table_index; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

int find_symbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < table_index; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}


void print_parser_error(int error_code, int case_code)
{
	switch (error_code)
	{
		case 1 :
			printf("Parser Error 1: missing . \n");
			break;
		case 2 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 2: missing identifier after keyword const\n");
					break;
				case 2 :
					printf("Parser Error 2: missing identifier after keyword var\n");
					break;
				case 3 :
					printf("Parser Error 2: missing identifier after keyword procedure\n");
					break;
				case 4 :
					printf("Parser Error 2: missing identifier after keyword call\n");
					break;
				case 5 :
					printf("Parser Error 2: missing identifier after keyword read\n");
					break;
				case 6 :
					printf("Parser Error 2: missing identifier after keyword def\n");
					break;
				default :
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 3 :
			printf("Parser Error 3: identifier is declared multiple times by a procedure\n");
			break;
		case 4 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 4: missing := in constant declaration\n");
					break;
				case 2 :
					printf("Parser Error 4: missing := in assignment statement\n");
					break;
				default :
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 5 :
			printf("Parser Error 5: missing number in constant declaration\n");
			break;
		case 6 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 6: missing ; after constant declaration\n");
					break;
				case 2 :
					printf("Parser Error 6: missing ; after variable declaration\n");
					break;
				case 3 :
					printf("Parser Error 6: missing ; after procedure declaration\n");
					break;
				case 4 :
					printf("Parser Error 6: missing ; after statement in begin-end\n");
					break;
				default :
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 7 :
			printf("Parser Error 7: procedures and constants cannot be assigned to\n");
			break;
		case 8 :
			switch (case_code)
			{
				case 1 :
					printf("Parser Error 8: undeclared identifier used in assignment statement\n");
					break;
				case 2 :
					printf("Parser Error 8: undeclared identifier used in call statement\n");
					break;
				case 3 :
					printf("Parser Error 8: undeclared identifier used in read statement\n");
					break;
				case 4 :
					printf("Parser Error 8: undeclared identifier used in define statement\n");
					break;
				case 5 :
					printf("Parser Error 8: undeclared identifier used in arithmetic expression\n");
					break;
				default :
					printf("Implementation Error: unrecognized error code\n");
			}
			break;
		case 9 :
			printf("Parser Error 9: variables and constants cannot be called\n");
			break;
		case 10 :
			printf("Parser Error 10: begin must be followed by end\n");
			break;
		case 11 :
			printf("Parser Error 11: if must be followed by then\n");
			break;
		case 12 :
			printf("Parser Error 12: while must be followed by do\n");
			break;
		case 13 :
			printf("Parser Error 13: procedures and constants cannot be read\n");
			break;
		case 14 :
			printf("Parser Error 14: variables and constants cannot be defined\n");
			break;
		case 15 :
			printf("Parser Error 15: missing {\n");
			break;
		case 16 :
			printf("Parser Error 16: { must be followed by }\n");
			break;
		case 17 :
			printf("Parser Error 17: missing relational operator\n");
			break;
		case 18 :
			printf("Parser Error 18: procedures cannot be used in arithmetic\n");
			break;
		case 19 :
			printf("Parser Error 19: ( must be followed by )\n");
			break;
		case 20 :
			printf("Parser Error 20: invalid expression\n");
			break;
		case 21 :
			printf("Parser Error 21: procedure being called has not been defined\n");
			break;
		case 22 :
			printf("Parser Error 22: procedures can only be defined within the procedure that declares them\n");
			break;
		case 23 :
			printf("Parser Error 23: procedures cannot be defined multiple times\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");

	}
}

void print_assembly_code()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < code_index; i++)
	{
		printf("%d\t%d\t", i, code[i].op);
		switch(code[i].op)
		{
			case LIT :
				printf("LIT\t");
				break;
			case OPR :
				switch (code[i].m)
				{
					case ADD :
						printf("ADD\t");
						break;
					case SUB :
						printf("SUB\t");
						break;
					case MUL :
						printf("MUL\t");
						break;
					case DIV :
						printf("DIV\t");
						break;
					case EQL :
						printf("EQL\t");
						break;
					case NEQ :
						printf("NEQ\t");
						break;
					case LSS :
						printf("LSS\t");
						break;
					case LEQ :
						printf("LEQ\t");
						break;
					case GTR :
						printf("GTR\t");
						break;
					case GEQ :
						printf("GEQ\t");
						break;
					default :
						printf("err\t");
						break;
				}
				break;
			case LOD :
				printf("LOD\t");
				break;
			case STO :
				printf("STO\t");
				break;
			case CAL :
				printf("CAL\t");
				break;
			case RTN :
				printf("RTN\t");
				break;
			case INC :
				printf("INC\t");
				break;
			case JMP :
				printf("JMP\t");
				break;
			case JPC :
				printf("JPC\t");
				break;
			case SYS :
				switch (code[i].m)
				{
					case WRT :
						printf("WRT\t");
						break;
					case RED :
						printf("RED\t");
						break;
					case HLT :
						printf("HLT\t");
						break;
					default :
						printf("err\t");
						break;
				}
				break;
			default :
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}

void print_symbol_table()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < table_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].address, table[i].mark);
	printf("\n");
}

/*
void constt(){ //CONST
    token_index++;
    int negative = 1;

    if(tokens[token_index].type == identifier || tokens[token_index].type == assignment_symbol){
        symbol tempSym;
        tempSym.kind = 1;
        tempSym.level = level;
        tempSym.address = 0;
        if (tokens[token_index].type == identifier && multiple_declaration_check(tokens[token_index].identifier_name)){
            error = 1;
            print_parser_error(3, 0);
            token_index++;
        }
        if (tokens[token_index].type == identifier){
            strcpy(tempSym.name, tokens[token_index].identifier_name);
        }
        else{
            error = 1;
            print_parser_error(2, 1);
            strcpy(tempSym.name, "null");
        }

        if(tokens[token_index].type == identifier){
            token_index++;
            if(tokens[token_index].type != assignment_symbol){
                if (tokens[token_index].type == number){
                    error = 1;
                    tempSym.value = tokens[token_index].number_value;
                    token_index++;
                }
                else if (tokens[token_index].type == minus && tokens[token_index+1].type == number){
                    error = 1;
                    negative = -1;
                    token_index++;
                    if(tokens[token_index].type == number) tempSym.value = tokens[token_index].number_value*negative;
                }
                else {
                    error = -1;
                }
                print_parser_error(4, 1);
            }
            else {
                token_index++
                if(tokens[token_index].type == minus){
                    negative = -1;
                    token_index++;
                }
                if(tokens[token_index].type == number){
                    tempSym.value = tokens[token_index].number_value*negative;

                }
                else if (tokens[token_index].type == semicolon){
                    error = 1;
                    print_parser_error(5, 0);
                    tempSym.value = 0;
                }
                else {
                    error = -1;
                    print_parser_error(5, 0);
                }
            }
        }
    }
    else{
        error = -1;
        print_parser_error(2, 1);
    }

}
*/
