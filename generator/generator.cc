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
#include "nodes.h"
#include "routes.h"
#include "rdriver.h"
#include "lines.h"
#include "p2r.h"
#include "p2p.h"
#include "inv.h"
#include "bdz-ph.h"
#include "drivers.h"

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
  uint32_t off_rnode_hash;
  uint32_t off_rnode_hash_to_index;
  uint32_t off_rnode_index_to_base;
  uint32_t off_line_info;
  uint32_t off_p2r_info;
  uint32_t off_p2p_info;
  uint32_t off_inv_info;
  uint32_t count_rnode;
  uint32_t count_p2r;
  uint32_t count_p2p;
  uint32_t count_inv;
};

struct global_data_header {
  uint32_t off_dnode_lookup;
  uint32_t off_dnode_table2;
  uint32_t off_dnode_table3;
  uint32_t off_dnode_drivers;
};

struct rnode_base {
  rnode_t node;
  uint8_t pattern;
  uint8_t target_count;
  uint8_t drivers[2];
  uint16_t line_info_index;
  uint16_t driver_position;
  uint32_t fw_pos;
}; // Followed by up to 44 sources and up to 64 targets and up to 64 target_positions.  Aligned to 4 bytes.

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
    fprintf(stderr, "Usage:\n%s data-dir [chip|-] target-dir <lzma-level>\n", argv[0]);
    exit(0);
  }

  std::string base_dir = argv[1];
  std::string chip = argv[2];
  std::string dest_dir = argv[3];

  if(chip == "-") {
    auto driver_models = file_load(base_dir + "/driver-models.txt.xz");
    DriversParser dp(driver_models);
    
    uint32_t s1 = sizeof(global_data_header);
    uint32_t s2 = sizeof(dnode_lookup);
    uint32_t s3 = dp.table2.size()*sizeof(dnode_table2);
    uint32_t s4 = dp.table3.size()*sizeof(dnode_table3);
    uint32_t s5 = dp.drivers.size()*sizeof(dnode_info);

    std::vector<uint8_t> output(s1+s2+s3+s4+s5);
    global_data_header *dh = reinterpret_cast<global_data_header *>(output.data());
    dh->off_dnode_lookup  = s1;
    dh->off_dnode_table2  = s1+s2;
    dh->off_dnode_table3  = s1+s2+s3;
    dh->off_dnode_drivers = s1+s2+s3+s4;
    memcpy(output.data() + dh->off_dnode_lookup,  &dp.lookup, s2);
    memcpy(output.data() + dh->off_dnode_table2,  dp.table2.data(), s3);
    memcpy(output.data() + dh->off_dnode_table3,  dp.table3.data(), s4);
    memcpy(output.data() + dh->off_dnode_drivers, dp.drivers.data(), s5);

    file_save(dest_dir + "/global.bin", output, strtol(argv[4], nullptr, 10));

  } else {
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

    auto r_data   = file_load(base_dir + '/' + chip + "-r.txt.xz");
    auto l_data   = file_load(base_dir + '/' + chip + "-l.txt.xz");
    auto d_data   = file_load(base_dir + '/' + chip + "-d.txt.xz");
    auto p2r_data = file_load(base_dir + '/' + chip + "-p2r.txt");
    auto p2p_data = file_load(base_dir + '/' + chip + "-p2p.txt");
    auto inv_data = file_load(base_dir + '/' + chip + "-inv.txt");

    NodesReader nr;
    RoutesParser rparse(nr, r_data, width);
    LinesParser lparse(nr, l_data);
    RDriverParser dparse(nr, d_data);
    P2RLoader p2r(nr, p2r_data);
    P2PLoader p2p(nr, p2p_data);
    InvLoader inv(nr, inv_data, width, p2r, p2p);

    std::vector<uint8_t> output(1024*1024*1024, 0);
    data_header *dh = reinterpret_cast<data_header *>(output.data());

    dh->off_rnode = sizeof(data_header);

    std::vector<uint32_t> rnode_vec;
    std::vector<uint32_t> rnode_pos;

    uint8_t *opos = output.data() + dh->off_rnode;
    while(rparse.rn || lparse.rn || dparse.rn) {

      rnode_t rn = rparse.rn;
      if(!rn || rn > lparse.rn)
	rn = lparse.rn;
      if(!rn || rn > dparse.rn)
	rn = dparse.rn;

      rnode_vec.push_back(rn);
      rnode_pos.push_back(opos - output.data() - dh->off_rnode);

      rnode_base *rb = reinterpret_cast<rnode_base *>(opos);
      
      rb->node = rn;
      bool rv = rparse.rn == rn;
      bool lv = lparse.rn == rn;
      bool dv = dparse.rn == rn;

      rb->pattern = rv ? rparse.pattern : 0xff;
      rb->target_count = lv ? lparse.target_count : 0;
      rb->drivers[0] = dv ? dparse.drivers[0] : 0xff;
      rb->drivers[1] = dv ? dparse.drivers[1] : 0xff;
      rb->line_info_index = lv ? get_line_info(lparse.li) : 0xffff;
      rb->driver_position = lv ? lparse.driver_position : 0;
      rb->fw_pos = rv ? rparse.fw_pos : 0;

      uint32_t span = rv ? rparse.pattern == 0xfe ? 1 : rmux_patterns[rparse.pattern].span : 0;
    
      opos += sizeof(rnode_base);

      if(span) {
	memcpy(opos, rparse.sources, span*4);
	opos += 4*span;
      }

      if(rb->target_count) {
	memcpy(opos, lparse.targets, rb->target_count*4);
	opos += rb->target_count*4;
	memcpy(opos, lparse.target_pos, rb->target_count*2);
	opos += rb->target_count*2;
	if(rb->target_count & 1) {
	  *opos ++ = 0;
	  *opos ++ = 0;
	}
      }

      if(rparse.rn == rn)
	rparse.next();
      if(lparse.rn == rn)
	lparse.next();
      if(dparse.rn == rn)
	dparse.next();
    }
    assert(size_t(opos-output.data()) <= output.size());

    r_data.clear();
    l_data.clear();

    auto hdata = bdz_ph_hash::make(rnode_vec);
    size_t lhash = (hdata.size() + 3) & ~3;
    uint32_t nhslot = (bdz_ph_hash::output_range(hdata)+7) >> 3;
    size_t lind = 4*nhslot;
    size_t lbase = 4*rnode_vec.size();
    size_t llines = sizeof(rnode_line_information)*rli_data.size();
    size_t p2rs = p2r.data.size()*sizeof(p2r_info);
    size_t p2ps = p2p.data.size()*sizeof(p2p_info);
    size_t invs = inv.data.size()*sizeof(inverter_info);

    dh->off_rnode_hash = opos - output.data();
    dh->off_rnode_hash_to_index = dh->off_rnode_hash + lhash;
    dh->off_rnode_index_to_base = dh->off_rnode_hash_to_index + lind;
    dh->off_line_info = dh->off_rnode_index_to_base + lbase;
    dh->off_p2r_info = dh->off_line_info + llines;
    dh->off_p2p_info = dh->off_p2r_info + p2rs;
    dh->off_inv_info = dh->off_p2p_info + p2ps;
    size_t end = dh->off_inv_info + invs;

    dh->count_rnode = rnode_vec.size();
    dh->count_p2r = p2r.data.size();
    dh->count_p2p = p2p.data.size();
    dh->count_inv = inv.data.size();

    output.resize(end, 0);
    dh = reinterpret_cast<data_header *>(output.data());

    memcpy(output.data() + dh->off_rnode_hash, hdata.data(), hdata.size());

    uint32_t *indexes = reinterpret_cast<uint32_t *>(output.data() + dh->off_rnode_hash_to_index);
    for(uint32_t i=0; i != rnode_vec.size(); i++) {
      uint32_t slot = bdz_ph_hash::lookup(hdata, rnode_vec[i]);
      indexes[slot >> 3] |= 0x01000000 << (slot & 7);
    }

    uint32_t cur_index = 0;
    for(uint32_t i=0; i != nhslot; i++) {
      indexes[i] |= cur_index;
      for(int j=24; j != 32; j++)
	if((indexes[i] >> j) & 1)
	  cur_index ++;
    }

    std::function<int (rnode_t)> rn2ri = [hdata, indexes](rnode_t rn) -> int {
      uint32_t slot = bdz_ph_hash::lookup(hdata, rn);
      uint32_t data = indexes[slot >> 3];
      int index = data & 0xffffff;
      for(uint32_t i = 0; i != (slot & 7); i++)
	if(data & (0x01000000 << i))
	  index ++;
      return index;
    };

    uint32_t *offsets = reinterpret_cast<uint32_t *>(output.data() + dh->off_rnode_index_to_base);
    for(uint32_t i=0; i != rnode_vec.size(); i++)
      offsets[rn2ri(rnode_vec[i])] = rnode_pos[i];


    rnode_base *cur = reinterpret_cast<rnode_base *>(output.data() + dh->off_rnode);
    rnode_base *nend = reinterpret_cast<rnode_base *>(output.data() + dh->off_rnode_hash);
    while(cur != nend) {
      rnode_t *node = reinterpret_cast<rnode_t *>(cur+1);
      int sspan = cur->pattern == 0xff ? 0 : cur->pattern == 0xfe ? 1 : rmux_patterns[cur->pattern].span;
      int sid = 0;
      for(int i=0; i != sspan; i++) {
	int v = *node ? rn2ri(*node) : -1;
	if(v >= 0)
	  v |= (sid++) << 24;
	*node++ = v;
      }

      int tid = 0;
      for(int i=0; i != cur->target_count; i++) {
	if(*node & 0x80000000)
	  node++;
	else {
	  int v = rn2ri(*node) | (tid++) << 24;
	  *node++ = v;  
	}
      }

      node += (cur->target_count + 1) >> 1;
      cur = reinterpret_cast<rnode_base *>(node);
    }

    p2r.remap(rn2ri);
    inv.remap(rn2ri);

    memcpy(output.data() + dh->off_line_info, rli_data.data(), llines);
    memcpy(output.data() + dh->off_p2r_info, p2r.data.data(), p2rs);
    memcpy(output.data() + dh->off_p2p_info, p2p.data.data(), p2ps);
    memcpy(output.data() + dh->off_inv_info, inv.data.data(), invs);

    fprintf(stderr, "%-6s size %9d nodeinfo %9d hash %7d hash2i %7d i2r %8d nodes %7d lines %5d p2r %5d p2p %4d inv %5d\n", chip.c_str(), int(output.size()), dh->off_rnode_hash - dh->off_rnode, int(lhash), int(lind), int(lbase), dh->count_rnode, int(rli_data.size()), int(p2r.data.size()), int(p2p.data.size()), int(inv.data.size()));

    file_save(dest_dir + '/' + chip + "-r.bin", output, strtol(argv[4], nullptr, 10));
  }

  exit(0);
}
