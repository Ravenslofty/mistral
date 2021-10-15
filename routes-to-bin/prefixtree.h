#ifndef PREFIXTREE_H
#define PREFIXTREE_H

#include <stdint.h>
#include <vector>

class PrefixTree {
public:
  PrefixTree(const char *const *words);
  ~PrefixTree();

  int lookup(const uint8_t *&p) const;

private:
  uint8_t cmap[256];
  int nentries;
  void *root;
  std::vector<void *> allocated_blocks;

  void *block_alloc();
  void **block_to_dispatch(void *block) const;
  int &block_to_id(void *block) const;
};

#endif
