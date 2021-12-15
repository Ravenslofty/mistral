#ifndef P2R_H
#define P2R_H

#include "nodes.h"

struct p2r_info {
  pnode_t p;
  rnode_t r;
  uint32_t padding;
};

class P2RLoader {
public:
  std::vector<p2r_info> data;

  P2RLoader(const NodesReader &nr, const std::vector<uint8_t> &data);

  pnode_t find_r(rnode_t node) const;

private:
  const NodesReader &nr;

  void error(const uint8_t *st, const char *err) const;
};

#endif
