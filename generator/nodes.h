#ifndef NODES_H
#define NODES_H


#include "prefixtree.h"

enum rnode_type_t {
#define P(x) x
#include "cv-rnodetypes.ipp"
#undef P
};

enum block_type_t {
#define P(x) x
#include "cv-blocktypes.ipp"
#undef P
};

enum port_type_t {
#define P(x) x
#include "cv-porttypes.ipp"
#undef P
};

extern const char *const rnode_type_names[];
extern const char *const block_type_names[];
extern const char *const port_type_names[];

using rnode_t = uint32_t;
using pnode_t = uint64_t;

static constexpr rnode_t rnode(uint32_t type, uint32_t x, uint32_t y, uint32_t z)
{
  return (type << 24) | (x << 17) | (y << 10) | z;
}

static constexpr pnode_t pnode(block_type_t bt, uint32_t x, uint32_t y, port_type_t pt, int8_t bindex, int16_t pindex) {
  return (uint64_t(bt) << 50) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (x << 23) | (y << 16)| (pindex & 0xffff);
}


class NodesReader {
public:
  NodesReader();

  rnode_t lookup_r(const uint8_t *&p) const;
  pnode_t lookup_p(const uint8_t *&p) const;

  block_type_t lookup_block(const uint8_t *&p) const;
  port_type_t lookup_port(const uint8_t *&p) const;

private:
  PrefixTree rnames, bnames, pnames;
};

#endif
