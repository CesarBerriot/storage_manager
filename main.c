//
// Created by César B. on 10/16/2023.
//

/*
 * TODO :
 * - try add to both g_ui.console_code.content and g_ui.console_code.lines[last_index].bytes so UICode can be concatenated
 * - make it so files and folders can be scrolled
 */

#include <pthread.h>
#include <assert.h>
#include "logic/logic.h"
#include "ui/ui.h"
#include <windows.h>

volatile bool ui_thread_initialized;

void * ui_thread_proc(void *)
{
	UIInitialise();
	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
	g_ui.root_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY);

	__ATOMIC_ACQUIRE;
	ui_thread_initialized = true;
	__ATOMIC_RELEASE;

	UIMessageLoop();
}

void * loading_screen_loop_thread_proc()
{
	while(!g_logic.analysis_stats.is_done)
	{
		ui_render_loading_screen();
		Sleep(50);
	}
	ui_render_loading_screen();
}

int main()
{
	// enable escape codes processing for debug purposes
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT);

	int r;
	pthread_t ui_thread;
	pthread_t loading_screen_thread;

	ui_thread_initialized = false;

	__ATOMIC_ACQUIRE;

	logic_initialize();

	__ATOMIC_RELEASE;

	r = pthread_create(&ui_thread, NULL, ui_thread_proc, NULL);
	assert(!r);

	__ATOMIC_ACQUIRE;

	while(!ui_thread_initialized);

	__ATOMIC_RELEASE;


	ui_create_loading_screen();

	__ATOMIC_ACQUIRE;

	r = pthread_create(&loading_screen_thread, NULL, loading_screen_loop_thread_proc, NULL);
	assert(!r);

	__ATOMIC_RELEASE;

	logic_analyze_current_directory();

	__ATOMIC_ACQUIRE;

	r = pthread_join(loading_screen_thread, NULL);
	assert(!r);

	__ATOMIC_RELEASE;

	ui_create_all();
	ui_render();

	__ATOMIC_ACQUIRE;

	r = pthread_join(ui_thread, NULL);
	assert(!r);
}
