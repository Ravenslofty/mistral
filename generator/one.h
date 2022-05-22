#ifndef ONE_H
#define ONE_H

#include "nodes.h"

class OneLoader {
public:
  std::vector<uint32_t> data;

  OneLoader(const std::vector<uint8_t> &data, int width);

private:
  void error(const uint8_t *st, const char *err) const;
};

#endif
