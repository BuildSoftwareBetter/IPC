#include "shm_sem_msg_common.h"

int main()
{
    key_t key;
    int shmid, semid, msqid;
    char *shm = nullptr;
    struct msg_form msg;

    int flag = 1;

    // 获取key
    if ((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

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
        perror("shmat error");
        exit(1);
    }

    // 创建消息队列
    if ((msqid = msgget(key, 0)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    // 获取信号量
    if ((semid = semget(key, 0, 0)) == -1)
    {
        perror("semget error");
        exit(1);
    }

    // 写数据
    printf("***************************************\n");
    printf("*                 IPC                 *\n");
    printf("*    Input r to send data to server.  *\n");
    printf("*    Input q to quit.                 *\n");
    printf("***************************************\n");

    while (flag)
    {

        char c;
        printf("Please input command: ");
        scanf("%c", &c);

        switch (c)
        {
        case 'r':
            printf("Data to send: ");
            sem_p(semid); //P操作
            scanf("%s", shm);
            sem_v(semid); //V操作

            printf("1 start send\n");

            // 清空标准输入缓冲区
            while ((c = getchar()) != '\n' && c != EOF)
                ;

            printf("start send\n");
            msg.mtype = SERVER_MSG_TYPE;
            msg.mtext = 'r'; //read 发送数据到服务端，告知服务端有数据可读
            msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
            break;
        case 'q':
            msg.mtype = SERVER_MSG_TYPE;
            msg.mtext = 'q';
            msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
            flag = 0;
            break;
        default:
            printf("wrong input!\n");
            // 清空标准输入缓冲区
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            break;
        }
    }

    // 断开共享内存连接
    shmdt(shm);
    return 0;
}