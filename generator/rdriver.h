#ifndef RDRIVER_H
#define RDRIVER_H

#include "rnode.h"
#include "prefixtree.h"

#include <vector>
#include <stdint.h>

class RDriverParser {
public:
  rnode_t rn;
  uint8_t drivers[2];

  RDriverParser(const RNodeReader &rnr, const std::vector<uint8_t> &data);

  void next();

private:
  const RNodeReader &rnr;
  PrefixTree drivermatch;
  const uint8_t *p, *e;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
