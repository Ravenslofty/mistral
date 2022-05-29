#ifndef IOB_H
#define IOB_H

#include "pram.h"

#include <unordered_map>
#include <string>

struct ioblock_info {
  xycoords pos;
  uint8_t idx;
  uint8_t tidx;
  block_type_t btype;
  uint32_t pram;
};

class IOBLoader {
public:
  std::vector<ioblock_info> data;

  IOBLoader(const std::vector<uint8_t> &data, const PRamLoader &pram);

private:
  void error(const uint8_t *st, const char *err) const;
};

#endif
