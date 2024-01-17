//
// Created by César B. on 1/12/2024.
//

#pragma once

#include <stdint.h>

enum
{
	F_UI_THREAD_INITIALIZED = 1,
	F_UI_THREAD_ALIVE = 1 << 1
};

extern struct g_main_threads_struct
{
	volatile uint8_t flags;
} g_main_threads;

void * ui_thread_proc(void *);

void * loading_screen_loop_thread_proc(void*);
