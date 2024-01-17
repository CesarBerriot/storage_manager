//
// Created by César B. on 1/17/2024.
//

#pragma once

#include <pthread.h>
#include <stdbool.h>

enum { THREAD_POOL_SIZE = 5 };

extern struct g_thread_pool_struct
{
	pthread_t * threads;
	struct thread_pool_task
	{
		void (* func)(void *);
		void * arg;
	} * tasks;
	volatile size_t tasks_len;
	pthread_mutex_t tasks_mutex;
	volatile bool should_exit;
} g_thread_pool;

void thread_pool_init();

void thread_pool_destroy();

void thread_pool_create_task(struct thread_pool_task task);

void * thread_pool_thread_proc(void *);
