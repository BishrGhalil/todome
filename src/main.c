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

#define _GNU_SOURCE

#include <fcntl.h>
#include <getopt.h>
#include <pcre.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "dna_string.h"
#include "errors.h"
#include "search.h"

#include <argp.h>

sigjmp_buf env;

void sighandler(int sig) {
  (void)sig;
  siglongjmp(env, -1);
  return;
}

const char *argp_program_version = "Version - 2.0";
const char *argp_program_bug_address = "beshrghalil@protonmail.com";

static char doc[] = "todome -- Language agnostic tool that collects TODOs, "
                    "FIXMEs, BUGs, NOTEs and DONEs in the source code.";

static char args_doc[] = "PATH";
static struct argp_option options[] = {
    {"todo", 't', 0, OPTION_ARG_OPTIONAL, "Collects TODOs"},
    {"fixme", 'f', 0, OPTION_ARG_OPTIONAL, "Collects FIXMEs"},
    {"bug", 'b', 0, OPTION_ARG_OPTIONAL, "Collects BUGs"},
    {"note", 'n', 0, OPTION_ARG_OPTIONAL, "Collects NOTEs"},
    {"done", 'd', 0, OPTION_ARG_OPTIONAL, "Collects DONEs"},
    {"input", 'i', "FILE", OPTION_ARG_OPTIONAL, "Search in a provided file"},
    {"hidden", 'h', 0, OPTION_ARG_OPTIONAL, "Searches hiddens"},
    {"output", 'o', "FILE", OPTION_ARG_OPTIONAL, "Output to FILE"},
    {"no-colors", 's', 0, OPTION_ARG_OPTIONAL, "Don't output colors"},
    {0}};

struct tags_arguments {
  int argc, all, todo, fixme, bug, note, done;
};

struct arguments {
  char *args[2]; /* path & outputfile */
  struct tags_arguments tags;
  SH_OPTIONS sh_options;
  char *output_file;
  char *input_file;
  char *dir_path;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 't':
    arguments->tags.todo = 1;
    arguments->tags.argc++;
    break;
  case 'f':
    arguments->tags.fixme = 1;
    arguments->tags.argc++;
    break;
  case 'b':
    arguments->tags.bug = 1;
    arguments->tags.argc++;
    break;
  case 'n':
    arguments->tags.note = 1;
    arguments->tags.argc++;
    break;
  case 'd':
    arguments->tags.done = 1;
    arguments->tags.argc++;
    break;
  case 'h':
    arguments->sh_options |= SH_HIDDEN;
    break;
  case 'o':
    arguments->output_file = state->argv[state->next];
    state->next++;
    break;

  case 'i':
    arguments->input_file = state->argv[state->next];
    state->next++;
    break;

  case 's':
    arguments->sh_options |= SH_STOP_COLORS;
    break;

  case ARGP_KEY_ARG:
    if (state->arg_num >= 2) // Too many arguments.
      argp_usage(state);

    arguments->args[state->arg_num] = arg;
    arguments->dir_path = arg;

    break;

  case ARGP_KEY_END:
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

pcre *compile_regex(const char *regex) {
  pcre *re;
  const char *error;
  int erroffset;

  re = pcre_compile(regex, 0, &error, &erroffset, NULL);
  errEAssert(re != NULL, "Regex compiling failed at offset %d: %s\n", erroffset,
             error);

  return re;
}

STRING *create_regex_pattern(struct tags_arguments tags) {
  if (tags.argc < 1) {
    char *pattern = "(TODO+|FIXME+|BUG+|NOTE+|DONE):.+";
    return string_new(pattern, strlen(pattern));
  }

  STRING *ptrn = string_new("(", 1);

  if (tags.todo)
    string_charr_concat(ptrn, "TODO+|", 6);
  if (tags.fixme)
    string_charr_concat(ptrn, "FIXME+|", 7);
  if (tags.bug)
    string_charr_concat(ptrn, "BUG+|", 5);
  if (tags.note)
    string_charr_concat(ptrn, "NOTE+|", 6);
  if (tags.done)
    string_charr_concat(ptrn, "DONE|", 5);

  string_rstrip(ptrn, '|');
  string_charr_concat(ptrn, "):.+", 4);
  return ptrn;
}

int main(int argc, char **argv) {
  struct arguments arguments;
  arguments.tags.argc = 0;
  arguments.tags.todo = 0;
  arguments.tags.fixme = 0;
  arguments.tags.bug = 0;
  arguments.tags.note = 0;
  arguments.tags.done = 0;

  arguments.sh_options = 1;

  arguments.dir_path = ".";
  arguments.output_file = NULL;
  arguments.input_file = NULL;
  FILE *output_file;

  // Signals Handling
  struct sigaction handler;
  if (sigsetjmp(env, 1) == -1)
    goto CLEAN_UP;

  // Initializing Signals
  handler.sa_handler = sighandler;
  sigemptyset(&handler.sa_mask);
  handler.sa_flags = 0;
  sigaction(SIGINT, &handler, NULL);
  sigaction(SIGABRT, &handler, NULL);
  sigaction(SIGHUP, &handler, NULL);
  sigaction(SIGTERM, &handler, NULL);
  sigaction(SIGTSTP, &handler, NULL);

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  STRING *regex_ptrn = create_regex_pattern(arguments.tags);
  pcre *re = compile_regex(regex_ptrn->data);
  string_free(regex_ptrn);

  if (!arguments.output_file)
    output_file = stdout;

  else {
    if ((output_file = fopen(arguments.output_file, "w")) == NULL) {
      fprintf(stderr, "%s\n", strerror(errno));
      goto CLEAN_UP;
    }
  }

  if (arguments.input_file)
    strgrep(arguments.input_file, re, output_file, arguments.sh_options);
  else
    ripgrep(arguments.dir_path, re, output_file, arguments.sh_options);

CLEAN_UP:
  pcre_free(re);
  errEAssert(errno == 0, strerror(errno));
  fclose(output_file);
  return 0;
}
