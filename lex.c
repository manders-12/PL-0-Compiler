/*
Michael Anderson
Professor Aedo
COP 3402
Assignment 2
11/25/2022
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"

#define MAX_IDENT_LENGTH 11
#define MAX_NUM_LENGTH 5
#define ERR_IDENT_LENGTH 1
#define ERR_NUM_LENGTH 2
#define ERR_INVALID_IDENT 3
#define ERR_INVALID_SYMBOL 4
#define ERR_INVALID_IDENT_NAME 5


int keyword_check(char buffer[]);
void print_lexeme_list(lexeme *list, int list_end);

lexeme *lex_analyze(int list_flag, char *input)
{
    char* buffer = malloc(sizeof(char)*(MAX_IDENT_LENGTH+1));
    lexeme* lexArray = malloc(sizeof(lexeme)*ARRAY_SIZE);
    //allocating memory for the buffer and lexeme array

    int index = 0;
    int j = 0;
    int i = 0;
    int error = 0;

    while(input[index] != '\0') {

        if (isalpha(input[index])){
            while((isalpha(input[index]) || isdigit(input[index])) && j < MAX_IDENT_LENGTH+1){
                buffer[j++] = input[index++];
            }
        //if the character at the current index is an alphabetical character, we will put it into the buffer along with any succeeding alphabetic characters or numbers
            if (j >= MAX_IDENT_LENGTH){
                //checks if buffer has exceeded maximum identifier length
                lexArray[i].type = -1;
                lexArray[i].error_type = ERR_IDENT_LENGTH;
                error = 1;
                while(isalpha(input[index]) || isdigit(input[index])) index++;
            }
            else {
                //stores the identifier in the lexeme array if the name is valid
                buffer[j] = '\0';
                lexArray[i].type = keyword_check(buffer);
                if(lexArray[i].type == identifier){
                    strcpy(lexArray[i].identifier_name, buffer);
                }
                else if (lexArray[i].type == -1){
                    lexArray[i].error_type = ERR_INVALID_IDENT_NAME;
                    error = 1;
                }
            }
            i++;
        }
        else if (isdigit(input[index])){
            while(isdigit(input[index]) && j < MAX_IDENT_LENGTH+1){
                buffer[j++] = input[index++];
            }
            //if the character at the current index is a number, we will put it in the buffer along with any succeeding numbers
            if(j >= MAX_NUM_LENGTH){
                lexArray[i].type = -1;
                lexArray[i].error_type = ERR_NUM_LENGTH;
                error = 1;
                while(isalpha(input[index]) || isdigit(input[index])) index++;
            }
            else if (isalpha(input[index])){
                //if we encounter an alphabetical character, throws invalid identifier error
                lexArray[i].type = -1;
                lexArray[i].error_type = ERR_INVALID_IDENT;
                error = 1;
                index++;
                while(isalpha(input[index]) || isdigit(input[index])) index++;
            }
            else {
                //adds the number to the buffer
                buffer[j] = '\0';
                lexArray[i].type = number;
                lexArray[i].number_value = atoi(buffer);
            }
            i++;
            //index++;
        }
        else if(input[index] == '?'){
            while(input[index]!='\0' && input[index] != '\n'){
                index++;
            }
        }
        else if(isspace(input[index])||iscntrl(input[index])){
            //while((isspace(input[index])||iscntrl(input[index]))&&(input[index]!='\0')) index++;
            index++;
        }
        else{
            //if-else-if statement for symbols
            if (input[index] == '.'){
                lexArray[i].type = period;
            }
            else if (input[index] == '-'){
                lexArray[i].type = minus;
            }
            else if (input[index] == ';'){
                lexArray[i].type = semicolon;
            }
            else if (input[index] == '{'){
                lexArray[i].type = left_curly_brace;
            }
            else if (input[index] == '}'){
                lexArray[i].type = right_curly_brace;
            }
            else if (input[index] == '+'){
                lexArray[i].type = plus;
            }
            else if (input[index] == '*'){
                lexArray[i].type = times;
            }
            else if (input[index] == '/'){
                lexArray[i].type = division;
            }
            else if (input[index] == '('){
                lexArray[i].type = left_parenthesis;
            }
            else if (input[index] == ')'){
                lexArray[i].type = right_parenthesis;
            }
            else if (input[index] == '>'){
                if (input[index+1] == '='){
                    index++;
                    lexArray[i].type = greater_than_or_equal_to;
                }
                else lexArray[i].type = greater_than;
            }
            else if (input[index] == '<'){
                if(input[index+1] == '='){
                    index++;
                    lexArray[i].type = less_than_or_equal_to;
                }
                else lexArray[i].type = less_than;
            }
            else if (input[index] == ':' && input[index+1] == '='){
                index++;
                lexArray[i].type = assignment_symbol;
            }
            else if (input[index] == '=' && input[index+1] == '='){
                index++;
                lexArray[i].type = equal_to;
            }
            else if (input[index] == '!' && input[index+1] == '='){
                index++;
                lexArray[i].type = not_equal_to;
            }
            else{
                lexArray[i].type = -1;
                lexArray[i].error_type = ERR_INVALID_SYMBOL;
                error = 1;
            }
            i++;
            index++;
        }
        //index++;
        j=0;
    }
    if(list_flag) print_lexeme_list(lexArray, i);
    if(error) return NULL;
    else return lexArray;
}

int keyword_check(char buffer[])
{
	if (strcmp(buffer, "const") == 0)
		return keyword_const;
	else if (strcmp(buffer, "var") == 0)
		return keyword_var;
	else if (strcmp(buffer, "procedure") == 0)
		return keyword_procedure;
	else if (strcmp(buffer, "call") == 0)
		return keyword_call;
	else if (strcmp(buffer, "begin") == 0)
		return keyword_begin;
	else if (strcmp(buffer, "end") == 0)
		return keyword_end;
	else if (strcmp(buffer, "if") == 0)
		return keyword_if;
	else if (strcmp(buffer, "then") == 0)
		return keyword_then;
	else if (strcmp(buffer, "while") == 0)
		return keyword_while;
	else if (strcmp(buffer, "do") == 0)
		return keyword_do;
	else if (strcmp(buffer, "read") == 0)
		return keyword_read;
	else if (strcmp(buffer, "write") == 0)
		return keyword_write;
	else if (strcmp(buffer, "def") == 0)
		return keyword_def;
	else if (strcmp(buffer, "return") == 0)
		return keyword_return;
	else if (strcmp(buffer, "main") == 0)
		return -1;
	else if (strcmp(buffer, "null") == 0)
		return -1;
	else
		return identifier;
}

void print_lexeme_list(lexeme *list, int list_end)
{
	int i;
	printf("Lexeme List: \n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < list_end; i++)
	{
		// not an error
		if (list[i].type != -1)
		{
			switch (list[i].type)
			{
				case identifier :
					printf("%11s\t%d\n", list[i].identifier_name, identifier);
					break;
				case number :
					printf("%11d\t%d\n", list[i].number_value, number);
					break;
				case keyword_const :
					printf("%11s\t%d\n", "const", keyword_const);
					break;
				case keyword_var :
					printf("%11s\t%d\n", "var", keyword_var);
					break;
				case keyword_procedure :
					printf("%11s\t%d\n", "procedure", keyword_procedure);
					break;
				case keyword_call :
					printf("%11s\t%d\n", "call", keyword_call);
					break;
				case keyword_begin :
					printf("%11s\t%d\n", "begin", keyword_begin);
					break;
				case keyword_end :
					printf("%11s\t%d\n", "end", keyword_end);
					break;
				case keyword_if :
					printf("%11s\t%d\n", "if", keyword_if);
					break;
				case keyword_then :
					printf("%11s\t%d\n", "then", keyword_then);
					break;
				case keyword_while :
					printf("%11s\t%d\n", "while", keyword_while);
					break;
				case keyword_do :
					printf("%11s\t%d\n", "do", keyword_do);
					break;
				case keyword_read :
					printf("%11s\t%d\n", "read", keyword_read);
					break;
				case keyword_write :
					printf("%11s\t%d\n", "write", keyword_write);
					break;
				case keyword_def :
					printf("%11s\t%d\n", "def", keyword_def);
					break;
				case keyword_return :
					printf("%11s\t%d\n", "return", keyword_return);
					break;
				case period :
					printf("%11s\t%d\n", ".", period);
					break;
				case assignment_symbol :
					printf("%11s\t%d\n", ":=", assignment_symbol);
					break;
				case minus :
					printf("%11s\t%d\n", "-", minus);
					break;
				case semicolon :
					printf("%11s\t%d\n", ";", semicolon);
					break;
				case left_curly_brace :
					printf("%11s\t%d\n", "{", left_curly_brace);
					break;
				case right_curly_brace :
					printf("%11s\t%d\n", "}", right_curly_brace);
					break;
				case equal_to :
					printf("%11s\t%d\n", "==", equal_to);
					break;
				case not_equal_to :
					printf("%11s\t%d\n", "!=", not_equal_to);
					break;
				case less_than :
					printf("%11s\t%d\n", "<", less_than);
					break;
				case less_than_or_equal_to :
					printf("%11s\t%d\n", "<=", less_than_or_equal_to);
					break;
				case greater_than :
					printf("%11s\t%d\n", ">", greater_than);
					break;
				case greater_than_or_equal_to :
					printf("%11s\t%d\n", ">=", greater_than_or_equal_to);
					break;
				case plus :
					printf("%11s\t%d\n", "+", plus);
					break;
				case times :
					printf("%11s\t%d\n", "*", times);
					break;
				case division :
					printf("%11s\t%d\n", "/", division);
					break;
				case left_parenthesis :
					printf("%11s\t%d\n", "(", left_parenthesis);
					break;
				case right_parenthesis :
					printf("%11s\t%d\n", ")", right_parenthesis);
					break;
				default :
					printf("Implementation Error: unrecognized token type\n");
					break;
			}
		}
		// errors
		else
		{
			switch (list[i].error_type)
			{
				case ERR_IDENT_LENGTH :
					printf("Lexical Analyzer Error: maximum identifier length is 11\n");
					break;
				case ERR_NUM_LENGTH :
					printf("Lexical Analyzer Error: maximum number length is 5\n");
					break;
				case ERR_INVALID_IDENT :
					printf("Lexical Analyzer Error: identifiers cannot begin with digits\n");
					break;
				case ERR_INVALID_SYMBOL :
					printf("Lexical Analyzer Error: invalid symol\n");
					break;
				case ERR_INVALID_IDENT_NAME :
					printf("Lexical Analyzer Error: identifiers cannot be named 'null' or 'main'\n");
					break;
				default :
					printf("Implementation Error: unrecognized error type\n");
					break;
			}
		}
	}
	printf("\n");
}
