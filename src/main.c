/* todome - search for TODOs FIXMEs BUGs and NOTEs ... in source code
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

#include <dirent.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "errors.h"
#include "recdir.h"
#include "search.h"

int search_hiddens = 1;

pcre *compile_regex(const char *regex) {
  pcre *re;
  const char *error;
  int erroffset;

  re = pcre_compile(regex, 0, &error, &erroffset, NULL);
  errEAssert(re != NULL, "Regex compiling failed at offset %d: %s\n", erroffset,
             error);

  return re;
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  char *dir_path = "/home/bishr/Documents";
  char *regex = "TODO:";
  struct dirent *ent;

  RECDIR *recdir = recdir_open(dir_path);
  pcre *re = compile_regex(regex);

  while ((ent = recdir_read(recdir, search_hiddens))) {
    char *path = join_path(recdir_top(recdir)->path, ent->d_name);

    if (strgrep(path, re))
      putchar('\n');

    free(path);
  }

  pcre_free(re);
  errEAssert(errno == 0, strerror(errno));
  return 0;
}
