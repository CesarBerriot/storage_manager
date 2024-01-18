//
// Created by César B. on 1/17/2024.
//

/// an easy to use thread pool. powerful, fast and flexible

#pragma once

#include <pthread.h>
#include <stdbool.h>

enum { THREAD_POOL_SIZE = 30 };

extern volatile struct g_thread_pool_struct
{
	pthread_t * threads;
	struct thread_pool_task
	{
		void (* func)(void *);
		void * arg;
	} * tasks;
	size_t tasks_len;
	pthread_mutex_t tasks_mutex;
	pthread_mutex_t tasks_queue_read_mutex; // used to lock all but one thread at a time for reading the queue
	bool should_exit;
} g_thread_pool;

void thread_pool_init();

// will join all threads, no need to call thread_pool_join()
void thread_pool_destroy();

void thread_pool_create_task(struct thread_pool_task task);

void thread_pool_join();
