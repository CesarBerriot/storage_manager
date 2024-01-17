//
// Created by César B. on 1/12/2024.
//

#include "threads.h"

#include <windows.h>
#include "ui/ui.h"
#include "logic/logic.h"

struct g_main_threads_struct g_main_threads;

void * ui_thread_proc(void *)
{
	g_main_threads.flags |= F_UI_THREAD_ALIVE;
	__ATOMIC_RELEASE;

	UIInitialise();
	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
	g_ui.root_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY);

	__ATOMIC_ACQUIRE;
	g_main_threads.flags |= F_UI_THREAD_INITIALIZED;
	__ATOMIC_RELEASE;

	UIMessageLoop();

	__ATOMIC_ACQUIRE;
	//g_main_threads.flags ^= g_main_threads.flags | F_UI_THREAD_ALIVE; // todo fix
	g_main_threads.flags = 0;
}

void * loading_screen_loop_thread_proc(void*)
{
	while(!g_logic.analysis_stats.is_done && g_main_threads.flags & F_UI_THREAD_ALIVE)
	{
		ui_render_loading_screen();
		Sleep(50);
	}
	ui_render_loading_screen();
}
