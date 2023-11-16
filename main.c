//
// Created by César B. on 10/16/2023.
//

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include "shared/cwd.h"






int main()
{
	setbuf(stdout, 0);
	refresh_cwd();
}
