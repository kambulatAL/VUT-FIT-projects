/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file error.h
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/


#ifndef ERROR_H
#define ERROR_H

#define PROGRAM_OK          0   // if there is no problem with lex. or syntax. analyzer 
#define SCANNER_ERR         1   // error in the scaner
#define SYNTAX_ERR          2   // error in the syntax 
#define FUNC_DEF_ERR        3   // semantic error - function is not defined, attempt to redefine a function 
#define FUNC_INTERNAL_ERR   4   // semantic error - wrong count/type of parameters in a function or wrong return type
#define VAR_DEF_ERR         5   // semantic error - variable is not defined
#define FUNC_RETURN_ERR     6   // semantic error - lack or presence of return expression in a function
#define TYPE_MATCHING_ERR   7   // semantic error - types are incompatible
#define SEM_ERR             8   // another semantic errors
#define INTERNAL_ERR       99   // internal errors (file reading error, malloc error etc.)

#endif
