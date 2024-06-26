//
// Created by César B. on 11/16/2023.
//

#include <dirent.h>
#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <Windows.h>
#include "dir_tree.h"
#include "../logic.h"
#include "../../shared/fio.h"

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

struct mk_dir_tree_dir_thread_args
{
	struct dir_tree_dir * dir; // dir to write results into
	char * path;
	char * name;
	size_t threading_depth;
};

// forwarded for the thread procedure
dir_tree_dir_s mk_dir_tree_dir(char * path, char * name, size_t threading_depth);

// pthread thread procedure that calls mk_dir_tree_dir(). used only by mk_dir_tree_dir() itself.
void * mk_dir_tree_dir_thread_proc(struct mk_dir_tree_dir_thread_args * args)
{
	// increase thread count stat
	pthread_mutex_lock(&g_logic.analysis_stats.thread_count_mutex);
	++g_logic.analysis_stats.thread_count;
	if(g_logic.analysis_stats.thread_count > g_logic.analysis_stats.max_recorded_threads)
		g_logic.analysis_stats.max_recorded_threads = g_logic.analysis_stats.thread_count;
	pthread_mutex_unlock(&g_logic.analysis_stats.thread_count_mutex);

	// make the dir tree directory and free the arguments as they're no longer needed
	*args->dir = mk_dir_tree_dir(args->path, args->name, args->threading_depth);
	free(args);

	// increase computed directories stat
	pthread_mutex_lock(&g_logic.analysis_stats.computed_directories_count_mutex);
	++g_logic.analysis_stats.computed_directories_count;
	pthread_mutex_unlock(&g_logic.analysis_stats.computed_directories_count_mutex);

	// decrease thread count stat
	pthread_mutex_lock(&g_logic.analysis_stats.thread_count_mutex);
	--g_logic.analysis_stats.thread_count;
	pthread_mutex_unlock(&g_logic.analysis_stats.thread_count_mutex);
}

/// @param threading_depth current path depth, used to determine whether next recursive calls should be parallelized
dir_tree_dir_s mk_dir_tree_dir(char * path, char * name, size_t threading_depth)
{
	// max depth at which recursive calls for subdirectories will be parallelized
	enum { MAX_THREADING_DEPTH = LOGIC_ANALYSIS_MAX_THREADING_DEPTH };

	// used to check the result of pthread calls
	int r;

	dir_tree_dir_s result;
	char * new_path;
	char ** dir_names;
	char ** file_names;

	memset(&result, 0, sizeof(dir_tree_dir_s));

	new_path = malloc(strlen(path) + strlen(name) + 2);
	sprintf(new_path, "%s/%s", path, name);
	size_t new_path_len = strlen(new_path);

	read_subobjects(new_path, &dir_names, &result.dirs_len, true);
	read_subobjects(new_path, &file_names, &result.files_len, false);

	result.dirs = malloc(sizeof(uintptr_t) * result.dirs_len);
	pthread_t * threads = threading_depth < MAX_THREADING_DEPTH ? malloc(sizeof(pthread_t) * result.dirs_len) : NULL;
	if(threads)
	{
		struct mk_dir_tree_dir_thread_args ** thread_args_arr = malloc(sizeof(intptr_t) * result.dirs_len);
		// run threads
		for(size_t i = 0; i < result.dirs_len; ++i)
		{
			struct dir_tree_dir * dir =
				result.dirs[i] = malloc(sizeof(dir_tree_dir_s));
			dir->name = dir_names[i];
			struct mk_dir_tree_dir_thread_args * args =
				thread_args_arr[i] = malloc(sizeof(struct mk_dir_tree_dir_thread_args));
			args->dir = dir;
			args->path = new_path;
			args->name = dir_names[i];
			args->threading_depth = threading_depth + 1;
			r = pthread_create(&threads[i], NULL, mk_dir_tree_dir_thread_proc, args);
			assert(!r);
		}
		// join threads
		for(size_t i = 0; i < result.dirs_len; ++i)
		{
			r = pthread_join(threads[i], NULL);
			assert(!r);
			struct dir_tree_dir * dir = result.dirs[i];
			dir->name = dir_names[i];
			result.size += dir->size;
		}
		free(thread_args_arr);
		free(threads);
	}
	else
		for(size_t i = 0; i < result.dirs_len; ++i)
		{
			struct dir_tree_dir * dir =
				result.dirs[i] = malloc(sizeof(dir_tree_dir_s));
			*dir = mk_dir_tree_dir(new_path, dir_names[i], threading_depth + 1);
			dir->name = dir_names[i];
			result.size += dir->size;

			pthread_mutex_lock(&g_logic.analysis_stats.computed_directories_count_mutex);
			++g_logic.analysis_stats.computed_directories_count;
			pthread_mutex_unlock(&g_logic.analysis_stats.computed_directories_count_mutex);
		}

	result.files = malloc(sizeof(uintptr_t) * result.files_len);
	for(size_t i = 0; i < result.files_len; ++i)
	{
		dir_tree_file_s * file_info =
			result.files[i] = malloc(sizeof(dir_tree_file_s));

		char * filename =
			file_info->name = file_names[i];

		char * full_file_path = malloc(new_path_len + strlen(filename) + 2);
		sprintf(full_file_path, "%s/%s", new_path, filename);

		FILE * file = fopen(full_file_path, "rb");

		free(full_file_path);

		if(!file) // TODO do smth about that
		{
			file_info->size = 0;
			continue;
		}
		fseek(file, 0, SEEK_END);
		file_info->size = ftell(file);
		fclose(file);
		result.size += file_info->size;
	}
	return result;
}

dir_tree_s mk_dir_tree()
{
	// used for pthread function call results
	int r;

	// initialize the tree
	dir_tree_s tree;
	tree.root = malloc(sizeof(dir_tree_dir_s));

	// get the current working directory
	char * cwd = get_cwd();

	// initialize mk stats, init mk stat mutexes
	memset(&g_logic.analysis_stats, 0, sizeof(struct g_logic_analysis_stats_struct));
	r = pthread_mutex_init(&g_logic.analysis_stats.thread_count_mutex, NULL);
	assert(!r);
	r = pthread_mutex_init(&g_logic.analysis_stats.computed_directories_count_mutex, NULL);
	assert(!r);

	// run mk_dir_tree_dir()
	*tree.root = mk_dir_tree_dir(cwd, "", 0);

	// debug infinite loop. used for ui debugging so far.
	__ATOMIC_ACQUIRE;
#if LOGIC_INFINITE_ANALYSIS
	for(;;);
#endif

	// inform other threads we're done with the analysis here, they don't need to know we're still destroying stuff
	__ATOMIC_ACQUIRE;
	g_logic.analysis_stats.is_done = true;
	__ATOMIC_RELEASE; // note : the release here ain't really required but I still did it by safety, I might end up writing order-dependant code below later who knows

	// destroy mk stats, destroy mutexes
	r = pthread_mutex_destroy(&g_logic.analysis_stats.thread_count_mutex);
	assert(!r);
	r = pthread_mutex_destroy(&g_logic.analysis_stats.computed_directories_count_mutex);
	assert(!r);

	// free cwd
	free(cwd);

	return tree;
}
