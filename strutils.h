#ifndef STRUTILS_H
#define STRUTILS_H

#include <string.h>

static inline bool string_prefix_is(const char *string, const char *prefix) {
  return strspn(string, prefix) == strlen(prefix);
}

static inline bool string_suffix_is(const char *string, const char *suffix) {
  return *(strstr(string, suffix) + 1) == '\0';
}

static inline bool string_contains(const char *string, const char *contains) {
  return strstr(string, contains) != NULL;
}

static inline char *pointer_to_first_whitespace(const char *string) {
  return strpbrk(string, " \t");
}

static inline char *pointer_to_first_newline(const char *string) {
  return strpbrk(string, "\r\n");
}

static inline char *pointer_to_first_occurance(const char *string,
                                               const char *substring) {
  return strstr(string, substring);
}

#endif
