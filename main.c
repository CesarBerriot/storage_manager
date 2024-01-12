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

#include <stdio.h>
int main()
{
	// enable escape codes processing for debug purposes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT);

	int r; // used for assertions. optimized away in release builds
	pthread_t ui_thread;
	pthread_t loading_screen_thread;

	// init thread flags
	g_threads.flags = 0;
	__ATOMIC_ACQUIRE;
	// memset(0) the tree struct
	logic_initialize();
	__ATOMIC_ACQUIRE;
	// run main ui thread
	r = pthread_create(&ui_thread, NULL, ui_thread_proc, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// await the ui thread's initialization
	while(!(g_threads.flags & F_UI_THREAD_INITIALIZED));
	__ATOMIC_ACQUIRE;
	// init loading screen ui
	ui_create_loading_screen();
	__ATOMIC_ACQUIRE;
	// run loading screen thread
	r = pthread_create(&loading_screen_thread, NULL, loading_screen_loop_thread_proc, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// run analysis on this thread
	logic_analyze_current_directory();
	__ATOMIC_ACQUIRE;
	// wait for the analysis to finish by joining the loading screen thread
	r = pthread_join(loading_screen_thread, NULL);
	assert(!r);
	__ATOMIC_ACQUIRE;
	// set up main ui
	ui_create_all();
	ui_render();
	__ATOMIC_ACQUIRE;
	puts("pre ui thread join");
	// let the ui thread do its thing
	r = pthread_join(ui_thread, NULL);
	assert(!r);

	puts("post ui thread join");

	// I couldn't begin to tell you why in the fuck this is required
	// ig it's got smth to do with the ui library
	// like smth along the lines of the library expecting the thread
	// that ends the ui to also be the last one
	// maybe there's some buggy post exit code in there
	// todo find an actual solution
	exit(EXIT_SUCCESS);
}
