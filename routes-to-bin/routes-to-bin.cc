#include <map>
#include <vector>
#include <string>
#include <unordered_map>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "prefixtree.h"
#include "io.h"
#include "rnode.h"
#include "routes.h"
#include "lines.h"
#include "bdz-ph.h"

static struct {
  const char *chip;
  uint32_t width;

} cinfo[7] = {
  { "e50f",    4958 },
  { "gt75f",   6006 },
  { "gt150f",  7605 },
  { "gt300f", 10038 },
  { "gx25f",   3856 },
  { "sx50f",   6006 },
  { "sx120f",  7605 },
};

struct data_header {
  uint32_t off_rnode;
  uint32_t off_rnode_end;
  uint32_t off_rnode_hash;
  uint32_t off_line_info;
  uint32_t size_rnode_opaque_hash;
  uint32_t count_rnode;
};

struct rnode_base {
  rnode_t node;
  uint8_t pattern;
  uint8_t target_count;
  uint16_t line_info_index;
  uint16_t driver_position;
  uint16_t padding;
  uint32_t fw_pos;
}; // Followed by up to 44 sources and up to 56 targets and up to 56 target_positions.  Aligned to 4 bytes.

std::vector<rnode_line_information> rli_data;
std::unordered_map<uint32_t, std::vector<uint16_t>> rli_lookup;

uint16_t rli_index_cache = 0;

uint32_t chash(const void *src, size_t size)
{
  const uint8_t *p = reinterpret_cast<const uint8_t *>(src);
  uint32_t hash = 0;
  for(size_t pos = 0; pos != size; pos++)
    hash = hash*23 + *p++;
  return hash;
}

uint16_t get_line_info(const rnode_line_information &rli)
{
  if(!rli_data.empty() && rli_data[rli_index_cache] == rli)
    return rli_index_cache;

  uint32_t k = chash(&rli, sizeof(rli));
  for(uint16_t i : rli_lookup[k])
    if(rli_data[i] == rli)
      return rli_index_cache = i;

  rli_index_cache = rli_data.size();
  rli_lookup[k].push_back(rli_index_cache);
  rli_data.push_back(rli);
  return rli_index_cache;
}

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
  for(int i=0; i != 7; i++)
    if(chip == cinfo[i].chip) {
      width = cinfo[i].width;
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
  LinesParser lparse(rnr, l_data);

  std::vector<uint8_t> output(800*1024*1024, 0);
  data_header *dh = reinterpret_cast<data_header *>(output.data());

  dh->off_rnode = sizeof(data_header);

  std::vector<uint32_t> rnode_vec;
  std::vector<uint32_t> rnode_pos;

  uint8_t *opos = output.data() + dh->off_rnode;
  while(rparse.rn || lparse.rn) {

    rnode_base rb;
    rnode_t rn = rparse.rn && (!lparse.rn || rparse.rn <= lparse.rn) ? rparse.rn : lparse.rn;

    rnode_vec.push_back(rn);
    rnode_pos.push_back(opos - output.data() - dh->off_rnode);

    rb.node = rn;
    bool rv = rparse.rn == rn;
    bool lv = lparse.rn == rn;

    rb.pattern = rv ? rparse.pattern : 0xff;
    rb.target_count = lv ? lparse.target_count : 0;
    rb.line_info_index = lv ? get_line_info(lparse.li) : 0xffff;
    rb.driver_position = lv ? lparse.driver_position : 0;
    rb.padding = 0;
    rb.fw_pos = rv ? rparse.fw_pos : 0;

    uint32_t span = rv ? rmux_patterns[rparse.pattern].span : 0;
    
    memcpy(opos, &rb, sizeof(rb));
    opos += sizeof(rb);

    if(span) {
      memcpy(opos, rparse.sources, span*4);
      opos += 4*span;
    }

    if(rb.target_count) {
      memcpy(opos, lparse.targets, rb.target_count*4);
      opos += rb.target_count*4;
      memcpy(opos, lparse.target_pos, rb.target_count*2);
      opos += rb.target_count*2;
      if(rb.target_count & 1) {
	*opos ++ = 0;
	*opos ++ = 0;
      }
    }

    if(rparse.rn == rn)
      rparse.next();
    if(lparse.rn == rn)
      lparse.next();
  }
  dh->off_rnode_end = opos - output.data();
  assert(size_t(opos-output.data()) <= output.size());

  r_data.clear();
  l_data.clear();

  auto hdata = bdz_ph_hash::make(rnode_vec);
  size_t len1 = (hdata.size() + 3) & ~3;
  size_t len2 = 4*bdz_ph_hash::output_range(hdata);
  size_t llines = sizeof(rnode_line_information)*rli_data.size();

  dh->off_rnode_hash = opos - output.data();
  dh->off_line_info = dh->off_rnode_hash + len1 + len2;
  dh->size_rnode_opaque_hash = len1;
  dh->count_rnode = rnode_vec.size();

  output.resize(dh->off_rnode_hash + len1 + len2 + llines, 0);
  dh = reinterpret_cast<data_header *>(output.data());
  memcpy(output.data() + dh->off_rnode_hash, hdata.data(), hdata.size());
  uint32_t *offsets = reinterpret_cast<uint32_t *>(output.data() + dh->off_rnode_hash + len1);

  for(uint32_t i=0; i != rnode_vec.size(); i++)
    offsets[bdz_ph_hash::lookup(hdata, rnode_vec[i])] = rnode_pos[i];

  memcpy(output.data() + dh->off_line_info, rli_data.data(), llines);

  fprintf(stderr, "%-6s size %9d nodeinfo %9d hash %9d hashmap %9d nodes %7d lines %5d\n", chip.c_str(), int(output.size()), dh->off_rnode_end - dh->off_rnode, int(len1), int(len2), dh->count_rnode, int(rli_data.size()));

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
