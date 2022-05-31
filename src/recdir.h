/* recdir - recursively get files in a directory
   Copyright (C) 2021-2021 Bishr Ghalil.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bishr Ghalil */

#ifndef RECDIR_H_
#define RECDIR_H_

#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#define RECDIR_STACK_CAP 2048

#define PATH_SEP "/"
#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)

typedef struct {
  DIR *dir;
  char *path;
} RECDIR_Frame;

typedef struct {
  RECDIR_Frame stack[RECDIR_STACK_CAP];
  size_t stack_size;
} RECDIR;

char *join_path(const char *base, const char *file);

RECDIR_Frame *recdir_top(RECDIR *recdir);
int recdir_push(RECDIR *recdir, char *path);
void recdir_pop(RECDIR *recdir);

RECDIR *recdir_open(const char *dir_path);
struct dirent *recdir_read(RECDIR *recdirp, int hidden);
void recdir_close(RECDIR *recdirp);
bool is_valid_dir(const char *dirname);
bool is_hidden(const char *dirname);

#endif // RECDIR_H_
