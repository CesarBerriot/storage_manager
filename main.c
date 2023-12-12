//
// Created by César B. on 10/16/2023.
//

// TODO try add to both g_ui.console_code.content and g_ui.console_code.lines[last_index].bytes so UICode can be concatenated

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

//@formatter:off
struct
{
	UIWindow *	window;
	UIPanel	 *		main_panel;
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

void ui_render()
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
		free(path_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print the current directory
	{
		size_t current_dir_name_len = g_logic.current_dir->name
									  ? strlen(g_logic.current_dir->name)
									  : 0;
		char * dir_str = malloc(prefix_dir_len + current_dir_name_len + 1);
		memcpy(dir_str, prefix_dir, prefix_dir_len);
		strcat(dir_str, current_dir_name_len ? g_logic.current_dir->name : "/");
		UI_CONSOLE_APPEND(dir_str, strlen(dir_str));
		free(dir_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print subdirectory names
	UI_CONSOLE_APPEND("subdirs :", -1);
	for(size_t i = 0; i < g_logic.current_dir->dirs_len; ++i)
	{
		if(g_logic.cursor_pos != i)
		{
			UI_CONSOLE_APPEND(g_logic.current_dir->dirs[i]->name, -1);
			continue;
		}
		char * current_subdir_name = g_logic.current_dir->dirs[i]->name;
		char * subdir_str = malloc(prefix_current_len + strlen(current_subdir_name) + 1);
		memcpy(subdir_str, prefix_current, prefix_current_len + 1);
		strcat(subdir_str, current_subdir_name);
		UI_CONSOLE_APPEND(subdir_str, strlen(subdir_str) + 1);
		free(subdir_str);
	}

	UI_CONSOLE_APPEND_BLANK();

	// print file names
	UI_CONSOLE_APPEND("files :", -1);
	for(size_t i = 0; i < g_logic.current_dir->files_len; ++i)
		UI_CONSOLE_APPEND(g_logic.current_dir->files[i]->name, -1);

	UI_CONSOLE_APPEND_BLANK();

	UIElementRefresh(g_ui.console_code);
}

void ui_rewind_button_cb(void *)
{
	if(!g_logic.current_dir_depth)
		return;
	--g_logic.current_dir_depth;
	g_logic.current_dir_path = realloc(g_logic.current_dir_path, g_logic.current_dir_path_len * sizeof(uintptr_t) - sizeof(uintptr_t));
	--g_logic.current_dir_path_len;
	g_logic.current_dir = g_logic.current_dir_path[g_logic.current_dir_path_len - 1];
	g_logic.cursor_pos = 0;
	ui_render();
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
	++g_logic.current_dir_depth;
	g_logic.current_dir_path = realloc(g_logic.current_dir_path, g_logic.current_dir_path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
	g_logic.current_dir_path[g_logic.current_dir_path_len] = g_logic.current_dir;
	++g_logic.current_dir_path_len;
	g_logic.current_dir = g_logic.current_dir->dirs[g_logic.cursor_pos];
	g_logic.cursor_pos = 0;
	ui_render();
}

void ui_next_button_cb(void *)
{
	++g_logic.cursor_pos;
	if(g_logic.cursor_pos > g_logic.current_dir->dirs_len - 1)
		g_logic.cursor_pos = 0;
	ui_render();
}

void ui_previous_button_cb(void *)
{
	--g_logic.cursor_pos;
	if(g_logic.cursor_pos > g_logic.current_dir->dirs_len - 1)
		g_logic.cursor_pos = g_logic.current_dir->dirs_len - 1;
	ui_render();
}

int main()
{
	UIInitialise();

	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 600, 400);
	g_ui.main_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY);
	g_ui.console_code = UICodeCreate(g_ui.main_panel, UI_ALIGN_CENTER | UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL);
	g_ui.button_panel = UIPanelCreate(g_ui.main_panel, UI_PANEL_HORIZONTAL | UI_ELEMENT_H_FILL);
	g_ui.rewind_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Bck", -1);
	g_ui.enter_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Ent", -1);
	g_ui.next_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Nxt", -1);
	g_ui.previous_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Prv", -1);

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

	/*dir_tree_s tree = mk_dir_tree();
	dir_tree_dir_s * currdir = tree.root;
	dir_tree_dir_s ** curr_dir_path = malloc(sizeof(uintptr_t));
	curr_dir_path[0] = currdir;
	size_t curr_dir_path_len = 1;
	size_t curr_dir_depth = 0;
	size_t curpos = 0;
	while(true)
	{

		puts("--------------------------------------------");
		fputs("path : ", stdout);
		for(register uint8_t i = 0; i < curr_dir_path_len; ++i)
		{
			fputs(curr_dir_path[i]->name, stdout);
			fputc('/', stdout);
		}
		fputc('\n', stdout);
		fputs("dir : ", stdout);
		puts(currdir->name);
		puts("subdirs :");
		for(size_t i = 0; i < currdir->dirs_len; ++i)
		{
			if(curpos == i)
				fputs("--> ", stdout);
			puts(currdir->dirs[i]->name);
		}
		puts("files :");
		for(size_t i = 0; i < currdir->files_len; ++i)
			puts(currdir->files[i]->name);
		char c;
		while((c = getchar()) == '\n');
		switch(c)
		{
			case '+':
				++curpos;
				if(curpos > currdir->dirs_len - 1)
					curpos = 0;
				break;
			case '-':
				--curpos;
				if(curpos > currdir->dirs_len - 1)
					curpos = currdir->dirs_len - 1;
				break;
			case '/':
				++curr_dir_depth;
				curr_dir_path = realloc(curr_dir_path, curr_dir_path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
				curr_dir_path[curr_dir_path_len] = currdir;
				++curr_dir_path_len;
				currdir = currdir->dirs[curpos];
				curpos = 0;
				break;
			case '.':
				if(!curr_dir_depth)
					break;
				--curr_dir_depth;
				curr_dir_path = realloc(curr_dir_path, curr_dir_path_len * sizeof(uintptr_t) - sizeof(uintptr_t));
				--curr_dir_path_len;
				currdir = curr_dir_path[curr_dir_path_len - 1];
				curpos = 0;
				break;
		}
	}
	free_dir_tree(tree);*/
}
