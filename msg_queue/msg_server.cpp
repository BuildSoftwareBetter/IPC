
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

/*
消息队列：
    1. 消息具有特定的格式以及特定的优先级
    2. 消息队列独立于收发进程，进程终止时，消息队列的内容并不会删除
    3. 消息队列可实现消息的随机查询，消息不一定要以先进先出的顺序读取，也可以按消息的类型读取

流程：
    1. 创建IPC的key ftok
    2. 创建消息队列msgget
    3. 发送消息msgsnd
    4. 接收消息msgrcv
*/

// 文件名
#define MSG_FILE "/etc/passwd"

// msg struct
struct msg_form
{
    long mtype;     // 第一个字段必须为long类型
    char mtext[256];
};

int main()
{
    int msqId;
    key_t key;
    struct msg_form msg;

    //获取key值，系统建立IPC通讯 （消息队列、信号量和共享内存） 时必须指定一个ID值。通常情况下，该id值通过ftok函数得到。
    //key_t ftok(const char *__pathname,int __proj_id)  // __pathname: 指定的文件名称，__proj_id: 子序号
    if ((key = ftok(MSG_FILE, 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    printf("Message Queue - Server key is: %d.\n", key);

    // 创建消息队列
    if ((msqId = msgget(key, IPC_CREAT | 0777)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    // 打印消息队列ID及进程ID
    printf("My msqid is: %d.\n", msqId);
    printf("My pid is: %d.\n", getpid());

    // 循环读取消息
    for (;;)
    {
        msgrcv(msqId, &msg, 256, 888, 0); // 返回类型为888的第一个消息
        printf("Server: receive msg.mtext is: %s.\n", msg.mtext);
        printf("Server: receive msg.mtype is: %d.\n", msg.mtype);

        msg.mtype = 999;    // 设置消息类型为999
        sprintf(msg.mtext, "hello, I'm server %d", getpid());
        msgsnd(msqId, &msg, sizeof(msg.mtext), 0);  // 发送消息
    }

    return 0;
}