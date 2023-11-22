//
// Created by César B. on 11/16/2023.
//

#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include "dir_tree.h"
#include "../shared/fio.h"

void read_subobjects(char * path, char *** out_objects, size_t * out_objects_len, bool dir_mode)
{
	DIR * dirstrm = opendir(path);
	*out_objects = NULL;
	*out_objects_len = 0;
	struct dirent * curr = NULL;
	//skip . and ..
	readdir(dirstrm);
	readdir(dirstrm);
	while(curr = readdir(dirstrm))
	{
		if(dir_mode ^ is_folder(curr->d_name))
			continue;
		*out_objects = realloc(*out_objects, *out_objects_len * sizeof(void *) + sizeof(void *));
		(*out_objects)[*out_objects_len] = strdup(curr->d_name);
		++*out_objects_len;
	}
	closedir(dirstrm);
}

void free_obj_arr(char ** objects, size_t len)
{
	for(size_t i = 0; i < len; ++i)
		free(objects[i]);
	free(objects);
}

dir_tree_dir_s mk_dir_tree_dir(char * path, char * name)
{
	dir_tree_dir_s result;
	char * new_path;
	char ** dir_names;
	char ** file_names;
	memset(&result, 0, sizeof(dir_tree_dir_s));
	new_path = malloc(strlen(path) + strlen(name) + 2);
	sprintf(new_path, "%s/%s", path, name);
	read_subobjects(new_path, &dir_names, &result.dirs_len, true);
	read_subobjects(new_path, &file_names, &result.files_len, false);
	result.dirs = malloc(sizeof(uintptr_t) * result.dirs_len);
	for(size_t i = 0; i < result.dirs_len; ++i)
	{
		result.dirs[i] = malloc(sizeof(dir_tree_dir_s));
		*result.dirs[i] = mk_dir_tree_dir(new_path, dir_names[i]);
		result.dirs[i]->name = dir_names[i];
	}
	result.files = malloc(sizeof(uintptr_t) * result.files_len);
	for(size_t i = 0; i < result.files_len; ++i)
	{
		result.files[i] = malloc(sizeof(dir_tree_file_s));
		result.files[i]->name = file_names[i];
		result.files[i]->size = 0; //TODO actually acquire the size
	}
//	DIR * dirstrm = opendir(new_path);
//	assert(dirstrm);
//	struct dirent * currdir = readdir(dirstrm);
//	while(currdir)
//	{
//		char * dir_name_cpy = strdup(currdir->d_name);
//		result.dirs = realloc(result.dirs, result.dirs_len * sizeof(void*) + sizeof(void*));
//		result.dirs[result.dirs_len-1] = malloc(sizeof(dir_tree_dir_s));
//		*result.dirs[result.dirs_len-1] = mk_dir_tree_dir(new_path, dir_name_cpy);
//		free(dir_name_cpy);
//		currdir = readdir(dirstrm);
//	}
//	free(new_path);
	return result;
}

dir_tree_s mk_dir_tree()
{
	dir_tree_s tree;
	tree.root = malloc(sizeof(dir_tree_dir_s));
	char * cwd = get_cwd();
	*tree.root = mk_dir_tree_dir(cwd, "");
	free(cwd);
	return tree;
}
