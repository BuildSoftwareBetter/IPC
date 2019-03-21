#include <stdio.h>
#include <unistd.h>

// 宏定义带参数
// #define a(a, b) (a > b ? a : b)
// #define cc(a) (a++)

/*
(匿名)管道：
    1. 半双工（数据只能在一个方向上流动：父->子、子->父），具有固定的读写端；可创建两个管道实现双向通信
    2. 只能用于父子或兄弟进程间通信
    3. 可以使用write、read等函数，但不属于任何文件系统，只存在与内存中

流程：
    1. 创建管道pipe
    2. 关闭读或写端close
    3. 写数据write
    4. 读数据read
*/

int main()
{
    int fd[2]; // 两个文件描述符，fd[0]读，fd[1]写
    pid_t pid;
    char buff[20] = {0};
    int i = 0;

    if (pipe(fd) < 0) // 创建管道
    {
        printf("Create Pipe Error \n");
        return -1;
    }

    // fork()拷贝创建新进程，将会同时在主进程和子进程中返回pid，主进程返回的pid>0，子进程返回的pid=0
    if ((pid = fork()) < 0)
    {
        printf("Fork Error!\n");
        return -1;
    }
    else if (pid > 0) //父进程
    {
        close(fd[0]);   //关闭读端
        while (i < 10)
        {
            sprintf(buff, "send times %d", i);
            write(fd[1], buff, 12);
            i++;
            sleep(1);
        }
    }
    else
    {
        close(fd[1]);   // 关闭写端
        while (i < 10)
        {
            // sleep(1);
            read(fd[0], buff, 20);
            printf("%s \n", buff);
            i++;
        }
    }

    return 0;
}