#ifndef PRAM_H
#define PRAM_H

#include "nodes.h"

#include <unordered_map>
#include <string>

struct pram_info {
  int strip;
  int start;
  int size;
  xycoords pos;
  int instance, var;
};

struct fixed_block_info {
  xycoords pos;
  uint32_t pram;
};

struct dqs16_info {
  xycoords pos;
  uint32_t pram;
};

enum {
  FB_FPLL   = 0,
  FB_CMUXC  = FB_FPLL   +  8,
  FB_CMUXV  = FB_CMUXC  +  4,
  FB_CMUXH  = FB_CMUXV  +  2,
  FB_DLL    = FB_CMUXH  +  2,
  FB_HSSI   = FB_DLL    +  4,
  FB_CBUF   = FB_HSSI   +  4,
  FB_LVL    = FB_CBUF   +  8,
  FB_PMA3   = FB_LVL    + 17,
  FB_SERPAR = FB_PMA3   +  4,
  FB_TERM   = FB_SERPAR + 10,
  FB_HIP    = FB_TERM   +  4,
  FB_HMC    = FB_HIP    +  2,
  FB_COUNT  = FB_HMC    +  2
};

class PRamLoader {
public:
  std::unordered_map<std::string, std::vector<pram_info>> data;

  PRamLoader(const std::vector<uint8_t> &data);

  std::vector<fixed_block_info> get_fixed_blocks() const;
  std::vector<dqs16_info> get_dqs16() const;

private:
  void error(const uint8_t *st, const char *err) const;

  void add(std::vector<fixed_block_info> &blocks, std::string key, int count) const;
};

#endif
