//
// Created by César B. on 12/14/2023.
//

#define UI_WINDOWS
#define UI_IMPLEMENTATION

#include "ui.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../shared/macros.h"
#include "../logic/logic.h"
#include "rendering.h"

void ui_create_loading_screen()
{
	// @formatter:off
	enum
	{
		UI_FILL_ALL = UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL,
		UI_LIST_FLAGS = UI_PANEL_SCROLL | UI_PANEL_MEDIUM_SPACING | UI_FILL_ALL
	};
	g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
		g_ui.loading_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY | UI_PANEL_MEDIUM_SPACING);
	// @formatter:on
}

void ui_render_loading_screen()
{

}

void ui_create_all()
{
	// if coming from the loading screen, make sure it gets cleared
	if(g_ui.window)
		UIElementDestroyDescendents(g_ui.window);

	// @formatter:off
	{
		enum
		{
			UI_FILL_ALL = UI_ELEMENT_H_FILL | UI_ELEMENT_V_FILL,
			UI_LIST_FLAGS = UI_PANEL_SCROLL | UI_PANEL_MEDIUM_SPACING | UI_FILL_ALL
		};
		UIElement * temp;
		g_ui.window = UIWindowCreate(NULL, 0, "Storage Manager", 1300, 700);
			g_ui.main_panel = UIPanelCreate(g_ui.window, UI_PANEL_GRAY | UI_PANEL_MEDIUM_SPACING);
				g_ui.display_modes_pane = UITabPaneCreate(g_ui.main_panel, UI_FILL_ALL, "gui\tconsole\tlog");
					g_ui.gui_display_pane = UISplitPaneCreate(g_ui.display_modes_pane, UI_FILL_ALL | UI_SPLIT_PANE_VERTICAL, .5f);
						g_ui.dirs_pane = UISplitPaneCreate(g_ui.gui_display_pane, UI_FILL_ALL, .55f);
							temp = UITabPaneCreate(g_ui.dirs_pane, UI_FILL_ALL, "directories");
							g_ui.dir_names_panel = UIPanelCreate(temp, UI_LIST_FLAGS);
							temp = UITabPaneCreate(g_ui.dirs_pane, UI_FILL_ALL, "sizes");
							g_ui.dir_sizes_panel = UIPanelCreate(temp, UI_LIST_FLAGS);
						g_ui.files_pane = UISplitPaneCreate(g_ui.gui_display_pane, UI_FILL_ALL, .6f);
							temp = UITabPaneCreate(g_ui.files_pane, UI_FILL_ALL, "files");
							g_ui.file_names_panel = UIPanelCreate(temp, UI_LIST_FLAGS);
							temp = UITabPaneCreate(g_ui.files_pane, UI_FILL_ALL, "sizes");
							g_ui.file_sizes_panel = UIPanelCreate(temp, UI_LIST_FLAGS);
					g_ui.console_code = UICodeCreate(g_ui.display_modes_pane, UI_ALIGN_CENTER | UI_FILL_ALL);
					g_ui.log_panel = UIPanelCreate(g_ui.display_modes_pane, UI_FILL_ALL);
						g_ui.log_code = UICodeCreate(g_ui.log_panel, UI_ALIGN_CENTER | UI_FILL_ALL);
						g_ui.clear_log_button = UIButtonCreate(g_ui.log_panel, UI_ELEMENT_H_FILL, "Clear Log", -1);
				g_ui.button_panel = UIPanelCreate(g_ui.main_panel, UI_PANEL_HORIZONTAL | UI_ELEMENT_H_FILL);
					g_ui.reload_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Rld", -1);
					g_ui.rewind_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Bck", -1);
					g_ui.enter_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Ent", -1);
					g_ui.next_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Nxt", -1);
					g_ui.previous_button = UIButtonCreate(g_ui.button_panel, UI_ELEMENT_H_FILL, "Prv", -1);
	}
	// @formatter:on

	g_ui.dir_names_panel->gap = 1;
	g_ui.dir_sizes_panel->gap = 1;
	g_ui.file_names_panel->gap = 1;
	g_ui.file_sizes_panel->gap = 1;

	g_ui.log_panel->gap = 4;
	// TODO find out why the border here gotta be rectified a bit
	g_ui.log_panel->border.r += 4;
	g_ui.clear_log_button->invoke = ui_clear_log_button_cb;

	g_ui.reload_button->invoke = ui_reload_button_cb;
	g_ui.rewind_button->invoke = ui_rewind_button_cb;
	g_ui.enter_button->invoke = ui_enter_button_cb;
	g_ui.next_button->invoke = ui_next_button_cb;
	g_ui.previous_button->invoke = ui_previous_button_cb;

	ui_log("initialized all ui elements");
}

void ui_render()
{
	ui_render_console();

	// clear panels
	UIElementDestroyDescendents(g_ui.dir_names_panel);
	UIElementDestroyDescendents(g_ui.dir_sizes_panel);
	UIElementDestroyDescendents(g_ui.file_names_panel);
	UIElementDestroyDescendents(g_ui.file_sizes_panel);

	// just in case, refresh clear UI panels, might be useless
	UIElementRefresh(g_ui.dir_names_panel);
	UIElementRefresh(g_ui.dir_sizes_panel);
	UIElementRefresh(g_ui.file_names_panel);
	UIElementRefresh(g_ui.file_sizes_panel);
	UIElementRefresh(g_ui.window);

	// add tile labels
	// UILabelCreate(g_ui.dir_names_panel, UI_ELEMENT_H_FILL, "directories", -1);
	// UILabelCreate(g_ui.dir_sizes_panel, UI_ELEMENT_H_FILL, "sizes", -1);
	// UILabelCreate(g_ui.dir_names_panel, 0, "", -1);
	// UILabelCreate(g_ui.dir_sizes_panel, 0, "", -1);
	// UILabelCreate(g_ui.file_names_panel, UI_ELEMENT_H_FILL, "files", -1);
	// UILabelCreate(g_ui.file_sizes_panel, UI_ELEMENT_H_FILL, "sizes", -1);
	// UILabelCreate(g_ui.file_names_panel, 0, "", -1);
	// UILabelCreate(g_ui.file_sizes_panel, 0, "", -1);

	// add folder names and sizes
#pragma push_macro("RENDER_LIST")
#define RENDER_LIST(list, show_enter_button) \
    DO_WHILE_FALSE \
    ( \
        for(size_t i = 0; i < g_logic.current_dir->list##s_len; ++i) \
        { \
            ui_render_list_item(g_ui.list##_names_panel, g_logic.current_dir->size, g_logic.current_dir->list##s[i]->element, false, g_logic.current_dir->list##s == (void*)g_logic.current_dir->dirs); \
            ui_render_list_item(g_ui.list##_sizes_panel, g_logic.current_dir->size, g_logic.current_dir->list##s[i]->element, true, g_logic.current_dir->list##s == (void*)g_logic.current_dir->dirs); \
        } \
    )
	RENDER_LIST(dir, true);
	RENDER_LIST(file, false);

#pragma pop_macro("RENDER_LIST")

	// refresh UI elements
	UIElementRefresh(g_ui.dirs_pane);
	UIElementRefresh(g_ui.dir_names_panel);
	UIElementRefresh(g_ui.dir_sizes_panel);
	UIElementRefresh(g_ui.files_pane);
	UIElementRefresh(g_ui.file_names_panel);
	UIElementRefresh(g_ui.file_sizes_panel);
}

void ui_render_console()
{
#pragma push_macro("UI_CLEAR_CONSOLE")
#pragma push_macro("UI_CONSOLE_APPEND")
#pragma push_macro("UI_CONSOLE_APPEND_BLANK")

#define UI_CLEAR_CONSOLE() UICodeInsertContent(g_ui.console_code, "", -1, true)
#define UI_CONSOLE_APPEND(str, len) UICodeInsertContent(g_ui.console_code, (str), (len) == -1 ? -1 : (len) + 1, false)
#define UI_CONSOLE_APPEND_BLANK() UI_CONSOLE_APPEND("", 0)

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
		char * subdir_size_str = ui_format_file_size(subdir->size);
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
		char * file_size_str = ui_format_file_size(file->size);
		sprintf(str, "%s | %s", file->name, file_size_str);
		free(file_size_str);
		//sprintf(str, "%s | %lluKb", file->name, file->size / 1000);
		UI_CONSOLE_APPEND(str, -1);
		free(str);
	}

	UI_CONSOLE_APPEND_BLANK();

	UIElementRefresh(g_ui.console_code);

#pragma pop_macro("UI_CLEAR_CONSOLE")
#pragma pop_macro("UI_CONSOLE_APPEND")
#pragma pop_macro("UI_CONSOLE_APPEND_BLANK")
}


UIPanel * ui_render_list_item(UIElement * parent, size_t parent_directory_size, struct dir_tree_element tree_element, bool size_mode, bool dir_mode)
{
	UIPanel * panel = UIPanelCreate(parent, UI_PANEL_HORIZONTAL | UI_ELEMENT_H_FILL);
	panel->gap = 3;
	char * label_text;
	label_text = size_mode ? ui_format_file_size(tree_element.size) : tree_element.name;
	UILabelCreate(panel, UI_ELEMENT_H_FILL, label_text, -1);
	if(size_mode)
		free(label_text);
	UIButton * button;
#pragma push_macro("RENDER_LIST_BUTTON")
#define RENDER_LIST_BUTTON(label, callback, use_small_buttons)                                                  \
    DO_WHILE_FALSE                                                                                              \
    (                                                                                                           \
        button = UIButtonCreate(panel, (UI_BUTTON_SMALL * use_small_buttons) | UI_BUTTON_CHECKED, label, -1);   \
        button->e.cp = tree_element.name;                                                                       \
        button->invoke = callback;                                                                              \
    )

	if(!size_mode)
	{
		UIGauge * gauge = UIGaugeCreate(panel, 0);
		gauge->position = tree_element.size / (float)parent_directory_size;
		UIElementRefresh(gauge);
	}

	if(size_mode)
		RENDER_LIST_BUTTON("<->", ui_switch_file_size_display_unit, false);
	else
	{
		RENDER_LIST_BUTTON("^", dir_mode ? ui_open_directory_in_explorer_cb : ui_select_file_in_explorer_cb, true);
		RENDER_LIST_BUTTON("X", dir_mode ? ui_delete_directory_cb : ui_delete_file_cb, true);
		if(dir_mode)
			RENDER_LIST_BUTTON("->", ui_enter_directory_cb, true);
	}

#pragma pop_macro("RENDER_LIST_BUTTON")
	return panel;
}

// TODO delete
UIPanel * ui_render_list()
{
/*#define
	for(size_t i = 0; i < g_logic.current_dir->list##s_len;
	++i)
	{
		UILabel * label =
		UILabelCreate(g_ui.list##_names_panel, UI_ELEMENT_H_FILL, g_logic.current_dir->list##s[i]->name, -1);
		if(show_enter_button)
		{
			UIPanel * panel = UIPanelCreate(g_ui.list##_names_panel, UI_PANEL_HORIZONTAL | UI_ELEMENT_H_FILL);
			UIElementChangeParent(label, panel, NULL);
			UIButton * button = UIButtonCreate(panel, UI_BUTTON_SMALL | UI_BUTTON_CHECKED, "->", 3);
		}
		char * formatted_##list##_size_str = ui_format_file_size(g_logic.current_dir->list##s[i]->size);
		UILabelCreate(g_ui.list##_sizes_panel, UI_ELEMENT_H_FILL, formatted_##list##_size_str, -1);
		free(formatted_##list##_size_str);
	}*/
}

char * ui_format_file_size(size_t size)
{
	static char * suffixes[] = { "b", "Kb", "Mb", "Gb", "Tb" };
	uint8_t index = 0;
	while(size > 1000)
	{
		++index;
		size = roundf(size / 1000.f);
	}
	char * result = malloc(20);
	sprintf(result, "%llu %s", size, suffixes[index]);
	return result;
}

void ui_log(char * msg)
{
	time_t current_time = time(NULL);
	struct tm * tm = localtime(&current_time);
	char * log_msg = malloc(strlen(msg) + 17); // max required characters for timestamp is 17. yes I did count myself like a fucking idiot
	sprintf(
		log_msg, "[log] [%s%i:%s%i:%s%i] %s"
		, tm->tm_hour > 9 ? "" : "0", tm->tm_hour
		, tm->tm_min > 9 ? "" : "0", tm->tm_min
		, tm->tm_sec > 9 ? "" : "0", tm->tm_sec
		, msg
	);
	UICodeInsertContent(g_ui.log_code, log_msg, -1, false);
	UIElementRefresh(g_ui.log_code);
	free(log_msg);
}

void ui_clear_log()
{
	UI_FREE(g_ui.log_code->content);
	UI_FREE(g_ui.log_code->lines);
	g_ui.log_code->content = NULL;
	g_ui.log_code->lines = NULL;
	g_ui.log_code->contentBytes = 0;
	g_ui.log_code->lineCount = 0;
	UIElementRefresh(g_ui.log_code);
}
