#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

// 文件路径
#define MSG_FILE "."

// msg struct
struct msg_form
{
    long mtype;
    long stype; // 数据发送者方的type
    char mtext[256];
};