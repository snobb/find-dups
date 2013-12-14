/*
 *  wqueue.c
 *  Author: Alex Kozadaev (2013)
 */

#include "finddup.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* queue node - linked list */
struct wnode {
    struct wqueue_item *item;   /* connection context */
    struct wnode *next;         /* next node */
};

/* pthread mutex and conditional variable */
static pthread_mutex_t  *g_mutex;
static pthread_cond_t   *g_condv;

/* global variables */
static struct wnode *g_first, *g_last;
static size_t g_size = 0;
static bool isinitialized = false;

/* prototypes */
static struct wnode *wqueue_make_node(struct wqueue_item *item);
static void wqueue_free_node(struct wnode *node);

/* initialize the working queue */
void wqueue_init(pthread_mutex_t *mutex, pthread_cond_t *condv)
{
    g_first = g_last = NULL;
    g_size = 0;
    g_mutex = mutex;
    g_condv = condv;
    pthread_mutex_init(g_mutex, NULL);
    pthread_cond_init(g_condv, NULL);
    isinitialized = true;
}

/* destroy the queue */
void wqueue_destroy(void )
{
    struct wnode *next;
    for(; g_first != NULL; g_first = next) {
        next = g_first->next;
        g_first->item->destroy(g_first->item);
        wqueue_free_node(g_first);
    }
    g_first = g_last = NULL;
    g_size = 0;
    pthread_mutex_destroy(g_mutex);
    pthread_cond_destroy(g_condv);
    isinitialized = false;
}

/* enqueue connection */
void wqueue_enqueue(struct wqueue_item *item)
{
    struct wnode *new;

    if (!isinitialized) {
        fputs("queue has not been initialized", stderr);
        return;
    }

    pthread_mutex_lock(g_mutex);       /* locking */

    new = wqueue_make_node(item);
    if (g_last) {
        g_last->next = new;
        g_last = g_last->next;  /* last element */
    } else {
        g_first = g_last = new; /* first element */
    }
    g_size++;

    pthread_cond_signal(g_condv);
    pthread_mutex_unlock(g_mutex);     /* unlocking */
}

/* dequeue connection. Block if empty */
struct wqueue_item* wqueue_dequeue(void)
{
    struct wqueue_item *item;
    struct wnode *first;

    if (!isinitialized) {
        fputs("queue has not been initialized", stderr);
        return NULL;
    }

    pthread_mutex_lock(g_mutex);       /* locking */
    while (g_size == 0) {
        pthread_cond_wait(g_condv, g_mutex);
    }
    first = g_first;
    item = first->item;
    g_first = first->next;
    wqueue_free_node(first);        /* freeing the first node */
    g_size--;
    if (g_size == 0) {
        g_first = g_last = NULL;
    }

    pthread_mutex_unlock(g_mutex);     /* unlocking */
    return item;
}

/* get the queue size */
size_t wqueue_getsize(void)
{
    if (!isinitialized) {
        fputs("queue has not been initialized", stderr);
    }

    return g_size;
}

/* make a new node */
static struct wnode* wqueue_make_node(struct wqueue_item *item)
{
    struct wnode *new;

    new = malloc(sizeof(*new));
    CHECK_MEM(new);
    new->item = item;
    new->next = NULL;
    return new;

error:
    exit(1);
}

/* free a node */
static void wqueue_free_node(struct wnode *node)
{
    free(node);
}

/* vim: ts=4 sts=8 sw=4 smarttab et si tw=80 ci cino+=t0(0:0 fo=crtocl list */

