//
// Created by César B. on 11/16/2023.
//

#pragma once

#include <corecrt.h>

typedef struct dir_tree_file
{
	char * name; //utf8 ?
	size_t size;
} dir_tree_file_s;

typedef struct dir_tree_dir
{
	char * name;
	struct dir_tree_dir ** dirs;
	dir_tree_file_s ** files;
	size_t dirs_len;
	size_t files_len;
} dir_tree_dir_s;

typedef struct dir_tree
{
	dir_tree_dir_s * root;
} dir_tree_s;

void free_dir_tree(dir_tree_s tree);

dir_tree_s mk_dir_tree();
