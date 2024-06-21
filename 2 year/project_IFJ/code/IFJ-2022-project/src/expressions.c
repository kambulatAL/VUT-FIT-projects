/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file expressions.c
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
* @author Andrei Kulinkovich (xkulin01@stud.fit.vutbr.cz)
*/

#include "expressions.h"
#include "error.h"
#include "symstack.h"
#include "scanner.h"


#define STACK_FREE(_return)								            \
	do {															\
		SymStackFree(&stack);									\
		return _return;												\
	} while(0)


#define TOKEN_FREE_TOKEN_INIT(_token)								\
		TokenFree(_token);											\
        TokenInit(_token);											
    


SymbStack stack;


typedef enum
{
	INST_PLUS_MINUS_CONCAT,		// 0 +-.
	INST_MUL_DIV,	        	// 1 */
	INST_REL_OP,				// 3 r
	INST_LEFT_BRACKET,			// 4 (
	INST_RIGHT_BRACKET,			// 5 )
	INST_DATA,					// 6 i
	INST_DOLLAR					// 7 $
} PrecTabIndEnum;



typedef enum
{
	S,   // < shift
	N,   // # err
	E,   // = equal
	R    // > apply rule
} PrecTabActEnum;


// precedence table
int prec_table[7][7] =
{
//	|+- | */| r | ( | ) | i | $ |
	{ R , S , R , S , R , S , R }, /// +-
	{ R , R , R , S , R , S , R }, /// */
	{ S , S , N , S , R , S , R }, /// r (realtion operators) ===, !==, <=, <, >, >=
	{ S , S , S , S , E , S , N }, /// (
	{ R , R , R , N , R , N , R }, /// )
	{ R , R , R , N , R , N , R }, /// i (id, int, double, string)
	{ S , S , S , S , N , S , N }  /// $
};


/*
 *  convertion of  symbol to precedence table index
 *
 * @param symbol symbol to be converted
 * @return return precedence table index
 */
PrecTabIndEnum GetPrecTabInd(PrecTabSymbEnum symbol)
{
	switch (symbol)
	{
	case PLUS:
	case MINUS:
    case CONCAT:
		return INST_PLUS_MINUS_CONCAT;

	case MUL:
	case S_DIV:
		return INST_MUL_DIV;

	case EQ:
	case NEQ:
	case LEQ:
	case LTN:
	case MEQ:
	case MTN:
		return INST_REL_OP;

	case LEFT_BRACKET:
		return INST_LEFT_BRACKET;

	case RIGHT_BRACKET:
		return INST_RIGHT_BRACKET;

	case IDENTIFIER:
	case INT_NUMBER:
	case DOUBLE_NUMBER:
	case STRING:
	case NULL_TYPE:
		return INST_DATA;

	default:
		return INST_DOLLAR;
	}
}


/*
 * Function converts token type to symbol.
 *
 * @param token Pointer to token.
 * @return Returns dollar if symbol is not supported or converted symbol if symbol is supported.
 */
PrecTabSymbEnum GetSymbFromToken( token_t *token)
{
    if (!strcmp(token->attribute,"+"))
        return PLUS;
    else if (!strcmp(token->attribute,"-"))
        return MINUS;
    else if (!strcmp(token->attribute,"*"))
        return MUL;
    else if (!strcmp(token->attribute,"/"))
        return S_DIV;
    else if (!strcmp(token->attribute,"."))
        return CONCAT;
    else if (!strcmp(token->attribute,"==="))
        return EQ;
    else if (!strcmp(token->attribute,"!=="))
        return NEQ;
    else if (!strcmp(token->attribute,"<="))
        return LEQ;
    else if (!strcmp(token->attribute,"<"))
        return LTN;
    else if (!strcmp(token->attribute,">="))
        return MEQ;
    else if (!strcmp(token->attribute,">"))
        return MTN;
	else if (!strcmp(token->attribute,"null"))
		return NULL_TYPE;
    else if (token->type == TOKEN_TYPE_BRACKET_OPEN)
        return LEFT_BRACKET;
    else if (token->type == TOKEN_TYPE_BRACKET_CLOSE)
        return RIGHT_BRACKET;
    else if (token->type == TOKEN_TYPE_IDENTIFIER)
        return IDENTIFIER;
    else if (token->type == TOKEN_TYPE_INT)
        return INT_NUMBER;
    else if (token->type == TOKEN_TYPE_FLOAT)
        return DOUBLE_NUMBER;
    else if (token->type == TOKEN_TYPE_STRING)
        return STRING;
    else
        return DOLLAR;
}


/*
 * Function function counts number of symbols after stop symbol on stack.
 *
 * @param stop_found Pointer to bool variable which will be changed to true if stop was found else to false.
 * @return Number of charatcters after stop symbol. Is valid only when stop_found was set to true.
 */
int NumOfSymbsAfterStop(bool* stop_found)
{
	SymbStackItem* tmp = SymStackTop(&stack);
	int count = 0;

	while (tmp != NULL)
	{
		if (tmp->symbol != STOP)
		{
			*stop_found = false;
			count++;
		}
		else
		{
			*stop_found = true;
			break;
		}

		tmp = tmp->next;
	}
	return count;
}


/*
 *  test if symbols in parameters are valid according to rules
 *
 * @param num Number of valid symbols in parameter
 * @param op1 Symbol 1
 * @param op2 Symbol 2
 * @param op3 Symbol 3
 * @return NOT_A_RULE if no rule is found or returns rule which is valid
 */
bool TestRule(int num, SymbStackItem* op1, SymbStackItem* op2, SymbStackItem* op3)
{
	switch (num)
	{
	case 1:
		// rule E -> i
		if (op1->symbol == IDENTIFIER || op1->symbol == INT_NUMBER || op1->symbol == DOUBLE_NUMBER || op1->symbol == STRING 
																									|| op1->symbol == NULL_TYPE)
			return true;

		return false;

	case 3:
		// rule E -> (E)
		if (op1->symbol == LEFT_BRACKET && op2->symbol == NON_TERM && op3->symbol == RIGHT_BRACKET)
			return true;

		if (op1->symbol == NON_TERM && op3->symbol == NON_TERM)
		{
			switch (op2->symbol)
			{
			// rule E -> E + E
			case PLUS:
				return true;

			// rule E -> E - E
			case MINUS:
				return true;
    
            // rule E -> E . E
            case CONCAT:
                return true;

			// rule E -> E * E
			case MUL:
				return true;

			// rule E -> E / E
			case S_DIV:
				return true;

			// rule E -> E === E
			case EQ:
				return true;

			// rule E -> E !== E
			case NEQ:
				return true;

			// rule E -> E <= E
			case LEQ:
				return true;

			// rule E -> E < E
			case LTN:
				return true;

			// rule E -> E >= E
			case MEQ:
				return true;

			// rule E -> E > E
			case MTN:
				return true;

			// invalid operator
			default:
				return false;
			}
		}
		return false;
	}
	return false;
}



/*
 *  "reduction" of symbols after STOP symbol if rule for reducing is found.
 *
 * @param data Pointer to table.
 * @return Given exit code.
 */
bool ReduceRule()
{

	//int result;

	SymbStackItem* op1 = NULL;
	SymbStackItem* op2 = NULL;
	SymbStackItem* op3 = NULL;

	bool right_rule = false;
	bool found = false;

	int count = NumOfSymbsAfterStop(&found);

	if (count == 1 && found)
	{
		op1 = stack.top;
		right_rule = TestRule(count, op1, NULL, NULL);
	}
	else if (count == 3 && found)
	{
		op1 = stack.top->next->next;
		op2 = stack.top->next;
		op3 = stack.top;
		right_rule = TestRule(count, op1, op2, op3);
	}
	else
		return false;

	if (!right_rule)
	{
		return false;
	}
	else
	{
		SymStackPopCount(&stack, count + 1);
		SymStackPush(&stack, NON_TERM);
	}

	return true;
}


int expression(FILE *input, token_t *token)
{
    int count_brackets = 0;
	int result = SYNTAX_ERR;

	SymStackInit(&stack);

	if (!SymStackPush(&stack, DOLLAR))
		STACK_FREE(INTERNAL_ERR);

	SymbStackItem* top_stack_terminal;
	PrecTabSymbEnum actual_symbol;

	bool success = false;

	do
	{
		actual_symbol = GetSymbFromToken(token);

		top_stack_terminal = SymStackTopTerm(&stack);
		
		if (top_stack_terminal == NULL)
			STACK_FREE(INTERNAL_ERR);

		switch (prec_table[GetPrecTabInd(top_stack_terminal->symbol)][GetPrecTabInd(actual_symbol)])
		{
		case E:
			SymStackPush(&stack, actual_symbol);
            TOKEN_FREE_TOKEN_INIT(token);
			if ((result = StartScanner(input, token)))
				STACK_FREE(result);
			break;

		case S:
			if (token->type == TOKEN_TYPE_BRACKET_OPEN)
			{
				count_brackets += 1;
			}
			if (!SymStackInsAfterTopTerm(&stack, STOP))
				STACK_FREE(INTERNAL_ERR);

			if(!SymStackPush(&stack, actual_symbol))
				STACK_FREE(INTERNAL_ERR);

            TOKEN_FREE_TOKEN_INIT(token);
			if ((result = StartScanner(input, token)))
				STACK_FREE(result);

            
			break;


		case R:
			if ((!ReduceRule()))
				STACK_FREE(SYNTAX_ERR);

			break;

		case N:
			if (actual_symbol == DOLLAR && top_stack_terminal->symbol == DOLLAR){
				success = true;
			}
			else
				STACK_FREE(SYNTAX_ERR);
			break;
		}

        if ((SymStackTop(&stack)->symbol != NON_TERM) && (token->type == TOKEN_TYPE_BRACKET_OPEN)){
            count_brackets += 1;
        }
        if ((SymStackTop(&stack)->symbol != NON_TERM) && (token->type== TOKEN_TYPE_BRACKET_CLOSE)){
            count_brackets -= 1;
            
			if (count_brackets < 0){
                STACK_FREE(0);
            }
        }

	} while (!success);

	SymbStackItem* final_non_terminal = SymStackTop(&stack);

    
	if (final_non_terminal == NULL)
	{
		STACK_FREE(INTERNAL_ERR);
	}

	if (final_non_terminal->symbol != NON_TERM)
	{
		STACK_FREE(SYNTAX_ERR);
	}

	STACK_FREE(0);
}


