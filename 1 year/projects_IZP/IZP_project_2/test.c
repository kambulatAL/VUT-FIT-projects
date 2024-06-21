#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *a,*b;
    b = NULL;
    a = malloc(sizeof(int));
    *a = -14;
    b = a;
    *b += 4;
    printf("%d",*a);
}