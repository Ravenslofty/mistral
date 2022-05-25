#ifndef ROUTES_H
#define ROUTES_H

#include "nodes.h"

#include <vector>
#include <stdint.h>

struct rmux_pattern {
  uint8_t bits;
  uint8_t span;
  uint16_t hashdiv;
  uint32_t def;
  uint16_t o_xy;
  uint16_t o_vals;
  uint16_t o_vhash;
};

extern const rmux_pattern rmux_patterns[70+4];

class RoutesParser {
public:
  rnode_coords rn;
  uint32_t pattern;
  uint32_t fw_pos;
  rnode_coords sources[44];

  RoutesParser(const NodesReader &nr, const std::vector<uint8_t> &data, uint32_t width);

  void next();

private:
  const NodesReader &nr;
  const uint8_t *p, *e;

  uint32_t width;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
