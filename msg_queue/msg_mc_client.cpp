/*
利用消息队列，多个客户端与一个服务端通信，服务端接收到客户端的消息后将数据返回值相应的客户端
*/

#include "msg_mc_common.h"

int main()
{
    int msqId;
    key_t key;
    struct msg_form msg;

    long server_type = 888;

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

    long mtype = getpid();      // 设置当前的消息类型
    // 添加消息
    msg.mtype = server_type;    // 消息接收端type
    msg.stype = mtype;          // 消息发送端type
    sprintf(msg.mtext, "hello, I'm client %d", getpid());
    msgsnd(msqId, &msg, sizeof(msg) - sizeof(long), 0);

    // 读取当前消息类型中的数据
    msgrcv(msqId, &msg, sizeof(msg) - sizeof(long), mtype, 0);
    printf("Client: receive msg.mtext is: %s from msg.stype: %d.\n", msg.mtext, msg.stype);
    printf("Client: receive msg.mtype is: %d.\n", msg.mtype);

    return 0;
}
