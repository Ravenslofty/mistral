#ifndef LINES_H
#define LINES_H

#include "rnode.h"

#include <vector>
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
  rnode_t rn;
  rnode_line_information li;
  uint16_t driver_position;
  uint16_t target_count;
  uint16_t target_pos[56];
  union {
    rnode_t rn;
    float caps;
  } targets[56];

  LinesParser(const RNodeReader &rnr, const std::vector<uint8_t> &data);

  void next();

private:
  const RNodeReader &rnr;
  const uint8_t *p, *e;

  void error(const uint8_t *st, const char *err = nullptr) const;
};

#endif
