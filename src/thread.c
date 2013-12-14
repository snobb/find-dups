/*
 *  thread.c
 *  Author: Alex Kozadaev (2013)
 */

#include "finddup.h"

/* initalize the thread structure */
void thread_init(struct thread_ctx *thread)
{
    thread->tid = 0;
    thread->isrunning = thread->isdetached = false;
}

/* create a thread */
int thread_start(struct thread_ctx *thread, void *(*cb)(void*), void *arg)
{
    int res = pthread_create(&thread->tid, NULL, cb, arg);
    thread->isrunning = (res == 0) ? true : false;
    return res;
}

/* join thread */
int thread_join(struct thread_ctx *thread)
{
    int res = -1;
    if (thread->isrunning) {
        res = pthread_join(thread->tid, NULL);
        thread->isdetached = (res == 0) ? true : false;
    }
    return res;
}

/* detach thread */
int thread_detach(struct thread_ctx *thread)
{
    int res = -1;
    if (thread->isrunning && !thread->isdetached) {
        res = pthread_detach(thread->tid);
    }
    return res;
}

/* stop thread and cleanup */
void thread_stop(struct thread_ctx *thread)
{
    if (thread->isrunning && !thread->isdetached) {
        pthread_detach(thread->tid);
    }
    if (thread->isrunning) {
        pthread_cancel(thread->tid);
    }
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0:0 fo=crtocl list */

