#ifndef INV_H
#define INV_H

#include "nodes.h"
#include "p2r.h"
#include "p2p.h"

struct inverter_info {
  enum {
    DEF_UNK  = 0x00000000, // must be zero or things will break

    DEF_0    = 0x10000000,
    DEF_1    = 0x20000000,

    DEF_GP   = 0x30000000, // Algorithmic, GPIO
    DEF_HMC  = 0x40000000, // Algorithmic, bypassable HMC

    DEF_MASK = 0xf0000000,
  };

  rnode_t node;
  uint32_t pos_and_def;
};

class InvLoader {
public:
  std::vector<inverter_info> data;

  InvLoader(const NodesReader &nr, const std::vector<uint8_t> &data, uint32_t width, const P2RLoader &p2r, const P2PLoader &p2p);

private:
  const NodesReader &nr;

  void add(const P2RLoader &p2r, const P2PLoader &p2p, rnode_t node, uint32_t pos);
  void error(const uint8_t *st, const char *err) const;
};

#endif
