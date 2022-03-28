#include <assert.h>
#include <fcntl.h>
#include <pcre.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "colors.h"
#include "errors.h"

#define OVECCOUNT 3000

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

char *strnsub(char *dest, const char *src, int start, int end) {
  strncpy(dest, &src[start], end - start + 1);
  return dest;
}

MAPED_FILE *map_filepath(const char *path) {

  int fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
  struct stat sb;
  MAPED_FILE *file = (MAPED_FILE *)malloc(sizeof(MAPED_FILE *));

  if (fd == -1 || errno == EACCES || fstat(fd, &sb) == -1 || sb.st_size < 5) {
    return NULL;
  }

  char *filebody =
      (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (filebody == MAP_FAILED) {
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

int strgrep(const char *filepath, pcre *re) {
  int ovector[OVECCOUNT];
  int rc = 0;
  int match_start = 0;
  int match_end = 0;
  int tmp_len = 0;
  int matched = 0;
  char *tmp;
  MAPED_FILE *file;

  file = map_filepath(filepath);
  if (file == NULL) {
    return matched;
  }

  tmp = (char *)file->content;
  tmp_len = strlen(tmp);
  rc = pcre_exec(re, NULL, tmp, tmp_len, 0, 0, ovector, OVECCOUNT);
  if (rc > 0) {
    printf(FCOLOR_GREEN "%s\n" RESET, filepath);
    matched = 1;
  }

  while (rc > 0) {
    match_start = ovector[0];
    match_end = ovector[1];

    char *dest = (char *)malloc(match_end - match_start);
    dest = strnsub(dest, tmp, match_start, match_end - 1);
    printf(FCOLOR_RED "%s" RESET, dest);
    free(dest);

    int startoffset = println(&tmp[match_end]) + match_start;
    rc = pcre_exec(re, NULL, tmp, tmp_len, startoffset, 0, ovector, OVECCOUNT);
  }
  free_mapedfile(file);
  return matched;
}
