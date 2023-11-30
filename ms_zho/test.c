#include <stdio.h>
#include <stdlib.h>

typedef struct hash
{
    /* data */
    int hash;
}hash;


int test(int *a)
{
    int *b = (int *)(malloc(sizeof(hash)));
    printf("%p\n",b);
    a = b;
}

int test2(int **a)
{
    int *b = (int *)(malloc(sizeof(hash)));
    printf("%p\n",b);
    *a = b;
}

int main()
{
    int *a = NULL;

#if 0
    test(a);
    printf("%p\n",a);

#else
    test2(&a);
    printf("%p\n",a);
    
#endif
    
    return 0;
}