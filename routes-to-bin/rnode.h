#ifndef RNODE_H
#define RNODE_H


#include "prefixtree.h"

enum rnode_type_t {
#define P(x) x
#include "cv-rnodetypes.ipp"
#undef P
};

extern const char *const rnode_type_names[];

using rnode_t = uint32_t;

static constexpr rnode_t rnode(uint32_t type, uint32_t x, uint32_t y, uint32_t z)
{
  return (type << 24) | (x << 17) | (y << 10) | z;
}


class RNodeReader {
public:
  RNodeReader();

  rnode_t lookup(const uint8_t *&p) const;

private:
  PrefixTree rnames;
};

#endif
