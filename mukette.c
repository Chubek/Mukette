#include <curses.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mukette.h"

struct Hyperlink {
  const char *key;
  const char *web_link;
  size_t key_hash;
  int y, x;
  Hyperlink *left;
  Hyperlink *right;
};

hash_t longword_hash(const char *str) {
  hash_t hash = 0;
  while (*str) {
    hash = ((hash << 5) + hash) + *str++;
  }
  return hash;
}

Hyperlink *new_hyperlink(Hyperlink *chain, int y, int x, const char *link,
                         const char *key) {
  Hyperlink *link = (Hyperlink *)calloc(1, sizeof(Hyperlink));
  link->web_link = link;
  link->key = key;
  link->key_hash = longword_hash(key);
  link->x = x;
  link->y = y;

  if (chain == NULL) {
    return new_link;
  } else {
    struct Hyperlink *current = chain;
    while (current->right != NULL) {
      current = current->right;
    }
    current->right = new_link;
    new_link->left = current;
    return chain;
  }

  return NULL;
}

const char *get_hyperlink_by_key(struct Hyperlink *chain,
                                 const char *search_key) {
  hash_t key_hash = longword_hash(search_key);
  struct Hyperlink *current = chain;

  while (current != NULL) {
    if (current->key_hash == key_hash) {
      if (STRING_PAIR_EQUAL(current->key, search_key, search_key))
        return current->web_link;
    }
    current = current->right;
  }

  return NULL;
}
