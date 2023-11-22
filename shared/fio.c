//
// Created by César B. on 11/16/2023.
//

#include "fio.h"
#include <limits.h>
#include <malloc.h>

char * get_cwd()
{
	char * cwd = malloc(PATH_MAX);
	getcwd(cwd, PATH_MAX);
	return cwd;
}

bool is_folder(char * name)
{
	for(char * ptr = name; *ptr; ++ptr)
		if(*ptr == '.')
			return false;
	return true;
}
