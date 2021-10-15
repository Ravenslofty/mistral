#include "routes.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RoutesParser::RoutesParser(const RNodeReader &_rnr, const std::vector<uint8_t> &data, uint32_t _width) : rnr(_rnr), width(_width)
{
  p = data.data();
  e = data.data() + data.size();
  next();
}

void RoutesParser::error(const uint8_t *st, const char *err) const
{
  if(err)
    fprintf(stderr, "%s\n", err);
  const uint8_t *en = st;
  while(*en != '\n' && *en != '\r')
    en++;
  std::string line(st, en);
  fprintf(stderr, "line: %s\n", line.c_str());
  exit(1);
};

void RoutesParser::next()
{
  pattern = 0;
  fw_pos = 0;
  memset(sources, 0, sizeof(sources));

  if(p == e) {
    rn = 0;
    return;
  }

  const uint8_t *st = p;

  rn = rnr.lookup(p);
  if(!rn)
    error(st);
  if(*p++ != ' ')
    error(st, "Space expected after destination node");
  
  int pat = lookup_int(p);
  if(pat == -1 || (*p != ':' && *p != '.'))
    error(st, "Incorrect pattern number");
  if(*p == '.') {
    if(pat != 6)
      error(st, "Only pattern 6 has options");
    p++;
    int patopt = lookup_int(p);
    if(pat == -1 || *p != ':')
      error(st, "Incorrect pattern option");
    pat = 70 + patopt;
  }
  p++;
  pattern = pat;

  int pat_x = lookup_int(p);
  if(pat_x == -1 || *p++ != '_')
    error(st, "Incorrect pattern x position");

  int pat_y = lookup_int(p);
  if(pat_y == -1 || (*p != ' ' && *p != '\r' && *p != '\n'))
    error(st, "Incorrect pattern y position");
  if(*p == ' ')
    p++;

  fw_pos = pat_x + pat_y * width;

  while(*p != '\r' && *p != '\n') {
    int slot = lookup_int(p);
    if(slot == -1 || slot >= 44 || *p++ != ':')
      error(st, "Incorrect slot number");
    uint32_t srn = rnr.lookup(p);
    if(!srn)
      error(st);
    sources[slot] = srn;
    if(*p == ' ')
      p++;
  }

  if(*p == '\r')
    p++;
  if(*p++ != '\n')
    error(st, "Missing \\n at end of line");
}

