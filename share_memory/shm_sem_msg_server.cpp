#include "shm_sem_msg_common.h"

int main()
{
    key_t key;
    int shmid, semid, msqid;

    char *shm = nullptr;
    char data[] = "this is server";
    struct shmid_ds buf1; // 用于删除共享内存
    struct msqid_ds buf2; // 用于删除消息队列
    struct msg_form msg;  // 消息队列，用于通知服务端和客户端共享内存有新信息

    // 获取key值
    if ((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    // 获取共享内存，不存在则创建，有读写权限
    if ((shmid = shmget(key, 1024, IPC_CREAT | 0666)) < 0)
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

    // 创建消息队列
    if ((msqid = msgget(key, IPC_CREAT | 0777)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    // 创建信号量
    semid = creat_sem(key);

    // 读数据
    while (true)
    {
        // 读取消息队列中的数据（阻塞）
        msgrcv(msqid, &msg, 1, SERVER_MSG_TYPE, 0);

        if (msg.mtext == 'q') //quit 退出
            break;
        if (msg.mtext == 'r') //read 读取共享内存
        {
            sem_p(semid); // P操作
            printf("%s\n", shm);
            sem_v(semid); // V操作
        }
    }

    // 断开共享内存连接
    shmdt(shm);

    // 删除共享内存、消息队列、信号量
    shmctl(shmid, IPC_RMID, &buf1);
    msgctl(msqid, IPC_RMID, &buf2);
    del_sem(semid);

    return 0;
}