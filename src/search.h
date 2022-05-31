#include <assert.h>
#include <fcntl.h>
#include <pcre.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "errors.h"

#define OVECCOUNT (3 * 2)
#define MAX_TAG_LENGTH 15

typedef enum { TODO, FIXME, BUG, NOTE, DONE } TAG_TYPE;

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
  MAPED_FILE *file = (MAPED_FILE *)malloc(sizeof(MAPED_FILE *));

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

char *get_tag_color(char *tag) {
  switch (get_tag_type(tag)) {
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

int strgrep(const char *filepath, pcre *re) {
  int saved_errno = errno;
  int ovector[OVECCOUNT];
  int rc = 0;
  int tmp_len = 0;
  int matched = 0;
  char *tmp;
  MAPED_FILE *file;

  file = map_filepath(filepath);
  if (file == NULL) {
    errno = saved_errno;
    return 0;
  }

  tmp = (char *)file->content;
  tmp_len = file->size;
  rc = pcre_exec(re, NULL, tmp, tmp_len, 0, 0, ovector, OVECCOUNT);

  if (rc > 0) {
    printf(FILENAME_COLOR "%s%s%s\n" RESET_COLOR, FILE_PREFIX, filepath, FILE_SUFFIX);
  }

  while (rc > 0) {
    char *tag = (char *)malloc(sizeof(char) * MAX_TAG_LENGTH + 1);
    matched += 1;
    unsigned int line_len = ovector[1] - ovector[3];
    unsigned int tag_len = ovector[3] - ovector[2];

    if (!is_valid_tag_level(tag_len)) {
      tag_len = MAX_TAG_LENGTH;
    }
    strncpy(tag, &tmp[ovector[2]], tag_len);
    tag[tag_len] = '\0';

    // PRINT TAG
    printf("%s%s%s%s%s", TAG_PREFIX, get_tag_color(tag), tag, RESET_COLOR, TAG_SUFFIX);

    // PRINT TODO_BODY
    printf(LINE_COLOR "%s%.*s%s\n" RESET_COLOR, LINE_PREFIX, line_len, &tmp[ovector[3]], LINE_SUFFIX);

    rc = pcre_exec(re, NULL, tmp, tmp_len, ovector[1], 0, ovector, OVECCOUNT);
    free(tag);
  }

  free_mapedfile(file);
  return matched;
}
