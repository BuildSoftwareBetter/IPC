/*
利用消息队列，多个客户端与一个服务端通信
*/

#include "msg_mc_common.h"

int main()
{
    int msqId;
    key_t key;
    struct msg_form msg;

    // 服务端在消息队列中的type值
    long mtype = 888;

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
        msgrcv(msqId, &msg, sizeof(msg) - sizeof(long), mtype, 0); // 返回类型为888的第一个消息
        printf("Server: receive msg.mtext is: %s from msg.stype: %d.\n", msg.mtext, msg.stype);
        printf("Server: receive msg.mtype is: %d.\n", msg.mtype);

        // 根据消息数据的前4位得到消息类型，根据消息类型发送消息至不同的客户端
        msg.mtype = msg.stype; // 设置消息类型为发送方的类型
        msg.stype = mtype;
        sprintf(msg.mtext, "hello, I'm server %d", getpid());
        msgsnd(msqId, &msg, sizeof(msg) - sizeof(long), 0); // 发送消息
    }

    return 0;
}