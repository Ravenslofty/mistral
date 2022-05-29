#include "iob.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <algorithm>

IOBLoader::IOBLoader(const std::vector<uint8_t> &_data, const PRamLoader &pram)
{
  std::unordered_map<xycoords, uint32_t> prami;
  for(const auto &ent : pram.data.find("gpio")->second)
    prami[ent.pos] = ent.start | (ent.strip << 16) | (ent.var << 25);

  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    int entry = lookup_int(p);
    if(entry == -1)
	error(st, "Incorrect iob entry number");
    if(p[0] != ' ')
	error(st, "Incorrect iob entry/pos separator");
    if(int(data.size()) != entry)
      error(st, "Incorrect ion entry current number");

    p++;
    int x = lookup_int(p);
    if(x == -1)
	error(st, "Incorrect x");
    if(p[0] != '.')
	error(st, "Incorrect iob x/y separator");
    p++;
    int y = lookup_int(p);
    if(y == -1)
	error(st, "Incorrect iob y");
    if(p[0] != '.')
	error(st, "Incorrect iob y/index separator");
    p++;
    int index = lookup_int(p);
    if(index == -1)
	error(st, "Incorrect iob index");
    if(p[0] != ' ')
	error(st, "Incorrect iob index/type separator");
    skipsp(p);
    auto p1 = p;
    while(*p != ' ')
      p++;
    std::string block(p1, p);
    block_type_t btype = BNONE;
    if(block == "dpp")
      btype = DEDICATED_PROGRAMMING_PAD;
    else if(block == "gpio")
      btype = GPIO;
    else if(block == "hssi_input")
      btype = HSSI_INPUT;
    else if(block == "hssi_output")
      btype = HSSI_OUTPUT;
    else if(block == "jtag")
      btype = JTAG;
    else
      error(st, "Unknown iob block type");
    skipsp(p);

    int slot = lookup_int(p);
    if(x == -1)
	error(st, "Incorrect iob slot");

    bool serdes = false;
    if(p[0] == ' ') {
      skipsp(p);
      p1 = p;
      while(*p != '\r' && *p != '\n')
	p++;
      std::string tag(p1, p);
      if(tag == "serdes")
	serdes = true;
      else
	error(st, "Incorrect iob tag");
    }

    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;

    xycoords pos(x, y);
    uint32_t pr = serdes ? 1<<24 : 0;
    auto pi = prami.find(pos);
    if(pi != prami.end())
      pr |= pi->second;
    data.emplace_back(ioblock_info{ pos, uint8_t(index), uint8_t(slot), btype, pr });
  }
}

void IOBLoader::error(const uint8_t *st, const char *err) const
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
