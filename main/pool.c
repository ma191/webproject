/*************************************************************************
	> File Name: pool.c
	> Author: 
	> Mail: 
	> Created Time: 2020年11月12日 星期四 16时51分23秒
 ************************************************************************/

#include<stdio.h>
#include <signal.h>
#include "thread_pool.h"
#include "epoll_server.h"
void *thread_run(void *arg) {
    Thread_pool *pool = (Thread_pool *)arg;
    while(1) {
        pthread_mutex_lock(&pool->mutex);
        pool->wait_cnt++;
        while (!pool->tasks_cnt && !pool->exit_flag) {
            pthread_cond_wait(&pool->have_task, &pool->mutex);
            if (pool->exit_flag) {
                pool->wait_cnt--;
                pthread_mutex_unlock(&pool->mutex);
                pthread_exit(NULL);
                return NULL;
            }
        }
        pool->wait_cnt--;
        Task *task = &pool->tasks[pool->head];
        pool->cnt++;
        pool->tasks_cnt--;
        pool->head = (pool->head + 1) % 20;
        pthread_mutex_unlock(&pool->mutex);
        task->run(task->arg);
        pool->cnt--;
        pthread_mutex_lock(&pool->mutex);
        if (pool->cnt == 0 && pool->exit_flag) {
            pthread_cond_signal(&pool->over);                
        }
        pthread_mutex_unlock(&pool->mutex);
    }
    return NULL;
}

int thread_is_alive(pthread_t thread) {
    return pthread_kill(thread, 0) == 0 ? 0 : 1;
}

int add_task(Thread_pool *pool, void *(*run)(void *args), void *arg) {
    Task task;
    task.run = run;
    task.arg = arg;
    
    pthread_mutex_lock(&pool->mutex);
    pool->tasks_cnt++;
    memcpy(&pool->tasks[pool->tail], &task, sizeof(task));
    pool->tail = (pool->tail + 1) % 20;
    if (pool->exit_flag) {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }
    if (pool->wait_cnt > 0) {
        pthread_cond_signal(&pool->have_task);
    } else if (pool->cnt < pool->max_thread_cnt) {
        for (int i = 0; i < pool->max_thread_cnt; i++) {
            if (!thread_is_alive(pool->threads[i])) {
                pthread_create(&pool->threads[i], NULL, thread_run, pool);
            }
        }
    }
    pthread_mutex_unlock(&pool->mutex);
    return 1;
}

void thread_pool_clear(Thread_pool *pool) {
	if (pool->exit_flag) return ;
    pthread_mutex_lock(&pool->mutex);
    pool->exit_flag = 1;
    if (pool->cnt == 0) {
        pthread_cond_signal(&pool->have_task);
    }
    if (pool->cnt > 0) {
        pthread_cond_wait(&pool->over, &pool->mutex);
    }
    for (int i = 0; i < pool->max_thread_cnt; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);
    free(pool->tasks);
    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_destroy(&pool->have_task);
    pthread_cond_destroy(&pool->over);
    pthread_mutex_destroy(&pool->mutex);
    return ;
}

void thread_pool_init(Thread_pool *pool) {
    pool->cnt = 0;
    pool->max_thread_cnt = 50;
    pool->wait_cnt = 0;
    pool->exit_flag = 0;
    pool->tasks_cnt = 0;
    pool->head = 0;
    pool->tail = 0;
    pool->tasks = (Task *)malloc(sizeof(Task) * 20);
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->have_task, NULL);
    pthread_cond_init(&pool->over, NULL);
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * pool->max_thread_cnt);
    for (int i = 0; i < pool->max_thread_cnt; i++) {
        pthread_create(&pool->threads[i], NULL, thread_run, pool);    
    }
	return ;
}

