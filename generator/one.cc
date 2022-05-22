#include "one.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

OneLoader::OneLoader(const std::vector<uint8_t> &_data, int width)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    int x = lookup_int(p);
    if(x == -1)
      error(st, "Incorrect x coordinate");
    if(p[0] != '.')
      error(st, "'.' expected between x and y");
    p++;
    int y = lookup_int(p);
    if(y == -1)
      error(st, "Incorrect y coordinate");
    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    data.push_back(x + y * width);
    p++;
  }
}

void OneLoader::error(const uint8_t *st, const char *err) const
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
