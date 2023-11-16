//
// Created by César B. on 11/16/2023.
//

#pragma once

#include <dirent.h>
#include <stdbool.h>

DIR * g_cwd;

void refresh_cwd();

void rewind_dir();

void enter_dir(char * dir);

bool is_folder(char * name);
