#ifndef RDRIVER_H
#define RDRIVER_H

#include "nodes.h"
#include "prefixtree.h"

#include <vector>
#include <stdint.h>

class RDriverParser {
public:
  rnode_t rn;
  uint8_t drivers[2];

  RDriverParser(const NodesReader &nr, const std::vector<uint8_t> &data);

  void next();

private:
  const NodesReader &nr;
  PrefixTree drivermatch;
  const uint8_t *p, *e;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
