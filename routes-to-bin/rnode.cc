#include "rnode.h"
#include "io.h"

#include <stdio.h>

const char *const rnode_type_names[] = {
#define P(x) #x
#include "cv-rnodetypes.ipp"
#undef P
  nullptr
};


RNodeReader::RNodeReader() : rnames(rnode_type_names)
{
}

rnode_t RNodeReader::lookup(const uint8_t *&p) const
{
  int t = rnames.lookup(p);
  if(t == -1) {
    fprintf(stderr, "Rnode type unknown\n");
    return 0;
  }
  if(*p++ != '.') {
    fprintf(stderr, "Missing '.' after rnode type\n");
    return 0;
  }
  int x = lookup_int(p);
  if(x == -1) {
    fprintf(stderr, "Missing x\n");
    return 0;
  }
  if(*p++ != '.') {
    fprintf(stderr, "Missing '.' after rnode x\n");
    return 0;
  }
  int y = lookup_int(p);
  if(y == -1) {
    fprintf(stderr, "Missing y\n");
    return 0;
  }
  if(*p++ != '.') {
    fprintf(stderr, "Missing '.' after rnode y\n");
    return 0;
  }
  int z = lookup_int(p);
  if(z == -1) {
    fprintf(stderr, "Missing z\n");
    return 0;
  }
  return rnode(t, x, y, z);
}

