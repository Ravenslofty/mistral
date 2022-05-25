#ifndef P2P_H
#define P2P_H

#include "nodes.h"

struct p2p_info {
  pnode_coords s;
  pnode_coords d;
};

class P2PLoader {
public:
  std::vector<p2p_info> data;

  P2PLoader(const NodesReader &nr, const std::vector<uint8_t> &data);

private:
  const NodesReader &nr;

  void error(const uint8_t *st, const char *err) const;
};

#endif
