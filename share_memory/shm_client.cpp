#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>

int main()
{
    key_t key;
    int shmid;
    char *shm = nullptr;

    // 获取key值
    if ((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    // 创建共享内存
    if ((shmid = shmget(key, 1024, IPC_CREAT | 0666)) == -1) // 0666 八进制
    {
        perror("create shared memory error");
        exit(1);
    }

    // // 获取共享内存
    // if ((shmid = shmget(key, 1024, 0)) == -1)
    // {
    //     perror("shmget error");
    //     exit(1);
    // }

    // 连接共享内存
    shm = (char *)shmat(shmid, 0, 0);
    if (nullptr == shm)
    {
        perror("attach shared memory error");
        exit(1);
    }

    // 写数据
    // scanf("%s", shm);
    memcpy(shm, "hello world", 12);

    // 断开连接
    shmdt(shm);

    return 0;
}