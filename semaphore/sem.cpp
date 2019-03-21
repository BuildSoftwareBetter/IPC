#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

/*
信号量：
    1. 通过系统的PV操作解决进程同步和互斥
    2. 进程独立，信号量不删除将会一直保留在系统中，即使程序已退出

流程：
    1. 使用ftok创建IPC的key值
    2. 创建信号量semctl（参数 IPC_CREAT，信号量初始值）
    3. 初始化信号量semctl （参数 SETVAL，信号量初始值）
    4. P操作 semop（sembuf.sem_op = -1)
    5. V操作 semop（sembuf.sem_op = 1)
    6. 删除信号量 semctl（参数 IPC_RMID）
*/


// 联合体，用于semctl初始化
union semun {
    int val;
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
    sbuf.sem_num = 0; // 序号，取值0 - _nsems-1(根据semget函数的_nsems参数)
    sbuf.sem_op = -1; // 信号量值在一次操作中的改变量，<0 P操作，判断信号量>=0则表示资源可用，进程继续执行，否则进程置为等待状态
    sbuf.sem_flg = SEM_UNDO;    // 退出时撤销操作，IPC_NOWAIT/SEM_UNDO

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
    sbuf.sem_op = 1;    // V操作，释放信号量资源，并将信号量值+1
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

int main()
{
    int sem_id; // 信号量集id
    key_t key;
    pid_t pid;

    // 获取key值
    if ((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    // 创建信号量集，_nsems 信号量个数
    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1)
    {
        perror("semget error");
        exit(1);
    }

    // 初始化：初始设为0资源被占用
    init_sem(sem_id, 0);

    // 创建子进程
    if ((pid = fork()) == -1)
    {
        perror("Fork Error");
    }
    else if (pid == 0) // 子进程
    {
        sleep(2);
        printf("Process child: pid = %d.\n", getpid());
        sem_v(sem_id); // 释放资源
    }
    else
    {
        sem_p(sem_id);
        printf("Process father: pid = %d.\n", getpid());
        sem_v(sem_id);   // 释放资源
        del_sem(sem_id); // 删除信号量集
    }

    return 0;
}