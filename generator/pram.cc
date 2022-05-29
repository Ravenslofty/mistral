#include "pram.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <algorithm>

PRamLoader::PRamLoader(const std::vector<uint8_t> &_data)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    int strip = lookup_int(p);
    if(strip == -1)
	error(st, "Incorrect pram strip number");
    if(p[0] != '.')
	error(st, "Incorrect pram strip/start separator");
    p++;
    int start = lookup_int(p);
    if(start == -1)
	error(st, "Incorrect pram start");
    if(p[0] != '-')
	error(st, "Incorrect pram start/end separator");
    p++;
    int end = lookup_int(p);
    if(end == -1)
	error(st, "Incorrect pram end");
    if(p[0] != ' ')
	error(st, "Incorrect pram end/block separator");
    skipsp(p);
    auto p1 = p;
    while(*p != ' ')
      p++;
    std::string block(p1, p);
    skipsp(p);
    int x = lookup_int(p);
    if(x == -1)
	error(st, "Incorrect pram x");
    if(p[0] != '.')
	error(st, "Incorrect pram x/y separator");
    p++;
    int y = lookup_int(p);
    if(y == -1)
	error(st, "Incorrect pram y");
    skipsp(p);
    int instance = 0;
    if(p[0] >= '0' && p[0] <= '9') {
      instance = lookup_int(p);
      if(instance == -1)
	error(st, "Incorrect instance");
      skipsp(p);
    } else if(p[0] == '-') {
      p++;
      skipsp(p);
    }
    int var = 0;
    if(p[0] >= '0' && p[0] <= '9') {
      var = lookup_int(p);
      if(var == -1)
	error(st, "Incorrect var");
      skipsp(p);      
    }
    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;

    data[block].emplace_back(pram_info{ strip, start, end-start+1, xycoords(x, y), instance, var });
  }
}

void PRamLoader::error(const uint8_t *st, const char *err) const
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

std::vector<fixed_block_info> PRamLoader::get_fixed_blocks() const
{
  std::vector<fixed_block_info> blocks;
  add(blocks, "fpll", 8);
  add(blocks, "cmuxc", 4);
  add(blocks, "cmuxv", 2);
  add(blocks, "cmuxh", 2);
  add(blocks, "dll", 4);
  add(blocks, "hssi", 4);
  add(blocks, "cbuf", 8);
  add(blocks, "lvl", 17);
  add(blocks, "pma3", 4);
  add(blocks, "serpar", 10);
  add(blocks, "term", 4);
  add(blocks, "hip", 2);
  if(data.find("hmcphy40") != data.end())
    add(blocks, "hmcphy40", 2);
  else
    add(blocks, "hmcphy24", 2);
  return blocks;
}

void PRamLoader::add(std::vector<fixed_block_info> &blocks, std::string key, int count) const
{
  auto i = data.find(key);
  if(i == data.end()) {
    for(int ii=0; ii != count; ii++)
      blocks.emplace_back(fixed_block_info{ xycoords(0xffff), 0xffffffff });
  } else {
    int ii = 0;
    for(const auto &e : i->second) {
      blocks.emplace_back(fixed_block_info{ e.pos, uint32_t(e.start | (e.strip << 16) | (e.instance << 24) | (e.var << 25)) });
      ii++;
    }    
    std::sort(blocks.end() - ii, blocks.end(), [](const fixed_block_info &a, const fixed_block_info &b) -> bool { return a.pos < b.pos; });
    while(ii != count) {
      blocks.emplace_back(fixed_block_info{ xycoords(0xffff), 0xffffffff });
      ii++;
    }
  }
}

std::vector<dqs16_info> PRamLoader::get_dqs16() const
{
  std::vector<dqs16_info> result;
  const auto &blk = data.find("dqs16")->second;
  for(unsigned int i = 0; i != blk.size(); i++) {
    const auto &b = blk[i];
    if(b.instance == 0)
      result.emplace_back(dqs16_info {
			    b.pos,
			    uint32_t((b.var ? 1 << 24 : 0) |
				     (1 << 23) |
				     ((i == 0 || blk[i-1].pos != b.pos) ? 1 << 22 : 0) |
				     (b.strip << 16) |
				     b.start)
			    });
  }

  std::sort(result.begin(), result.end(), [](const dqs16_info &a, const dqs16_info &b) -> bool { return a.pos < b.pos; });
  return result;
}

