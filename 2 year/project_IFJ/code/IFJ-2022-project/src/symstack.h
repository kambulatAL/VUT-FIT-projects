/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file symstack.h
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
* @author Andrei Kulinkovich (xkulin01@stud.fit.vutbr.cz)
*/

#ifndef _SYMSTACK_H
#define _SYMSTACK_H


#include <stdbool.h>

#include "expressions.h"
#include "symtable.h"


/*
 * @struct Stack item represetation.
 */
typedef struct stack_i
{
	PrecTabSymbEnum symbol; /// Symbol of stack item.
	struct stack_i *next; /// Pointer to next stack item.
} SymbStackItem;

/*
 * @struct Stack representation.
 */
typedef struct
{
	SymbStackItem *top; /// Pointer to stack item on top of stack.
} SymbStack;


/*
 * Function initializes stack.
 *
 * @param stack Pointer to stack.
 */
void SymStackInit(SymbStack* stack);

/*
 * Function pushes symbol to stack and sets its data type.
 *
 * @param stack Pointer to stack.
 * @param symbol Symbol to be pushed.
 * @param type Data type to be set.
 * @return True if successfull else false.
 */
bool SymStackPush(SymbStack* stack, PrecTabSymbEnum symbol);

/*
 * Function pops top symbol from stack.
 *
 * @param stack Pointer to stack.
 * @return True if successfull else false.
 */
bool SymStackPop(SymbStack* stack);

/*
 * Function pops stack more times.
 *
 * @param stack Pointer to stack.
 * @param count How many times stack will be popped.
 */
void SymStackPopCount(SymbStack* stack, int count);

/*
 * Function returns top termial.
 *
 * @param stack Pointer to stack.
 * @return Returns pointer to top terminal.
 */
SymbStackItem* SymStackTopTerm(SymbStack* stack);

/*
 * Function inserts symbol after top terminal.
 *
 * @param stack Pointer to stack.
 * @param symbol Symbol to be pushed.
 * @param type Data type to be set.
 * @return True if successfull else false.
 */
bool SymStackInsAfterTopTerm(SymbStack* stack, PrecTabSymbEnum symbol);

/*
 * Function returns top symbol.
 *
 * @param stack Pointer to stack.
 * @return Pointer to symbol on top of stack.
 */
SymbStackItem* SymStackTop(SymbStack* stack);

/*
 * Function frees resources used for stack.
 *
 * @param stack Pointer to stack.
 */
void SymStackFree(SymbStack* stack);


#endif //_SYMSTACK_H
