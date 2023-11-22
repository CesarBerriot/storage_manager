//
// Created by César B. on 10/16/2023.
//

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include "shared/fio.h"
#include "dir_tree/dir_tree.h"

int main()
{
	dir_tree_s tree = mk_dir_tree();
	dir_tree_dir_s * currdir = tree.root;
	dir_tree_dir_s ** curr_dir_path = malloc(sizeof(uintptr_t));
	curr_dir_path[0] = currdir;
	size_t curr_dir_path_len = 1;
	size_t curr_dir_depth = 0;
	size_t curpos = 0;
	while(true)
	{
		puts("--------------------------------------------");
		fputs("path : ", stdout);
		for(register uint8_t i = 0; i < curr_dir_path_len; ++i)
		{
			fputs(curr_dir_path[i]->name, stdout);
			fputc('/', stdout);
		}
		fputc('\n', stdout);
		fputs("dir : ", stdout);
		puts(currdir->name);
		puts("subdirs :");
		for(size_t i = 0; i < currdir->dirs_len; ++i)
		{
			if(curpos == i)
				fputs("--> ", stdout);
			puts(currdir->dirs[i]->name);
		}
		puts("files :");
		for(size_t i = 0; i < currdir->files_len; ++i)
			puts(currdir->files[i]->name);
		char c;
		while((c = getchar()) == '\n');
		switch(c)
		{
			case '+':
				++curpos;
				if(curpos > currdir->dirs_len - 1)
					curpos = 0;
				break;
			case '-':
				--curpos;
				if(curpos > currdir->dirs_len - 1)
					curpos = currdir->dirs_len - 1;
				break;
			case '/':
				++curr_dir_depth;
				curr_dir_path = realloc(curr_dir_path, curr_dir_path_len * sizeof(uintptr_t) + sizeof(uintptr_t));
				curr_dir_path[curr_dir_path_len] = currdir;
				++curr_dir_path_len;
				currdir = currdir->dirs[curpos];
				curpos = 0;
				break;
			case '.':
				if(!curr_dir_depth)
					break;
				--curr_dir_depth;
				curr_dir_path = realloc(curr_dir_path, curr_dir_path_len * sizeof(uintptr_t) - sizeof(uintptr_t));
				--curr_dir_path_len;
				currdir = curr_dir_path[curr_dir_path_len - 1];
				curpos = 0;
				break;
		}
	}
	free_dir_tree(tree);
}
