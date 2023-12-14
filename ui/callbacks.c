//
// Created by César B. on 12/14/2023.
//

#include "ui.h"

#include "callbacks.h"
#include "../logic/logic.h"

void ui_rewind_button_cb(void *)
{
	logic_rewind_directory();
	ui_render();
}

void ui_next_button_cb(void *)
{
	logic_directory_cursor_set_next();
	ui_render();
}

void ui_previous_button_cb(void *)
{
	logic_directory_cursor_set_previous();
	ui_render();
}

void ui_enter_button_cb(void *)
{
	logic_load_selected_directory();
	ui_render();
}
