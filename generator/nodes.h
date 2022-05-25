#ifndef NODES_H
#define NODES_H


#include "prefixtree.h"

enum rnode_coordsype_t {
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

extern const char *const rnode_coordsype_names[];
extern const char *const block_type_names[];
extern const char *const port_type_names[];

using pos_t = uint16_t;
using rnode_coords = uint32_t;
using pnode_coords = uint64_t;

static constexpr rnode_coords rnode(uint32_t type, uint32_t x, uint32_t y, uint32_t z)
{
  return (type << 24) | (x << 17) | (y << 10) | z;
}

static constexpr pnode_coords pnode(uint32_t bt, uint32_t x, uint32_t y, uint32_t pt, int8_t bindex, int16_t pindex) {
  return (uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (x << 23) | (y << 16)| (pindex & 0xffff);
}

static constexpr rnode_coordsype_t rn2t(rnode_coords rn) { return rnode_coordsype_t(rn >> 24); }
static constexpr uint32_t rn2x(rnode_coords rn) { return (rn >> 17) & 0x7f; }
static constexpr uint32_t rn2y(rnode_coords rn) { return (rn >> 10) & 0x7f; }
static constexpr uint32_t rn2z(rnode_coords rn) { return rn & 0x3ff; }

static constexpr block_type_t pn2bt(pnode_coords pn) { return block_type_t((pn >> 52) & 0xff); }
static constexpr port_type_t  pn2pt(pnode_coords pn) { return port_type_t((pn >> 40) & 0xfff); }
static constexpr pos_t        pn2p (pnode_coords pn) { return (pn >> 16) & 0x3fff; }
static constexpr uint32_t     pn2x (pnode_coords pn) { return (pn >> 23) & 0x7f; }
static constexpr uint32_t     pn2y (pnode_coords pn) { return (pn >> 16) & 0x7f; }
static constexpr int8_t       pn2bi(pnode_coords pn) { return (pn >> 32) & 0xff; }
static constexpr int16_t      pn2pi(pnode_coords pn) { return  pn        & 0xffff; }

static constexpr uint32_t pos2x(pos_t xy) { return (xy >> 7) & 0x7f; }
static constexpr uint32_t pos2y(pos_t xy) { return xy & 0x7f; }
static constexpr pos_t xy2pos(uint32_t x, uint32_t y) { return (x << 7) | y; }

class NodesReader {
public:
  NodesReader();

  rnode_coords lookup_r(const uint8_t *&p) const;
  pnode_coords lookup_p(const uint8_t *&p) const;

  block_type_t lookup_block(const uint8_t *&p) const;
  port_type_t lookup_port(const uint8_t *&p) const;

private:
  PrefixTree rnames, bnames, pnames;
};

#endif
