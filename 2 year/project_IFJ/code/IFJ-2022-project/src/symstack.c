/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file symstack.c
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
* @author Andrei Kulinkovich (xkulin01@stud.fit.vutbr.cz)
*/


#include "symstack.h"
#include <stdlib.h>



void SymStackInit(SymbStack* stack)
{
	stack->top = NULL;
}


bool SymStackPush(SymbStack* stack, PrecTabSymbEnum symbol)
{
	SymbStackItem* new_item = (SymbStackItem*)malloc(sizeof(SymbStackItem));

	if (new_item == NULL)
		return false;

	new_item->symbol = symbol;
	new_item->next = stack->top;

	stack->top = new_item;

	return true;
}


bool SymStackPop(SymbStack* stack)
{
	if (stack->top != NULL)
	{
		SymbStackItem* tmp = stack->top;
		stack->top = tmp->next;
		free(tmp);

		return true;
	}
	return false;
}


void SymStackPopCount(SymbStack* stack, int count)
{
	for (int i = 0; i < count; i++)
	{
		SymStackPop(stack);
	}
}


SymbStackItem* SymStackTopTerm(SymbStack* stack)
{
	for (SymbStackItem* tmp = stack->top; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->symbol < STOP)
			return tmp;
	}

	return NULL;
}


bool SymStackInsAfterTopTerm(SymbStack* stack, PrecTabSymbEnum symbol)
{
	SymbStackItem* prev = NULL;

	for (SymbStackItem* tmp = stack->top; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->symbol < STOP)
		{
			SymbStackItem* new_item = (SymbStackItem*)malloc(sizeof(SymbStackItem));

			if (new_item == NULL)
				return false;

			new_item->symbol = symbol;

			if (prev == NULL)
			{
				new_item->next = stack->top;
				stack->top = new_item;
			}
			else
			{
				new_item->next = prev->next;
				prev->next = new_item;
			}

			return true;
		}

		prev = tmp;
	}
	return false;
}


SymbStackItem* SymStackTop(SymbStack* stack)
{
	return stack->top;
}


void SymStackFree(SymbStack* stack)
{
	while (SymStackPop(stack));
}
