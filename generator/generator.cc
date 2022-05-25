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
#include "one.h"
#include "dcram.h"
#include "hps.h"
#include "pram.h"
#include "iob.h"
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
  uint32_t off_rnode_end;
  uint32_t off_rnode_hash;
  uint32_t off_line_info;
  uint32_t off_p2r_info;
  uint32_t off_p2p_info;
  uint32_t off_inv_info;
  uint32_t off_1_info;
  uint32_t off_dcram_info;
  uint32_t off_hps_info;
  uint32_t off_fixed_info;
  uint32_t off_dqs16_info;
  uint32_t off_iob_info;

  uint32_t size_rnode_opaque_hash;
  uint32_t count_rnode;
  uint32_t count_p2r;
  uint32_t count_p2p;
  uint32_t count_inv;
  uint32_t count_1;
  uint32_t count_dcram;
  uint32_t count_dqs16;
  uint32_t count_iob;
};

struct global_data_header {
  uint32_t off_dnode_lookup;
  uint32_t off_dnode_table2;
  uint32_t off_dnode_table3;
  uint32_t off_dnode_drivers;
};

struct rnode_object {
  rnode_coords node;
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

    auto r_data     = file_load(base_dir + '/' + chip + "-r.txt.xz");
    auto l_data     = file_load(base_dir + '/' + chip + "-l.txt.xz");
    auto d_data     = file_load(base_dir + '/' + chip + "-d.txt.xz");
    auto p2r_data   = file_load(base_dir + '/' + chip + "-p2r.txt");
    auto p2p_data   = file_load(base_dir + '/' + chip + "-p2p.txt");
    auto inv_data   = file_load(base_dir + '/' + chip + "-inv.txt");
    auto one_data   = file_load(base_dir + '/' + chip + "-1.txt");
    auto pram_data  = file_load(base_dir + '/' + chip + "-pram.txt");
    auto iob_data   = file_load(base_dir + '/' + chip + "-iob.txt");
    auto dcram_data = file_load(base_dir + "/pma3-cram.txt");

    NodesReader nr;
    RoutesParser rparse(nr, r_data, width);
    LinesParser lparse(nr, l_data);
    RDriverParser dparse(nr, d_data);
    P2RLoader p2r(nr, p2r_data);
    P2PLoader p2p(nr, p2p_data);
    InvLoader inv(nr, inv_data, width, p2r, p2p);
    OneLoader one(one_data, width);
    DCRamLoader dcram(dcram_data);
    PRamLoader pram(pram_data);
    HPSLoader hps(p2p, p2r);
    IOBLoader iob(iob_data, pram);

    auto dcram_info = dcram.get_for_die(chip, width);
    auto fixed_info = pram.get_fixed_blocks();
    auto dqs16_info = pram.get_dqs16();
    std::vector<uint8_t> output(1024*1024*1024, 0);
    data_header *dh = reinterpret_cast<data_header *>(output.data());

    dh->off_rnode = sizeof(data_header);

    std::vector<uint32_t> rnode_vec;
    std::vector<uint32_t> rnode_pos;

    uint8_t *opos = output.data() + dh->off_rnode;
    while(rparse.rn || lparse.rn) {

      rnode_object rb;
      rnode_coords rn = rparse.rn;
      if(!rn || rn > lparse.rn)
	rn = lparse.rn;
      if(!rn || rn > dparse.rn)
	rn = dparse.rn;

      rnode_vec.push_back(rn);
      rnode_pos.push_back(opos - output.data() - dh->off_rnode);

      rb.node = rn;
      bool rv = rparse.rn == rn;
      bool lv = lparse.rn == rn;
      bool dv = dparse.rn == rn;

      rb.pattern = rv ? rparse.pattern : 0xff;
      rb.target_count = lv ? lparse.target_count : 0;
      rb.drivers[0] = dv ? dparse.drivers[0] : 0xff;
      rb.drivers[1] = dv ? dparse.drivers[1] : 0xff;
      rb.line_info_index = lv ? get_line_info(lparse.li) : 0xffff;
      rb.driver_position = lv ? lparse.driver_position : 0;
      rb.fw_pos = rv ? rparse.fw_pos : 0;

      uint32_t span = rv ? rparse.pattern == 0xfe ? 1 : rmux_patterns[rparse.pattern].span : 0;
    
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
      if(dparse.rn == rn)
	dparse.next();
    }
    dh->off_rnode_end = opos - output.data();
    assert(size_t(opos-output.data()) <= output.size());

    r_data.clear();
    l_data.clear();

    auto hdata = bdz_ph_hash::make(rnode_vec);
    size_t len1   = (hdata.size() + 3) & ~3;
    size_t len2   = 4*bdz_ph_hash::output_range(hdata);
    size_t llines = sizeof(rnode_line_information)*rli_data.size();
    size_t p2rs   = p2r.data.size()*sizeof(p2r_info);
    size_t p2ps   = p2p.data.size()*sizeof(p2p_info);
    size_t invs   = inv.data.size()*sizeof(inverter_info);
    size_t ones   = one.data.size()*sizeof(uint32_t);
    size_t dcrams = dcram_info.size()*sizeof(uint32_t);
    size_t hpss   = hps.data.empty() ? 0 : sizeof(pos_t)*I_HPS_COUNT;
    size_t fixeds = FB_COUNT * sizeof(fixed_block_info);
    size_t dqs16s = dqs16_info.size() * sizeof(dqs16_info);
    size_t iobs   = iob.data.size() * sizeof(ioblock_info);

    dh->off_rnode_hash = opos - output.data();
    dh->off_line_info = dh->off_rnode_hash + len1 + len2;
    dh->off_p2r_info = dh->off_line_info + llines;
    dh->off_p2p_info = dh->off_p2r_info + p2rs;
    dh->off_inv_info = dh->off_p2p_info + p2ps;
    dh->off_1_info = dh->off_inv_info + invs;
    dh->off_dcram_info = dh->off_1_info + ones;
    dh->off_hps_info = hpss ? dh->off_dcram_info + dcrams : 0;
    dh->off_fixed_info = dh->off_dcram_info + dcrams + hpss;
    dh->off_dqs16_info = dh->off_fixed_info + fixeds;
    dh->off_iob_info = dh->off_dqs16_info + dqs16s;
    uint32_t end = dh->off_iob_info + iobs;

    dh->size_rnode_opaque_hash = len1;
    dh->count_rnode = rnode_vec.size();
    dh->count_p2r = p2r.data.size();
    dh->count_p2p = p2p.data.size();
    dh->count_inv = inv.data.size();
    dh->count_1 = one.data.size();
    dh->count_dcram = dcram_info.size();
    dh->count_dqs16 = dqs16_info.size();
    dh->count_iob = iob.data.size();

    output.resize(end, 0);
    dh = reinterpret_cast<data_header *>(output.data());
    memcpy(output.data() + dh->off_rnode_hash, hdata.data(), hdata.size());
    uint32_t *offsets = reinterpret_cast<uint32_t *>(output.data() + dh->off_rnode_hash + len1);

    for(uint32_t i=0; i != rnode_vec.size(); i++)
      offsets[bdz_ph_hash::lookup(hdata, rnode_vec[i])] = rnode_pos[i];

    memcpy(output.data() + dh->off_line_info, rli_data.data(), llines);
    memcpy(output.data() + dh->off_p2r_info, p2r.data.data(), p2rs);
    memcpy(output.data() + dh->off_p2p_info, p2p.data.data(), p2ps);
    memcpy(output.data() + dh->off_inv_info, inv.data.data(), invs);
    memcpy(output.data() + dh->off_1_info, one.data.data(), ones);
    memcpy(output.data() + dh->off_dcram_info, dcram_info.data(), dcrams);
    if(hpss)
      memcpy(output.data() + dh->off_hps_info, hps.data.data(), hpss);
    memcpy(output.data() + dh->off_fixed_info, fixed_info.data(), fixeds);
    memcpy(output.data() + dh->off_dqs16_info, dqs16_info.data(), dqs16s);
    memcpy(output.data() + dh->off_iob_info, iob.data.data(), iobs);

    fprintf(stderr, "%-6s size %9d nodeinfo %9d hash %7d hashmap %8d nodes %7d lines %5d p2r %5d p2p %4d inv %5d one %2d dcram %2d hps %2d fixed %2d dqs16 %2d iob %3d\n",
	    chip.c_str(),
	    int(output.size()),
	    dh->off_rnode_end - dh->off_rnode,
	    int(len1),
	    int(len2),
	    dh->count_rnode,
	    int(rli_data.size()),
	    int(p2r.data.size()),
	    int(p2p.data.size()),
	    int(inv.data.size()),
	    int(one.data.size()),
	    int(dcram_info.size()),
	    int(hps.data.size()),
	    int(fixed_info.size()),
	    int(dqs16_info.size()),
	    int(iob.data.size()));

    file_save(dest_dir + '/' + chip + "-r.bin", output, strtol(argv[4], nullptr, 10));
  }

  exit(0);
}
