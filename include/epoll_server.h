/*************************************************************************
	> File Name: epoll_server.h
	> Author: 
	> Mail: 
	> Created Time: 2020年11月09日 星期一 20时56分31秒
 ************************************************************************/

#ifndef _EPOLL_SERVER_H
#define _EPOLL_SERVER_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>

int init_listen_fd(int port, int epfd);
void epoll_run(int port);
void do_accept(int lfd, int epfd);
void *do_read(void *arg);
int get_line(int sock, char *buf, int size);
void disconnect(int cfd, int epfd);
void http_request(const char* request, int cfd);
void send_respond_head(int cfd, int no, const char* desp, const char* type, long len);
void send_file(int cfd, const char* filename);
void send_dir(int cfd, const char* dirname);
void encode_str(char* to, int tosize, const char* from);
void decode_str(char *to, char *from);
const char *get_file_type(const char *name);
void *do_run(void *epfd);


#endif
