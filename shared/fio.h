//
// Created by César B. on 11/16/2023.
//

/// fio.h - shared files/folder I/O code

#pragma once

#include <dirent.h>
#include <stdbool.h>

char * get_cwd();

bool is_folder(char * name);
