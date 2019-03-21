/*
本地socket实现IPC
    1. 虽然网络socket也可用于同一台主机的进程间通讯(通过 loopback 地址 127.0.0.1)，但是 UNIX domain socket 用于 IPC 更有效率：不需要经过网络协议栈，不需要打包拆包、计算校验和、维护序号和应答等，只是将应用层数据从一个进程拷贝到另一个进程。这是因为，IPC 机制本质上是可靠的通讯，而网络协议是为不可靠的通讯设计的。
    2. UNIX domain socket 是全双工的，API 接口语义丰富，相比其它 IPC 机制有明显的优越性，目前已成为使用最广泛的 IPC 机制，比如 X Window 服务器和 GUI 程序之间就是通过 UNIX domain socket 通讯的。
    3. Unix domain socket 是 POSIX 标准中的一个组件。

流程：
    1. 创建本地socket（参数 AF_UNIX）
    2. 检查socket地址文件是否存在，存在则删除，unlink删除一个文件的目录项并减少它的链接数
    3. 绑定socket，并根据socket地址创建本地文件
    4. 监听listen
    5. 接收客户端的连接accept
    6. 读/写数据read/write
    7. 关闭socket close
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAXLINE 80

char *socket_path = "server.socket";

int main(void)
{
    struct sockaddr_un serun, cliun;
    socklen_t cliun_len;
    int listenfd, connfd, size;
    char buf[MAXLINE];
    int i, n;

    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    printf("%s\n", socket_path);

    memset(&serun, 0, sizeof(serun));
    serun.sun_family = AF_UNIX;
    strcpy(serun.sun_path, socket_path);
    size = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
    unlink(socket_path);

    // bind时将会创建socket文件，有可能会因为权限文件导致bind失败
    if (bind(listenfd, (struct sockaddr *)&serun, size) < 0)
    {
        perror("bind error");
        exit(1);
    }
    printf("UNIX domain socket bound\n");

    if (listen(listenfd, 20) < 0)
    {
        perror("listen error");
        exit(1);
    }
    printf("Accepting connections ...\n");

    while (1)
    {
        cliun_len = sizeof(cliun);
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliun, &cliun_len)) < 0)
        {
            perror("accept error");
            continue;
        }

        while (1)
        {
            n = read(connfd, buf, sizeof(buf));
            if (n < 0)
            {
                perror("read error");
                break;
            }
            else if (n == 0)
            {
                printf("EOF\n");
                break;
            }

            printf("received: %s", buf);

            for (i = 0; i < n; i++)
            {
                buf[i] = toupper(buf[i]);
            }
            write(connfd, buf, n);
        }
        close(connfd);
    }
    close(listenfd);
    return 0;
}