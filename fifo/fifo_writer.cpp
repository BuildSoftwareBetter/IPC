#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define FIFO_NAME "/var/tmp/fifo_test"

/*
FIFO 命名管道：
    1. 可在无关的进程间交换数据
    2. 与特定路径名关联，以一种特殊的设备文件形式存在于文件系统中
    3. 在创建命名管道的进程关闭后释放

流程：
    1. 创建命名管道mkfifo
    2. 打开FIFO命名管道open
    3. 写数据write/读数据read
    4. 关闭命名管道close

每个客户端和服务端都创建一个FIFO管道（根据文件名区分，客户端发送消息时带上文件名），可实现一个服务端对多个客户端的通信
*/

int main()
{
    int fd;
    int n, i;
    char buf[1024];
    time_t tp;

    if ((fd = open(FIFO_NAME, O_WRONLY)) < 0)
    {
        perror("Open FIFO Failed");
        exit(1);
    }

    for (size_t i = 0; i < 10; i++)
    {
        time(&tp);
        n = sprintf(buf, "Process %d's time is %s", getpid(), ctime(&tp));
        printf("Send message: %s", buf);
        if (write(fd, buf, n + 1) < 0)
        {
            perror("Write FIFO Failed");
            close(fd);
            exit(1);
        }
        sleep(1);
    }

    close(fd);
    return 0;
}