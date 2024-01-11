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

enum
{
	F_UI_THREAD_INITIALIZED,
	F_UI_THREAD_ALIVE
};

volatile uint8_t flags;

void * ui_thread_proc(void *)
{
	flags |= F_UI_THREAD_ALIVE;
	__ATOMIC_RELEASE;

	UIInitialise();
	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
	g_ui.root_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY);

	__ATOMIC_ACQUIRE;
	flags |= F_UI_THREAD_INITIALIZED;
	__ATOMIC_RELEASE;

	UIMessageLoop();

	__ATOMIC_ACQUIRE;
	flags ^= flags | F_UI_THREAD_ALIVE; // todo fix
	flags = 0;
}

void * loading_screen_loop_thread_proc()
{
	while(!g_logic.analysis_stats.is_done && flags & F_UI_THREAD_ALIVE)
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

	flags = 0;

	__ATOMIC_ACQUIRE;

	logic_initialize();

	__ATOMIC_RELEASE;

	r = pthread_create(&ui_thread, NULL, ui_thread_proc, NULL);
	assert(!r);

	__ATOMIC_ACQUIRE;

	while(!((flags & F_UI_THREAD_INITIALIZED) > 0));

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
