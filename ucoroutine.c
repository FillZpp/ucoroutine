/*
 * Copyright (C) 2014 FillZpp
 */

#include <stddef.h>      // for size_t

#include "ucoroutine.h"

#define U_READY 0
#define U_RUNNING 1


struct u_coroutine {
    struct u_coroutine *next;
    cor_func_t func;
    void *arg;
};

struct u_sched {
    size_t count;
    struct u_coroutine *cor_ready;
    pthread_mutex_t mutex;
};


/* global variables */
static int status = U_READY;
static int cor_id = 0;
static pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
static int u_threads_cap = 0;
static pthread_t **pool = NULL;
static struct u_sched *schedule = NULL;

/* threads static functions */
static void u_run_coroutine(struct u_coroutine *cor);
static void u_thread_func();
static void u_pool_init(int num);
/* schedule static functions */



/* ====================================================== */
/*             Definition for threads function            */
/* ====================================================== */
void u_thread_func()
{
    struct u_coroutine *cor;
    while (status) {
        pthread_mutex_lock(&schedule->mutex);
        while (schedule->count == 0)
            pthread_cond_wait(&cond_, &schedule->mutex_);
        
        if (schedule->cor_ready != NULL) {
            cor = schedule->cor_ready;
            schedule->cor_ready = cor->next;
            schedule->count--;
            
            u_run_coroutine(cor);
        }
        pthread_mutex_unlock(&schedule->mutex);
    }
}

void u_pool_init(int num)
{
    int i;
    pthread_t *ptd;
    u_thread_cap = num;
    
    for (i=0; i<num; ++i) {
        ptd = malloc(sizeof(*ptd));
        pool[i] = ptd;
        pthread_create(ptd, NULL, (void*)u_thread_func, NULL);
    }
}


/* ====================================================== */
/*             Definition for schedule functions          */
/* ====================================================== */
int u_sched_open(int num)
{
    int ret = -1;
    pthread_mutex_lock(&mutex_);
    if (status == U_READY) {
        cor_id = 0;
        /* Initialize schedule and pool */
        schedule = malloc(sizeof(*schedule));
        pthread_mutex_init(&schedule->mutex);
        pthread_cond_init(&schedule->cond, PTHREAD_PROCESS_PRIVATE);
        schedule->count = 0;
        schedule->cor_ready = NULL;
        if (num > 0)
            u_pool_init(num);

        ret = 0;
        status = U_RUNNING;
    }
    pthread_mutex_unlock(&mutex_);

    return ret;
}


    
