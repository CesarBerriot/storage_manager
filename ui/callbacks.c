//
// Created by César B. on 12/14/2023.
//

#include "ui.h"

#include <stdio.h>
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

void ui_enter_directory_cb(char * directory)
{
	printf("ui_enter_directory_cb : %s\n", directory);
}

void ui_open_directory_in_explorer_cb(char * directory)
{
	printf("ui_open_directory_in_explorer_cb : %s\n", directory);
}

void ui_delete_directory_cb(char * directory)
{
	printf("ui_delete_directory_cb : %s\n", directory);
}

void ui_select_file_in_explorer_cb(char * file)
{
	printf("ui_select_file_in_explorer_cb : %s\n", file);
}

void ui_delete_file_cb(char * file)
{
	printf("ui_delete_file_cb : %s\n", file);
}

void ui_switch_file_size_display_unit(char * file)
{
	printf("ui_switch_file_size_display_unit : %s\n", file);
}
