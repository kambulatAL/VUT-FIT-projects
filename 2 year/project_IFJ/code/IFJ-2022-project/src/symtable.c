/*
***************************************************************
*                         IFJ22                               *
***************************************************************
* @file symtable.c (Based on xalaka00's solution of the second IAL project, but with some changes)
* @author Kambulat Alakaev (xalaka00@stud.fit.vutbr.cz)
*/


#include "symtable.h"
#include <stdlib.h>



void SymTabInit(SymTabNode_t **tree)
{
    *tree = NULL;
}


SymTabNode_t* SymTabSearch(SymTabNode_t *tree, char *key)
{   
    if ( tree == NULL)
    { 
        return NULL;
        }

    else if (strcmp(tree->key,key) == 0)
    {
        return tree;
    }
    else if (strcmp(tree->key,key) > 0)
    {
        return SymTabSearch(tree->left,key);
    }
    else
    {
        return SymTabSearch(tree->right,key);
    }
}


void SymTabInsert(SymTabNode_t **tree, char *func_key, int num_of_params)
{

    if (*tree == NULL)
    {
        *tree = (SymTabNode_t *)malloc(sizeof(SymTabNode_t));
        (*tree)->key =  malloc((strlen(func_key)+1) * sizeof(char));
        strcpy((*tree)->key,func_key);
        (*tree)->num_of_params = num_of_params;
        (*tree)->right = NULL;
        (*tree)->left = NULL;
    }
    else if (strcmp((*tree)->key,func_key) > 0)
        {   
            SymTabInsert(&(*tree)->left,func_key,num_of_params);
        }
    else if (strcmp((*tree)->key,func_key) < 0)
        {
           
            SymTabInsert(&(*tree)->right,func_key,num_of_params);
        }
    
}


void SymTabDel(SymTabNode_t **tree)
{
    if (*tree != NULL)
    {
        SymTabDel(&(*tree)->left);
        SymTabDel(&(*tree)->right);
        free((*tree)->key);
        free((*tree));
        *tree = NULL;
    }
}
