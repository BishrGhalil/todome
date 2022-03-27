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

#define _DEFAULT_SOURCE
#include "./recdir.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

char *join_path(const char *base, const char *file) {
  size_t base_len = strlen(base);
  size_t file_len = strlen(file);

  char *begin = (char *)malloc(base_len + file_len + PATH_SEP_LEN + 1);
  assert(begin != NULL);
  begin[0] = '\0';

  begin = strncat(begin, base, base_len);

  if (base[base_len - 1] != '/' && file[0] != '/') {
    begin = strncat(begin, "/", 2);
  }

  begin = strncat(begin, file, file_len);

  return begin;
}


RECDIR_Frame *recdir_top(RECDIR *recdir) {
  assert(recdir->stack_size > 0);
  return &recdir->stack[recdir->stack_size - 1];
}


int recdir_push(RECDIR *recdir, char *path) {
  assert(recdir->stack_size < RECDIR_STACK_CAP);
  recdir->stack_size += 1;
  RECDIR_Frame *top = recdir_top(recdir);
  top->path = path;
  top->dir = opendir(top->path);
  if (top->dir == NULL) {
    recdir_pop(recdir);
    return -1;
  }
  return 0;
}


void recdir_pop(RECDIR *recdir) {
  assert(recdir->stack_size > 0);
  RECDIR_Frame *top = &recdir->stack[--recdir->stack_size];
  if (top->dir != NULL) {
    int ret = closedir(top->dir);
    assert(ret == 0);
  }
  free(top->path);
}


RECDIR *recdir_open(const char *dir_path) {
  RECDIR *recdir = malloc(sizeof(RECDIR));
  assert(recdir != NULL);
  memset(recdir, 0, sizeof(RECDIR));

  if (recdir_push(recdir, strdup(dir_path)) < 0) {
    free(recdir);
    return NULL;
  }

  return recdir;
}


bool is_valid_dir(char *dirname) {
  if (!is_hidden(dirname))
    return true;

  else if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0)
    return false;

  else
    return true;
}


bool is_hidden(char *dirname) {
  if (dirname[0] == '.')
    return true;

  return false;
}


struct dirent *recdir_read(RECDIR *recdir, int read_hidden) {
  while (recdir->stack_size > 0) {
    RECDIR_Frame *top = recdir_top(recdir);
    struct dirent *ent = readdir(top->dir);
    if (!ent) {
      if (errno)
        return NULL;
      else {
        recdir_pop(recdir);
        continue;
      }

    } else {

      if (ent->d_type == DT_DIR) {
          if ((!read_hidden && is_hidden(ent->d_name)) || !is_valid_dir(ent->d_name))
            continue;

          recdir_push(recdir, join_path(top->path, ent->d_name));
      } else
        return ent;

    }
  }

  return NULL;
}


void recdir_close(RECDIR *recdir) {
  while (recdir->stack_size > 0) {
    recdir_pop(recdir);
  }
  free(recdir);
}
