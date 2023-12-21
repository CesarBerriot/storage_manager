//
// Created by César B. on 12/14/2023.
//

#pragma once

#include "../logic/dir_tree/dir_tree.h"

void ui_create_all();

void ui_render();

void ui_render_console();

// TODO never used fsr ? check it out
UIPanel * ui_render_list_item(UIElement * parent, struct dir_tree_element tree_element, bool size_mode, bool dir_mode);

char * ui_format_file_size(size_t size);

void ui_log(char * msg);

void ui_clear_log();
