
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

// 文件路径
#define MSG_FILE "/etc/passwd"

// msg struct
struct msg_form
{
    long mtype;
    char mtext[256];
};

int main()
{
    int msqId;
    key_t key;
    struct msg_form msg;

    // 获取key值
    if ((key = ftok(MSG_FILE, 'z')) < 0)
    {
        perror("Fork Error");
        exit(1);
    }

    printf("Message Queue - Client key is: %d.\n", key);

    // 打开消息队列
    //int msgget (key_t __key, int __msgflg) __msgflg: 消息队列的建立标志与存取权限
    if ((msqId = msgget(key, IPC_CREAT | 0777)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    // 打印消息队列ID及进程ID
    printf("My msqid is: %d.\n", msqId);
    printf("My pid is: %d.\n", getpid());

    // 添加消息，类型为888
    msg.mtype = 888;
    sprintf(msg.mtext, "hello, I'm client %d", getpid());
    msgsnd(msqId, &msg, sizeof(msg.mtext), 0);

    // 读取类型为999的消息
    msgrcv(msqId, &msg, 256, 999, 0);
    printf("Client: receive msg.mtext is: %s.\n", msg.mtext);
    printf("Client: receive msg.mtype is: %d.\n", msg.mtype);

    return 0;
}