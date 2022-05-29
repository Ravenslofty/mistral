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
  targets_count = 0;
  targets_caps_count = 0;

  std::fill(targets_pos.begin(), targets_pos.end(), 0);
  std::fill(targets.begin(), targets.end(), rnode_coords());
  std::fill(targets_caps.begin(), targets_caps.end(), 0.0);

  if(p == e) {
    rn = rnode_coords();
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
      targets_caps[targets_caps_count] = tcap;
      targets_pos[targets_count+targets_caps_count] = pos | 0x8000;
      targets_caps_count ++;
    } else {
      rnode_coords trn = nr.lookup_r(p);
      if(*p++ != ':')
	error(st, "Incorrect subslot information");
      uint16_t subslot_mask = *p++ == '1' ? 0x4000 : 0;
      targets[targets_count] = trn;
      targets_pos[targets_count+targets_caps_count] = pos | subslot_mask;
      targets_count ++;
    }
    if(*p == ' ')
      p++;
  }

  if(*p == '\r')
    p++;
  if(*p++ != '\n')
    error(st, "Missing \\n at end of line");
}

