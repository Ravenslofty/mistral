#include "cyclonev.h"

#include <lzma.h>
#include <set>

std::string mistral::CycloneV::rn2s(rnode_t rn)
{
  char buf[4096];
  sprintf(buf, "%s.%03d.%03d.%04d", rnode_type_names[rn2t(rn)], rn2x(rn), rn2y(rn), rn2z(rn));
  return buf;
}


std::string mistral::CycloneV::pn2s(pnode_t pn)
{
  char buf[4096];
  auto bi = pn2bi(pn);
  auto pi = pn2pi(pn);
  if(bi == -1)
    if(pi == -1)
      sprintf(buf, "%s.%03d.%03d:%s", block_type_names[pn2bt(pn)], pn2x(pn), pn2y(pn), port_type_names[pn2pt(pn)]);
    else
      sprintf(buf, "%s.%03d.%03d:%s.%d", block_type_names[pn2bt(pn)], pn2x(pn), pn2y(pn), port_type_names[pn2pt(pn)], pi);
  else
    if(pi == -1)
      sprintf(buf, "%s.%03d.%03d.%d:%s", block_type_names[pn2bt(pn)], pn2x(pn), pn2y(pn), bi, port_type_names[pn2pt(pn)]);
    else
      sprintf(buf, "%s.%03d.%03d.%d:%s.%d", block_type_names[pn2bt(pn)], pn2x(pn), pn2y(pn), bi, port_type_names[pn2pt(pn)], pi);
  return buf;
}


void mistral::CycloneV::rmux_load()
{
  rmux_info = std::make_unique<rmux []>(di.rmux_count);
  lzma_stream strm = LZMA_STREAM_INIT;
  lzma_ret ret;
  if ((ret = lzma_stream_decoder(&strm, UINT64_MAX, 0)) != LZMA_OK) {
    fprintf(stderr, "failed to initialise liblzma: %d\n", ret);
    exit(1);
  }

  strm.next_in = di.routing_data_start;
  strm.avail_in = di.routing_data_end - di.routing_data_start;
  strm.next_out = (uint8_t*)rmux_info.get();
  strm.avail_out = di.rmux_count*sizeof(rmux);
  
  if((ret = lzma_code(&strm, LZMA_RUN)) != LZMA_STREAM_END) {
    fprintf(stderr, "rmux data decompression failure: %d\n", ret);
    exit(1);
  }

  for(unsigned int i=0; i != di.rmux_count; i++)
    dest_node_to_rmux[rmux_info[i].destination] = i;
}

uint32_t mistral::CycloneV::rmux_get_val(const rmux &r) const
{
  uint32_t val = 0;
  const rmux_pattern &pat = rmux_patterns[r.pattern];
  const uint8_t *bits = rmux_xy + pat.o_xy*2;
  for(uint8_t bit = 0; bit != pat.bits; bit++) {
    uint32_t pos = r.fw_pos + bits[0] + bits[1]*di.cram_sx;
    if((cram[pos >> 3] >> (pos & 7)) & 1)
      val |= 1 << bit;
    bits += 2;
  }
  return val;
}

void mistral::CycloneV::rmux_set_val(const rmux &r, uint32_t val)
{
  const rmux_pattern &pat = rmux_patterns[r.pattern];
  const uint8_t *bits = rmux_xy + pat.o_xy*2;
  for(uint8_t bit = 0; bit != pat.bits; bit++) {
    uint32_t pos = r.fw_pos + bits[0] + bits[1]*di.cram_sx;
    if((val >> bit) & 1)
      cram[pos >> 3] |= 1 << (pos & 7);
    else
      cram[pos >> 3] &= ~(1 << (pos & 7));
    bits += 2;
  }
}

int mistral::CycloneV::rmux_get_slot(const rmux &r) const
{
  const rmux_pattern &pat = rmux_patterns[r.pattern];
  uint32_t val = rmux_get_val(r);
  uint32_t vh = val % pat.hashdiv;
  int slot = rmux_vhash[pat.o_vhash + vh];
  if(slot == -1 || rmux_vals[pat.o_vals + slot] != val)
    return -1;
  return slot;
}

mistral::CycloneV::rnode_t mistral::CycloneV::rmux_get_source(const rmux &r) const
{
  int slot = rmux_get_slot(r);
  if(slot == -1)
    return 0;
  return r.sources[slot];
}

bool mistral::CycloneV::rnode_do_link(rnode_t n1, rnode_t n2)
{
  auto i = dest_node_to_rmux.find(n2);
  if(i == dest_node_to_rmux.end())
    return false;

  const rmux &r = rmux_info[i->second];
  const rmux_pattern &pat = rmux_patterns[r.pattern];
  for(int slot = 0; slot != pat.span; slot++)
    if(r.sources[slot] == n1) {
      rmux_set_val(r, rmux_vals[pat.o_vals + slot]);
      return true;
    }
  return false;
}

void mistral::CycloneV::rnode_link(rnode_t n1, rnode_t n2)
{
  if(!rnode_do_link(n1, n2)) {
    fprintf(stderr, "Error: No possible direct link between rnodes %s and %s\n", rn2s(n1).c_str(), rn2s(n2).c_str());
    exit(1);
  }
}

bool mistral::CycloneV::rmux_is_default(rnode_t node) const
{
  auto i = dest_node_to_rmux.find(node);
  if(i == dest_node_to_rmux.end())
    return true;
  const auto &r = rmux_info[i->second];
  return rmux_get_val(r) == rmux_patterns[r.pattern].def;
}

void mistral::CycloneV::route_set_defaults()
{
  for(unsigned int i=0; i != di.rmux_count; i++) {
    const auto &r = rmux_info[i];
    const rmux_pattern &pat = rmux_patterns[r.pattern];
    rmux_set_val(r, pat.def);
  } 
}

std::vector<std::pair<mistral::CycloneV::rnode_t, mistral::CycloneV::rnode_t>> mistral::CycloneV::route_all_active_links() const
{
  std::vector<std::pair<rnode_t, rnode_t>> links;
  for(unsigned int i=0; i != di.rmux_count; i++) {
    rnode_t snode = rmux_get_source(rmux_info[i]);
    if(snode) {
      rnode_t dnode = rmux_info[i].destination;
      if(rn2t(dnode) == DCMUX && rn2t(snode) == TCLK && rmux_is_default(snode))
	continue;

      links.emplace_back(std::make_pair(snode, dnode));
    } else {
      uint32_t val = rmux_get_val(rmux_info[i]);
      if(val != rmux_patterns[rmux_info[i].pattern].def)
	fprintf(stderr, "Source unknown on rnode %s (%2d, %0*x)\n", rn2s(rmux_info[i].destination).c_str(), rmux_info[i].pattern, (rmux_patterns[rmux_info[i].pattern].bits+3)/4, val);
    }
  }
  return links;
}

std::vector<std::pair<mistral::CycloneV::rnode_t, mistral::CycloneV::rnode_t>> mistral::CycloneV::route_frontier_links() const
{
  std::vector<std::pair<rnode_t, rnode_t>> links;
  auto blinks = route_all_active_links();
  std::unordered_map<rnode_t, rnode_t> backtracks;
  std::unordered_map<rnode_t, uint32_t> mode;

  for(const auto &l : blinks) {
    mode[l.first] |= 1;
    mode[l.second] |= 2;
    backtracks[l.second] = l.first;
  }

  for(const auto &m : mode) {
    if(m.second == 2) {
      rnode_t s = m.first;
      while(s && mode[s] != 1)
	s = backtracks[s];
      assert(s);
      links.emplace_back(std::make_pair(s, m.first));
    }
  }

  return links;
}

std::vector<std::vector<mistral::CycloneV::rnode_t>> mistral::CycloneV::route_frontier_links_with_path() const
{
  std::vector<std::vector<rnode_t>> links;
  auto blinks = route_all_active_links();
  std::unordered_map<rnode_t, rnode_t> backtracks;
  std::unordered_map<rnode_t, uint32_t> mode;

  for(const auto &l : blinks) {
    mode[l.first] |= 1;
    mode[l.second] |= 2;
    backtracks[l.second] = l.first;
  }

  for(const auto &m : mode) {
    if(m.second == 2) {
      rnode_t s = m.first;
      links.resize(links.size()+1);
      auto &l = links.back();
      l.insert(l.begin(), s);
      while(s && mode[s] != 1) {
	s = backtracks[s];
	l.insert(l.begin(), s);
      }
      assert(s);
    }
  }

  return links;
}

void mistral::CycloneV::init_p2r_maps()
{
  for(const p2r_info *p2r = di.p2r; p2r->p; p2r++) {
    p2r_map[p2r->p] = p2r->r;
    r2p_map[p2r->r] = p2r->p;
  };
}

std::vector<std::pair<mistral::CycloneV::pnode_t, mistral::CycloneV::rnode_t>> mistral::CycloneV::get_all_p2r() const
{
  std::vector<std::pair<pnode_t, rnode_t>> result;
  for(const p2r_info *p2r = di.p2r; p2r->p; p2r++)
    result.emplace_back(std::make_pair(p2r->p, p2r->r));
  return result;
}

std::vector<std::pair<mistral::CycloneV::pnode_t, mistral::CycloneV::pnode_t>> mistral::CycloneV::get_all_p2p() const
{
  std::vector<std::pair<pnode_t, pnode_t>> result;
  for(const p2p_info *p2p = di.p2p; p2p->s; p2p++)
    result.emplace_back(std::make_pair(p2p->s, p2p->d));
  return result;
}


std::vector<std::pair<mistral::CycloneV::pnode_t, mistral::CycloneV::rnode_t>> mistral::CycloneV::get_all_p2ri() const
{
  std::vector<std::pair<pnode_t, rnode_t>> result;
  pos_t lab = lab_pos[0];
  pos_t mlab = mlab_pos[0];
  pos_t m10k = m10k_pos[0];
  pos_t dsp = dsp_pos[0];
  pos_t dsp2 = xy2pos(pos2x(dsp), pos2y(dsp)+1);

  auto tt = [lab, mlab, m10k, dsp, dsp2](rnode_t n) -> bool { auto p = rn2p(n); return n && (p == lab || p == mlab || p == m10k || p == dsp || p == dsp2); };

  std::set<rnode_t> nodes;
  for(unsigned int i=0; i != di.rmux_count; i++) {
    const auto &r = rmux_info[i];
    if(tt(r.destination))
      nodes.insert(r.destination);
    int span = rmux_patterns[r.pattern].span;
    for(int j = 0; j != span; j++)
      if(tt(r.sources[j]))
	nodes.insert(r.sources[j]);
  }

  for(rnode_t n : nodes) {
    auto p = rnode_to_pnode(n);
    if(p)
      result.emplace_back(std::make_pair(p, n));
  }

  return result;
}

mistral::CycloneV::rnode_t mistral::CycloneV::pnode_to_rnode(pnode_t pn) const
{
  auto i = p2r_map.find(pn);
  if(i != p2r_map.end())
    return i->second;

  pos_t p = pn2p(pn);
  int bi = pn2bi(pn);
  int pi = pn2pi(pn);
  switch(pn2bt(pn)) {
  case LAB: case MLAB:
    switch(pn2pt(pn)) {
    case E0:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+0);
      break;

    case F0:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+1);
      break;

    case A:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+2);
      break;

    case B:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+3);
      break;

    case C:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+4);
      break;

    case D:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+5);
      break;

    case E1:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+6);
      break;

    case F1:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GOUT, p, 8*bi+7);
      break;

    case DATAIN:
      if(bi == -1 && pi >= 0 && pi < 4)
	return rnode(GOUT, p, 80 + pi);
      break;

    case FFT0:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GIN, p, 4*bi+0);
      break;

    case FFT1:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GIN, p, 4*bi+1);
      break;

    case FFB0:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GIN, p, 4*bi+2);
      break;

    case FFB1:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(GIN, p, 4*bi+3);
      break;

    case FFT1L:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(LD, p, 2*bi+0);
      break;

    case FFB1L:
      if(bi >= 0 && bi < 10 && pi == -1)
	return rnode(LD, p, 2*bi+1);
      break;

    case CLKIN:
      if(bi == -1 && pi >= 0 && pi < 2)
	return rnode(TCLK, p, pi);
      break;

    case ACLR:
      if(bi == -1 && pi >= 0 && pi < 2)
	return rnode(TCLK, p, pi+2);
      break;

    default:
      break;
    }
    break;

  case DSP: {
    switch(pn2pt(pn)) {
    case DATAIN:
      if(bi < 0 || bi >= 12 || pi < 0 || pi >= 9)
	break;
      switch(bi) {
      case  0: return rnode(GOUT, p+1, pi);
      case  1: return rnode(GOUT, p+1, pi+9);
      case  2: return rnode(GOUT, p+1, pi+18);
      case  3: return rnode(GOUT, p+1, pi <= 2 ? pi+27 : 61+3-pi);
      case  4: return rnode(GOUT, p+1, 55-pi);
      case  5: return rnode(GOUT, p+1, 46-pi);
      case  6: return rnode(GOUT, p,   pi);
      case  7: return rnode(GOUT, p,   pi+9);
      case  8: return rnode(GOUT, p,   pi+18);
      case  9: return rnode(GOUT, p,   pi <= 2 ? pi+27 : 61+3-pi);
      case 10: return rnode(GOUT, p,   55-pi);
      case 11: return rnode(GOUT, p,   46-pi);
      }
      break;

    case RESULT:
      if(pi < 0 || pi >= 74)
	break;
      if(pi <= 17)
	return rnode(GIN, p+1, pi);
      if(pi <= 35)
	return rnode(GIN, p,   pi-18);
      if(pi <= 54)
	return rnode(GIN, p+1, pi-36+18);
      if(pi <= 73)
	return rnode(GIN, p,   pi-55+18);
      break;

    case ACLR:
      if(pi >= 0 && pi <= 1)
	return rnode(TCLK, p, 3 + pi);
      else if(pi >= 2 && pi <= 3)
	return rnode(GOUT, p, 37-(pi-2));
      break;

    case NEGATE:
      if(pi != -1)
	break;
      return rnode(GOUT, p, 32);

    case LOADCONST:
      if(pi != -1)
	break;
      return rnode(GOUT, p, 33);

    case ACCUMULATE:
      if(pi != -1)
	break;
      return rnode(GOUT, p, 34);

    case SUB:
      if(pi != -1)
	break;
      return rnode(GOUT, p, 35);

    case ENABLE:
      if(pi < 0 || pi >= 3)
	break;
      return rnode(GOUT, p+1, 37-2*pi);

    case UNK_IN:
      return pi >= 64 ? rnode(GOUT, p+1, pi-64) : rnode(GOUT, p, pi);

    case CLKIN:
      if(pi >= 0 && pi <= 2)
	return rnode(TCLK, p, pi);
      else if(pi >= 3 && pi <= 5)
	return rnode(GOUT, p+1, 36-2*(pi-3));
    default:
      break;
    }
    break;
  }

  case M10K: {
    switch(pn2pt(pn)) {
    case BYTEENABLEA:
      return rnode(GOUT, p, pi);
    case BYTEENABLEB:
      return rnode(GOUT, p, pi+23);
    case ENABLE:
      return rnode(GOUT, p, pi < 2 ? pi + 18 : pi - 2 + 39);
    case ACLR:
      return rnode(GOUT, p, pi == 0 ? 6 : 29);
    case RDEN:
      return rnode(GOUT, p, pi == 0 ? 20 : 41);
    case WREN:
      return rnode(GOUT, p, pi == 0 ? 17 : 38);
    case ADDRSTALLA:
      return rnode(GOUT, p, 7);
    case ADDRSTALLB:
      return rnode(GOUT, p, 8);
    case ADDRA: {
      static const int z[12] = { 42,  2,  3,  4,  9, 10, 11, 12, 13, 14, 15, 16 };
      return rnode(GOUT, p, z[pi]);
    }
    case ADDRB: {
      static const int z[12] = { 64, 27, 26, 25, 37, 36, 35, 34, 33, 32, 31, 30 };
      return rnode(GOUT, p, z[pi]);
    }
    case DATAAIN: {
      static const int z[20] = { 43, 47, 51, 55, 59, 63, 80, 76, 72, 68, 45, 49, 53, 57, 61, 82, 78, 74, 70, 66 };
      return rnode(GOUT, p, z[pi]);
    }
    case DATABIN: {
      static const int z[20] = { 44, 48, 52, 56, 60, 83, 79, 75, 71, 67, 46, 50, 54, 58, 62, 81, 77, 73, 69, 65 };
      return rnode(GOUT, p, z[pi]);
    }
    case DATAAOUT:
      return rnode(GIN, p, pi);
    case DATABOUT:
      return rnode(GIN, p, pi + 20);
    case CLKIN:
      return pi < 6 ? rnode(TCLK, p, pi) : rnode(GOUT, p, pi == 6 ? 5 : 28);
    default:
      break;
    }
    break;
  }

  default:
    break;
  }

  return 0;
}

mistral::CycloneV::pnode_t mistral::CycloneV::rnode_to_pnode(rnode_t rn) const
{
  auto tt = tile_types[rn2p(rn)];

  if(tt == T_LAB || tt == T_MLAB) {
    auto bt = tt == T_LAB ? LAB : MLAB;
    switch(rn2t(rn)) {
    case GOUT: {
      int z = rn2z(rn);
      if(z < 80) {
	static const port_type_t ports[8] = {
	  E0, F0, A, B, C, D, E1, F1
	};
	return pnode(bt, rn2p(rn), ports[z & 7], z / 8, -1);
      }
      return pnode(bt, rn2p(rn), DATAIN, -1, z - 80);
    }

    case GIN: {
      int z = rn2z(rn);
      static const port_type_t ports[4] = {
	FFT0, FFT1, FFB0, FFB1
      };
      return pnode(bt, rn2p(rn), ports[z & 3], z / 4, -1);
    }

    case LD: {
      int z = rn2z(rn);
      static const port_type_t ports[2] = {
	FFT1L, FFB1L
      };
      return pnode(bt, rn2p(rn), ports[z & 1], z / 2, -1);
    }

    case TCLK: {
      int z = rn2z(rn);
      if(z < 2)
	return pnode(bt, rn2p(rn), CLKIN, -1, z);
      else
	return pnode(bt, rn2p(rn), ACLR, -1, z-2);
    }

    default: break;
    }
  }

  if(tt == T_DSP) {
    switch(rn2t(rn)) {
    case GOUT: {
      int z = rn2z(rn);
      if(z <= 8)
	return pnode(DSP, rn2p(rn), DATAIN, 6, z);
      if(z <= 17)
	return pnode(DSP, rn2p(rn), DATAIN, 7, z-9);
      if(z <= 26)
	return pnode(DSP, rn2p(rn), DATAIN, 8, z - 18);
      if(z <= 29)
	return pnode(DSP, rn2p(rn), DATAIN, 9, z - 27);
      if(z == 32)
	return pnode(DSP, rn2p(rn), NEGATE, -1, -1);
      if(z == 33)
	return pnode(DSP, rn2p(rn), LOADCONST, -1, -1);
      if(z == 34)
	return pnode(DSP, rn2p(rn), ACCUMULATE, -1, -1);
      if(z == 35)
	return pnode(DSP, rn2p(rn), SUB, -1, -1);
      if(z >= 36 && z <= 37)
	return pnode(DSP, rn2p(rn), ACLR, -1, 37 - z + 2);
      if(z >= 38 && z <= 46)
	return pnode(DSP, rn2p(rn), DATAIN, 11, 46 - z);
      if(z >= 47 && z <= 55)
	return pnode(DSP, rn2p(rn), DATAIN, 10, 55 - z);
      if(z >= 56 && z <= 61)
	return pnode(DSP, rn2p(rn), DATAIN, 9, 61 - z + 3);

      return pnode(DSP, rn2p(rn), UNK_IN, -1, z);
    }

    case GIN: {
      int z = rn2z(rn);
      if(z <= 17)
	return pnode(DSP, rn2p(rn), RESULT, -1, z + 18);
      if(z <= 36)
	return pnode(DSP, rn2p(rn), RESULT, -1, z - 18 + 55);
      break;
    }

    case TCLK: {
      int z = rn2z(rn);
      if(z <= 2)
	return pnode(DSP, rn2p(rn), CLKIN, -1, z);
      else
	return pnode(DSP, rn2p(rn), ACLR, -1, z-3);

    }

    default: break;
    }
  }

  if(tt == T_DSP2) {
    switch(rn2t(rn)) {
    case GOUT: {
      int z = rn2z(rn);
      if(z <= 8)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 0, z);
      if(z <= 17)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 1, z - 9);
      if(z <= 26)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 2, z - 18);
      if(z <= 29)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 3, z - 27);
      if(z == 32)
	return pnode(DSP, rn2p(rn)-1, CLKIN, -1, 5);
      if(z == 33)
	return pnode(DSP, rn2p(rn)-1, ENABLE, -1, 2);
      if(z == 34)
	return pnode(DSP, rn2p(rn)-1, CLKIN, -1, 4);
      if(z == 35)
	return pnode(DSP, rn2p(rn)-1, ENABLE, -1, 1);
      if(z == 36)
	return pnode(DSP, rn2p(rn)-1, CLKIN, -1, 3);
      if(z == 37)
	return pnode(DSP, rn2p(rn)-1, ENABLE, -1, 0);
      if(z >= 38 && z <= 46)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 5, 46 - z);
      if(z >= 47 && z <= 55)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 4, 55 - z);
      if(z >= 56 && z <= 61)
	return pnode(DSP, rn2p(rn)-1, DATAIN, 3, 61 - z + 3);
      return pnode(DSP, rn2p(rn)-1, UNK_IN, -1, rn2z(rn)+64);
    }

    case GIN: {
      int z = rn2z(rn);
      if(z <= 17)
	return pnode(DSP, rn2p(rn)-1, RESULT, -1, z);
      if(z <= 36)
	return pnode(DSP, rn2p(rn)-1, RESULT, -1, z - 18 + 36);
      break;
    }

    default: break;
    }
  }

  if(tt == T_M10K) {
    switch(rn2t(rn)) {
    case GOUT: {
      int z = rn2z(rn);
      if(z < 2)
	return pnode(M10K, rn2p(rn), BYTEENABLEA, -1, z);

      if(z < 5) {
	static const int zt[3] = { 1, 2, 3 };
	return pnode(M10K, rn2p(rn), ADDRA, -1, zt[z-2]);
      }
      if(z < 6)
	return pnode(M10K, rn2p(rn), CLKIN, -1, 6);

      if(z < 7)
	return pnode(M10K, rn2p(rn), ACLR, -1, 0);

      if(z < 8)
	return pnode(M10K, rn2p(rn), ADDRSTALLA, -1, -1);

      if(z < 9)
	return pnode(M10K, rn2p(rn), ADDRSTALLB, -1, -1);

      if(z < 17)
	return pnode(M10K, rn2p(rn), ADDRA, -1, z-9+4);

      if(z < 18)
	return pnode(M10K, rn2p(rn), WREN, -1, 0);

      if(z < 20)
	return pnode(M10K, rn2p(rn), ENABLE, -1, z-18);

      if(z < 21)
	return pnode(M10K, rn2p(rn), RDEN, -1, 0);

      if(z < 23)
	return 0;

      if(z < 25)
	return pnode(M10K, rn2p(rn), BYTEENABLEB, -1, z-23);

      if(z < 28) {
	static const int zt[3] = { 3, 2, 1 };
	return pnode(M10K, rn2p(rn), ADDRB, -1, zt[z-25]);
      }

      if(z < 29)
	return pnode(M10K, rn2p(rn), CLKIN, -1, 7);

      if(z < 30)
	return pnode(M10K, rn2p(rn), ACLR, -1, 1);

      if(z < 38)
	return pnode(M10K, rn2p(rn), ADDRB, -1, 4+(7-(z-30)));

      if(z < 39)
	return pnode(M10K, rn2p(rn), WREN, -1, 1);

      if(z < 41)
	return pnode(M10K, rn2p(rn), ENABLE, -1, z-39+2);

      if(z < 42)
	return pnode(M10K, rn2p(rn), RDEN, -1, 1);

      if(z < 43)
	return pnode(M10K, rn2p(rn), ADDRA, -1, 0);

      if(z < 64) {
	static const int bits[21] = { 0, 20, 10, 30, 1, 21, 11, 31, 2, 22, 12, 32, 3, 23, 13, 33, 4, 24, 14, 34, 5 };
	int bit = bits[z-43];
	return pnode(M10K, rn2p(rn), bit < 20 ? DATAAIN : DATABIN, -1, bit < 20 ? bit : bit-20);
      }

      if(z < 65)
	return pnode(M10K, rn2p(rn), ADDRB, -1, 0);

      if(z < 84) {
	static const int bits[19] = { 39, 19, 29, 9, 38, 18, 28, 8, 37, 17, 27, 7, 36, 16, 26, 6, 35, 15, 25 };
	int bit = bits[z-65];
	return pnode(M10K, rn2p(rn), bit < 20 ? DATAAIN : DATABIN, -1, bit < 20 ? bit : bit-20);
      }

      return 0;
    }

    case GIN: {
      int z = rn2z(rn);
      if(z >= 20)
	return pnode(M10K, rn2p(rn), DATABOUT, -1, z-20);
      else
	return pnode(M10K, rn2p(rn), DATAAOUT, -1, z);
    }

    case TCLK: {
      return pnode(M10K, rn2p(rn), CLKIN, -1, rn2z(rn));
    }

    default: break;
    }
  }

  auto i = r2p_map.find(rn);
  if(i != r2p_map.end())
    return i->second;
    
  return 0;
}

std::vector<mistral::CycloneV::pnode_t> mistral::CycloneV::p2p_from(pnode_t pn) const
{
  std::vector<pnode_t> res;
  for(const p2p_info *p2p = di.p2p; p2p->s; p2p++)
    if(p2p->s == pn)
      res.push_back(p2p->d);
  return res;
}

mistral::CycloneV::pnode_t mistral::CycloneV::p2p_to(pnode_t pn) const
{
  for(const p2p_info *p2p = di.p2p; p2p->s; p2p++)
    if(p2p->d == pn)
      return p2p->s;
  return 0;
}

mistral::CycloneV::pnode_t mistral::CycloneV::hmc_get_bypass(pnode_t pn) const
{
  if(pn2bt(pn) != HMC)
    return 0;

  port_type_t npt;

  switch(pn2pt(pn)) {
  case PHYDDIOADDRACLR: npt = IOINTADDRACLR; break;
  case IOINTADDRACLR: npt = PHYDDIOADDRACLR; break;
  case PHYDDIOADDRDOUT: npt = IOINTADDRDOUT; break;
  case IOINTADDRDOUT: npt = PHYDDIOADDRDOUT; break;
  case PHYDDIOBAACLR: npt = IOINTBAACLR; break;
  case IOINTBAACLR: npt = PHYDDIOBAACLR; break;
  case PHYDDIOBADOUT: npt = IOINTBADOUT; break;
  case IOINTBADOUT: npt = PHYDDIOBADOUT; break;
  case PHYDDIOCASNACLR: npt = IOINTCASNACLR; break;
  case IOINTCASNACLR: npt = PHYDDIOCASNACLR; break;
  case PHYDDIOCASNDOUT: npt = IOINTCASNDOUT; break;
  case IOINTCASNDOUT: npt = PHYDDIOCASNDOUT; break;
  case PHYDDIOCKDOUT: npt = IOINTCKDOUT; break;
  case IOINTCKDOUT: npt = PHYDDIOCKDOUT; break;
  case PHYDDIOCKEACLR: npt = IOINTCKEACLR; break;
  case IOINTCKEACLR: npt = PHYDDIOCKEACLR; break;
  case PHYDDIOCKEDOUT: npt = IOINTCKEDOUT; break;
  case IOINTCKEDOUT: npt = PHYDDIOCKEDOUT; break;
  case PHYDDIOCKNDOUT: npt = IOINTCKNDOUT; break;
  case IOINTCKNDOUT: npt = PHYDDIOCKNDOUT; break;
  case PHYDDIOCSNACLR: npt = IOINTCSNACLR; break;
  case IOINTCSNACLR: npt = PHYDDIOCSNACLR; break;
  case PHYDDIOCSNDOUT: npt = IOINTCSNDOUT; break;
  case IOINTCSNDOUT: npt = PHYDDIOCSNDOUT; break;
  case PHYDDIODMDOUT: npt = IOINTDMDOUT; break;
  case IOINTDMDOUT: npt = PHYDDIODMDOUT; break;
  case PHYDDIODQDOUT: npt = IOINTDQDOUT; break;
  case IOINTDQDOUT: npt = PHYDDIODQDOUT; break;
  case PHYDDIODQOE: npt = IOINTDQOE; break;
  case IOINTDQOE: npt = PHYDDIODQOE; break;
  case PHYDDIODQSBDOUT: npt = IOINTDQSBDOUT; break;
  case IOINTDQSBDOUT: npt = PHYDDIODQSBDOUT; break;
  case PHYDDIODQSBOE: npt = IOINTDQSBOE; break;
  case IOINTDQSBOE: npt = PHYDDIODQSBOE; break;
  case PHYDDIODQSDOUT: npt = IOINTDQSDOUT; break;
  case IOINTDQSDOUT: npt = PHYDDIODQSDOUT; break;
  case PHYDDIODQSOE: npt = IOINTDQSOE; break;
  case IOINTDQSOE: npt = PHYDDIODQSOE; break;
  case PHYDDIOODTACLR: npt = IOINTODTACLR; break;
  case IOINTODTACLR: npt = PHYDDIOODTACLR; break;
  case PHYDDIOODTDOUT: npt = IOINTODTDOUT; break;
  case IOINTODTDOUT: npt = PHYDDIOODTDOUT; break;
  case PHYDDIORASNACLR: npt = IOINTRASNACLR; break;
  case IOINTRASNACLR: npt = PHYDDIORASNACLR; break;
  case PHYDDIORASNDOUT: npt = IOINTRASNDOUT; break;
  case IOINTRASNDOUT: npt = PHYDDIORASNDOUT; break;
  case PHYDDIORESETNACLR: npt = IOINTRESETNACLR; break;
  case IOINTRESETNACLR: npt = PHYDDIORESETNACLR; break;
  case PHYDDIORESETNDOUT: npt = IOINTRESETNDOUT; break;
  case IOINTRESETNDOUT: npt = PHYDDIORESETNDOUT; break;
  case PHYDDIOWENACLR: npt = IOINTWENACLR; break;
  case IOINTWENACLR: npt = PHYDDIOWENACLR; break;
  case PHYDDIOWENDOUT: npt = IOINTWENDOUT; break;
  case IOINTWENDOUT: npt = PHYDDIOWENDOUT; break;
  case DDIOPHYDQDIN: npt = IOINTDQDIN; break;
  case IOINTDQDIN: npt = DDIOPHYDQDIN; break;
  default: return 0;
  }

  return pnode(HMC, pn2p(pn), npt, pn2bi(pn), pn2pi(pn));
}

