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
  uint32_t off_ro;
  uint32_t off_roh;
  uint32_t off_ri;
  uint32_t off_rsrc;
  uint32_t off_line;
  uint32_t off_p2r;
  uint32_t off_p2p;
  uint32_t off_inv;
  uint32_t off_1;
  uint32_t off_dcram;
  uint32_t off_hps;
  uint32_t off_fixed;
  uint32_t off_dqs16;
  uint32_t off_iob;

  uint32_t count_ro;
  uint32_t count_ri;
  uint32_t count_rsrc;
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

    dh->off_ro = sizeof(data_header);

    std::vector<uint32_t> rnode_vec;
    std::vector<uint32_t> rnode_pos;
    std::vector<uint8_t> source_patterns;
    std::unordered_map<uint16_t, std::vector<uint32_t>> source_offsets;
    uint32_t source_offsets_counts = 0;

    uint8_t *opos = output.data() + dh->off_ro;
    while(rparse.rn || lparse.rn) {

      rnode_object rb;
      rnode_coords rn = rparse.rn;
      if(!rn || rn > lparse.rn)
	rn = lparse.rn;
      if(!rn || rn > dparse.rn)
	rn = dparse.rn;

      rnode_vec.push_back(rn.v);
      rnode_pos.push_back(opos - output.data() - dh->off_ro);

      rb.ro_rc = rn;
      bool rv = rparse.rn == rn;
      bool lv = lparse.rn == rn;
      bool dv = dparse.rn == rn;

      rb.ro_pattern = rv ? rparse.pattern : 0xff;
      rb.ro_targets_count = lv ? lparse.targets_count : 0;
      rb.ro_targets_caps_count = lv ? lparse.targets_caps_count : 0;
      rb.ro_drivers[0] = dv ? dparse.drivers[0] : 0xff;
      rb.ro_drivers[1] = dv ? dparse.drivers[1] : 0xff;
      rb.ro_line_info_index = lv ? get_line_info(lparse.li) : 0xffff;
      rb.ro_driver_position = lv ? lparse.driver_position : 0;
      rb.ro_fw_pos = rv ? rparse.fw_pos : 0;
      uint32_t span = rv ? rparse.pattern == 0xfe ? 1 : rmux_patterns[rparse.pattern].span : 0;

      uint8_t *rbpos = opos;
      opos += sizeof(rb);

      if(span) {
	uint8_t srcidx[64];
	uint8_t backidx[64];
	uint32_t srclen = 0;

	for(uint32_t i = 0; i != span; i++) {
	  if(rparse.sources[i]) {
	    if(i != srclen)
	      rparse.sources[srclen] = rparse.sources[i];
	    srcidx[srclen] = i;
	    backidx[i] = srclen++;
	  } else
	    backidx[i] = 0xff;
	}

	rb.ro_span = span;
	rb.ro_srclen = srclen;

	uint16_t key = (span << 8) | srclen;
	auto &offs = source_offsets[key];
	for(uint32_t off : offs)
	  if(!memcmp(source_patterns.data() + off, srcidx, srclen)) {
	    rb.ro_srcoff = off;
	    goto found;
	  }
	{
	  uint32_t off = source_patterns.size();
	  rb.ro_srcoff = off;
	  source_patterns.resize(off + srclen + span);
	  memcpy(source_patterns.data() + off, srcidx, srclen);
	  memcpy(source_patterns.data() + off + srclen, backidx, span);
	  offs.push_back(off);
	  source_offsets_counts ++;
	}

      found:
	memcpy(opos, rparse.sources.data(), srclen*4);
	opos += 4*srclen;
      } else {
	rb.ro_span = 0;
	rb.ro_srclen = 0;
	rb.ro_srcoff = 0xffff;
      }

      memcpy(rbpos, &rb, sizeof(rb));

      if(rb.ro_targets_count || rb.ro_targets_caps_count) {
	memcpy(opos, lparse.targets.data(), rb.ro_targets_count*4);
	opos += rb.ro_targets_count*4;
	memcpy(opos, lparse.targets_caps.data(), rb.ro_targets_caps_count*4);
	opos += rb.ro_targets_caps_count*4;
	uint32_t tt = rb.ro_targets_count+rb.ro_targets_caps_count;
	memcpy(opos, lparse.targets_pos.data(), tt*2);
	opos += tt*2;
	if(tt & 1) {
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
    size_t rohs   = (hdata.size() + 3) & ~3;
    size_t ris    = 4*bdz_ph_hash::output_range(hdata);
    size_t rsrc   = (source_patterns.size() + 3) & ~3;
    size_t llines = sizeof(rnode_line_information)*rli_data.size();
    size_t p2rs   = p2r.data.size()*sizeof(p2r_info);
    size_t p2ps   = p2p.data.size()*sizeof(p2p_info);
    size_t invs   = inv.data.size()*sizeof(inverter_info);
    size_t ones   = one.data.size()*sizeof(uint32_t);
    size_t dcrams = dcram_info.size()*sizeof(uint32_t);
    size_t hpss   = hps.data.empty() ? 0 : sizeof(xycoords)*I_HPS_COUNT;
    size_t fixeds = FB_COUNT * sizeof(fixed_block_info);
    size_t dqs16s = dqs16_info.size() * sizeof(dqs16_info);
    size_t iobs   = iob.data.size() * sizeof(ioblock_info);

    dh->off_roh = opos - output.data();
    dh->off_ri = dh->off_roh + rohs;
    dh->off_rsrc = dh->off_ri + ris;
    dh->off_line = dh->off_rsrc + rsrc;
    dh->off_p2r = dh->off_line + llines;
    dh->off_p2p = dh->off_p2r + p2rs;
    dh->off_inv = dh->off_p2p + p2ps;
    dh->off_1 = dh->off_inv + invs;
    dh->off_dcram = dh->off_1 + ones;
    dh->off_hps = hpss ? dh->off_dcram + dcrams : 0;
    dh->off_fixed = dh->off_dcram + dcrams + hpss;
    dh->off_dqs16 = dh->off_fixed + fixeds;
    dh->off_iob = dh->off_dqs16 + dqs16s;
    uint32_t end = dh->off_iob + iobs;

    dh->count_ro = rnode_vec.size();
    dh->count_ri = bdz_ph_hash::output_range(hdata);
    dh->count_rsrc = source_patterns.size() ;
    dh->count_p2r = p2r.data.size();
    dh->count_p2p = p2p.data.size();
    dh->count_inv = inv.data.size();
    dh->count_1 = one.data.size();
    dh->count_dcram = dcram_info.size();
    dh->count_dqs16 = dqs16_info.size();
    dh->count_iob = iob.data.size();

    output.resize(end, 0);
    dh = reinterpret_cast<data_header *>(output.data());
    memcpy(output.data() + dh->off_roh, hdata.data(), hdata.size());
    uint32_t *offsets = reinterpret_cast<uint32_t *>(output.data() + dh->off_ri);
    memset(offsets, 0xff, ris);

    for(uint32_t i=0; i != rnode_vec.size(); i++) {
      rnode_index idx = bdz_ph_hash::lookup(hdata, rnode_vec[i]);
      offsets[idx] = rnode_pos[i];
      rnode_object *ro = reinterpret_cast<rnode_object *>(output.data() + dh->off_ro + rnode_pos[i]);
      ro->ro_ri = idx;
    }

    memcpy(output.data() + dh->off_rsrc, source_patterns.data(), dh->count_rsrc);
    memcpy(output.data() + dh->off_line, rli_data.data(), llines);
    memcpy(output.data() + dh->off_p2r, p2r.data.data(), p2rs);
    memcpy(output.data() + dh->off_p2p, p2p.data.data(), p2ps);
    memcpy(output.data() + dh->off_inv, inv.data.data(), invs);
    memcpy(output.data() + dh->off_1, one.data.data(), ones);
    memcpy(output.data() + dh->off_dcram, dcram_info.data(), dcrams);
    if(hpss)
      memcpy(output.data() + dh->off_hps, hps.data.data(), hpss);
    memcpy(output.data() + dh->off_fixed, fixed_info.data(), fixeds);
    memcpy(output.data() + dh->off_dqs16, dqs16_info.data(), dqs16s);
    memcpy(output.data() + dh->off_iob, iob.data.data(), iobs);

    fprintf(stderr, "%-6s size %9d ro %9d rh %7d ri %8d rc %7d rsrc %d/%d lines %5d p2r %5d p2p %4d inv %5d one %2d dcram %2d hps %2d fixed %2d dqs16 %2d iob %3d\n",
	    chip.c_str(),
	    int(output.size()),
	    dh->off_roh - dh->off_ro,
	    dh->off_ri - dh->off_roh,
	    dh->count_ri,
	    dh->count_ro,
	    source_offsets_counts,
	    dh->count_rsrc,
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
