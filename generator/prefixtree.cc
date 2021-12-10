#include "prefixtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PrefixTree::PrefixTree(const char *const *words)
{
  nentries = 0;
  memset(cmap, 0xff, sizeof(cmap));
  for(const char *const *w = words; *w; w++)
    for(const char *e = *w; *e; e++) {
      uint8_t c = *e;
      if(cmap[c] == 0xff)
	cmap[c] = nentries++;
    }

  for(int i = 'a'; i <= 'z'; i++)
    if(cmap[i] == 0xff)
      cmap[i] = cmap[i - ('a' - 'A')];

  root = block_alloc();
  int id = 0;
  for(const char *const *w = words; *w; w++) {
    void *cblock = root;
    for(const char *e = *w; *e; e++) {
      uint8_t c = *e;
      void **dispatch = block_to_dispatch(cblock);
      int index = cmap[c];
      if(dispatch[index])
	cblock = dispatch[index];
      else
	cblock = dispatch[index] = block_alloc();
    }
    int &idp = block_to_id(cblock);
    if(idp != -1) {
      fprintf(stderr, "Collision on %s\n", *w);
      exit(1);
    }
    idp = id++;
  }
}

void *PrefixTree::block_alloc()
{
  size_t bsize = (nentries+1)*sizeof(void *);
  void *block = malloc(bsize);
  memset(block, 0, bsize);
  block_to_id(block) = -1;
  allocated_blocks.push_back(block);
  return block;
}

PrefixTree::~PrefixTree()
{
  for(void *block : allocated_blocks)
    free(block);
}

void **PrefixTree::block_to_dispatch(void *block) const
{
  return (void **)block;
}

int &PrefixTree::block_to_id(void *block) const
{
  return *(int *)(reinterpret_cast<char *>(block) + nentries*sizeof(void *));
}

int PrefixTree::lookup(const uint8_t *&p) const
{
  void *block = root;
  for(;;) {
    uint8_t c =  *p;
    if(cmap[c] == 0xff)
      return block_to_id(block);
    p++;
    block = block_to_dispatch(block)[cmap[c]];
    if(!block)
      return -1;
  }
}
