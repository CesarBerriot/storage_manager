//
// Created by César B. on 12/14/2023.
//

#pragma once

#include "dir_tree/dir_tree.h"

extern struct g_logic_struct
{
	dir_tree_s tree;
	dir_tree_dir_s * current_dir;
	dir_tree_dir_s ** current_dir_path;
	size_t current_dir_path_len;
	size_t current_dir_depth;
	size_t cursor_pos;
} g_logic;

void logic_analyze_current_directory();

void logic_reload_tree();

void logic_load_selected_directory();

void logic_rewind_directory();

void logic_directory_cursor_set_next();

void logic_directory_cursor_set_previous();
