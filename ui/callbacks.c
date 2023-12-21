//
// Created by César B. on 12/14/2023.
//

#include "ui.h"

#include <windows.h>
// #include <shlobj_core.h>
#include <stdio.h>
#include "callbacks.h"
#include "../logic/logic.h"
#include "../shared/fio.h"

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
#include "../logic/ref_globals.def"

	if(!dir->dirs_len)
		return;
	++depth;
	path = realloc(path, path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
	dir_tree_dir_s * new_current_dir;
	for(size_t i = 0; i < dir->dirs_len; ++i)
	{
		new_current_dir = dir->dirs[i];
		if(new_current_dir->name == directory)
			break;
	}
	path[path_len] = new_current_dir;
	++path_len;
	dir = new_current_dir;
	cursor = 0;
	ui_render();

#include "../logic/unref_globals.def"
}

void ui_open_directory_in_explorer_cb(char * directory)
{
	char * current_path_str = mk_dir_tree_path_str(g_logic.tree.root);
	char * full_path_str = malloc(MAX_PATH + 1);
	memcpy(full_path_str, current_path_str, strlen(current_path_str) + 1);
	strcat(full_path_str, directory);
	ensure_path_separator(full_path_str, PS_ANTISLASH);
	printf("path : %s\n", full_path_str);
	ShellExecuteA(NULL, "open", full_path_str, NULL, NULL, SW_SHOW);
	free(current_path_str);
	free(full_path_str);
}

void ui_delete_directory_cb(char * directory)
{
	printf("ui_delete_directory_cb : %s\n", directory);
}

void ui_select_file_in_explorer_cb(char * file)
{
	// TODO find a better way to do this, I couldn't get ShellExecuteA() working for this.
	// I heard about SHOpenFolderAndSelectItems() online but I couldn't include the header (shlobj_core.h).
	// MinGW issue ?

	static char * prefix = "explorer.exe /select,\"";
	char * cwd = get_cwd();
	char * current_path_str = mk_dir_tree_path_str(g_logic.tree.root);
	ensure_path_separator(current_path_str, PS_ANTISLASH);
	char * full_command_str = malloc(strlen(prefix) + MAX_PATH + 1);
	sprintf(full_command_str, "%s\"%s\\%s%s\"", prefix, cwd, current_path_str, file);
	system(full_command_str);

	free(cwd);
	free(current_path_str);
	free(full_command_str);
}

void ui_delete_file_cb(char * file)
{
	printf("ui_delete_file_cb : %s\n", file);
}

void ui_switch_file_size_display_unit(char * file)
{
	printf("ui_switch_file_size_display_unit : %s\n", file);
}
