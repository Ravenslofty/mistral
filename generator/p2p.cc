#include "p2p.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

P2PLoader::P2PLoader(const NodesReader &_nr, const std::vector<uint8_t> &_data) : nr(_nr)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    pnode_t s = nr.lookup_p(p);
    if(!s)
      error(st, "Incorrect source pnode");
    skipsp(p);
    pnode_t d = nr.lookup_p(p);
    if(!d)
      error(st, "Incorrect destination pnode");
    skipsp(p);
    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;
    data.emplace_back(p2p_info{s, d});
  }

  std::sort(data.begin(), data.end(), [](const p2p_info &a, const p2p_info &b) { return a.s < b.s || (a.s == b.s && a.d < b.d); });
}

void P2PLoader::error(const uint8_t *st, const char *err) const
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
