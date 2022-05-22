#ifndef DCRAM_H
#define DCRAM_H

#include "nodes.h"

#include <unordered_map>
#include <string>

class DCRamLoader {
public:
  std::unordered_map<std::string, std::vector<std::pair<int, int>>> data;

  DCRamLoader(const std::vector<uint8_t> &data);

  std::vector<uint32_t> get_for_die(std::string die, int width) const;

private:
  void error(const uint8_t *st, const char *err) const;
};

#endif
