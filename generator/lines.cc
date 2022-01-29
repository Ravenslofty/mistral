#include "lines.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LinesParser::LinesParser(const NodesReader &_nr, const std::vector<uint8_t> &data) : nr(_nr)
{
  p = data.data();
  e = data.data() + data.size();
  next();
}

void LinesParser::error(const uint8_t *st, const char *err) const
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

void LinesParser::next()
{
  memset(&li, 0, sizeof(li));
  driver_position = 0;
  target_count = 0;
  memset(target_pos, 0, sizeof(target_pos));
  memset(targets, 0, sizeof(targets));

  if(p == e) {
    rn = 0;
    return;
  }

  const uint8_t *st = p;

  rn = nr.lookup_r(p);
  if(!rn)
    error(st);
  if(*p++ != ' ')
    error(st, "Space expected after source node");

  li.tc1 = lookup_float(p);
  if(*p++ != ' ')
    error(st, "Space expected after tc1");
  li.tc2 = lookup_float(p);
  if(*p++ != ' ')
    error(st, "Space expected after tc2");
  li.r85 = lookup_float(p);
  if(*p++ != ' ')
    error(st, "Space expected after r85");
  li.c = lookup_float(p) * 1e-15;
  if(*p++ != ' ')
    error(st, "Space expected after capacitance");
  driver_position = lookup_int(p);
  if(*p++ != ' ')
    error(st, "Space expected after driver position");
  li.length = lookup_int(p);
  if(*p != ' ' && *p != '\r' && *p != '\n')
    error(st, "Space or end of line expected after length");
  if(*p == ' ')
    p++;

  while(*p != '\r' && *p != '\n') {
    int pos = lookup_int(p);
    if(pos == -1 || *p++ != ':')
      error(st, "Incorrect target position");
    if(p[0] >= '0' && p[0] <= '9') {
      float tcap = lookup_float(p);
      targets[target_count].caps = -tcap;
      target_pos[target_count] = pos;
    } else {
      char toto[64];
      memcpy(toto, p, 63);
      toto[63] = 0;
      rnode_t trn = nr.lookup_r(p);
      if(!trn) {
	fprintf(stderr, "[%s]\n", toto);
	error(st);
      }
      if(*p++ != ':')
	error(st, "Incorrect subslot information");
      uint16_t subslot_mask = *p++ == '1' ? 0x8000 : 0;
      targets[target_count].rn = trn;
      target_pos[target_count] = pos | subslot_mask;
    }
    if(*p == ' ')
      p++;
    target_count ++;
  }

  if(*p == '\r')
    p++;
  if(*p++ != '\n')
    error(st, "Missing \\n at end of line");
}

