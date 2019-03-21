/*
1. 使用共享内存进行进程间通信
2. 使用信号量做共享内存数据读写控制
3. 使用消息队列用于通知对方有新数据
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <string.h>

#define SERVER_MSG_TYPE 888

// 消息队列结构
struct msg_form
{
    long mtype;
    char mtext;
};

// 联合体，共用体，在一个union中可以定义多种不同的数据类型，长度为最长的成员数据类型长度
union semun {
    int val; // for SETVAL
    struct semid_ds *buf;
    unsigned short *array;
};

// 初始化信号量
int init_sem(int sem_id, int value)
{
    union semun tmp;
    tmp.val = value;

    // Semaphore控制
    // 初始化id为sem_id，序号为0的信号量，值为tmp.val
    if (semctl(sem_id, 0, SETVAL, tmp) == -1)
    {
        perror("Init Semaphore Error");
        return -1;
    }

    return 0;
}

// P操作（通过）
// 将信号量值减1
// 若信号量值 < 0，进程挂起等待
int sem_p(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0;        // 序号，取值0 - _nsems-1(根据semget函数的_nsems参数)
    sbuf.sem_op = -1;        // 信号量值在一次操作中的改变量，<0 P操作，判断信号量>=0则表示资源可用，进程继续执行，否则进程置为等待状态
    sbuf.sem_flg = SEM_UNDO; // 退出时撤销操作，IPC_NOWAIT/SEM_UNDO

    // Semaphore操作
    if (semop(sem_id, &sbuf, 1) == -1)
    {
        perror("P operation Error");
        return -1;
    }

    return 0;
}

// V操作（释放）
// 释放资源并将信号量值+1
// 如果有进程正在挂起等待，则唤醒他们
int sem_v(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = 1; // V操作，释放信号量资源，并将信号量值+1
    sbuf.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sbuf, 1) == -1)
    {
        perror("V operation Error");
        return -1;
    }

    return 0;
}

// 删除信号量集
int del_sem(int sem_id)
{
    union semun tmp;
    if (semctl(sem_id, 0, IPC_RMID, tmp) == -1)
    {
        perror("Delete Semaphore Error");
        return -1;
    }

    return 0;
}

// 创建一个信号量集
int creat_sem(key_t key)
{
    int sem_id;
    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1)
    {
        perror("semget error");
        exit(-1);
    }
    init_sem(sem_id, 1); /*初值设为1资源未占用*/
    return sem_id;
}