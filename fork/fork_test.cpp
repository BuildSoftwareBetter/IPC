#include <unistd.h>
#include <stdio.h>

/*
fork()
    1. 函数将创建一个与原来进程几乎完全相同的进程
    2. fork创建失败时返回-1，创建成功将会有两个返回值（0 代表子进程；> 0代表父进程，值为子进程id）
    3. fork将可能产生孤儿进程和僵尸进程
    4. 父进程退出但子进程没有退出，子进程将成为孤儿进程；孤儿进程将会由init进程处理，不影响系统运行，孤儿进程的父进程id为1
    5. 子进程退出，而父进程没有调用wait或waitpid获取子进程的状态信息，那么子进程的进程描述符仍然保存系统中称为僵尸进程；僵尸进程会一直占用进程号，而系统进程号是有限的，将会导致系统异常
    6. 通过signal()函数（signal(SIGCHLD,sig_child)）处理僵尸进程。
*/

int main()
{
    pid_t pid;
    printf("current pid : %d\n", getpid());

    // 创建子进程
    pid = fork();

    if (pid == -1) //创建失败
    {
        return -1;
    }
    else if (pid > 0) // 父进程
    {
        printf("parent pid : %d\n", getpid());
        sleep(5);
    }
    else // 子进程
    {
        printf("child pid : %d , parent pid : %d\n", getpid(), getppid());
        sleep(20);
        printf("child pid : %d , parent pid : %d\n", getpid(), getppid());
    }

    return 0;
}
