//
// Created by César B. on 1/17/2024.
//

#include <memory.h>
#include <malloc.h>
#include "thread_pool.h"

struct g_thread_pool_struct g_thread_pool;

void thread_pool_init()
{
	memset(&g_thread_pool, 0, sizeof(struct g_thread_pool_struct));
	g_thread_pool.threads = malloc(sizeof(pthread_t) * THREAD_POOL_SIZE);
	pthread_mutex_init(&g_thread_pool.tasks_mutex, NULL);
}

void thread_pool_destroy()
{
	g_thread_pool.should_exit = true;
	__ATOMIC_RELEASE;

	for(size_t i = 0; i < THREAD_POOL_SIZE; ++i)
		pthread_join(g_thread_pool.threads[i]);
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

}

void * thread_pool_thread_proc(void *)
{
	return NULL;
}
