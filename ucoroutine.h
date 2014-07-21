/*
 * Copyright (C) 2014 FillZpp
 */

#ifndef UCOROUTINE_UCOROUTINE_H_
#define UCOROUTINE_UCOROUTINE_H_

#include <pthread.h>


struct u_coroutine;
typedef struct u_coroutine u_coroutine_t;
typedef void (*cor_func_t)(void *arg);

/* schedule control functions */
int u_sched_open(int num);
int u_sched_close();
void u_sched_info();

/* coroutine control functions */
u_coroutine_t *u_cor_register(cor_func_t func, void *arg);
int u_cor_resume(u_coroutine_t *cor);
void u_cor_yield();
int u_cor_join(u_coroutine_t *cor);
int u_cor_join_all();
int u_cor_status(u_coroutine_t *cor);


#endif
