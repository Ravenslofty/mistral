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
#include <cv-porttypes.ipp>
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

static constexpr pnode_t pnode(uint32_t bt, uint32_t x, uint32_t y, uint32_t pt, int8_t bindex, int16_t pindex) {
  return (uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (x << 23) | (y << 16)| (pindex & 0xffff);
}

static constexpr rnode_type_t rn2t(rnode_t rn) { return rnode_type_t(rn >> 24); }
static constexpr uint32_t rn2x(rnode_t rn) { return (rn >> 17) & 0x7f; }
static constexpr uint32_t rn2y(rnode_t rn) { return (rn >> 10) & 0x7f; }
static constexpr uint32_t rn2z(rnode_t rn) { return rn & 0x3ff; }

static constexpr block_type_t pn2bt(pnode_t pn) { return block_type_t((pn >> 52) & 0xff); }
static constexpr port_type_t  pn2pt(pnode_t pn) { return port_type_t((pn >> 40) & 0xfff); }
static constexpr uint32_t     pn2x (pnode_t pn) { return (pn >> 23) & 0x7f; }
static constexpr uint32_t     pn2y (pnode_t pn) { return (pn >> 16) & 0x7f; }
static constexpr int8_t       pn2bi(pnode_t pn) { return (pn >> 32) & 0xff; }
static constexpr int16_t      pn2pi(pnode_t pn) { return  pn        & 0xffff; }


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
