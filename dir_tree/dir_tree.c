//
// Created by César B. on 11/16/2023.
//

#include "dir_tree.h"
#include <stdlib.h>

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
