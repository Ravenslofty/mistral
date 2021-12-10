#include "p2r.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

P2RLoader::P2RLoader(const NodesReader &_nr, const std::vector<uint8_t> &_data) : nr(_nr)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  block_type_t cblock = BNONE;
  int x = 0;
  int y = 0;
  while(p != e) {
    const uint8_t *st = p;
    if(p[0] != ' ') {
      cblock = nr.lookup_block(p);
      if(!cblock)
	error(st, "Incorrect block name");
      skipsp(p);
      x = lookup_int(p);
      if(x == -1)
	error(st, "Incorrect x coordinate");
      skipsp(p);
      y = lookup_int(p);
      if(y == -1)
	error(st, "Incorrect y coordinate");
      skipsp(p);
    } else {
      if(!cblock)
	error(st, "Missing block at start of file");
      skipsp(p);
      port_type_t port = nr.lookup_port(p);
      if(!port)
	error(st, "Bad port name");
      skipsp(p);
      int ninst = 1, nbit = 1;
      bool minst = false, mbit = false;
      if(*p == '.')
	p++;
      else {
	minst = true;
	ninst = lookup_int(p);
	if(ninst == -1)
	  error(st, "Bas instance count");
      }
      skipsp(p);
      if(*p == '.')
	p++;
      else {
	mbit = true;
	nbit = lookup_int(p);
	if(nbit == -1)
	  error(st, "Bas bit count");
      }
      skipsp(p);

      for(int inst = 0; inst != ninst; inst++)
	for(int bit = 0; bit != nbit; bit++) {
	  if(*p == '-')
	    p++;
	  else {
	    rnode_t rn = nr.lookup_r(p);
	    if(!rn)
	      error(st, "Incorrect rnode");
	    data.emplace_back(p2r_info{pnode(cblock, x, y, port, minst ? inst : -1, mbit ? bit : -1), rn, 0});
	  }
	  skipsp(p);
	}
    }
    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;
  }
}

void P2RLoader::error(const uint8_t *st, const char *err) const
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
