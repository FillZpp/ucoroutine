/*
 * Copyright (C) 2014 FillZpp
 */

#ifndef UCOROUTINE_UCOROUTINE_H_
#define UCOROUTINE_UCOROUTINE_H_

#include <pthread.h>
#include <stddef.h>


typedef void (*cor_func_t)(void *arg);

/* schedule control functions */
int u_sched_open(int num);
int u_sched_close();
void u_sched_info();

/* coroutine control functions */
size_t u_cor_register(cor_func_t func, void *arg);
int u_cor_resume(size_t cor_id);
void u_cor_yield();
int u_cor_join(size_t cor_id);
int u_cor_join_all();
int u_cor_status(size_t cor_id);


#endif
