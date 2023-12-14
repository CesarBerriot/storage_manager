//
// Created by César B. on 10/16/2023.
//

/*
 * TODO :
 * - try add to both g_ui.console_code.content and g_ui.console_code.lines[last_index].bytes so UICode can be concatenated
 * - make it so files and folders can be scrolled
 */

#include "logic/logic.h"
#include "ui/ui.h"

int main()
{
	logic_analyze_current_directory();
	UIInitialise();
	ui_create_all();
	ui_render();
	UIMessageLoop();
}
