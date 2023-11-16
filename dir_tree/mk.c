//
// Created by César B. on 11/16/2023.
//

#include <dirent.h>
#include <malloc.h>
#include "dir_tree.h"
#include "../shared/cwd.h"

void read_subobjects(char *** out_objects, size_t * out_objects_len, bool dir_mode)
{
	*out_objects = NULL;
	*out_objects_len = 0;
	struct dirent * curr = NULL;
	while(curr = readdir(g_cwd))
	{
		if(dir_mode ^ is_folder(curr->d_name))
			continue;
		*out_objects = realloc(*out_objects, *out_objects_len * sizeof(void *) + sizeof(void *));
		(*out_objects)[*out_objects_len] = strdup(curr->d_name);
		++*out_objects_len;
	}
}

void free_obj_arr(char ** objects, size_t len)
{
	for(size_t i = 0; i < len; ++i)
		free(objects[i]);
	free(objects);
}

dir_tree_dir_s mk_dir_tree_dir(char * name)
{
	refresh_cwd();
	enter_dir(name);
	char ** subdirs;
	size_t subdirs_len;
	read_subobjects(&subdirs, &subdirs_len, true);
	for(size_t i = 0; i < subdirs_len; ++i)
		mk_dir_tree_dir(subdirs[i]);
	char ** files;
	size_t files_len;
	read_subobjects(&files, &files_len, false);

	free(subdirs);
	rewind_dir();
}

dir_tree_s mk_dir_tree()
{
	dir_tree_s tree;
	tree.root = malloc(sizeof(dir_tree_dir_s));
	*tree.root = mk_dir_tree_dir("");
	return tree;
}
