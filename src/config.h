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


#include "colors.h"

// COLORS
#define FILENAME_COLOR  BOLD FCOLOR_MAGENTA
#define TODO_COLOR      FCOLOR_BLUE
#define FIXME_COLOR     FCOLOR_YELLOW
#define BUG_COLOR       FCOLOR_RED
#define NOTE_COLOR      FCOLOR_CYAN
#define DONE_COLOR      FCOLOR_GREEN
#define LINE_COLOR      FCOLOR_WHITE
#define RESET_COLOR     RESET


// PRINTING
const char *FILE_PREFIX = " ";
const char *FILE_SUFFIX = " ";
const char *TAG_PREFIX = "  * ";
const char *TAG_SUFFIX = "";
const char *LINE_PREFIX = "";
const char *LINE_SUFFIX = "";
