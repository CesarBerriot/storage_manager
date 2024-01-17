//
// Created by César B. on 10/16/2023.
//

/*
 * TODO :
 * - try add to both g_ui.console_code.content and g_ui.console_code.lines[last_index].bytes so UICode can be concatenated
 * - find a way to have a common scrollbar between entries and sizes, or even synchronize them
 * - the **cheeeese**
 */

#include <pthread.h>
#include <assert.h>
#include <windows.h>
#include "logic/logic.h"
#include "threads/threads.h"
#include "ui/ui.h"
#include "threads/thread pool/thread_pool.h"

#include <stdio.h>
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	thread_pool_init();
	thread_pool_destroy();
	puts("ok");
	return 0;
	// enable escape codes processing for debug purposes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT);

	int r; // used for assertions. optimized away in release builds
	pthread_t ui_thread;
	pthread_t loading_screen_thread;

	// init thread pool
	thread_pool_init();
	// init thread flags
	g_main_threads.flags = 0;
	__ATOMIC_ACQUIRE;
	// memset(0) the tree struct
	logic_initialize();
	__ATOMIC_ACQUIRE;
	// run main ui thread
	r = pthread_create(&ui_thread, NULL, ui_thread_proc, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// await the ui thread's initialization
	while(!(g_main_threads.flags & F_UI_THREAD_INITIALIZED));
	__ATOMIC_ACQUIRE;
	// init loading screen ui
	ui_create_loading_screen();
	__ATOMIC_ACQUIRE;
	// run loading screen thread
	r = pthread_create(&loading_screen_thread, NULL, loading_screen_loop_thread_proc, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// acquire realtime process priority for the analysis
	r = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	assert(r);
	__ATOMIC_ACQUIRE;
	// run analysis on this thread
	logic_analyze_current_directory();
	__ATOMIC_ACQUIRE;
	// restore process priority
	r = SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	assert(r);
	__ATOMIC_ACQUIRE;
	// wait for the analysis to finish by joining the loading screen thread
	r = pthread_join(loading_screen_thread, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// set up main ui
	ui_create_all();
	ui_render();
	__ATOMIC_ACQUIRE;
	// let the ui thread do its thing
	r = pthread_join(ui_thread, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// destroy the thread pool
	thread_pool_destroy();

	// I couldn't begin to tell you why in the fuck this is required
	// ig it's got smth to do with the ui library
	// like smth along the lines of the library expecting the thread
	// that ends the ui to also be the last one
	// maybe there's some buggy post exit code in there
	// although I couldn't find any atexit() calls in it
	// so it gotta be some obscure winapi function or smth
	// todo find an actual solution
	exit(EXIT_SUCCESS);
}
