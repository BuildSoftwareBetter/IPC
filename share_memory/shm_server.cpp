#include <stdio.h>
#include <stdlib.h> // 标准库函数头文件
#include <sys/shm.h>

/*
共享内存：
    1. 共享内存是一种最快的IPC，因为进程是直接对内存进行存取
    2. 多个进程可用同时操作，所以需要进行同步
    3. 信号量+共享内存通常结合一起使用，信号量可用来同步对共享内存的访问
    4. 进程独立，共享内存不删除将一直保留在系统中

流程：
    1. 创建IPC的key
    2. 创建共享内存/获取共享内存shmget
    3. 连接共享内存shmat
    4. 读写数据（和普通内存一样读写）
    5. 断开连接shmdt
    6. 删除共享内存shmctl

*/

int main()
{
    key_t key;
    int shmid;
    char *shm = nullptr;
    struct shmid_ds buf1; /*用于删除共享内存*/

    if ((key = ftok(".", 'z')) < 0)
    {
        perror("fork error");
        exit(1);
    }

    // // 创建共享内存
    // if ((shmid = shmget(key, 1024, IPC_CREAT | 0666)) == -1) // 0666 八进制
    // {
    //     perror("create shared memory error");
    //     exit(1);
    // }

    // 获取共享内存
    if ((shmid = shmget(key, 1024, 0)) == -1)
    {
        perror("shmget error");
        exit(1);
    }

    // 连接共享内存
    shm = (char *)shmat(shmid, 0, 0);
    if (nullptr == shm)
    {
        perror("attach shared memory error");
        exit(1);
    }

    // 读数据
    printf("%s\n", shm);

    // 断开连接
    shmdt(shm);

    // 删除共享内存
    shmctl(shmid, IPC_RMID, &buf1);

    return 0;
}