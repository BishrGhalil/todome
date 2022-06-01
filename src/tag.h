/* todome - tag - tag type and macros
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

#ifndef TAG_H_
#define TAG_H_

#include "dna_string.h"
#include <stdlib.h>

#define MAX_TAG_LENGTH 15
#define MAX_TAG_LEVEL 10

typedef enum { DONE, NOTE, TODO, FIXME, BUG } TAG_TYPE;

typedef struct {
  STRING *str;
  TAG_TYPE type;
  size_t level;
} TAG;

int is_valid_tag_level(int tag_len) {
  if (tag_len > MAX_TAG_LENGTH)
    return 0;
  return 1;
}

TAG_TYPE get_tag_type(char *tag) {
  switch (tag[0]) {
  case 'T':
    return TODO;
  case 'F':
    return FIXME;
  case 'B':
    return BUG;
  case 'N':
    return NOTE;
  case 'D':
    return DONE;
  default:
    return TODO;
  }
}

size_t tag_level(TAG *tag) {
  int start_index = 0;
  switch (tag->type) {
  case TODO:
  case DONE:
  case NOTE:
    start_index = 3;
    break;
  case BUG:
    start_index = 2;
    break;
  case FIXME:
    start_index = 4;
    break;
  }

  return (size_t)(tag->str->len - start_index);
}

TAG *tag_new(STRING *str, TAG_TYPE type) {
  TAG *tag = (TAG *)malloc(sizeof(*tag));
  tag->type = type;
  tag->str = str;
  tag->level = tag_level(tag);

  return tag;
}

int tag_cmp(TAG *a, TAG *b) {
  if (a->type > b->type || (a->type == b->type && a->level > b->level)) {
    return 1;
  }

  else if (a->type == b->type && a->level == b->level) {
    return 0;
  }
  return -1;
}

char *tag_data(TAG *tag) { return tag->str->data; }

void tag_free(TAG *tag) {
  string_free(tag->str);
  free(tag);
}

#endif
