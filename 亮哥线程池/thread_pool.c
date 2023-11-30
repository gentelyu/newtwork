#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>


#define THREAD_NUM      5
#define TASK_NUM        10
#define COUNT_NUM       20 

#define CHECK_PTR(ptr) \
    if (!ptr)          \
        return NULL;

/* 任务队列结点 */
typedef struct t_task
{
    void * (*task_callback) (void *arg);
    void *arg;
    struct t_task *next;
} t_task;

typedef struct thread_pool
{
    pthread_t *threadId;                /* 动态数组 */
    pthread_mutex_t mutex_lock;         /* 互斥锁1: 用于互斥共享资源 */
    pthread_mutex_t mutex_busythrcnt_lock;         /* 互斥锁2: 用来互斥忙碌的线程数量 目的:提升效率 */
    pthread_cond_t mutex_cond_ptoc;     /* 条件变量 */
    pthread_cond_t mutex_cond_ctop;     /* 条件变量 */

    t_task * queueHead;                 /* 任务队列 这边使用的是虚拟头结点 取任务出队列从头开始取 时间复杂度也是O(1) */
    t_task * queueTail;                 /* 任务队列 该结点指向单向链表的尾部 尾插的时间复杂度就是O(1) */
    int     task_size;                  /* 任务队列的任务数 */

    /* 最小、最大线程数量 */
    int min_thread_num;
    int max_thread_num;
    /* 忙碌线程数量 */
    int busy_thread_num;
    /* 当前线程数量 */
    int current_live_threadnum;
    /* 需要释放的线程数 释放线程资源 减少资源竞争 */
    int need_exit_threadnum;

    /* 管理者线程 */
    pthread_t manager_thread;
    
    int shutdown;                   /* 使能关闭开关 */
} thread_pool;

/* 状态码 */
typedef enum STAT_CODE
{
    MALLOC_ERROR,
    THREAD_CREATE_ERROR,
    THREAD_MUTEX_LOCK_ERROR,
} STAT_CODE;

/* 函数前置声明 */

/* 线程池的线程资源回收 */
static int threadPool_ResourceFree(thread_pool *pool);
/* 线程池的内存释放 */
static int threadPool_Free(thread_pool *pool);
/* 校验线程数的合法性 */
static int checkThreadVaild(int *threadNums);
/* 提供给外部的接口 */
/* 初始化线程池 */
int threadPool_Init(thread_pool **pool, int threadNums,int min_thread_num,int max_thread_num);
/* 生产者- 添加任务 */
int threadPool_addTask(thread_pool *tpool, void *(*func)(void *), void *arg);
/* 销毁线程池 */
int threadPool_Destory(thread_pool *pool);



/* 我是消费者 */
void * thread_func(void *arg)
{
    /* 处理任务 */
    thread_pool *tpool = (thread_pool *)arg;
    
    t_task *task = NULL;

    while (1)
    {
        pthread_mutex_lock(&(tpool->mutex_lock));
        while (tpool->task_size == 0 && !tpool->shutdown)
        {
            pthread_cond_wait(&(tpool->mutex_cond_ptoc), &(tpool->mutex_lock));

            /* todo... */

            /* 清楚指定数量的空闲线程(没有正在处理任务的线程切存活的)，如果要结束的线程个数大于0, 结束线程 */
            if (tpool->need_exit_threadnum > 0)
            {
                printf("consume thread id:%ld is exiting\n", pthread_self());
                /* 线程杀死之后，存活的线程数量就减一 */
                tpool->current_live_threadnum--;
                /* 需要自杀的线程数减少一 */
                tpool->need_exit_threadnum--;
                pthread_mutex_unlock(&(tpool->mutex_lock));
                pthread_exit(NULL);
            }
        }

        /* 强制关闭线程 回收资源 */
        if (tpool->shutdown)
        {
            printf("Workers'thread ID 0x%x is exiting\n", (unsigned int)pthread_self());
            pthread_mutex_unlock(&(tpool->mutex_lock));
            pthread_exit(NULL);
        }

        task = tpool->queueHead->next;      /* 这就是我的任务 */
        tpool->queueHead->next = task->next;
        tpool->task_size--;
        printf("pthread_self get it id:%ld\n", pthread_self());
        pthread_mutex_unlock(&(tpool->mutex_lock));
        pthread_cond_broadcast(&(tpool->mutex_cond_ctop));

        pthread_mutex_lock(&(tpool->mutex_busythrcnt_lock));
        tpool->busy_thread_num++;
        pthread_mutex_unlock(&(tpool->mutex_busythrcnt_lock));

        /* 执行回调函数 */
        task->task_callback(task->arg);
	/* 释放链表节点内存 */
	free(task);
	printf("threadId %ld get task to handle\n", pthread_self());
        pthread_mutex_lock(&(tpool->mutex_busythrcnt_lock));
        tpool->busy_thread_num--;
        pthread_mutex_unlock(&(tpool->mutex_busythrcnt_lock));
    }
    pthread_exit(NULL);
}
/* 校验线程数的合法性 */
static int checkThreadVaild(int *threadNums)
{
    int ret = 0;
    if (*threadNums > 10 || *threadNums <= 0)
    {
        *threadNums = THREAD_NUM;
    }
    return ret;
}

/* 管理者线程 */
void * manager_func(void *arg)
{
    thread_pool * t_pool = (thread_pool*)arg;

    while(1)
    {
        sleep(5);
        pthread_mutex_lock(&t_pool->mutex_lock);
        
	/* 忙碌的线程数 */
        int busy_thread_num = t_pool->busy_thread_num;
        int task_num = t_pool->task_size;
        int current_live_threadnum = t_pool->current_live_threadnum;
        int max_thread_num = t_pool->max_thread_num;
        int min_thread_num = t_pool->min_thread_num;

        /* 增线程 */
        if(busy_thread_num * 2 < task_num  && current_live_threadnum < max_thread_num)
        {
            int add_thread_num  = (max_thread_num - current_live_threadnum) / 2;
            for(int idx = 0 ; idx < add_thread_num ; idx++)
            {
                pthread_create(t_pool->threadId,NULL,thread_func,NULL);

                t_pool->current_live_threadnum++;
            }

        }

        /* 减线程 */
    }
    
}


/* 
 * @brief: 初始化线程池
 * @param1:
 * @param2:   
 */
int threadPool_Init(thread_pool **pool, int threadNums,int min_thread_num,int max_thread_num)
{
    int ret = 0;
    
    checkThreadVaild(&threadNums);

    do 
    {
        thread_pool * t_pool = (thread_pool *)malloc(sizeof(thread_pool) * 1);
        if (!t_pool)
        {
            /* 状态码 */
            return MALLOC_ERROR;
        }
        memset(t_pool, 0, sizeof(thread_pool));
        /* 数组的方式存储线程池里面的线程Id号, 一开始就把线程池最大的容量开辟好 */
        t_pool->threadId = (pthread_t *)malloc(sizeof(pthread_t) * max_thread_num);
        if (t_pool->threadId)
        {
            /* 状态码 */
            return MALLOC_ERROR;
        }
        memset(t_pool->threadId, 0, sizeof(pthread_t) * max_thread_num);

        /* 管理者线程 */
        ret = pthread_create(&(t_pool->manager_thread), NULL, manager_func, t_pool);
        if (ret == -1)
        {
            return THREAD_CREATE_ERROR;
        }

        /* 循环创建工作线程 (初始化时 存活线程数为创建的线程数) */
        for (int idx = 0; idx < threadNums; idx++)
        {
            ret = pthread_create(&(t_pool->threadId[idx]), NULL, thread_func, t_pool);
            if (ret == -1)
            {
                return THREAD_CREATE_ERROR;
            }

        }
        /* todo... */
        pthread_mutex_init(&(t_pool->mutex_lock), NULL);
        pthread_mutex_init(&(t_pool->mutex_busythrcnt_lock), NULL);///
        pthread_cond_init(&(t_pool->mutex_cond_ptoc),NULL);
        pthread_cond_init(&(t_pool->mutex_cond_ctop),NULL);

        /* 使用单向链表实现任务队列 */
        t_pool->queueHead = (t_task *)malloc(sizeof(t_task) * 1);
        if (t_pool->queueHead == NULL)
        {
            printf("malloc is error\n");
            return MALLOC_ERROR;
        }

#if 0
        t_pool->queueHead->task_callback = NULL;
        t_pool->queueHead->arg = NULL;
        t_pool->queueHead->next = NULL;
#else
        memset(t_pool->queueHead, 0, sizeof(t_task));
#endif
        t_pool->task_size = 0;                  /* 任务数 */
        t_pool->busy_thread_num = 0;            /* 忙碌的线程数 (工作的线程数) */    
        t_pool->max_thread_num = max_thread_num;
        t_pool->min_thread_num = min_thread_num;
        t_pool->current_live_threadnum = threadNums;
        /* 使能 让线程自己回收 */
        t_pool->shutdown = 0;   
        
        /* 指针赋值 */
        *pool = t_pool; 
    } while(0);  

    
  	  
    
    return ret;
}

/* 生产者- 添加任务 */
int threadPool_addTask(thread_pool *tpool, void *(*func)(void *), void *arg)
{
    /* 判断参数的合法性 */
    // CHECK_PTR(tpool);

    int ret = 0;    
    pthread_mutex_lock(&(tpool->mutex_lock));     
       
    while(tpool->task_size == TASK_NUM && !(tpool->shutdown))
    {
        pthread_cond_wait(&(tpool->mutex_cond_ctop),&(tpool->mutex_lock));
    }

    if (tpool->shutdown)
    {
        pthread_mutex_unlock(&(tpool->mutex_lock));
        return ret;
    }

    t_task *taskNode = (t_task *)malloc(sizeof(t_task) * 1);
    taskNode->task_callback = func;
    taskNode->arg = arg;
    taskNode->next = NULL;

    /* 将任务节点尾插到任务队列中 */
    t_task *travelTask = tpool->queueHead;
    while (travelTask->next != NULL)
    {
        travelTask = travelTask->next;
    }
    travelTask->next = taskNode;
    /* 任务数量加一 */
    tpool->task_size++;
    pthread_mutex_unlock(&(tpool->mutex_lock));
    pthread_cond_broadcast(&(tpool->mutex_cond_ptoc));

    return ret;
}

/* 销毁线程池 */
int threadPool_Destory(thread_pool *pool)
{
    int ret = 0;
    ret = threadPool_ResourceFree(pool);
    ret = threadPool_Free(pool);
    return ret;
}

/* 
 * @brief: 线程池的资源回收 
 * @param: 
 */
static int threadPool_ResourceFree(thread_pool *pool)
{
    int ret = 0;
    pool->shutdown = 1;

    /* 先销毁管理者线程 */
    pthread_join(pool->manager_thread, NULL);
    for (int idx = 0; idx < pool->current_live_threadnum; idx++)
    {
        pthread_cond_broadcast(&(pool->mutex_cond_ptoc));
    }

    /* 回收工作线程 */
    for (int idx = 0; idx < pool->current_live_threadnum; idx++)
    {
        pthread_join(pool->threadId[idx], NULL);
    }

    return ret;
}

/* 
 * @brief:  线程池的内存释放 
 * @param1: 
 */
static int threadPool_Free(thread_pool *pool)
{   
    int ret = 0;

    /* 释放线程 */
    if (pool->threadId)
    {
        free(pool->threadId);
        pool->threadId = NULL;
    }

    /* 释放队列 */
    while(pool->queueHead->next != NULL)
    {
        t_task *freeNode = pool->queueHead->next;
        pool->queueHead->next = freeNode->next;
        free(freeNode);
    }
    free(pool->queueHead);

    /* 释放锁资源 和 条件变量 */
    pthread_mutex_destroy(&(pool->mutex_lock));
    pthread_mutex_destroy(&(pool->mutex_busythrcnt_lock));
    pthread_cond_destroy(&(pool->mutex_cond_ptoc));
    pthread_cond_destroy(&(pool->mutex_cond_ptoc));

    /* 释放线程池 */
    if (pool)
    {
        free(pool);
        pool = NULL;
    }
    return ret;
}


void * printf_func(void *arg)
{
    printf("hello\n");

    return NULL;
}

int main()
{   
    thread_pool *t_pool = (thread_pool *)malloc(sizeof(thread_pool) * 1);
    memset(t_pool, 0, sizeof(t_pool));             

    threadPool_Init(&t_pool, THREAD_NUM,5,20);
    for (int idx = 0; idx < COUNT_NUM; idx++)
    {
        threadPool_addTask(t_pool, printf_func, NULL);
    }
    threadPool_Destory(t_pool);

    return 0;
}
