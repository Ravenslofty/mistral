#ifndef LINES_H
#define LINES_H

#include "nodes.h"

#include <vector>
#include <array>
#include <stdint.h>

struct rnode_line_information {
  float tc1;
  float tc2;
  float r85;
  float c;
  uint32_t length;

  bool operator==(const rnode_line_information &rhs) const {
    return tc1 == rhs.tc1 && tc2 == rhs.tc2 && r85 == rhs.r85 && c == rhs.c && length == rhs.length;
  }
};

class LinesParser {
public:
  rnode_coords rn;
  rnode_line_information li;
  uint16_t driver_position;
  uint16_t targets_count;
  uint16_t targets_caps_count;
  std::array<uint16_t, 64> targets_pos;
  std::array<rnode_coords, 64> targets;
  std::array<float, 64> targets_caps;

  LinesParser(const NodesReader &nr, const std::vector<uint8_t> &data);

  void next();

private:
  const NodesReader &nr;
  const uint8_t *p, *e;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
