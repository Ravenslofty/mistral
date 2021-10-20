#include <map>
#include <vector>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "prefixtree.h"
#include "io.h"
#include "rnode.h"
#include "routes.h"

static struct {
  const char *chip;
  uint32_t width;
  uint32_t hash_size;

} cinfo[7] = {
  { "e50f",    4958, 612514*2+7 },
  { "gt75f",   6006, 959166*2+1 },
  { "gt150f",  7605, 1707292*2+3 },
  { "gt300f", 10038, 3330206*2+1 },
  { "gx25f",   3856, 433482*2+7 },
  { "sx50f",   6006, 560293*2+5 },
  { "sx120f",  7605, 1303736*2+1 },
};

struct data_header {
  uint32_t off_rnode;
  uint32_t off_rnode_end;
  uint32_t off_rnode_hash;
  uint32_t size_rnode_hash;
  uint32_t count_rnode;
};

struct rnode_hash_entry {
  uint32_t rnode_offset;
  uint32_t next;
};

int main(int argc, char **argv)
{
  if(argc != 5) {
    fprintf(stderr, "Usage:\n%s data-dir chip target-dir <lzma-level>\n", argv[0]);
    exit(0);
  }

  std::string base_dir = argv[1];
  std::string chip = argv[2];
  std::string dest_dir = argv[3];

  uint32_t width = 0;
  uint32_t hash_size = 0;
  for(int i=0; i != 7; i++)
    if(chip == cinfo[i].chip) {
      width = cinfo[i].width;
      hash_size = cinfo[i].hash_size;
      break;
    }

  if(!width) {
    fprintf(stderr, "Chip %s unknown, no information available\n", chip.c_str());
    exit(1);
  }

  auto r_data = file_load(base_dir + '/' + chip + "-r.txt.xz");
  auto l_data = file_load(base_dir + '/' + chip + "-l.txt.xz");

  RNodeReader rnr;
  RoutesParser rparse(rnr, r_data, width);

  std::vector<uint8_t> output(700*1024*1024);
  data_header *dh = reinterpret_cast<data_header *>(output.data());

  dh->off_rnode = sizeof(data_header);

  std::vector<std::pair<rnode_t, uint32_t>> rnode_positions;

  uint8_t *opos = output.data() + dh->off_rnode;
  int nr = 0;
  while(rparse.rn) {
    rnode_positions.push_back(std::make_pair(rparse.rn, uint32_t(opos - output.data() - dh->off_rnode)));

    uint32_t span = rmux_patterns[rparse.pattern].span;

    memcpy(opos+ 0, &rparse.rn, 4);
    memcpy(opos+ 4, &rparse.pattern, 4);
    memcpy(opos+ 8, &rparse.fw_pos,  4);
    memcpy(opos+12, rparse.sources, span*4);
    opos += 12 + 4*span;
    nr++;
    rparse.next();
  }
  dh->off_rnode_end = opos - output.data();
  assert(size_t(opos-output.data()) <= output.size());

  r_data.clear();
  l_data.clear();


  dh->off_rnode_hash = opos - output.data();
  dh->size_rnode_hash = hash_size;
  dh->count_rnode = nr;

  output.resize(dh->off_rnode_hash + sizeof(rnode_hash_entry) * dh->size_rnode_hash);
  dh = reinterpret_cast<data_header *>(output.data());
  rnode_hash_entry *rh = reinterpret_cast<rnode_hash_entry *>(output.data() + dh->off_rnode_hash);
  for(uint32_t i=0; i != dh->size_rnode_has; i++) {
    rh[i].rnode_offset = 0xffffffff;
    rh[i].next = 0;
  }

  int hl[10];
  memset(hl, 0, sizeof(hl));
  for(auto &rm : rnode_positions) {
    uint32_t key = rm.first % hash_size;
    if(rh[key].rnode_offset == 0xffffffff) {
      hl[0]++;
      rh[key].rnode_offset = rm.second;
      rm.second = 0;
    }
  }

  uint32_t ne = 1;
  for(auto &rm : rnode_positions) {
    if(!rm.second)
      continue;
    uint32_t entry = rm.first % hash_size;
    size_t depth = 0;
    while(rh[entry].next) {
      depth++;
      entry = rh[entry].next;
    }
    while(rh[ne].rnode_offset != 0xffffffff)
      ne++;
    rh[entry].next = ne;
    rh[ne].rnode_offset = rm.second;
    hl[depth >= 10 ? 9 : depth]++;
  }

  int maxdepth;
  for(maxdepth = 9; maxdepth >= 0; maxdepth--)
    if(hl[maxdepth])
      break;

  fprintf(stderr, "%-6s size %9d nodes %7d hash", chip.c_str(), int(output.size()), nr);
  for(int i=0; i <= maxdepth; i++)
    fprintf(stderr, " %7d", hl[i]);
  fprintf(stderr, "\n");

  file_save(dest_dir + '/' + chip + "-r.bin", output, strtol(argv[4], nullptr, 10));

  exit(0);
}

/*
gx25f  size  43152740 entries  433482
sx50f  size  55721800 entries  560293
e50f   size  61597172 entries  612514
gt75f  size  96691956 entries  959166
sx120f size 132029132 entries 1303736
gt150f size 173120316 entries 1707292
gt300f size 339541444 entries 3330206
 */
