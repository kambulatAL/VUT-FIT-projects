/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file scanner.c
* @brief This file contains the implementation of the scanner.
* @author Mikhail Diachenko (xdiach00@stud.fit.vutbr.cz)
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
* @author Andrei Kulinkovich (xkulin01@stud.fit.vutbr.cz)
* @author Herman Isaichanka (xisaic00@stud.fit.vutbr.cz)
*
*/


#include "scanner.h"
#include "error.h"


void TokenInit(token_t *token)
{
    token->type = TOKEN_TYPE_UNKNOWN;
    token->attribute = malloc(sizeof(char));
    token->attribute[0] = '\0';
}


void TokenFree(token_t *token)
{
    free(token->attribute);
    token->attribute = NULL;
    token->type = TOKEN_TYPE_UNKNOWN;
}


void TokenAccumulation(token_t *token, char current_char)
{
    int len = strlen(token->attribute);
    token->attribute = realloc(token->attribute, len + 2);
    token->attribute[len] = current_char;
    token->attribute[len + 1] = '\0';
}


void WriteNonEscapeToToken(token_t *token, char hex_oct_arr[])
{
 // pass all read symbols to the token and set token type to string
    TokenAccumulation(token,'\\');
    if (hex_oct_arr[1] == 'x')
    {
        // write symbols from the pseudo-hex array to a token
        for(int i=1; hex_oct_arr[i]!='\0'; i++)
        {
            TokenAccumulation(token,hex_oct_arr[i]);
        }
    }
    else 
    {
        // write symbols from the pseudo-octal array to a token
        for(int i=0;hex_oct_arr[i]!='\0'; i++)
        {
            TokenAccumulation(token,hex_oct_arr[i]);
        }
    }

    // set all values of the array to the '\0' symbol
    memset(hex_oct_arr,'\0',5);
}


bool CheckIfPrologue(FILE *input) 
{
    char current_char = getc(input); 
    char php_arr[] = {'?','p','h','p'};
    char declare_arr[] = "declare(strict_types=1);";

    if (current_char != '<')
    {  
        return false;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            current_char = getc(input);
            if (current_char != php_arr[i])
            {
                return false;
            }
        }

        current_char = getc(input);

        if (current_char != ' ' && current_char != '\t' && current_char != '\n' && current_char != '\r')
        {
            return false;
        }
       
        current_char = getc(input);
        
        if (current_char == '/')
        {
            current_char = getc(input);
            if (current_char == '/')
            {
                while(true)
                {
                    if (current_char == '\n' || current_char == '\r')
                        break;
                    current_char = getc(input);
                }
            }
            else if (current_char == '*')
            {
                while (true)
                {
                    current_char = getc(input);
                    if (current_char == '*')
                    {
                        current_char = getc(input);
                        if (current_char == '/')
                        {
                            break;
                        }
                        else
                            ungetc(current_char,input);
                    }
                }
            }
            else
                return false;
        }
        else 
            ungetc(current_char,input);
        
    
        current_char = getc(input);
        while(true)
        {
            if (current_char == ' ' || current_char == '\t' || current_char == '\n' || current_char == '\r')
            {
                current_char = getc(input);
            }
            else if (current_char == 'd')
            {
                ungetc(current_char,input);
                break;
            }
            else
            {
                return false;
            }
        }

        for (int i = 0; i < 24; i++)
        {
            current_char = getc(input);
            if (current_char != declare_arr[i])
            {
                return false;
            }
        }
        return true;
        
    }
}

//check if the token is a keyword
int SetTokenKeywType(token_t *token) 
{
    if (token->attribute[0] == '?')
    {
        if (!strcmp(token->attribute,"?int")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"?float")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"?string")) token->type = TOKEN_TYPE_KEYWORD;
        else return 0;
    }
    else
    {
        if (!strcmp(token->attribute,"else")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"if")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"int")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"float")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"string")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"null")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"function")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"return")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"void")) token->type = TOKEN_TYPE_KEYWORD;
        else if (!strcmp(token->attribute,"while")) token->type = TOKEN_TYPE_KEYWORD;
        else token->type = TOKEN_TYPE_FUNC_NAME;
    }
    return 1;
}


int StartScanner(FILE *input, token_t *token) 
{
    // control if there is valid prologue else return corresponding error

    static int passed_prologue = 0;
    if (!passed_prologue)
    {
        if ( CheckIfPrologue(input) != 1)
        {
            return SYNTAX_ERR; // syntax error in the prologue
        }
        passed_prologue = 1;
    }

    scannerState state = START;

    // array for hex/octal reprezentation of a single symbol in ASCII
    char hex_or_oct_str[5] = {'\0'};
 
    while(true) 
    {
        char current_char = getc(input);
        switch(state) 
        {
            
            case START:
                switch (current_char)
                {
                    case ' ': case '\t': case '\n': case '\r':
                        state = START;
                        break;

                    case ';':
                        TokenAccumulation(token,current_char);
                        token->type = TOKEN_TYPE_SEMICOLON;                
                        return 0;
                        break;
                    
                    case ',':
                        TokenAccumulation(token,current_char);
                        token->type = TOKEN_TYPEOMMAA;
                        return 0;
                        break;
                    
                    case '?':
                        // if we have got "?>" then check if there is no any symbol after that tag else throw error
                        {
                            char prog_end = getc(input);
                            if (prog_end == '>')
                            {
                                prog_end = getc(input);
                                if (prog_end != EOF)
                                {
                                   
                                    return SYNTAX_ERR;
                                }
                                else
                                {   
                                    token->type = TOKEN_TYPE_EOF;                      
                                    return 0;
                                }
                            }
                            else
                            {
                                TokenAccumulation(token,current_char);
                                ungetc(prog_end,input);
                            }
                        }
                        state = KEYW;
                        break;
                        
                    case ':':
                        TokenAccumulation(token,current_char);
                        token->type = TOKEN_TYPE_COLON;
                             
                        return 0;
                        break;

                    case EOF:
                        token->type = TOKEN_TYPE_EOF;
                                   
                        return 0;
                    
                    case '"':
                        state = CHAR_1;
                        break;

                    case '+': case '-': case '*': case '.':
                        TokenAccumulation(token, current_char); // accumulate symbol and set type, cause of the end state
                        token->type = TOKEN_TYPE_OPERATOR;               
                        return 0;
                        break;

                    case '=':
                        TokenAccumulation(token, current_char);
                        state = ASSIGN;
                        break;

                    case '<': case '>':
                        TokenAccumulation(token, current_char);
                        state = COMP_1;
                        break;

                    case '!':
                        TokenAccumulation(token, current_char);
                        state = NOT_EQ;
                        break;

                    case '$':
                        TokenAccumulation(token, current_char);
                        state = ID_1;
                        break;

                    case '/':
                        TokenAccumulation(token, current_char);
                        state = DIV;
                        break;

                    case '(':
                        TokenAccumulation(token, current_char);
                        token->type = TOKEN_TYPE_BRACKET_OPEN;                
                        return 0;
                        break;

                    case ')':
                        TokenAccumulation(token, current_char);
                        token->type = TOKEN_TYPE_BRACKET_CLOSE;       
                        return 0;
                        break;

                    case '{':
                        TokenAccumulation(token, current_char);
                        token->type = TOKEN_TYPE_CURLY_BRACKET_OPEN;                             
                        return 0;
                        break;

                    case '}':
                        TokenAccumulation(token, current_char);
                        token->type = TOKEN_TYPE_CURLY_BRACKET_CLOSE;                         
                        return 0;
                        break;

                    default:
                        if (isdigit(current_char))
                        {
                            TokenAccumulation(token, current_char);
                            state = INT;
                        }
                        else if (isalpha(current_char) || current_char == '_')
                        {
                            TokenAccumulation(token,current_char);
                            state = KEYW;
                        }
                        else
                        {   
                            return SCANNER_ERR;
                        }
                }
                break;


            case CHAR_1:
                if (current_char == '$')
                { 
                    return SCANNER_ERR;
                }
                else if(current_char == '"')
                {   
                    token->type = TOKEN_TYPE_STRING;
                     
                    return 0;
                }
                else if (current_char == '\\')
                {
                    state = CHAR_2;
                }
                else if (current_char == '\n') 
                {
                    // give '\n' to the token
                    TokenAccumulation(token,'\n');
                    state = CHAR_1;
                }
                else if (current_char == '\t') 
                {
                    // give '\t' to the token
                    TokenAccumulation(token,'\t');
                    state = CHAR_1;
                }
                else if (current_char == '\r') 
                {
                    // give '\r' to the token
                    TokenAccumulation(token,'\r');
                    state = CHAR_1;
                }
                else if (current_char < 32)
                {   
                    return SCANNER_ERR;
                }
                else
                {   
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                }
                break;


            case CHAR_2:
                if (current_char == 'x')
                {   //add symbols to the array for convertion of a (possible) escape sequence
                    hex_or_oct_str[0] = '0';
                    hex_or_oct_str[1] = 'x';
                    state = HEX_1;
                }
                else if (current_char >= '0' && current_char <= '3')
                {          
                    hex_or_oct_str[0] = current_char;
                    state = OCT_1;
                }
                else if (current_char == '"') 
                {
                    TokenAccumulation(token,'"');
                    state = CHAR_1;
                }
                else if (current_char == '\\') 
                {
                    // give '\\' to the token
                    TokenAccumulation(token,'\\');
                    state = CHAR_1;
                }
                else if (current_char == '$') 
                {
                    // give '$' to the token
                    TokenAccumulation(token,'$');
                    state = CHAR_1;
                }
                else if (current_char < 32)
                {                    
                    return SCANNER_ERR;
                }
                else 
                {
                    //pass non-escape sequence as it is to the token
                    TokenAccumulation(token,'\\');
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                }
                break;


            case HEX_1:
                if ((current_char >= 'a'  && current_char <= 'f') ||
                    (current_char >= 'A'  && current_char <= 'F') ||
                    isdigit(current_char))
                {   
                    hex_or_oct_str[2] = current_char;
                    state = HEX_2;
                }
                else if (current_char == '"')
                {
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    token->type = TOKEN_TYPE_STRING;
                                
                    return 0;
                    
                } 
                else if (current_char < 32 || current_char =='$')
                {             
                    return SCANNER_ERR;
                }
                else if (current_char == '\\')
                {
                    // we have "\x" as a non-escape seq. and '\' is for the next seq.
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    ungetc(current_char,input);
                    state = CHAR_1;
                }  
                else
                {   
                    // sequence is not a hex symbol, just pass the sequence to a token as it is
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                } 
                break;          


            case HEX_2:

                if ((current_char >= 'a'  && current_char <= 'f') ||
                    (current_char >= 'A'  && current_char <= 'F') ||
                    (current_char >= '1' && current_char <= '9'))
                {
                    // try to convert hex symbol to the char type and pass it to a token
                    hex_or_oct_str[3] = current_char;
                    long int hex_to_dec = strtol(hex_or_oct_str,0,16);
                    char c = (char)hex_to_dec;
                    
                    TokenAccumulation(token,c);
                    state = CHAR_1;
                    memset(hex_or_oct_str,'\0',5);
                }
                else if (current_char == '"')
                {
                    // pass read symbols to token and set token type to string
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    token->type = TOKEN_TYPE_STRING;
                                     
                    return 0;
                }
                else if (current_char == '\\')
                {
                    // we have "\x[0-9]" or "\x[a-fA-F]" as a non-escape seq. and '\' is for the next seq.
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    ungetc(current_char,input);
                    state = CHAR_1;
                } 
                else if (current_char < 32 || current_char == '$')
                {                 
                    return SCANNER_ERR;
                } 
                else
                {   // sequence is not a hex symbol, just pass sequence to the token as it is
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                }
                break;


            case OCT_1:
                if (current_char >= '0' && current_char <= '7')
                {   hex_or_oct_str[1] = current_char;
                    state = OCT_2;
                }
                else if (current_char == '"')
                {   
                    // pass to token and set token type to string
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    token->type = TOKEN_TYPE_STRING;
                    return 0;
                }
                else if (current_char == '\\')
                {
                    // we have "\[0-9]" as a non-escape seq. and '\' is for the next seq.
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    ungetc(current_char,input);
                    state = CHAR_1;
                }
                 else if (current_char < 32 || current_char == '$')
                {   
                    return SCANNER_ERR;
                }  
                else
                {   
                    // sequence is not an octal symbol, just pass sequence to the token as it is
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                }
                break;


            case OCT_2:
                if (current_char >= '1' && current_char<= '7')
                {
                    // try to convert octal symbol to the char and pass to token 
                    hex_or_oct_str[2] = current_char;
                    long int hex_to_dec = strtol(hex_or_oct_str,0,8);
                    char c = (char)hex_to_dec;
                    TokenAccumulation(token,c);
                    state = CHAR_1;
                    memset(hex_or_oct_str,'\0',5);    
                }
                else if (current_char == '"')
                {
                    // pass to token and set token type to string
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    token->type = TOKEN_TYPE_STRING;           
                    return 0;
                    
                }
                else if (current_char == '\\')
                {
                    // we have "\[0-9][0-9]" as a non-escape seq. and '\' is for the next seq.
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    ungetc(current_char,input);
                    state = CHAR_1;
                }
                else if (current_char < 32 || current_char == '$')
                {   
                    return SCANNER_ERR;
                }  
                else
                {   // sequence is not an octal symbol, just pass sequence to the token as it is
                    WriteNonEscapeToToken(token,hex_or_oct_str);
                    TokenAccumulation(token,current_char);
                    state = CHAR_1;
                }
                break;


            case ID_1:
                 if ((isalpha(current_char) || current_char=='_'))
                {
                    TokenAccumulation(token, current_char);
                    state=ID;
                }
                else
                {               
                    return SCANNER_ERR;
                }
                break;


            case ID:
                if ((isalpha(current_char) || current_char=='_' || isdigit(current_char)))
                {
                    TokenAccumulation(token, current_char);
                    state=ID;
                }
                else
                {    
                    ungetc(current_char,input);
                    token->type = TOKEN_TYPE_IDENTIFIER;                            
                    return 0;
                }
                break;
            
            
            case DIV:
                if (current_char=='/')
                {        
                    state = COMM_1;              
                }
                else if(current_char=='*')
                {
                    state = COMM_2;                
                }
                else
                {   
                    ungetc(current_char,input);
                    token->type = TOKEN_TYPE_OPERATOR;             
                    return 0;
                }
                break;


            case COMM_1:
                if (current_char=='\n')
                {
                    TokenFree(token);
                    TokenInit(token);
                    state = START;
                }
                else
                {
                    state = COMM_1;
                }
                break;


            case COMM_2:
                if (current_char=='*')
                {
                    state = COMM_3;
                }
                else
                {
                    state = COMM_2;
                }
                break;
            

            case COMM_3:
                if (current_char=='/')
                {
                    TokenFree(token);
                    TokenInit(token);
                    state = START;
                }
                else
                {
                    state = COMM_2;
                }
                break;


            case OPERATOR:
                TokenAccumulation(token, current_char);
                token->type = TOKEN_TYPE_OPERATOR;
                           
                return 0;
                break;


            case INT: 
                if (current_char == 'e' || current_char == 'E')
                {
                    TokenAccumulation(token, current_char);
                    state = INT_E;
                }
                else if (current_char == '.')
                {
                    TokenAccumulation(token, current_char);
                    state = NUM_1;
                }
                else if (isdigit(current_char) == 0)
                {
                    token->type = TOKEN_TYPE_INT;
                    ungetc(current_char, input);
                    return 0;
                }
                else
                {
                    TokenAccumulation(token, current_char);
                    state = INT;
                }
                break;


            case NUM_1:
                if (isdigit(current_char))
                {
                    TokenAccumulation(token, current_char);
                    state = FLT;
                }
                else
                {                 
                    return SCANNER_ERR;
                }
                break;


            case FLT:
                if (current_char == 'e' || current_char == 'E')
                {
                    TokenAccumulation(token, current_char);
                    state = NUM_E;
                }
                else if (isdigit(current_char) == 0)
                {
                    token->type = TOKEN_TYPE_FLOAT;
                    ungetc(current_char, input);               
                    return 0;
                }
                else
                {
                    TokenAccumulation(token, current_char);
                    state = FLT;
                }
                break;


            case INT_E:
                if (current_char == '+' || current_char == '-')
                {
                    TokenAccumulation(token, current_char);
                    state = I_SIGN;
                }
                else if (isdigit(current_char))
                {
                    TokenAccumulation(token, current_char);
                    state = EXPI;
                }
                else
                {
                    return SCANNER_ERR;
                }
                break;


            case I_SIGN:
                if (isdigit(current_char))
                {
                    TokenAccumulation(token, current_char);
                    state = EXPI;
                }
                else
                {
                    return SCANNER_ERR;
                }
                break;
            

            case EXPI:
                if (isdigit(current_char) == 0)
                {
                    token->type = TOKEN_TYPE_INT;
                    ungetc(current_char, input);             
                    return 0;
                }
                else
                {
                    TokenAccumulation(token, current_char);
                    state = EXPI;
                }
                break;


            case NUM_E:
                if (current_char == '-' || current_char == '+')
                {
                    TokenAccumulation(token, current_char);
                    state = F_SIGN;
                }
                else if (isdigit(current_char))
                {
                    TokenAccumulation(token, current_char);
                    state = EXPF;
                }
                else
                {           
                    return SCANNER_ERR;
                }
                break;


            case F_SIGN:
                if (isdigit(current_char))
                {
                    TokenAccumulation(token, current_char);
                    state = EXPF;
                }
                else
                {             
                    return SCANNER_ERR;
                }
                break;

                
            case EXPF:
                if (isdigit(current_char) == 0)
                {
                    token->type = TOKEN_TYPE_FLOAT;
                    ungetc(current_char, input);             
                    return 0;
                }
                else
                {
                    TokenAccumulation(token, current_char);
                    state = EXPF;
                }
                break;


            case ASSIGN:
                if (current_char == '=')   
                {
                    TokenAccumulation(token, current_char);
                    state = EQ_1;
                }
                else
                { 
                    token->type = TOKEN_TYPE_OPERATOR;
                    ungetc(current_char,input);              
                    return 0;
                }
                break;

            case COMP_1:
                if (current_char == '=')
                {
                    TokenAccumulation(token, current_char);
                    token->type = TOKEN_TYPE_OPERATOR;            
                    return 0;
                } 
                else
                {
                    token->type = TOKEN_TYPE_OPERATOR;    
                    ungetc(current_char,input);
                    return 0;
                }
                break;

            case NOT_EQ:
                if (current_char == '=')
                {
                    TokenAccumulation(token, current_char);
                    state = EQ_1;
                }
                else
                {            
                    return SCANNER_ERR;
                }
                break;

            case EQ_1:
                if (current_char == '=')
                {
                    TokenAccumulation(token, current_char);
                    token->type = TOKEN_TYPE_OPERATOR;
                                
                    return 0;
                }
                else
                {          
                    return SCANNER_ERR;
                }
                break;
            
            case KEYW:
                if  (isalpha(current_char) || current_char == '_' || isdigit(current_char))
                {
                    TokenAccumulation(token,current_char);
                    state = KEYW;
                }
                else 
                {
                    // control the type of token(what keyword it is) and set it
                    if (!SetTokenKeywType(token))
                    {
                        return SCANNER_ERR;
                    }
                    ungetc(current_char,input);
                    return 0;
                }
                break;
        }
    }
}
