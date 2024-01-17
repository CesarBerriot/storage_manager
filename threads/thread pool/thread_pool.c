//
// Created by César B. on 1/17/2024.
//

//#include <malloc.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <memory.h>
#include <assert.h>
#include "thread_pool.h"

struct g_thread_pool_struct g_thread_pool;

void thread_pool_init()
{
	_malloc_dbg()
	int r;
	memset(&g_thread_pool, 0, sizeof(struct g_thread_pool_struct));
	g_thread_pool.threads = malloc(sizeof(pthread_t) * THREAD_POOL_SIZE);
	r = pthread_mutex_init(&g_thread_pool.tasks_mutex, NULL);
	assert(!r);
	__ATOMIC_RELEASE;
	for(size_t i = 0; i < THREAD_POOL_SIZE; ++i)
	{
		r = pthread_create(g_thread_pool.threads + i, NULL, thread_pool_thread_proc, NULL);
		assert(!r);
	}
}

void thread_pool_destroy()
{
	int r;
	g_thread_pool.should_exit = true;
	__ATOMIC_RELEASE;

	for(size_t i = 0; i < THREAD_POOL_SIZE; ++i)
		pthread_join(g_thread_pool.threads[i], NULL);
	__ATOMIC_RELEASE;

	pthread_mutex_lock(&g_thread_pool.tasks_mutex);
	free(g_thread_pool.tasks);
	pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
	pthread_mutex_destroy(&g_thread_pool.tasks_mutex);
	__ATOMIC_RELEASE;

	free(g_thread_pool.threads);

	memset(&g_thread_pool, 0, sizeof(struct g_thread_pool_struct));
}

void thread_pool_create_task(struct thread_pool_task task)
{
	pthread_mutex_lock(&g_thread_pool.tasks_mutex);

	++g_thread_pool.tasks_len;
	g_thread_pool.tasks = realloc(g_thread_pool.tasks, sizeof(struct thread_pool_task) * (g_thread_pool.tasks_len));
	g_thread_pool.tasks[g_thread_pool.tasks_len - 1] = task;

	pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
}

void * thread_pool_thread_proc(void *)
{
	while(!g_thread_pool.should_exit)
	{
		while(!g_thread_pool.tasks_len) // might catch a data race but irdc since all I need is to know whether any bit is set
			if(g_thread_pool.should_exit)
				goto continue_main_loop_label;

		__ATOMIC_RELEASE;

		pthread_mutex_lock(&g_thread_pool.tasks_mutex);
		__ATOMIC_RELEASE;

		struct thread_pool_task task = g_thread_pool.tasks[g_thread_pool.tasks_len - 1];
		++g_thread_pool.tasks_len;
		g_thread_pool.tasks = realloc(g_thread_pool.tasks, sizeof(struct thread_pool_task) * (g_thread_pool.tasks_len));
		__ATOMIC_RELEASE;

		pthread_mutex_unlock(&g_thread_pool.tasks_mutex);

		__ATOMIC_RELEASE;

		assert(task.func);
		task.func(task.arg);
		continue_main_loop_label:
	}
}
