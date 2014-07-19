/*
 * Copyright (C) 2014 FillZpp
 */

#ifndef UCOROUTINE_UCOROUTINE_H_
#define UCOROUTINE_UCOROUTINE_H_

#include <pthread.h>


typedef void (*cor_func_t)(void *arg);

/* schedule control functions */
int u_sched_open(int num);
int u_sched_close();
void u_sched_info();


/* coroutine control functions */
int u_cor_new(cor_func_t cor_func, void *arg);
void u_cor_resume(int cor_id);
void u_cor_yield();
int u_cor_join(int cor_id);
int u_cor_status(int cor_id);


#endif
