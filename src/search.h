/* todome - search - search and print functions
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

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "binSTree.h"
#include "config.h"
#include "dna_string.h"
#include "errors.h"
#include "recdir.h"
#include "tag.h"

// TODOOO: Prints the line after the tag

#define OVECCOUNT (3 * 2)

typedef enum { SH_STOP_COLORS = 2, SH_HIDDEN = 4 } SH_OPTIONS;

typedef struct {
  char *content;
  int size;
} MAPED_FILE;

MAPED_FILE *map_filepath(const char *path) {

  int fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
  struct stat sb;
  MAPED_FILE *file = (MAPED_FILE *)malloc(sizeof(*file));

  if (fd == -1 || errno == EACCES || fstat(fd, &sb) == -1 || sb.st_size < 5) {
    free(file);
    close(fd);
    return NULL;
  }

  char *filebody =
      (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (filebody == MAP_FAILED) {
    free(file);
    return NULL;
  }

  file->content = filebody;
  file->size = sb.st_size;
  return file;
}

void free_mapedfile(MAPED_FILE *file) {
  munmap(file->content, file->size);
  free(file);
}

char *get_tag_color(TAG *tag) {
  switch (tag->type) {
  case TODO:
    return (char *)TODO_COLOR;
  case FIXME:
    return (char *)FIXME_COLOR;
  case BUG:
    return (char *)BUG_COLOR;
  case NOTE:
    return (char *)NOTE_COLOR;
  case DONE:
    return (char *)DONE_COLOR;
  default:
    return (char *)LINE_COLOR;
  }
}

void colored_print_node(TREE_NODE *node, FILE *fd) {
  if (!node)
    return;
  if (node->right) {
    colored_print_node(node->right, fd);
  }

  fprintf(fd, "%s%s%s%s%s", TAG_PREFIX, get_tag_color(node->tag),
          tag_data(node->tag), RESET_COLOR, TAG_SUFFIX);

  fprintf(fd, LINE_COLOR "%s%s%s\n" RESET_COLOR, LINE_PREFIX,
          string_data(node->line), LINE_SUFFIX);

  if (node->left) {
    colored_print_node(node->left, fd);
  }
}

void print_node(TREE_NODE *node, FILE *fd) {
  if (!node)
    return;
  if (node->right) {
    print_node(node->right, fd);
  }

  fprintf(fd, "%s%s%s", TAG_PREFIX, tag_data(node->tag), TAG_SUFFIX);
  fprintf(fd, "%s%s%s\n", LINE_PREFIX, string_data(node->line), LINE_SUFFIX);

  if (node->left) {
    print_node(node->left, fd);
  }
}

void strgrep(const char *filepath, pcre *re, FILE *output_fd, SH_OPTIONS opt) {
  int saved_errno = errno;
  int ovector[OVECCOUNT];
  int rc = 0;
  int tmp_len = 0;
  char *tmp;
  MAPED_FILE *file;

  file = map_filepath(filepath);
  if (file == NULL) {
    errno = saved_errno;
    return;
  }

  BINSTREE *tree = binstree_new();
  tmp = (char *)file->content;
  tmp_len = file->size;
  rc = pcre_exec(re, NULL, tmp, tmp_len, 0, 0, ovector, OVECCOUNT);

  if (rc > 0) {
    if (!(opt & SH_STOP_COLORS)) {
      fprintf(output_fd, FILENAME_COLOR "%s%s%s" RESET_COLOR "\n", FILE_PREFIX,
              filepath, FILE_SUFFIX);
    } else {
      fprintf(output_fd, "%s%s%s\n", FILE_PREFIX, filepath, FILE_SUFFIX);
    }
  }

  while (rc > 0) {
    unsigned int line_len = ovector[1] - ovector[3];
    unsigned int tag_len = ovector[3] - ovector[2];

    if (!is_valid_tag_level(tag_len)) {
      tag_len = MAX_TAG_LENGTH;
    }

    STRING *tag_str = string_new(&tmp[ovector[2]], tag_len);
    STRING *line_str = string_new(&tmp[ovector[3]], line_len);
    TAG *tag = tag_new(tag_str, get_tag_type(tag_str->data));
    TREE_NODE *node = tnode_new(tag, line_str);

    binstree_insert(tree, node);
    rc = pcre_exec(re, NULL, tmp, tmp_len, ovector[1], 0, ovector, OVECCOUNT);
  }

  if (tree->root) {
    if (!(opt & SH_STOP_COLORS))
      colored_print_node(tree->root, output_fd);
    else {
      print_node(tree->root, output_fd);
    }
    fprintf(output_fd, "\n");
  }

  binstree_free(tree);
  free_mapedfile(file);
}

void ripgrep(const char *dir_path, pcre *re, FILE *output_fd, SH_OPTIONS opt) {
  struct dirent *ent;
  RECDIR *recdir = recdir_open((const char *)dir_path);
  errEAssert(recdir != NULL, "\'%s\' not a valid directory path\n", dir_path);

  while ((ent = recdir_read(recdir, opt & SH_HIDDEN))) {
    char *path = join_path(recdir_top(recdir)->path, ent->d_name);
    strgrep(path, re, output_fd, opt);
    free(path);
  }

  recdir_close(recdir);
}
