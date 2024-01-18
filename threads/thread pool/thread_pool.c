//
// Created by César B. on 1/17/2024.
//

#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include "thread_pool.h"

#include <stdint.h>

// for debugging purposes
// #include <stdio.h>
#define printf

volatile struct g_thread_pool_struct g_thread_pool;

struct thread_pool_task read_latest_task_from_queue()
{
	struct thread_pool_task r = { NULL, NULL };
	bool done = false;

	pthread_mutex_lock(&g_thread_pool.tasks_queue_read_mutex);

	while(!g_thread_pool.should_exit && !done)
	{
		// try to lock the queue mutex
		if(pthread_mutex_trylock(&g_thread_pool.tasks_mutex))
			continue;
		__ATOMIC_RELEASE;

		// look for tasks
		if(!g_thread_pool.tasks)
		{
			pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
			continue;
		}

		// update the queue and store the result into r
		printf("thread %llu takes task %llu\n", pthread_self(), g_thread_pool.tasks_len);
		--g_thread_pool.tasks_len;
		r = g_thread_pool.tasks[g_thread_pool.tasks_len];
		g_thread_pool.tasks = realloc(g_thread_pool.tasks, sizeof(struct thread_pool_task) * (g_thread_pool.tasks_len));
		__ATOMIC_RELEASE;

		pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
		__ATOMIC_RELEASE;

		done = true;
	}

	pthread_mutex_unlock(&g_thread_pool.tasks_queue_read_mutex);

	return r;
}

void * thread_pool_thread_proc(void *)
{
	pthread_mutex_lock(&g_thread_pool.tasks_mutex);
	static uint8_t count = 0;
	++count;
	printf("new thread : %llu | task count : %llu | thread count : %u\n", pthread_self(), g_thread_pool.tasks_len, count);
	pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
	while(!g_thread_pool.should_exit)
	{
		struct thread_pool_task task = read_latest_task_from_queue();
		if(task.func)
			task.func(task.arg);
	}
	printf("exiting thread %llu\n", pthread_self());
}

void thread_pool_init()
{
	int r;
	memset(&g_thread_pool, 0, sizeof(struct g_thread_pool_struct));
	g_thread_pool.threads = malloc(sizeof(pthread_t) * THREAD_POOL_SIZE);
	r = pthread_mutex_init(&g_thread_pool.tasks_mutex, NULL);
	assert(!r);
	r = pthread_mutex_init(&g_thread_pool.tasks_queue_read_mutex, NULL);
	assert(!r);
	__ATOMIC_RELEASE;
	for(size_t i = 0; i < THREAD_POOL_SIZE; ++i)
	{
		r = pthread_create(&g_thread_pool.threads[i], NULL, thread_pool_thread_proc, NULL);
		assert(!r);
	}
}

void thread_pool_destroy()
{
	g_thread_pool.should_exit = true;
	__ATOMIC_RELEASE;

	thread_pool_join();
	__ATOMIC_RELEASE;

	pthread_mutex_lock(&g_thread_pool.tasks_mutex);
	free(g_thread_pool.tasks);
	pthread_mutex_unlock(&g_thread_pool.tasks_mutex);
	pthread_mutex_destroy(&g_thread_pool.tasks_mutex);
	pthread_mutex_destroy(&g_thread_pool.tasks_queue_read_mutex);
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

void thread_pool_join()
{
	// todo should thread_pool_join() assert all threads haven't already been joined
	// instead of implicitly returning success ?
	if(!g_thread_pool.threads)
		return;
	for(size_t i = 0; i < THREAD_POOL_SIZE; ++i)
		pthread_join(g_thread_pool.threads[i], NULL);
}
