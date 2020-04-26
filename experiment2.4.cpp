//
// Created by Godlowd on 2020/4/26.
//
#include <cstdio>
#include <windows.h>
#include <cstdlib>
#define MAX 5

typedef struct goods{
    int val=0;
    int index=0;
    struct goods *next = NULL;
} Goods;

Goods* inilist();
Goods* head;
CRITICAL_SECTION cs;//创建临界区变量
HANDLE empty, full;
//semaphore empty = MAX;
//semaphore full = 0;
int produce_time = 500;
int consume_time = 500;
//生产者线程
DWORD WINAPI producer(void)
{
    while(produce_time-- >0){
    WaitForSingleObject(empty, INFINITE);
    //进入临界区
    EnterCriticalSection(&cs);
    Goods* temp;
    temp = head;
    //寻找val值为0的节点,并写入数据
    while(temp->val!=0)
        temp = temp->next;
    temp->val = rand()%10+1;
    printf("写入数据: %d\n", temp->val);
    //离开临界区
    LeaveCriticalSection(&cs);
    ReleaseSemaphore(full,1,NULL);
    }
    return 0;
}
//消费者线程
DWORD WINAPI consumer(void)
{
    while(consume_time-- > 0)
    {
        WaitForSingleObject(full, INFINITE);
        //进入临界区
        EnterCriticalSection(&cs);
        Goods* temp;
        temp = head;
        //寻找val值不为0的节点,并读取数据
        while(temp->val!=0&&(temp->next!=NULL&&temp->next->val!=0))
            temp = temp->next;
        printf("\t\t\t\t\t\t\t\t读取数据: %d\n",temp->val);
        temp->val = 0;
        //离开临界区
        LeaveCriticalSection(&cs);
        ReleaseSemaphore(empty,1,NULL);
    }
    return 0;
}
int main(void)
{
    full = CreateSemaphoreA(NULL,0, MAX,NULL );
    empty = CreateSemaphoreA(NULL,5, MAX,NULL );
    head = inilist();
    HANDLE consumeThread[MAX];
    HANDLE produceThread[MAX];
    InitializeCriticalSection(&cs);
    DWORD ThreadID;
    for(int i=0; i < MAX; i++)
    {
        produceThread[i]=CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(producer), NULL, 0, &ThreadID);
        consumeThread[i]=CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(consumer), NULL, 0, &ThreadID);

    }
    WaitForMultipleObjects(5,consumeThread, TRUE, INFINITE);
    WaitForMultipleObjects(5, produceThread, TRUE,INFINITE);
    DeleteCriticalSection(&cs);

    return 0;
}


//初始化链表
Goods* inilist()
{
    Goods *node, *head, *end;
    head = (Goods*)malloc(sizeof(Goods));
    head->val=0;
    head->index=0;
    end = head;
    for (int i = 0; i < MAX; ++i) {
        node = (Goods*)malloc(sizeof(Goods));
        node->index=i+1;
        node->val=0;
        end->next = node;
        end = node;
    }
    end->next = NULL;
    return head;
}

