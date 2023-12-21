//
// Created by César B. on 12/14/2023.
//

#pragma once


void ui_reload_button_cb(void *);

void ui_rewind_button_cb(void *);

void ui_next_button_cb(void *);

void ui_previous_button_cb(void *);

void ui_enter_button_cb(void *);

void ui_enter_directory_cb(char * directory);

void ui_open_directory_in_explorer_cb(char * directory);

void ui_delete_directory_cb(char * directory);

void ui_select_file_in_explorer_cb(char * file);

void ui_delete_file_cb(char * file);

void ui_switch_file_size_display_unit(char * file);
