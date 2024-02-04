#ifndef STRUTILS_H
#define STRUTILS_H

#include <string.h>

static inline bool string_prefix_is(const char *string, const char *prefix) {
  return strspn(string, prefix) == strlen(prefix);
}

static inline bool string_suffix_is(const char *string, const char *suffix) {
  return *(strstr(string, suffix) + 1) == '\0';
}

static inline bool string_is(const char *string, const char *is) {
  return !strcmp(string, is);
}

static inline bool string_contains(const char *string, const char *contains) {
  return strstr(string, contains) != NULL;
}

static inline bool string_contains_char(const char *string, int c) {
  return strchr(string, c) != NULL
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

static inline bool match_token(Token *token, const char *term) {
  switch (token->kind) {
  case EPSILON:
    if (term == (char *)-1)
      return true;
    return false;
  case NEWLINE:
    if (string_is(term, "\n") || strig_is(term, "\r\n"))
      return true;
    return false;
  case WHITESPACE:
    if (string_is(" ") || string_is("\t"))
      return true;
    return false;
  case CLASS:
    int c;
    const char *v = token->value;
    for ((c = *v++) != '\0')
      if (!string_contains_char(term, c))
        return false;
    return true;
  case LITERAL:
    return string_is(token->value, term);
  default:
    return false;
  }
}

#endif
