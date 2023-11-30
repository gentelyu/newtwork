/* 手撕哈希表 
    值得思考的点：
        1：指针传递问题
        2：形参和实参的问题
        3：sizeof在申请结构体内存和指针内存的*/

#include <stdio.h>
#include "hash_table.h"
#include <time.h>
#include <stdlib.h> 

#define VLAUE_NUM   10
#define RANDOM_NUM  666

int main(int argc, char const *argv[])
{
    srand(time(NULL));

#if 0
    HashTable *hashtable = (HashTable *)malloc(sizeof(HashTable) * 1);
#else
    HashTable *hashtable = NULL;
#endif
    // if (!hashtable)
    // {
    //     return 0;
    // }
    hashTableInit(&hashtable);


    // int rand_key = rand() % 1000;
    // for (int idx = 0; idx < VLAUE_NUM * 2; idx++)
    // {
        
    //     hashTableInsert(hashtable, rand_key, idx + 66);
    //     hashTableInsert(hashtable, rand_key, idx + 99);
    // }

    hashTableInsert(hashtable, 1, 100);
    hashTableInsert(hashtable, 11, 120);

    hashTableInsert(hashtable, 2, 120);
    hashTableInsert(hashtable, 22, 120);

    hashTableInsert(hashtable, 3, 120);
    hashTableInsert(hashtable, 33, 120);


    // for(int idx = 0; idx < VLAUE_NUM * 4; idx++)
    // {
    //     int value = getAppointKeyValue(hashtable, idx);
    //     printf("key : %d -> value : %d\n", idx, value);
    // }
    //int value = getAppointKeyValue(hashtable, rand_key);
   // printf("key : %d -> value : %d\n", rand_key, value);  

   printf("value : %d\n", getAppointKeyValue(hashtable, 1)); 
   printf("value : %d\n", getAppointKeyValue(hashtable, 11)); 
 
    destoryHash(hashtable);
   
    
    return 0;
}

