/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file parser.c
* @brief This file contains the implementation of the parser
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
* @author Andrei Kulinkovich (xkulin01@stud.fit.vutbr.cz)
*/

#include "parser.h"
#include "expressions.h"
#include "symtable.h"

token_t token;
SymTabNode_t *symtable;

int parse_res = 0;
int count_of_func_params = 0;

bool indentif_exprs = false;

#define CLEAR_INIT_TOKEN()  TokenFree(&token); \
                            TokenInit(&token); 


#define CLEAR_TOKEN_EXIT_CODE(_err_code) \
            TokenFree(&token);           \
            SymTabDel(&symtable);         \
            exit(_err_code);             \


#define CHECK_SCAN_ERR(error_str)                                       \
            if ((parse_res = StartScanner(input,&token)) != 0)          \
            {                                                           \
                fprintf(stderr,error_str);                              \
                CLEAR_TOKEN_EXIT_CODE(parse_res);                       \
            }                                               

#define CLEAR_TOK_N_CHECK_SCAN(error_str)           \
                        CLEAR_INIT_TOKEN();         \
                        CHECK_SCAN_ERR(error_str);


bool Match(char *lexem)
{
    if(strcmp(token.attribute, lexem) == 0)
    {
        return true;
    }
    return false;
}


bool CheckTypeKeyword()
{
    char *arr[] = {"?int","int","string","?string", "?float", "float","void"};

    for (int i=0;i<7;i++)
    {
        if (Match(arr[i]))
            return true;
    }
    return false;
}


bool Prolog(FILE *input)
{
    CHECK_SCAN_ERR("ERROR IN PROLOG OR IN FIRST TOKEN\n");
    return true;
}


bool Prog(FILE * input)
{   
    TokenInit(&token);
    SymTabInit(&symtable);

    if (symtable == NULL){
        SymTabInsert(&symtable,"reads",0); 
        SymTabInsert(&symtable,"readi",0); 
        SymTabInsert(&symtable,"readf",0); 
        SymTabInsert(&symtable,"write",0); 
        SymTabInsert(&symtable,"floatval",1); 
        SymTabInsert(&symtable,"intval",1); 
        SymTabInsert(&symtable,"strval",1); 
        SymTabInsert(&symtable,"strlen",1); 
        SymTabInsert(&symtable,"substring",3); 
        SymTabInsert(&symtable,"ord",1); 
        SymTabInsert(&symtable,"chr",1); 
        //print2D(&symtable);
    
    }

    if (Prolog(input))
    {
         
        if (Body(input) && Ending())
            return true;
        else 
            return false;
    }  
    return false;
}


bool Body(FILE *input)
{

    if (token.type == TOKEN_TYPE_FUNC_NAME)
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }
    else if (token.type == TOKEN_TYPE_IDENTIFIER)
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }
    else if (Match("function"))
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }
    else if (Match("return"))
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }
    else if (Match("if"))
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }
    else if (Match("while"))
    {
        if (CodeBlock(input) && Body(input))
            return true;
        else
            return false; 
    }

    else if (token.type == TOKEN_TYPE_EOF)
    {
        return true;
    }
    else if (CodeBlock(input) && Body(input)) // expression <<<<<<<<<<<<<<----------------------
    {
        return true;
    } 
    else 
    {
        return false;
    }
}


bool Ending()
{
    if (token.type == TOKEN_TYPE_EOF)
    {
        TokenFree(&token);
        return true;
    }
    else 
        return false;
}
                         

bool CodeBlock(FILE *input)
{
    if (Match("function"))
    {
        if (DefFunction(input))
            return true;
        else 
            return false;
    }
    else if (token.type == TOKEN_TYPE_FUNC_NAME)
    {
        if (Statement(input))
            return true;
        else 
            return false;
    }
    else if (token.type == TOKEN_TYPE_IDENTIFIER)
    {
        if (Statement(input))
            return true;
        else 
            return false;
    }
    else if (Match("return"))
    {
        if (Statement(input))
            return true;
        else 
            return false;
    }
    else if (Match("if"))
    {
        if (Statement(input))
            return true;
        else 
            return false;
    }
    else if (Match("while"))
    {
        if (Statement(input))
            return true;
        else 
            return false;
    }
    else if (Statement(input)) // expression <<<<<<<<<<<<<<----------------------
    { 
        return true;
    }
    else 
    {
        return false;
    }
}


bool Statement(FILE *input)
{
    if (token.type == TOKEN_TYPE_FUNC_NAME)
    {
        if (FunctionCall(input) && Match(";"))
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT func name");
            return true;
        }
        else
            return false;
    }
    else if (token.type == TOKEN_TYPE_IDENTIFIER )
    {
        if (!(parse_res = expression(input,&token)) && Match(";")) // expression <<<<<<<<<<<<<<----------------------
        {  
            CLEAR_TOK_N_CHECK_SCAN("ERR: from STATEMENT in IDENTIFIER with EXPRESSIONS");
            return true;
        }

        if (Match("="))
        {   
        
            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT -> ID = (2) ");
            
            if (token.type == TOKEN_TYPE_FUNC_NAME)
            {
                if (FunctionCall(input) && Match(";"))
                {
                    CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT -> ID = func(); ");
                    return true;
                }
                else
                    return false;
            }
            if ((parse_res = expression(input,&token))) {  // expression <<<<<<<<<<<<<<----------------------
                fprintf(stderr,"ERROR FROM STATEMENT \"ID =\"  IN EXPRESSION: %d\n",parse_res);
                return false;
            }
            
            if (Match(";"))
            {
                CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT -> ID = (3) ");
                return true;
            }
            else
                return false;
        }
        else
        {
            indentif_exprs = true;
            return false;
        }
    }

    else if (Match("return"))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT return opt_expr");

        if (OptExpr(input) && Match(";"))
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT return opt_expr ");
            return true;
        }
        return false;
    }

    else if (Match("if"))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if ");

        if (Match("("))
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if ( ");

            if ((parse_res = expression(input,&token))) {  // expression <<<<<<<<<<<<<<----------------------
                fprintf(stderr,"ERROR FROM if IN EXPRESSION: %d\n",parse_res);
                return false;
            }

            if (Match(")"))
            {
                CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if (expression ) ");

                if (Match("{")) 
                {
                    CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if { ");
                    if (Statements(input))
                    {
                        if (Match("}"))
                        {
                            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if { statements } ");
                        }
                        else
                            return false;

                        //CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if } ");

                        if (Match("else")) 
                        {
                            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if else ");

                            if (Match("{"))
                            {
                                CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if else {");

                                if (Statements(input))
                                {
                                    if (Match("}"))
                                    {
                                        CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT if else }");
                                        return true;
                                    }
                                    else 
                                        return false;
                                }
                                else
                                    return false;
                            }
                            else 
                                return false;
                        }
                        else 
                            return false;  
                    }
                    else 
                        return false;
                }
                else 
                    return false;
            }
            else 
                return false;
        }
        else 
            return false;
    }

    else if (Match("while"))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT while ");
        
        if (Match("("))
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT while ( ");

            if ((parse_res = expression(input,&token))) {  // expression <<<<<<<<<<<<<<----------------------
                fprintf(stderr,"ERROR FROM while IN EXPRESSION: %d\n",parse_res);
                return false;
            }
            if (Match(")"))
            {
                CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT while ) ");
                if (Match("{"))
                {
                    CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT while { ");
                    if (Statements(input))
                    {
                        if ( Match("}"))
                        {
                            CLEAR_TOK_N_CHECK_SCAN("ERR: STATEMENT while { }");
                            return true;
                        }
                        else 
                            return false;
                    }
                    else 
                        return false;
                }
                else
                    return false;
            }
            else 
                return false;       
        }
        else 
            return false;
    }

    else if ((parse_res = expression(input,&token))) // expression <<<<<<<<<<<<<<----------------------
    {
        fprintf(stderr,"ERROR FROM STATEMENT IN EXPRESSION: %d\n",parse_res);
        return false;
    }
    else if (parse_res == 0 && Match(";"))
    {
         CLEAR_TOK_N_CHECK_SCAN("ERROR FROM STATEMENT IN EXPRESSION in ;");
        return true;
    }
    else
    {
        return false;
    }
}


bool DefFunction(FILE *input)
{
    if (Match("function"))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction "); 
        int def_func_err = 0;

        if (token.type == TOKEN_TYPE_FUNC_NAME)
        {
             SymTabNode_t *pos_new_node = SymTabSearch(symtable,token.attribute);
            if (pos_new_node == NULL)
            {
                SymTabInsert(&symtable,token.attribute,0);
                pos_new_node = SymTabSearch(symtable,token.attribute);
            }
            else
            {
                
                def_func_err = FUNC_DEF_ERR;
            }
            
           
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name ");


            if (Match("("))
            {
                 CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name ( ");

                if (ParamDecList1(input) && Match(")"))
                {
                    pos_new_node->num_of_params = count_of_func_params;
                    count_of_func_params = 0;

                    CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name (params ) ");

                    if (Match(":"))
                    {
                        CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name (params ) : ");
                        if (CheckTypeKeyword())
                        {
                            CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name (params ) : type ");

                            if (Match("{"))
                            {
                                CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name (params ) : type {");
                                if (Statements(input))
                                {
                                    if (Match("}"))
                                    {
                                        if (def_func_err)
                                        {
                                            parse_res = def_func_err;
                                            return false;
                                        }
                                        CLEAR_TOK_N_CHECK_SCAN("ERR: DefFunction func_name (params ) : type { }");
                                        return true;
                                    }
                                    else
                                        return false;
                                }
                                else 
                                    return false;
                            }
                            else
                                return false;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
                else 
                    return false;
            }
            else
                return false;
        }
        else 
            return false;
    }
    else
        return false;
}


bool Statements(FILE *input)
{
    if (token.type == TOKEN_TYPE_FUNC_NAME  || 
        token.type == TOKEN_TYPE_IDENTIFIER ||
        Match("return") || Match("if")|| Match("while"))
    {
        if (Statement(input) && Statements(input))
        {
            return true;
        }
        else
        {
            return false;
        }   
    }
    else if (Match("}"))
    {
        return true;
    }
    else if (Match(";"))
    {   
        CLEAR_TOK_N_CHECK_SCAN("ERR: IN STATEMENTS FOR ; ");
        return true;
    }
    else if (Statement(input) && Statements(input)) // expression <<<<<<<<<<<<<<----------------------
    {
        return true;
    } 
    else
    { 
        return false;
    }
}


bool ParamDecList1(FILE *input)
{
    if (Match(")"))
    {
        return true;
    }
    else if (CheckTypeKeyword())
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: ParamDecList1 type ");
        if (token.type == TOKEN_TYPE_IDENTIFIER)
        {
            count_of_func_params += 1;
            CLEAR_TOK_N_CHECK_SCAN("ERR: ParamDecList1 type param_dec_list_2 ");
            if (ParamDecList2(input))
            {
                return true;
            }
            else
                return false;
        }
        else 
            return false;
    }
    else
        return false;
}


bool ParamDecList2(FILE *input)
{
    if (Match(")"))
    {
        return true;
    }
    else if (Match(","))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: ParamDecList2 , ");

        if (CheckTypeKeyword())
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: ParamDecList2 , type ");

            if (token.type == TOKEN_TYPE_IDENTIFIER)
            {
                count_of_func_params += 1;
                CLEAR_TOK_N_CHECK_SCAN("ERR: ParamDecList2 , type id ");

                if (ParamDecList2(input))
                {
                    return true;
                }
                else
                    return false;
            }
            else 
                return false;
        }
        else
            return false;
    }
    else
        return false;
}


bool OptExpr(FILE *input)
{
    if (Match(";"))
        return true;
    else if ((parse_res = expression(input,&token)))  // expression <<<<<<<<<<<<<<----------------------
    { 
        fprintf(stderr,"ERROR FROM OptExpr IN EXPRESSION: %d\n",parse_res);
        //CLEAR_TOKEN_EXIT_CODE(parse_res);
        return false;
    }
    else if (parse_res == 0)
        return true;
    else
        return false;
}


bool FunctionCall(FILE *input)
{
    if (token.type == TOKEN_TYPE_FUNC_NAME)
    {  

        SymTabNode_t *declared_func = SymTabSearch(symtable,token.attribute);
        CLEAR_TOK_N_CHECK_SCAN("ERR: FunctionCall func_name ");
        if (Match("("))
        {
            CLEAR_TOK_N_CHECK_SCAN("ERR: FunctionCall func_name ( ");
            if (ParamList(input) && Match(")"))
            {
                if (declared_func == NULL)
                {
                    parse_res = FUNC_DEF_ERR;
                    return false;
                }

                if (strcmp(declared_func->key,"write") != 0)
                {
                    if (declared_func->num_of_params != count_of_func_params)
                    {
                        parse_res = FUNC_INTERNAL_ERR;
                        return false;
                    }
                }

                count_of_func_params = 0;

                CLEAR_TOK_N_CHECK_SCAN("ERR: FunctionCall func_name ( )");
                return true;
            }
            else 
                return false;
        }
        else 
            return false;
    }
    else 
        return false;
}


bool ParamList(FILE *input)
{
    if (Match(")"))
    {
        return true;
    }
    else if (Arg(input) && ArgList(input))
     {

        return true;
    } 
    else 
        return false;

}


bool Arg(FILE *input)
{
    if ((parse_res = expression(input,&token)))  // expression <<<<<<<<<<<<<<----------------------
    { 
        fprintf(stderr,"ERROR FROM Arg IN EXPRESSION: %d\n",parse_res);
        return false;
    }
    else
    {
        count_of_func_params += 1;
        return true;
    }

}


bool ArgList(FILE *input)
{
    if (Match(")"))
    {
        return true;
    }
    else if (Match(","))
    {
        CLEAR_TOK_N_CHECK_SCAN("ERR: ArgList , ");
        if (Arg(input) && ArgList(input))
        {
            return true;
        }
        else 
            return false;
    }
    else 
        return false;
}


int RunParser(FILE *input)
{
    if (!Prog(input) && parse_res == 0)
    {  
        CLEAR_TOKEN_EXIT_CODE(SYNTAX_ERR);
    }
    else if (indentif_exprs)
    {
        CLEAR_TOKEN_EXIT_CODE(SYNTAX_ERR);
    }
    else if (parse_res) 
    { 
        CLEAR_TOKEN_EXIT_CODE(parse_res); 
    }
    else 
    {
        CLEAR_TOKEN_EXIT_CODE(PROGRAM_OK);
    }
}
