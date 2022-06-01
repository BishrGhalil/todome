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


#ifndef STRING_TYPE_H_
#define STRING_TYPE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_SIZE 128

typedef struct {
  char *data;
  size_t len;
  size_t size;
} STRING;

STRING *string_new(char *data, size_t n);

void string_printinfo(STRING *str);
void string_free(STRING *str);
void string_concat(STRING *dest, STRING *src);
void string_charr_concat(STRING *dest, char *src, size_t n);
STRING *string_lstrip(STRING *dest, char ch);
STRING *string_rstrip(STRING *dest, char ch);
STRING *string_strip(STRING *dest, char ch);
char *string_data(STRING *str);
#endif
