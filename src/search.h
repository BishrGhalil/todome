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
#include "tag.h"

#define OVECCOUNT (3 * 2)

typedef struct {
  char *content;
  int size;
} MAPED_FILE;

int println(const char *src) {
  int i = 0;
  while (src[i] != '\0' && src[i] != '\n') {
    putchar(src[i++]);
  }
  putchar('\n');
  return i + 2;
}

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

void strgrep(const char *filepath, pcre *re, FILE *fd) {
  int saved_errno = errno;
  int ovector[OVECCOUNT];
  int rc = 0;
  int tmp_len = 0;
  int matched = 0;
  int print_colors = 1;
  char *tmp;
  MAPED_FILE *file;

  if (fd != stdout)
    print_colors = 0;

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
    if (print_colors) {
      fprintf(fd, FILENAME_COLOR "%s%s%s" RESET_COLOR "\n", FILE_PREFIX,
              filepath, FILE_SUFFIX);
    } else {
      fprintf(fd, "%s%s%s\n", FILE_PREFIX, filepath, FILE_SUFFIX);
    }
  }

  while (rc > 0) {
    matched += 1;
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
    if (!print_colors)
      print_node(tree->root, fd);
    else {
      colored_print_node(tree->root, fd);
    }
    fprintf(fd, "\n");
  }

  binstree_free(tree);
  free_mapedfile(file);
}
