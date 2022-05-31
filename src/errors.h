/* errors - errors handling functions
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

#ifndef ERRORS_H_
#define ERRORS_H_
#ifdef __GNUC__
/* This macro stops 'gcc -Wall' complaining that "control reaches
       end of non-void function" if we use the following functions to
       terminate main() or some other non-void function. */
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asprintf.h"

static void terminate(bool useExit3);
void errExit(const char *format, ...);
static void outputError(bool flushStdout, const char *format, va_list ap);
void errMsg(const char *format, ...);

static void terminate(bool useExit3) {
  char *s;
  /* Dump core if EF_DUMPCORE environment variable is defined and
     is a nonempty string; otherwise call exit(3) or _exit(2), */
  s = getenv("EF_DUMPCORE");
  if (s != NULL && *s != '\0')
    abort();
  else if (useExit3)
    exit(EXIT_FAILURE);
  else
    _exit(EXIT_FAILURE);
}
void errExit(const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  outputError(true, format, argList);
  va_end(argList);

  terminate(true);
}

static void outputError(bool flushStdout, const char *format, va_list ap) {
#define BUF_SIZE 1000
  char *userMsg, errText[BUF_SIZE];
  char *buf;
  vasprintf(&userMsg, format, ap);
  snprintf(errText, BUF_SIZE, "ERROR:");
  asprintf(&buf, "%s %s\n", errText, userMsg);
  if (flushStdout)
    fflush(stdout);
  fputs(buf, stderr);
  fflush(stderr);
}

void errMsg(const char *format, ...) {
  va_list argList;
  int savedErrno;
  savedErrno = errno; /* In case we change it here */
  va_start(argList, format);
  outputError(true, format, argList);
  va_end(argList);

  errno = savedErrno;
}

// Assert a message
int errAssert(int exp, const char *msg, ...) {
  if (!exp) {
    int savedErrno;
    savedErrno = errno;
    va_list argList;
    va_start(argList, msg);
    outputError(true, msg, argList);
    va_end(argList);
    errno = savedErrno;
    return 1;
  }
  return 0;
}

// Assert and Exit
void errEAssert(int exp, const char *msg, ...) {
  if (!exp) {
    int savedErrno;
    savedErrno = errno;
    va_list argList;
    va_start(argList, msg);
    outputError(true, msg, argList);
    va_end(argList);
    errno = savedErrno;

    terminate(true);
  }
}

#endif
