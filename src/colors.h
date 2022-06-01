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


#ifndef COLORS_H_
#define COLORS_H_

#define RESET "\e[0m"

#define BOLD "\e[1m"
#define RSTBOLD "\e[21m"

#define DIM "\e[2m"
#define RSTDIM "\e[22m"

#define UNDER "\e[4m"
#define RSTUNDER "\e[24m"

#define BLINK "\e[5m"
#define RSTBLINK "\e[25m"

#define REVERSE "\e[7m"
#define RSTREVERSE "\e[27m"

#define HIDDEN "\e[8m"
#define RSTHIDDEN "\e[28m"

// Forground colors
#define FCOLOR_DFLT "\e[39m" // default forground color
#define FCOLOR_BLACK "\e[30m"
#define FCOLOR_RED "\e[31m"
#define FCOLOR_LRED "\e[91m"
#define FCOLOR_GREEN "\e[32m"
#define FCOLOR_LGREEN "\e[92m"
#define FCOLOR_YELLOW "\e[33m"
#define FCOLOR_LYELLOW "\e[93m"
#define FCOLOR_BLUE "\e[34m"
#define FCOLOR_LBLUE "\e[94m"
#define FCOLOR_MAGENTA "\e[35m"
#define FCOLOR_LMAGENTA "\e[95m"
#define FCOLOR_CYAN "\e[36m"
#define FCOLOR_LCYAN "\e[96m"
#define FCOLOR_LGRAY "\e[37m"
#define FCOLOR_DGRAY "\e[90m"
#define FCOLOR_WHITE "\e[97m"

// Background colors
#define BCOLOR_DEFAULT "\e[49m" // default background color
#define BCOLOR_BLACK "\e[40m"
#define BCOLOR_RED "\e[41m"
#define BCOLOR_LRED "\e[101m"
#define BCOLOR_GREEN "\e[42m"
#define BCOLOR_LGREEN "\e[102m"
#define BCOLOR_YELLOW "\e[43m"
#define BCOLOR_LYELLOW "\e[103m"
#define BCOLOR_BLUE "\e[44m"
#define BCOLOR_LBLUE "\e[104m"
#define BCOLOR_MAGENTA "\e[45m"
#define BCOLOR_LMAGENTA "\e[105m"
#define BCOLOR_CYAN "\e[46m"
#define BCOLOR_LCYAN "\e[106m"
#define BCOLOR_LGRAY "\e[47m"
#define BCOLOR_DGRAY "\e[100m"
#define BCOLOR_WHITE "\e[107m"

#endif
