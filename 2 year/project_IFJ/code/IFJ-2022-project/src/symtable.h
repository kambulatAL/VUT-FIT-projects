/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file symtable.h (Based on xalaka00's solution of the second IAL project, but with some changes)
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "scanner.h"


// node of a tree
typedef struct sym_bst_node {
	char *key;               		// name for function
	int num_of_params;				// count of parameters in a function
 	struct sym_bst_node *left;  	// left son
  	struct sym_bst_node *right; 	// right son
} SymTabNode_t;


void print2DUtil(SymTabNode_t **root, int space);
void print2D(SymTabNode_t **root);


void SymTabInit(SymTabNode_t **tree);

SymTabNode_t* SymTabSearch(SymTabNode_t *tree, char *key);

void SymTabInsert(SymTabNode_t **tree, char *func_key, int num_of_params);

void SymTabDel(SymTabNode_t **tree);


#endif
