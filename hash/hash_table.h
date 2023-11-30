#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_

#define SLOT_CAPACITY   10

#if 0
typedef struct valueNode
{

};
#endif

/* 散列表对映的链表 */
typedef struct hashNode
{
    int real_key;
    int value;
    struct hashNode *next;
}hashNode;

/* 哈希表 */
typedef struct hashTable
{
#if 0
    int *slotId;  /* 动态数组 作为哈希表的key */
    int * value;   /* 作为哈希表的value. */
#else
    hashNode *slotKeyId;    /* 散列表 */
#endif
} HashTable;

/* 哈希表初始化函数 */
int hashTableInit(HashTable **hashtable);

/* 哈希插入函数 */
int hashTableInsert(HashTable *hashtable, int key, int value);

/* 从哈希表拿值 */
int getAppointKeyValue(HashTable *pHashtable, int key);

/* 销毁hash */
int destoryHash(HashTable *hashTable);



#endif //__HASH_TABLE_H_