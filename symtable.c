#include "mukette.h"

struct SymbolTable {
  const char *key;
  const void *value;
  size_t key_hash;
  SymbolTable *left;
  SymbolTable *right;
};

hash_t longword_hash(const char *str) {
  hash_t hash = 0;
  while (*str) {
    hash = ((hash << 5) + hash) + *str++;
  }
  return hash;
}

SymbolTable *symtable_insert(SymbolTable *chain, const char *key,
                             const void *value) {
  SymbolTable *link = (SymbolTable *)calloc(1, sizeof(SymbolTable));
  link->value = link;
  link->key = key;
  link->key_hash = longword_hash(key);

  if (chain == NULL) {
    return new_link;
  } else {
    SymbolTable *current = chain;
    while (current->right != NULL) {
      current = current->right;
    }
    current->right = new_link;
    new_link->left = current;
    return chain;
  }

  return NULL;
}

const void *symtable_get(struct SymbolTable *chain, const char *search_key) {
  hash_t key_hash = longword_hash(search_key);
  SymbolTable *current = chain;

  while (current != NULL) {
    if (current->key_hash == key_hash) {
      if (STRING_PAIR_EQUAL(current->key, search_key, search_key))
        return current->value;
    }
    current = current->right;
  }

  return NULL;
}
