//
// Created by César B. on 10/16/2023.
//

/*
 * TODO :
 * - try add to both g_ui.console_code.content and g_ui.console_code.lines[last_index].bytes so UICode can be concatenated
 * - make it so files and folders can be scrolled
 */

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "shared/fio.h"
#include "dir_tree/dir_tree.h"

#define UI_WINDOWS
#define UI_IMPLEMENTATION

#include "luigi/luigi.h"

#define DO_WHILE_FALSE(...) \
do                          \
{                           \
    __VA_ARGS__             \
}                           \
while(false)

//@formatter:off
struct
{
	UIWindow *	window;
	UIPanel	 *		main_panel;
	UIPanel	 * 			display_panel;
	UIPanel	 * 				dirs_panel;
	UIPanel	 * 					dir_names_panel;
	UIPanel	 * 					dir_sizes_panel;
	UIPanel	 * 				files_panel;
	UIPanel	 * 					file_names_panel;
	UIPanel	 * 					file_sizes_panel;
	UICode	 * 			console_code;
	UIPanel	 * 			button_panel;
	UIButton * 				rewind_button;
	UIButton * 				enter_button;
	UIButton * 				next_button;
	UIButton * 				previous_button;
} g_ui;
//@formatter:on

struct
{
	dir_tree_s tree;
	dir_tree_dir_s * current_dir;
	dir_tree_dir_s ** current_dir_path;
	size_t current_dir_path_len;
	size_t current_dir_depth;
	size_t cursor_pos;
} g_logic;

#define UI_CLEAR_CONSOLE() UICodeInsertContent(g_ui.console_code, "", -1, true)
#define UI_CONSOLE_APPEND(str, len) UICodeInsertContent(g_ui.console_code, (str), (len) == -1 ? -1 : (len) + 1, false)
#define UI_CONSOLE_APPEND_BLANK() UI_CONSOLE_APPEND("", 0)

char * format_file_size(size_t size)
{
	static char * suffixes[] = { "b", "Kb", "Mb", "Gb", "Tb" };
	uint8_t index = 0;
	while(size > 1000)
	{
		++index;
		size /= 1000;
	}
	char * result = malloc(20);
	sprintf(result, "%llu %s", size, suffixes[index]);
	return result;
}

void ui_render_console()
{
	static char * prefix_path = "path : ";
	static char * prefix_dir = "dir : ";
	static char * prefix_current = "-->";

	static size_t prefix_path_len = 0;
	static size_t prefix_dir_len = 0;
	static size_t prefix_current_len = 0;

	if(!prefix_path_len)
	{
		prefix_path_len = strlen(prefix_path);
		prefix_dir_len = strlen(prefix_dir);
		prefix_current_len = strlen(prefix_current);
	}

	// clear the console
	UI_CLEAR_CONSOLE();

	// print the current path
	{
		char * path_str = malloc(prefix_path_len + MAX_PATH + 1);
		memcpy(path_str, prefix_path, prefix_path_len + 1);
		for(register uint8_t i = 0; i < g_logic.current_dir_path_len; ++i)
		{
			char * dir_name = g_logic.current_dir_path[i]->name;
			if(dir_name)
				strcat(path_str, dir_name);
			strcat(path_str, "/");
		}
		UI_CONSOLE_APPEND(path_str, strlen(path_str));
		//free(path_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print the current directory
	{
		size_t current_dir_name_len =
			g_logic.current_dir->name ? strlen(g_logic.current_dir->name) : 0;
		char * dir_str = malloc(prefix_dir_len + current_dir_name_len + 2);
		memcpy(dir_str, prefix_dir, prefix_dir_len + 1);
		strcat(dir_str, current_dir_name_len ? g_logic.current_dir->name : "/");
		UI_CONSOLE_APPEND(dir_str, strlen(dir_str));
		free(dir_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print subdirectory names and sizes
	UI_CONSOLE_APPEND("subdirs :", -1);
	for(size_t i = 0; i < g_logic.current_dir->dirs_len; ++i)
	{
		struct dir_tree_dir * subdir = g_logic.current_dir->dirs[i];
		char * subdir_str = malloc(prefix_current_len + strlen(subdir->name) + 20);
		char * subdir_size_str = format_file_size(subdir->size);
		sprintf(
			subdir_str
			, "%s%s | %s"
			, g_logic.cursor_pos == i ? prefix_current : ""
			, subdir->name
			, subdir_size_str
		);
		free(subdir_size_str);
		//memcpy(subdir_str, prefix_current, prefix_current_len + 1);
		//strcat(subdir_str, current_subdir_name);
		UI_CONSOLE_APPEND(subdir_str, strlen(subdir_str) + 1);
		free(subdir_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print file names and sizes
	UI_CONSOLE_APPEND("files :", -1);
	for(size_t i = 0; i < g_logic.current_dir->files_len; ++i)
	{
		dir_tree_file_s * file = g_logic.current_dir->files[i];
		char * str = malloc(strlen(file->name) + 50);
		char * file_size_str = format_file_size(file->size);
		sprintf(str, "%s | %s", file->name, file_size_str);
		free(file_size_str);
		//sprintf(str, "%s | %lluKb", file->name, file->size / 1000);
		UI_CONSOLE_APPEND(str, -1);
		free(str);
	}

	UI_CONSOLE_APPEND_BLANK();

	UIElementRefresh(g_ui.console_code);
}

void ui_render()
{
	ui_render_console();

	// clear panels
#pragma push_macro("DESTROY_CHILDREN")
#define DESTROY_CHILDREN(panel)         \
    DO_WHILE_FALSE                      \
    (                                   \
        child = g_ui.panel->e.children; \
        while(child)                    \
        {                               \
            UIElementDestroy(child);    \
            ++child;                    \
        }                               \
    )
	struct UIElement * child;
	DESTROY_CHILDREN(dir_names_panel);
	DESTROY_CHILDREN(dir_sizes_panel);
	DESTROY_CHILDREN(file_names_panel);
	DESTROY_CHILDREN(file_sizes_panel);
#pragma pop_macro("DESTROY_CHILDREN")

	// just in case, refresh clear UI panels, might be ueless
	UIElementRefresh(g_ui.dir_names_panel);
	UIElementRefresh(g_ui.dir_sizes_panel);
	UIElementRefresh(g_ui.file_names_panel);
	UIElementRefresh(g_ui.file_sizes_panel);

	// add tile labels
	UILabelCreate(g_ui.dir_names_panel, UI_ELEMENT_H_FILL, "directories", -1);
	UILabelCreate(g_ui.dir_sizes_panel, UI_ELEMENT_H_FILL, "sizes", -1);
	UILabelCreate(g_ui.dir_names_panel, 0, "", -1);
	UILabelCreate(g_ui.dir_sizes_panel, 0, "", -1);
	UILabelCreate(g_ui.file_names_panel, UI_ELEMENT_H_FILL, "files", -1);
	UILabelCreate(g_ui.file_sizes_panel, UI_ELEMENT_H_FILL, "sizes", -1);
	UILabelCreate(g_ui.file_names_panel, 0, "", -1);
	UILabelCreate(g_ui.file_sizes_panel, 0, "", -1);

	// add folder names and sizes
#pragma push_macro("RENDER_LIST")
#define RENDER_LIST(list) \
    DO_WHILE_FALSE \
    ( \
        for(size_t i = 0; i < g_logic.current_dir->list##s_len; ++i) \
        { \
            UILabelCreate(g_ui.list##_names_panel, UI_ELEMENT_H_FILL, g_logic.current_dir->list##s[i]->name, -1); \
            char * formatted_##list##_size_str = format_file_size(g_logic.current_dir->list##s[i]->size); \
            UILabelCreate(g_ui.list##_sizes_panel, UI_ELEMENT_H_FILL, formatted_##list##_size_str, -1); \
            free(formatted_##list##_size_str); \
        } \
    )
	RENDER_LIST(dir);
	RENDER_LIST(file);
#pragma pop_macro("RENDER_LIST")

	// refresh UI elements
	UIElementRefresh(g_ui.dirs_panel);
	UIElementRefresh(g_ui.dir_names_panel);
	UIElementRefresh(g_ui.dir_sizes_panel);
	UIElementRefresh(g_ui.files_panel);
	UIElementRefresh(g_ui.file_names_panel);
	UIElementRefresh(g_ui.file_sizes_panel);
}

void ui_rewind_button_cb(void *)
{
#include "ref_logic.def"

	if(!depth)
		return;
	--depth;
	path = realloc(path, path_len * sizeof(uintptr_t) - sizeof(uintptr_t));
	--path_len;
	dir = path[path_len - 1];
	cursor = 0;
	ui_render();

#include "unref_logic.def"
	/*
	UIElementDestroy(g_ui.console_label);
	g_ui.console_label = UILabelCreate(g_ui.main_panel, UI_ALIGN_CENTER | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL, "dsgsdfgsdfg", -1);
	UILabelSetContent(g_ui.console_label, "slkfjsdkjfs", -1);
	UIElementRefresh(g_ui.console_label);
	UIElementRepaint(g_ui.console_label, NULL);
	UIElementRepaint(g_ui.main_panel, NULL);
	struct UIElement * child = g_ui.console_label->e.parent->children;
	while(child)
	{
		UIElementDestroy(child);
		child = child->next;
	}
	 //= NULL;
	UIElementRefresh(&g_ui.main_panel->e);
	char * str = malloc(50);
	memset(str, 0, 50);
	static uint8_t count = 0;
	sprintf(str, "%i\ntest", count);
	g_ui.console_label->string = "test\nudflkjsdf";
	UIElementDestroy(&g_ui.console_label->e);
	g_ui.console_label->e.flags |= UI_ELEMENT_HIDE;
	UILabelSetContent(g_ui.console_label, str, -1);
	UICodeInsertContent(g_ui.console_code, "test1\ntest2", -1, false);
	UIElementRefresh(g_ui.console_code);
	free(str);
	++count;
	*/
}

void ui_enter_button_cb(void *)
{
#include "ref_logic.def"

	if(!dir->dirs_len)
		return;
	++depth;
	path = realloc(path, path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
	path[path_len] = dir->dirs[cursor];
	++path_len;
	dir = dir->dirs[cursor];
	cursor = 0;
	ui_render();

#include "unref_logic.def"
}

void ui_next_button_cb(void *)
{
#include "ref_logic.def"

	++cursor;
	if(cursor > dir->dirs_len - 1)
		cursor = 0;
	ui_render();

#include "unref_logic.def"
}

void ui_previous_button_cb(void *)
{
#include "ref_logic.def"

	--cursor;
	if(cursor > dir->dirs_len - 1)
		cursor = dir->dirs_len - 1;
	ui_render();

#include "unref_logic.def"
}

int main()
{
	UIInitialise();
	// @formatter:off
	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
		g_ui.main_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY | UI_PANEL_MEDIUM_SPACING);
			g_ui.display_panel = UIPanelCreate(g_ui.main_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
				g_ui.dirs_panel = UIPanelCreate(g_ui.main_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL | UI_PANEL_HORIZONTAL);
					g_ui.dir_names_panel = UIPanelCreate(g_ui.dirs_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
					g_ui.dir_sizes_panel = UIPanelCreate(g_ui.dirs_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
				g_ui.files_panel = UIPanelCreate(g_ui.display_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL | UI_PANEL_HORIZONTAL);
					g_ui.file_names_panel = UIPanelCreate(g_ui.files_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
					g_ui.file_sizes_panel = UIPanelCreate(g_ui.files_panel, UI_PANEL_MEDIUM_SPACING | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
			g_ui.console_code = UICodeCreate(g_ui.main_panel, UI_ALIGN_CENTER | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
			g_ui.button_panel = UIPanelCreate(g_ui.main_panel, UI_PANEL_HORIZONTAL | UI_ELEMENT_H_FILL);
				g_ui.rewind_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Bck", -1);
				g_ui.enter_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Ent", -1);
				g_ui.next_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Nxt", -1);
				g_ui.previous_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Prv", -1);
	// @formatter:on

	g_ui.rewind_button->invoke = ui_rewind_button_cb;
	g_ui.enter_button->invoke = ui_enter_button_cb;
	g_ui.next_button->invoke = ui_next_button_cb;
	g_ui.previous_button->invoke = ui_previous_button_cb;

	g_logic.tree = mk_dir_tree();
	g_logic.current_dir = g_logic.tree.root;
	g_logic.current_dir_path = malloc(sizeof(uintptr_t));
	g_logic.current_dir_path[0] = g_logic.current_dir;
	g_logic.current_dir_path_len = 1;
	g_logic.current_dir_depth = 0;
	g_logic.cursor_pos = 0;

	ui_render();
	UIMessageLoop();
}
