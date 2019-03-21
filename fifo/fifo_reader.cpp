#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#define FIFO_NAME "/var/tmp/fifo_test"



int main()
{
    int fd;
    int len;
    char buf[1024];

    //int mkfifo(const char * pathname,mode_t mode); mode 文件权限
    if (mkfifo(FIFO_NAME, 0666) < 0 && errno != EEXIST)
        perror("Create FIFO Failed");

    if ((fd = open(FIFO_NAME, O_RDONLY)) < 0)
    {
        perror("Open FIFO Failed");
        exit(1);
    }

    while ((len = read(fd, buf, 1024)) > 0)
    {
        printf("Read message: %s", buf);
    }

    close(fd);
    return 0;
}