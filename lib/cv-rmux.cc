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


void mistral::CycloneV::rmux_load(const std::string &mistral_root)
{
  char path[2048];
  char msg[4096];
  sprintf(path, "%s/gdata/%s-r.bin", mistral_root.c_str(), di.name);
  sprintf(msg, "Error opening %s for reading", path);
  FILE *fd = fopen(path, "rb");
  if(!fd) {
    perror(msg);
    exit(1);
  }

  fseek(fd, 0, SEEK_END);
  int size = ftell(fd);
  rewind(fd);
  auto cdata = std::make_unique<uint8_t []>(size);
  fread(cdata.get(), size, 1, fd);
  fclose(fd);

  rmux_info = std::make_unique<rmux []>(di.rmux_count);
  lzma_stream strm = LZMA_STREAM_INIT;
  lzma_ret ret;
  if ((ret = lzma_stream_decoder(&strm, UINT64_MAX, 0)) != LZMA_OK) {
    fprintf(stderr, "failed to initialise liblzma: %d\n", ret);
    exit(1);
  }

  strm.next_in = cdata.get();
  strm.avail_in = size;
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
      return pi >= 64 ? rnode(GOUT, p+1, pi-64) : rnode(GOUT, p, pi);
    case DATAOUT:
      return pi >= 37 ? rnode(GIN, p+1, pi-37) : rnode(GIN, p, pi);
    case CLKIN:
      return rnode(TCLK, p, pi);
    default:
      break;
    }
    break;
  }

  case M10K: {
    switch(pn2pt(pn)) {
    case DATAIN:
      return rnode(GOUT, p, pi);
    case DATAOUT:
      return rnode(GIN, p, pi);
    case CLKIN:
      return rnode(TCLK, p, pi);
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
      return pnode(DSP, rn2p(rn), DATAIN, -1, rn2z(rn));
    }

    case GIN: {
      return pnode(DSP, rn2p(rn), DATAOUT, -1, rn2z(rn));
    }

    case TCLK: {
      return pnode(DSP, rn2p(rn), CLKIN, -1, rn2z(rn));
    }

    default: break;
    }
  }

  if(tt == T_DSP2) {
    switch(rn2t(rn)) {
    case GOUT: {
      return pnode(DSP, rn2p(rn)-1, DATAIN, -1, rn2z(rn)+64);
    }

    case GIN: {
      return pnode(DSP, rn2p(rn)-1, DATAOUT, -1, rn2z(rn)+37);
    }

    default: break;
    }
  }

  if(tt == T_M10K) {
    switch(rn2t(rn)) {
    case GOUT: {
      return pnode(M10K, rn2p(rn), DATAIN, -1, rn2z(rn));
    }

    case GIN: {
      return pnode(M10K, rn2p(rn), DATAOUT, -1, rn2z(rn));
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
