/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 21时01分58秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "thread_pool.h"
#include "epoll_server.h"

Thread_pool pool;

int main(int argc, const char* argv[])
{
    if(argc != 2) {
        printf("argc == 2\n");
        exit(1);
    }
    thread_pool_init(&pool);
    int port = atoi(argv[1]);
    // 启动epoll模型 
    epoll_run(port);
    thread_pool_clear(&pool);
    return 0;
}

