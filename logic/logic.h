//
// Created by César B. on 12/14/2023.
//

#pragma once

#include <stdbool.h>
#include <pthread.h>
#include "dir_tree/dir_tree.h"

extern struct g_logic_struct
{
	dir_tree_s tree;
	dir_tree_dir_s * current_dir;
	dir_tree_dir_s ** current_dir_path;
	size_t current_dir_path_len;
	size_t current_dir_depth;
	size_t cursor_pos; // used in console mode's navigation
	struct g_logic_analysis_stats_struct // stats modified during an ongoing analysis
	{
		size_t thread_count; // current amount of working threads during analysis
		size_t max_recorded_threads; // max amount of threads recorded at once during analysis. uses thread_count's mutex
		size_t computed_directories_count; // amount of folders the analysis went through
		bool is_done; // analysis has finished
		pthread_mutex_t thread_count_mutex;
		pthread_mutex_t computed_directories_count_mutex;
	} analysis_stats;
} g_logic;

void logic_analyze_current_directory();

void logic_reload_tree(); // todo have a gauge somewhere (down left would look nice, to the left of the reload button) in the UI showing the reloading progression

void logic_load_selected_directory();

void logic_rewind_directory();

void logic_directory_cursor_set_next();

void logic_directory_cursor_set_previous();
