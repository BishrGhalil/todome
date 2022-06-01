/* todome - dna_string - string implementation
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dna_string.h"

STRING *string_new(char *data, size_t n) {
  STRING *str = (STRING *)malloc(sizeof(*str));
  if (n < DEFAULT_STRING_SIZE) {
    str->size = DEFAULT_STRING_SIZE;
  } else {
    str->size = DEFAULT_STRING_SIZE + n;
  }
  str->data = (char *)malloc(sizeof(char) * str->size);
  str->len = n;
  while (n--) {
    str->data[n] = data[n];
  }

  str->data[str->len] = '\0';
  return str;
}

void string_printinfo(STRING *str) {
  printf("(%zu), %s\n", str->len, str->data);
}

STRING *string_nresize(STRING *str, size_t newsize) {
  str->size = newsize;
  str->data = (char *)realloc(str->data, newsize);
  return str;
}

STRING *string_resize(STRING *str) {
  return string_nresize(str, str->size + DEFAULT_STRING_SIZE);
}

void string_free(STRING *str) {
  free(str->data);
  free(str);
}

void string_concat(STRING *dest, STRING *src) {
  dest->size += src->size;
  dest->len += src->len;
  dest->data = (char *)realloc(dest->data, dest->size);
  strncat(dest->data, src->data, src->len);
}

void string_charr_concat(STRING *dest, char *src, size_t n) {
  if (dest->size <= dest->len + n) {
    string_nresize(dest, dest->size + n);
  }

  strncat(dest->data, src, n);
  dest->len+= n;
}

STRING *string_rstrip(STRING *dest, char ch) {
  while (dest->data[dest->len - 1] == ch) {
    dest->data[--dest->len] = '\0';
  }
  return dest;
}

STRING *string_lstrip(STRING *dest, char ch) {
  int i = 0;
  while (dest->data[i++] == ch)
    ;
  i -= 1;
  STRING *tmp = string_new(&dest->data[i], dest->len - i);
  string_free(dest);
  dest = tmp;
  return dest;
}

STRING *string_strip(STRING *dest, char ch) {
  string_lstrip(dest, ch);
  string_rstrip(dest, ch);
  return dest;
}

char *string_data(STRING *str) { return str->data; }
