#ifndef ROUTES_H
#define ROUTES_H

#include "rnode.h"

#include <vector>
#include <stdint.h>

class RoutesParser {
public:
  rnode_t rn;
  uint32_t pattern;
  uint32_t fw_pos;
  rnode_t sources[44];

  RoutesParser(const RNodeReader &rnr, const std::vector<uint8_t> &data, uint32_t width);

  void next();

private:
  const RNodeReader &rnr;
  const uint8_t *p, *e;

  uint32_t width;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
