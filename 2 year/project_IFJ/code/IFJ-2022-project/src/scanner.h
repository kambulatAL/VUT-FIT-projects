/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file scanner.h
* @author Mikhail Diachenko (xdiach00@stud.fit.vutbr.cz)
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
 
typedef enum TokenType
{
    TOKEN_TYPE_UNKNOWN,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_INT,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_CURLY_BRACKET_OPEN, 
    TOKEN_TYPE_CURLY_BRACKET_CLOSE, 
    TOKEN_TYPE_BRACKET_OPEN, 
    TOKEN_TYPE_BRACKET_CLOSE,
    TOKEN_TYPE_EOF,
    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPEOMMAA,
    TOKEN_TYPE_COLON,
    TOKEN_TYPE_FUNC_NAME
} tokenType;

typedef struct Token
{
    tokenType type; // type of token
    char *attribute; // value
} token_t;

typedef enum ScannerState
{
    START,
    KEYW,
    ID_1, ID,
    COMM_1, COMM_2, COMM_3,
    OPERATOR,
    DIV,
    NOT_EQ, EQ_1, ASSIGN, COMP_1,
    NUM_1, NUM_E, I_SIGN, F_SIGN,INT, FLT, 
    EXPI,EXPF,INT_E,
    CHAR_1, CHAR_2, HEX_1, HEX_2, OCT_1, OCT_2,
} scannerState;




/*
* @brief main function of the scanner
* @param token: pointer to a token
* @param input: pointer to a source file
*/
int StartScanner(FILE *input,token_t *token); 

/*
*   Accept a source file to be scanned
*   @param f: pointer to a source file
*/
void ReadFile(FILE *f);


/*
* @brief prints lexical error to stderr
* @param  error: 
*/
void PrintScannerErr();


/*
* @brief initialization of token 
* @param  token: pointer to a token
*/
void TokenInit(token_t *token);


/*
* @brief free allocated memory for attribute in a token and set default token state
* @param  token: pointer to a token
*/
void TokenFree(token_t *token);


/*
* @brief accumulate attribute string and pass it to the token
* @param  token: pointer to a token
*/
void TokenAccumulation(token_t *token, char current_char); 


/*
* @brief pass non-escape sequence to the token attribute 
* @param  token: pointer to a token
* @param hex_oct_arr: array of a non-escape sequence
*/
void WriteNonEscapeToToken(token_t *token, char hex_oct_arr[]);


/*
* @brief set keyword type to a token if it is a keyword 
* @param  token: pointer to a token
*/
int SetTokenKeywType(token_t *token);

/*
* @brief check if there is right prologue in the code
* @param  input: input code file
*/
bool CheckIfPrologue(FILE *input);
#endif //excluse redefinition
