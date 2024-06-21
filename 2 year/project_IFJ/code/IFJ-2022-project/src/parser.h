/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file parser.h
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "error.h"


bool Prog(FILE *input);
bool Prolog(FILE *input);
bool Body(FILE *input);
bool Ending();
bool CodeBlock(FILE *input);
bool Statement(FILE *input);
bool DefFunction(FILE *input);
bool ParamDecList1(FILE *input);
bool ParamDecList2(FILE *input);
bool OptExpr(FILE *input);
bool Statements(FILE *input);
bool FunctionCall(FILE *input);
bool ParamList(FILE *input);
bool Arg(FILE *input);
bool ArgList(FILE *input);

bool CheckTypeKeyword();
bool Match(char *lexem);
int RunParser(FILE *input);
#endif
