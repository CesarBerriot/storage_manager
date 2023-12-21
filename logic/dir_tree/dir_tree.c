//
// Created by César B. on 11/16/2023.
//

#include "dir_tree.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <minwindef.h>

#include "../logic.h"

void free_dir_tree_dir(dir_tree_dir_s * dir)
{
	for(size_t i = 0; i < dir->dirs_len; ++i)
		free_dir_tree_dir(dir->dirs[i]);
	for(size_t i = 0; i < dir->files_len; ++i)
	{
		free(dir->files[i]->name);
		free(dir->files[i]);
	}
}

void free_dir_tree(dir_tree_s tree)
{
	if(tree.root)
		free_dir_tree_dir(tree.root);
	free(tree.root);
}

char * mk_dir_tree_path_str(dir_tree_dir_s * dir)
{
	char * path_str = malloc(MAX_PATH + 1);
	path_str[0] = '\0';
	for(register uint8_t i = 0; i < g_logic.current_dir_path_len; ++i)
	{
		char * dir_name = g_logic.current_dir_path[i]->name;
		if(!dir_name)
			continue;
		strcat(path_str, dir_name);
		strcat(path_str, "/");
	}
	return path_str;
}
