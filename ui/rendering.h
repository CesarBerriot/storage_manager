//
// Created by César B. on 12/14/2023.
//

#pragma once

#include "logic/dir_tree/dir_tree.h"

void ui_force_full_redraw();

// loading screen functions. only called before any of the below-mentioned 'main' functions

// used only by ui_loading_screen_thread_proc()
void ui_create_loading_screen();

// recomputes the loading screen panel's size to fit all elements in
void ui_resize_loading_screen_panel();

void ui_render_loading_screen();

// void * ui_loading_screen_thread_proc(void *);

// main ui functions. only called before any of the above-mentioned 'main' functions

void ui_create_all();

void ui_render();

void ui_render_console();

// used by ui_render() only. not shown as used bc it's called by a macro
UIPanel * ui_render_list_item(UIElement * parent, size_t parent_directory_size, struct dir_tree_element tree_element, bool size_mode, bool dir_mode);

char * ui_format_file_size(size_t size);

void ui_log(char * msg);

void ui_clear_log();
