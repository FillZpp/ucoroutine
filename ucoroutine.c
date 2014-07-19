/*
 * Copyright (C) 2014 FillZpp
 */

#include "ucoroutine.h"

#define U_READY 0
#define U_RUNNING 1


struct u_thread {
    int status;
    struct u_thread *next;
    struct u_thread *prev;
    pthread_t thread;
    pthread_mutex_t mutex;
};

struct u_threads_pool {
    int cap;
    struct u_thread *head;
};


/* global variables */
static int status = U_READY;
static pthread_mutex_t mutex_;
static struct u_threads_pool *pool;
/* threads static functions */
void u_pool_init(int num);
static struct u_thread *u_get_thread();
static struct u_thread *u_get_thread_noblock();
/* schedule static functions */



/* ====================================================== */
/*             Definition for threads function            */
/* ====================================================== */
void u_pool_init(int num)
{
    int i;
    pool = malloc(sizeof(*pool));

    /* Initialize threads */
    for (i=0; i<num; ++i) {
        struct u_thread *node = malloc(sizeof(*node));
        node->mutex = PTHREAD_MUTEX_INITIALIZER;
        node->status = U_READY;
        
        if (i == 0) {
            pool->head = node;
            node->prev = node;
            node->next = node;
        } else {
            node->next = pool->head;
            node->prev = pool->head->prev;
            pool->head->prev->next = node;
            pool->head->prev = node;
        }
    }
}

struct u_thread *u_get_thread()
{
    struct u_thread *node = pool->head;
    struct u_thread *ret = NULL;

    while (ret == NULL) {
        pthread_mutex_lock(&node->mutex);
        if (node->status == U_READY) {
            node->status = U_RUNNING;
            ret = node;
        }
        pthread_mutex_unlock(&node->mutex);
        node = node->next;
    }
    return ret;
}

struct u_thread *u_get_thread_noblock()
{
    struct u_thread *node = pool->head;
    struct u_thread *ret = NULL;

    do {
        pthread_mutex_lock(&node->mutex);
        if (node->status == U_READY) {
            node->status == U_RUNNING;
            ret = node;
        }
        pthread_mutex_unlock(&node->mutex);
        node = node->next;
    } while (ret == NULL && node != pool->head);

    return ret;
}


/* ====================================================== */
/*             Definition for schedule functions          */
/* ====================================================== */
int u_sched_open(int num)
{
    int ret = -1;
    pthread_mutex_lock(&mutex_);
    if (status == U_READY) {
        ret = 0;
        status = U_RUNNING;
        if (num > 0)
            u_pool_init(num);
    }
    pthread_mutex_unlock(&mutex_);

    return ret;
}


    
