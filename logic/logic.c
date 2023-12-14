//
// Created by César B. on 12/14/2023.
//

#include "logic.h"

#include <malloc.h>
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
