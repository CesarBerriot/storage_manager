//
// Created by César B. on 11/16/2023.
//

#include "cwd.h"
#include <limits.h>

DIR * g_cwd = NULL;

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

bool is_folder(char * name)
{
	for(char * ptr = name; *ptr; ++ptr)
		if(*ptr == '.')
			return false;
	return true;
}
