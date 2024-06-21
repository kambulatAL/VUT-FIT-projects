/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file expressions.h
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H



/*
 * @enum Symbols used for precednece analysis
 */
typedef enum
{
	PLUS,			/// +
	MINUS,			/// -
	MUL,			/// *
	S_DIV,			/// /
    CONCAT,         /// .
	EQ,				/// ===
	NEQ,			/// !==
	LEQ,			/// <=
	LTN,			/// <
	MEQ,			/// >=
	MTN,			/// >
	LEFT_BRACKET,	/// (
	RIGHT_BRACKET,	/// )
	IDENTIFIER,		/// ID
	NULL_TYPE,		/// null
	INT_NUMBER,		/// int
	DOUBLE_NUMBER,	/// double
	STRING,			/// string
	DOLLAR,			/// $
	STOP,			/// stop symbol used when reducing >
	NON_TERM		/// non-terminal
} PrecTabSymbEnum;


/*
 * f <expression> rule for expression parser
 *
 * @param data Pointer to parser's internal data
 * 
 * @return Given exit code.
 * PData* data
 */
int expression();


#endif //_EXPRESSION_H
