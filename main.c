#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <memory.h>
#include <stdlib.h>

#define KEY_UP '8'
#define KEY_DOWN '5'
#define KEY_BACK '4'
#define KEY_ENTER '6'

DIR * g_cwd = NULL;

bool is_control_key(char key)
{
	return
		key == KEY_UP ||
		key == KEY_DOWN ||
		key == KEY_BACK ||
		key == KEY_ENTER;
}

bool is_folder(char * name)
{
	for(char * ptr = name; *ptr; ++ptr)
		if(*ptr == '.')
			return false;
	return true;
}

void refresh_cwd()
{
	if(g_cwd)
		closedir(g_cwd);
	char cwd_path[PATH_MAX];
	getcwd(cwd_path, PATH_MAX);
	g_cwd = opendir(cwd_path);
}

void rewind_dir()
{
	chdir("..");
	refresh_cwd();
}

void enter_dir(char * dir)
{
	chdir(dir);
	refresh_cwd();
}

void read_subdirs(char *** out_dirs, size_t * out_dirs_len)
{
	*out_dirs = NULL;
	*out_dirs_len = 0;
	struct dirent * curr = NULL;
	while(curr = readdir(g_cwd))
	{
		if(!is_folder(curr->d_name))
			continue;
		*out_dirs = realloc(*out_dirs, *out_dirs_len * sizeof(void *) + sizeof(void *));
		(*out_dirs)[*out_dirs_len] = strdup(curr->d_name);
		++*out_dirs_len;
	}
}

void free_dirs(char ** dirs, size_t len)
{
	for(size_t i = 0; i < len; ++i)
		free(dirs[i]);
	free(dirs);
}

int main()
{
	refresh_cwd();
	size_t curpos = 0;
	char ** dirs;
	bool refresh_subdirs = true;
	size_t dirs_len;
	while(true)
	{
		puts("-----------------------------------");
		if(refresh_subdirs)
			read_subdirs(&dirs, &dirs_len);
		printf("found subdirs : %llu\n", dirs_len);
		printf("curpos : %llu\n", curpos);
		for(size_t i = 0; i < dirs_len; ++i)
		{
			if(curpos == i)
				fputs("--> ", stdout);
			puts(dirs[i]);
		}
		char key;
		do { key = getch(); } while(!is_control_key(key));
		switch(key)
		{
			case KEY_UP:
				--curpos;
				printf("curpos : %llu\n", curpos);
				printf("dirs_len : %llu\n", dirs_len);
				if(curpos > dirs_len)
					curpos = dirs_len;
				refresh_subdirs = false;
			case KEY_DOWN:
				++curpos;
				if(curpos > dirs_len)
					curpos = dirs_len;
				refresh_subdirs = false;
		}
		if(refresh_subdirs)
			free_dirs(dirs, dirs_len);
	}
	return 0;
}
