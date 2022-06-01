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
#include <getopt.h>
#include <pcre.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "dna_string.h"
#include "errors.h"
#include "recdir.h"
#include "search.h"

#include <argp.h>

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
    {"hidden", 'h', 0, OPTION_ARG_OPTIONAL,
     "Searches hidden directories and files"},
    {"output", 'o', "FILE", OPTION_ARG_OPTIONAL,
     "Output to FILE instead of standard output"},
    {0}};

struct tags_arguments {
  int argc, all, todo, fixme, bug, note, done;
};

struct arguments {
  char *args[2]; /* path & outputfile */
  struct tags_arguments tags;
  int hidden;
  FILE *output_file;
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
    arguments->hidden = 1;
    break;
  case 'o':
    arguments->output_file = fopen(arg, "w");
    break;

  case ARGP_KEY_ARG:
    if (state->arg_num >= 1) // Too many arguments.
      argp_usage(state);

    arguments->args[state->arg_num] = arg;

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

  arguments.hidden = 0;

  arguments.args[0] = NULL;
  arguments.args[1] = NULL;

  arguments.dir_path = ".";
  arguments.output_file = stdout;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (arguments.args[0])
    arguments.dir_path = arguments.args[0];

  STRING *regex_ptrn = create_regex_pattern(arguments.tags);
  pcre *re = compile_regex(regex_ptrn->data);
  string_free(regex_ptrn);

  struct dirent *ent;
  RECDIR *recdir = recdir_open(arguments.dir_path);
  errEAssert(recdir != NULL, "\'%s\' not a valid directory path\n",
             arguments.dir_path);

  while ((ent = recdir_read(recdir, arguments.hidden))) {
    char *path = join_path(recdir_top(recdir)->path, ent->d_name);
    strgrep(path, re, arguments.output_file);
    free(path);
  }

  recdir_close(recdir);
  pcre_free(re);
  errEAssert(errno == 0, strerror(errno));
  fclose(arguments.output_file);
  return 0;
}
