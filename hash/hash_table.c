#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int hashTableInit(HashTable **phashTable)
{
    int ret = 0;
    HashTable *pHashtable = (HashTable *)malloc(sizeof(HashTable) * 1);
    if (!pHashtable)
    {
        return -1;
    }
    memset(pHashtable, 0, sizeof(HashTable) * 1);

    /* 这个作为虚拟头节点 */
    pHashtable->slotKeyId = malloc(sizeof(hashNode) * SLOT_CAPACITY);
    if (!(pHashtable->slotKeyId))
    {
        return -1;
    }
    memset(pHashtable->slotKeyId, 0, sizeof(hashNode) * SLOT_CAPACITY);
    
    *phashTable = pHashtable;
    return ret;
}
 
/* 使用循环的方式获取slotId. */
static int calHashValue(int key)
{
    int slotId = key % SLOT_CAPACITY;
    return slotId;
}


int hashTableInsert(HashTable *pHashtable, int key, int value)
{
    if (!pHashtable)
    {
        return -1;
    }

    int slotId = calHashValue(key);

    hashNode * new_hash = (hashNode *)(malloc(sizeof(hashNode) * 1));
    memset(new_hash, 0, sizeof(hashNode) * 1);
    new_hash->real_key = key;
    new_hash->value = value;
    new_hash->next = NULL;

    /* 如果第一个里面没有值 */
    if(pHashtable->slotKeyId[slotId].next == NULL)
    {
        pHashtable->slotKeyId[slotId].next = new_hash;
    }
    else
    {
        /* 一开始在第二个节点 */
        hashNode *travel = (hashNode*)(pHashtable->slotKeyId[slotId].next);

        /* travel遍历到最后一个节点 */
        while(travel->next != NULL)
        {
            travel = travel->next;
        }
        travel->next = new_hash;
    }
    return 0;
}

int getAppointKeyValue(HashTable *pHashtable, int key)
{
    int slotId = calHashValue(key);
    
#if 0
    /* 我自己写的将头节点也存数据的写法，发现代码很乱 */
    /* 如果第一个里面正好是要找的节点 */
    if(pHashtable->slotKeyId[slotId].real_key == key)
    {
        return pHashtable->slotKeyId[slotId].value;
    }
    else
    {
        /* 一开始在第二个节点 */
        hashNode *travel = (hashNode*)pHashtable->slotKeyId[slotId].next;

        while(travel->next != NULL)
        {          
            if(travel->real_key == key)
            {
                return travel->value;
            }
            travel = travel->next;
        }
        if(travel->real_key == key)
        {
            return travel->value;
        }
        else
        {
            printf(" no this number!\n");
        }
        return -1;
    }
#else
    hashNode * travel = pHashtable->slotKeyId[slotId].next;
    while(travel)
    {
        if(travel->real_key == key)
        {
            return travel->value;
        }
        travel = travel->next;
    }

#endif
}

int destoryHash(HashTable *hashTable)
{
    int ret = 0;
    for(int idx = 0; idx < SLOT_CAPACITY; idx++)
    {
        hashNode *travel = (hashNode*)hashTable->slotKeyId[idx].next;
        while(travel)
        {
            hashTable->slotKeyId[idx].next = travel->next;
            free(travel);
            travel = hashTable->slotKeyId[idx].next;
        }
    }
    free(hashTable->slotKeyId);
    free(hashTable);

    return 0;
}
