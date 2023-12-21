//
// Created by César B. on 12/14/2023.
//

#include "logic.h"

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "ref_globals.def"

struct g_logic_struct g_logic;

void logic_analyze_current_directory()
{
	tree = mk_dir_tree();
	dir = tree.root;
	path = malloc(sizeof(uintptr_t));
	path[0] = dir;
	path_len = 1;
	depth = 0;
	cursor = 0;
}

void logic_reload_tree()
{
	dir_tree_s new_tree = mk_dir_tree();
	dir_tree_dir_s ** new_path = malloc(sizeof(uintptr_t) * path_len);
	new_path[0] = new_tree.root;
	for(size_t i = 1; i < path_len; ++i)
	{
		dir_tree_dir_s * new_parent = new_path[i - 1];
		for(size_t i2 = 0; i2 < new_parent->dirs_len; ++i2)
			if(!strcmp(new_parent->dirs[i2]->name, path[i]->name))
			{
				new_path[i] = new_parent->dirs[i2];
				break;
			}
	}
	free_dir_tree(tree);
	tree = new_tree;
	free(path);
	path = new_path;
	dir = path[path_len - 1];
	cursor = 0;
}

void logic_load_selected_directory()
{
	if(!dir->dirs_len)
		return;
	++depth;
	path = realloc(path, path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
	path[path_len] = dir->dirs[cursor];
	++path_len;
	dir = dir->dirs[cursor];
	cursor = 0;
}

void logic_rewind_directory()
{
	if(!depth)
		return;
	--depth;
	path = realloc(path, path_len * sizeof(uintptr_t) - sizeof(uintptr_t));
	--path_len;
	dir = path[path_len - 1];
	cursor = 0;
}

void logic_directory_cursor_set_next()
{
	++cursor;
	if(cursor > dir->dirs_len - 1)
		cursor = 0;
}

void logic_directory_cursor_set_previous()
{
	--cursor;
	if(cursor > dir->dirs_len - 1)
		cursor = dir->dirs_len - 1;
}
