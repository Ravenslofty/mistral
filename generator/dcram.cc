#include "dcram.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

DCRamLoader::DCRamLoader(const std::vector<uint8_t> &_data)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    while(*p != ' ')
      p++;
    std::string die(st, p);
    skipsp(p);
    auto &dest = data[die];
    int idx = lookup_int(p);
    if(idx == -1)
      error(st, "Incorrect index");
    if(int(dest.size()) != 18*idx)
      error(st, "Index mismatch");

    for(int i=0; i != 18; i++) {
      skipsp(p);
      int x = lookup_int(p);
      if(x == -1)
	error(st, "Incorrect x coordinate");
      if(p[0] != '.')
	error(st, "'.' expected between x and y");
      p++;
      int y = lookup_int(p);
      if(y == -1)
	error(st, "Incorrect y coordinate");
      dest.push_back(std::make_pair(x, y));
    }
    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;
  }
}

std::vector<uint32_t> DCRamLoader::get_for_die(std::string die, int width) const
{
  std::vector<uint32_t> result;
  auto i = data.find(die);
  if(i != data.end()) {
    for(const auto &xy : i->second)
      result.push_back(xy.first + xy.second * width);
  }
  return result;
}

void DCRamLoader::error(const uint8_t *st, const char *err) const
{
  if(err)
    fprintf(stderr, "%s\n", err);
  const uint8_t *en = st;
  while(*en != '\n' && *en != '\r')
    en++;
  std::string line(st, en);
  fprintf(stderr, "line: %s\n", line.c_str());
  exit(1);
}
