/*
 * Copyright (C) 2014 FillZpp
 */


#include "ucoroutine.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>

#define U_READY 0
#define U_RUNNING 1
#define U_SUSPEND 2
#define U_NEW 3


struct u_coroutine {
    int status;
    size_t id;
    pthread_mutex_t mutex;
    ucontext_t ctx;
    cor_func_t func;
    void *arg;
    char *stack;
    struct u_coroutine *next;
    pthread_t *ptd;
};

struct u_sched {
    struct u_coroutine *cor_new;
    struct u_coroutine *cor_ready;
    struct u_coroutine *cor_running;
    struct u_coroutine *cor_suspend;
    pthread_mutex_t mutex_new;
    pthread_mutex_t mutex_ready;
    pthread_mutex_t mutex_running;
    pthread_mutex_t mutex_suspend;
    pthread_cond_t cond;
};


/* global variables */
static int status = U_READY;
static size_t u_cor_id = 1;
static pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
static ptd_cap = 0;
static pthread_t **ptd_pool = NULL;
static struct u_sched *schedule = NULL;
static __thread pthread_t *current_ptd = NULL;

/* threads static functions */
static void u_run_coroutine(struct u_coroutine *cor);
static void u_thread_func();
static void u_pool_init(int num);
/* schedule static functions */



/* ====================================================== */
/*             Definition for threads function            */
/* ====================================================== */
static void u_run_coroutine(struct u_coroutine *cor)
{
    cor->status = U_RUNNING;
    cor->ptd = current_ptd;
}

static void u_thread_func(void *ptd)
{
    struct u_coroutine *cor;
    current_ptd = ptd;
    while (status) {
        pthread_mutex_lock(&schedule->mutex_ready);
        while (schedule->cor_ready == NULL)
            pthread_cond_wait(&schedule->cond, &schedule->mutex_ready);

        cor = schedule->cor_ready;
        schedule->cor_ready = cor->next;
        pthread_mutex_lock(&schedule->mutex_running);
        cor->next = schedule->cor_running;
        schedule->cor_running = cor;
        pthread_mutex_unlock(&schedule->mutex_running);
        pthread_mutex_unlock(&schedule->mutex_ready);
        u_run_coroutine(cor);
    }
}

static void u_pool_init(int num)
{
    int i;
    pthread_t *ptd;
    ptd_cap = num;
    ptd_pool = malloc(num*sizeof(ptd));
    for (i=0; i<num; ++i) {
        ptd = malloc(sizeof(*ptd));
        ptd_pool[i] = ptd;
        pthread_create(ptd, NULL, (void*)u_thread_func, (void*)ptd);
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
        /* Initialize schedule */
        schedule = malloc(sizeof(*schedule));
        pthread_mutex_init(&schedule->mutex_new, NULL);
        pthread_mutex_init(&schedule->mutex_ready, NULL);
        pthread_mutex_init(&schedule->mutex_running, NULL);
        pthread_mutex_init(&schedule->mutex_suspend, NULL);
        pthread_cond_init(&schedule->cond, NULL);
        schedule->cor_ready = NULL;
        /* Initialize threads pool */
        if (num > 0)
            u_pool_init(num);

        ret = 0;
        status = U_RUNNING;
    }
    pthread_mutex_unlock(&mutex_);

    return ret;
}


/* ====================================================== */
/*          Definition for coroutine functions            */
/* ====================================================== */
u_coroutine_t *u_coroutine_register(cor_func_t func, void *arg)
{
    if (status)
        return NULL;
    struct u_coroutine *cor = malloc(sizeof(*cor));
    pthread_mutex_lock(&mutex_);
    cor->id = u_cor_id++;
    pthread_mutex_unlock(&mutex_);
    pthread_mutex_init(&cor->mutex, NULL);
    cor->func = func;
    cor->arg = arg;
    cor->status = U_NEW;
    pthread_mutex_lock(&schedule->mutex_new);
    cor->next = schedule->cor_new;
    schedule->cor_new = cor;
    pthread_mutex_unlock(&schedule->mutex_new);
    return cor;
}

int u_coroutine_resume(u_coroutine_t *cor)
{
    assert(cor);
    if (status)
        return -1;
    
}

    
