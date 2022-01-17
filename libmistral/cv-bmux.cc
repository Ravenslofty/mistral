#include "cyclonev.h"

void mistral::CycloneV::add_pram_fixed(std::vector<pos_t> &pos, block_type_t block, int start, int count)
{
  for(int i=0; i != count; i++)
    if(di.fixed_blocks[start+i].pos != 0xffff) {
      pos_t p = di.fixed_blocks[start+i].pos;
      pos.push_back(p);
      tile_bels[p].push_back(block);
      if(block == CMUXVG)
	tile_bels[p].push_back(CMUXVR);
      if(block == CMUXHG)
	tile_bels[p].push_back(CMUXHR);
    }
}


uint32_t mistral::CycloneV::find_pram_fixed(pos_t p, int start, int count) const
{
  for(int i=0; i != count; i++)
    if(di.fixed_blocks[start+i].pos == p)
      return di.fixed_blocks[start+i].pram;
  return 0xffffffff;
}

void mistral::CycloneV::add_pram_blocks()
{
  for(uint32_t i = 0; i != di.ioblocks_count; i++)
    if(di.ioblocks[i].idx == 0 && di.ioblocks[i].btype == GPIO)
      gpio_pos.push_back(di.ioblocks[i].pos);

  for(uint32_t i = 0; i != di.dqs16_count; i++)
    dqs16_pos.push_back(di.dqs16s[i].pos);

  add_pram_fixed(fpll_pos,   FPLL,   FB_FPLL,    8);
  add_pram_fixed(cmuxc_pos,  CMUXCR, FB_CMUXC,   4);
  add_pram_fixed(cmuxv_pos,  CMUXVG, FB_CMUXV,   2);
  add_pram_fixed(cmuxh_pos,  CMUXHG, FB_CMUXH,   2);
  add_pram_fixed(dll_pos,    DLL,    FB_DLL,     4);
  add_pram_fixed(hssi_pos,   HSSI,   FB_HSSI,    4);
  add_pram_fixed(cbuf_pos,   CBUF,   FB_CBUF,    8);
  add_pram_fixed(lvl_pos,    LVL,    FB_LVL,    17);
  add_pram_fixed(pma3_pos,   PMA3,   FB_PMA3,    4);
  add_pram_fixed(serpar_pos, SERPAR, FB_SERPAR, 10);
  add_pram_fixed(term_pos,   TERM,   FB_TERM,    4);
  add_pram_fixed(hip_pos,    HIP,    FB_HIP,     2);
  add_pram_fixed(hmc_pos,    HMC,    FB_HMC,     2);
}

uint32_t mistral::CycloneV::fpll2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_FPLL,    8);
}

uint32_t mistral::CycloneV::cmuxc2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_CMUXC,   4);
}

uint32_t mistral::CycloneV::cmuxv2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_CMUXV,   2);
}

uint32_t mistral::CycloneV::cmuxh2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_CMUXH,   2);
}

uint32_t mistral::CycloneV::dll2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_DLL,     4);
}

uint32_t mistral::CycloneV::hssi2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_HSSI,    4);
}

uint32_t mistral::CycloneV::cbuf2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_CBUF,    8);
}

uint32_t mistral::CycloneV::lvl2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_LVL,    17);
}

uint32_t mistral::CycloneV::pma32pram(pos_t p) const
{
  return find_pram_fixed(p, FB_PMA3,    4);
}

uint32_t mistral::CycloneV::serpar2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_SERPAR, 10);
}

uint32_t mistral::CycloneV::term2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_TERM,    4);
}

uint32_t mistral::CycloneV::hip2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_HIP,     2);
}

uint32_t mistral::CycloneV::hmc2pram(pos_t p) const
{
  return find_pram_fixed(p, FB_HMC,     2);
}

void mistral::CycloneV::bmux_dqs16_adjust(uint32_t &pos, uint32_t offset, bool up)
{
  if(up)
    pos += offset >= 768 ? 3*191 : offset >= 512 ? 2*191 : offset >= 256 ? 191 : 0;
  else
    pos -= offset >= 768 ? 3*191 + 256*5 + 836 : offset >= 512 ? 2*191 + 256*4 : offset >= 256 ? 191 + 256*2 : 0;
}

void mistral::CycloneV::bmux_b_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const
{
  switch(btype) {
  case GPIO:
    if(mux->mux == USE_WEAK_PULLUP) {
      if(pin_find_pos(pos, idx))
	def = 1;
      else
	def = 0;
    }
    break;

  case FPLL:
    def = (base & (1 << 24)) ? 1 : 0;
    break;

  case TERM:
    def = (base & (1 << 24)) ? 0 : 1;
    break;

  case PMA3:
    def = (base >> 29) & 1;
    break;

  case DQS16: {
    int type = (base >> 24) & 1;
    if(mux->mux == RB_2X_CLK_DQS_INV || mux->mux == RB_ACLR_LFIFO_EN)
      def = type ? 1 : 0;
    if(mux->mux == RB_LFIFO_BYPASS)
      def = type ? 0 : 1;
    break;
  }

  default: break;
  }
}

void mistral::CycloneV::bmux_m_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const
{
  switch(btype) {
  case GPIO:
    if(mux->mux == DRIVE_STRENGTH) {
      if(pin_find_pos(pos, idx))
	def = OFF;
      else
	def = PROG_GND;
    }
    if(mux->mux == IOCSR_STD) {
      for(uint32_t i = 0; i != di.ioblocks_count; i++)
	if(di.ioblocks[i].pos == pos && di.ioblocks[i].idx == idx && di.ioblocks[i].btype == GPIO) {
	  def = (di.ioblocks[i].pram & (1 << 25)) ? NVR_LOW : NVR_HIGH;
	  break;
	}
    }
    break;

  case PMA3:
    if(mux->mux == FFPLL_IQCLK_DIRECTION) {
      bmux_type_t bdef[6] = { UP, UP, DOWN, UP, TRISTATE, TRISTATE };
      def = bdef[((base >> 26) & 3) * 2 + idx];
    }
    if(mux->mux == HCLK_TOP_OUT_DRIVER)
      def = ((base >> 28) & 1 ) ? DOWN_EN : TRISTATE;
    break;

  case DQS16: {
    if(mux->mux == INPUT_REG4_SEL)
      def = pos == di.packages[model->package]->dqs_pos && idx == di.packages[model->package]->dqs_index ? SEL_LOCKED_DPA : SEL_BYPASS;
    break;
  }

  default: break;
  }
}

void mistral::CycloneV::bmux_r_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const
{
  switch(btype) {
  case HSSI: {
    static const uint16_t hssi_defvals[3][3] = { { 0x096, 0x226, 0x3b6 }, { 0x0fa, 0x28a, 0x41a }, { 0x064, 0x1f4, 0x384 } };
    if(mux->mux == PCS8G_BASE_ADDR)
      def = hssi_defvals[0][idx];
    else if(mux->mux == PLD_PCS_IF_BASE_ADDR)
      def = hssi_defvals[1][idx];
    else if(mux->mux == PMA_PCS_IF_BASE_ADDR)
      def = hssi_defvals[2][idx];
    break;
  }

  case PMA3: {
    static const uint16_t pma3_defvals[3] = { 0x000, 0x190, 0x320 };
    if(mux->mux == DPRIO_REG_PLD_PMA_IF_BADDR)
      def = pma3_defvals[idx];
    break;
  }

  case DQS16: {
    if(mux->mux == RB_T9_SEL_EREG_CFF_DELAY)
      def = pos == di.packages[model->package]->dqs_pos && idx == di.packages[model->package]->dqs_index ? 0x1f : 0x00;
    break;
  }

  default: break;
  }
}

uint64_t mistral::CycloneV::bmux_val_read(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const
{
  uint64_t val = 0;
  switch(mode) {
  case BM_CRAM: {
    const uint16_t *bt = bmux_cram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
      if((cram[pos >> 3] >> (pos & 7)) & 1)
	val |= uint64_t(1) << b;
      bt += 2;
    }
    break;
  }
  case BM_PRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    auto &prami = pram[(base >> 16) & 31];
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = (base & 0xffff) + bt[0];
      if(base & (1<<23))
	bmux_dqs16_adjust(pos, bt[0], base & (1 << 22));
      if(prami[pos])
	val |= uint64_t(1) << b;
      bt ++;
    }
    break;
  }
  case BM_ORAM: {
    const uint16_t *bt = bmux_oram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      if((oram[bt[0]] >> bt[1]) & 1)
	val |= uint64_t(1) << b;
      bt += 2;
    }
    break;
  }
  case BM_DCRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
      if((cram[pos >> 3] >> (pos & 7)) & 1)
	val |= uint64_t(1) << b;
      bt ++;
    }
    break;
  }
  }
  return val;
}

void mistral::CycloneV::bmux_val_set(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, uint64_t val)
{
  switch(mode) {
  case BM_CRAM: {
    const uint16_t *bt = bmux_cram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
      if((b < 64) && (val >> b) & 1)
	cram[pos >> 3] |= 1 << (pos & 7);
      else
	cram[pos >> 3] &= ~(1 << (pos & 7));
      bt += 2;
    }
    break;
  }
  case BM_PRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    auto &prami = pram[(base >> 16) & 31];
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = (base & 0xffff) + bt[0];
      if(base & (1<<23))
	bmux_dqs16_adjust(pos, bt[0], base & (1 << 22));
      prami[pos] = b >= 64 ? 0 : (val >> b) & 1;
      bt ++;
    }
    break;
  }
  case BM_ORAM: {
    const uint16_t *bt = bmux_oram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      if((val >> b) & 1)
	oram[bt[0]] |= uint64_t(1) << bt[1];
      else
	oram[bt[0]] &= ~(uint64_t(1) << bt[1]);
      bt += 2;
    }
    break;
  }
  case BM_DCRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
      if((b < 64) && (val >> b) & 1)
	cram[pos >> 3] |= 1 << (pos & 7);
      else
	cram[pos >> 3] &= ~(1 << (pos & 7));
      bt ++;
    }
    break;
  }
  }
}

void mistral::CycloneV::bmux_val_set(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, const std::vector<uint8_t> &val)
{
  switch(mode) {
  case BM_CRAM: {
    const uint16_t *bt = bmux_cram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
      if((val[b >> 3] >> (b & 7)) & 1)
	cram[pos >> 3] |= 1 << (pos & 7);
      else
	cram[pos >> 3] &= ~(1 << (pos & 7));
      bt += 2;
    }
    break;
  }
  case BM_PRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    auto &prami = pram[(base >> 16) & 31];
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = (base & 0xffff) + bt[0];
      if(base & (1<<23))
	bmux_dqs16_adjust(pos, bt[0], base & (1 << 22));
      prami[pos] = (val[b >> 3] >> (b & 7)) & 1;
      bt ++;
    }
    break;
  }
  case BM_ORAM: {
    const uint16_t *bt = bmux_oram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      if((val[b >> 3] >> (b & 7)) & 1)
	oram[bt[0]] |= uint64_t(1) << bt[1];
      else
	oram[bt[0]] &= ~(uint64_t(1) << bt[1]);
      bt += 2;
    }
    break;
  }
  case BM_DCRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
      if((val[b >> 3] >> (b & 7)) & 1)
	cram[pos >> 3] |= 1 << (pos & 7);
      else
	cram[pos >> 3] &= ~(1 << (pos & 7));
      bt ++;
    }
    break;
  }
  }
}

std::pair<mistral::CycloneV::bmux_type_t, bool> mistral::CycloneV::bmux_m_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const
{
  uint32_t val = bmux_val_read(base, mux, idx, mode);
  const bmux_sel_entry *sel = bmux_sel_entries + mux->entries_offset;
  for(uint8_t m = 0; m != mux->entries; m++)
    if(sel[m].mask == val) {
      auto v = sel[m].sel;
      int def = mux->def;
      if(def == -2) {
	bmux_m_solve_default(btype, pos, idx, mux, base, def);
	if(def != -2)
	  return std::make_pair(v, v == def);
	else
	  return std::make_pair(v, false);
      }
      return std::make_pair(v, m == def);
    }
  return std::make_pair(BMNONE, false);
}

std::pair<int, bool> mistral::CycloneV::bmux_n_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const
{
  uint32_t val = bmux_val_read(base, mux, idx, mode);
  const bmux_num_entry *sel = bmux_num_entries + mux->entries_offset;
  for(uint8_t m = 0; m != mux->entries; m++)
    if(sel[m].mask == val)
      return std::make_pair(sel[m].num, mux->def != -2 && m == mux->def);
  return std::make_pair(-1, false);
}

std::pair<bool, bool> mistral::CycloneV::bmux_b_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const
{
  bool r = bmux_val_read(base, mux, idx, mode);
  int def = mux->def;
  if(def == -2)
    bmux_b_solve_default(btype, pos, idx, mux, base, def);
  return std::make_pair(r, r == def);
}

bool mistral::CycloneV::bmux_r_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, std::vector<uint8_t> &r) const
{
  r.clear();
  r.resize((mux->bits+7) >> 3, 0);
  bool all_0 = true, all_1 = true;
  switch(mode) {
  case BM_CRAM: {
    const uint16_t *bt = bmux_cram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
      if((cram[pos >> 3] >> (pos & 7)) & 1) {
	all_0 = false;
	r[b >> 3] |= 1 << (b & 7);
      } else
	all_1 = false;
      bt += 2;
    }
    break;
  }
  case BM_PRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    auto &prami = pram[(base >> 16) & 31];
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = (base & 0xffff) + bt[0];
      if(base & (1<<23))
	bmux_dqs16_adjust(pos, bt[0], base & (1 << 22));
      if(prami[pos]) {
	all_0 = false;
	r[b >> 3] |= 1 << (b & 7);
      } else
	all_1 = false;
      bt ++;
    }
    break;
  }
  case BM_ORAM: {
    const uint16_t *bt = bmux_oram_bpos + 2*(mux->bit_offset + idx * mux->bits);
    for(uint8_t b = 0; b != mux->bits; b++) {
      if((oram[bt[0]] >> bt[1]) & 1) {
	all_0 = false;
	r[b >> 3] |= 1 << (b & 7);
      } else
	all_1 = false;
      bt += 2;
    }
    break;
  }
  case BM_DCRAM: {
    const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
    for(uint8_t b = 0; b != mux->bits; b++) {
      uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
      if((cram[pos >> 3] >> (pos & 7)) & 1) {
	all_0 = false;
	r[b >> 3] |= 1 << (b & 7);
      } else
	all_1 = false;
      bt ++;
    }
    break;
  }
  }

  int def = mux->def;

  if(def == -2)
    bmux_r_solve_default(btype, pos, idx, mux, base, def);

  if(def == -2)
    return false;
  if(def == -1)
    return all_1;
  if(def == 0)
    return all_0;
  for(unsigned int p = 3; p < r.size(); p++)
    if(r[p] != 0)
      return false;
  if(r[0] != (def & 0xff))
    return false;
  if(r.size() > 1 && (r[1] != ((def >> 8) & 0xff)))
    return false;
  if(r.size() > 2 && (r[2] != ((def >> 16) & 0xff)))
    return false;
  return true;
}

void mistral::CycloneV::bmux_get_any(block_type_t btype, pos_t pos, uint32_t base, const bmux *muxes, bmux_ram_t mode, std::vector<bmux_setting_t> &res, int variant) const
{
  while(muxes->mux) {
    if(muxes->variant == -1 || muxes->variant == variant) {
      bmux_setting_t bms;
      bms.btype = btype;
      bms.pos = pos;
      bms.mux = muxes->mux;
      bms.type = muxes->stype;
      int span = muxes->span;
      for(int i=0; i != span; i++) {
	bms.midx = span == 1 ? -1 : i;
	switch(muxes->stype) {
	case MT_MUX: {
	  auto r = bmux_m_read(btype, pos, base, muxes, i, mode);
	  bms.s = r.first;
	  bms.def = r.second;
	  res.push_back(bms);
	  break;
	}
	case MT_NUM: {
	  auto r = bmux_n_read(btype, pos, base, muxes, i, mode);
	  bms.s = r.first;
	  bms.def = r.second;
	  res.push_back(bms);
	  break;
	}
	case MT_BOOL: {
	  auto r = bmux_b_read(btype, pos, base, muxes, i, mode);
	  bms.s = r.first;
	  bms.def = r.second;
	  res.push_back(bms);
	  break;
	}
	case MT_RAM: {
	  bms.s = muxes->bits;
	  bms.def = bmux_r_read(btype, pos, base, muxes, i, mode, bms.r);
	  res.push_back(bms);
	  break;
	}
	}
      }
    }
    muxes++;
  }
}

void mistral::CycloneV::bmux_set_default(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, bmux_ram_t mode, int variant)
{
  while(mux->mux) {
    if(mux->variant == -1 || mux->variant == variant) {
      int span = mux->span;
      for(int idx=0; idx != span; idx++) {
	switch(mux->stype) {
	case MT_MUX: {
	  int def = mux->def;
	  if(def == -2) {
	    bmux_m_solve_default(btype, pos, idx, mux, base, def);
	    if(def != -2) {
	      int want = def;
	      const bmux_sel_entry *sel = bmux_sel_entries + mux->entries_offset;
	      for(def = 0; sel[def].sel != want; def++);
	    }
	  }
	  if(def != -2) {
	    const bmux_sel_entry *sel = bmux_sel_entries + mux->entries_offset;
	    bmux_val_set(base, mux, idx, mode, sel[def].mask);
	  }
	  break;
	}
	  
	case MT_NUM:
	  if(mux->def != -2) {
	    const bmux_num_entry *sel = bmux_num_entries + mux->entries_offset;
	    bmux_val_set(base, mux, idx, mode, sel[mux->def].mask);
	  }
	  break;

	case MT_BOOL: {
	  int def = mux->def;
	  if(def == -2)
	    bmux_b_solve_default(btype, pos, idx, mux, base, def);
	  if(def != -2)
	    bmux_val_set(base, mux, idx, mode, def);
	  break;
	}
	  
	case MT_RAM: {
	  int def = mux->def;
	  if(def == -2)
	    bmux_r_solve_default(btype, pos, idx, mux, base, def);
	  
	  if(def == -2)
	    break;

	  if(def <= 0) {
	    switch(mode) {
	    case BM_CRAM: {
	      const uint16_t *bt = bmux_cram_bpos + 2*(mux->bit_offset + idx * mux->bits);
	      if(def) {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
		  cram[pos >> 3] |= 1 << (pos & 7);
		  bt += 2;
		}
	      } else {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  uint32_t pos = base + bt[1] * di.cram_sx + bt[0];
		  cram[pos >> 3] &= ~(1 << (pos & 7));
		  bt += 2;
		}
	      }
	      break;
	    }
	    case BM_PRAM: {
	      const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
	      auto &prami = pram[(base >> 16) & 31];
	      bool v = def != 0;
	      for(uint8_t b = 0; b != mux->bits; b++) {
		uint32_t pos = (base & 0xffff) + bt[0];
		if(base & (1<<23))
		  bmux_dqs16_adjust(pos, bt[0], base & (1 << 22));
		prami[pos] = v;
		bt ++;
	      }
	      break;
	    }
	    case BM_ORAM: {
	      const uint16_t *bt = bmux_oram_bpos + 2*(mux->bit_offset + idx * mux->bits);
	      if(def) {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  oram[bt[0]] |= uint64_t(1) << bt[1];
		  bt += 2;
		}
	      } else {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  oram[bt[0]] &= ~(uint64_t(1) << bt[1]);
		  bt += 2;
		}
	      }
	      break;
	    }
	    case BM_DCRAM: {
	      const uint16_t *bt = bmux_pram_bpos + mux->bit_offset + idx * mux->bits;
	      if(def) {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
		  cram[pos >> 3] |= 1 << (pos & 7);
		  bt ++;
		}
	      } else {
		for(uint8_t b = 0; b != mux->bits; b++) {
		  uint32_t pos = di.dcram_pos[(base & 0xffff) + bt[0]].y * di.cram_sx + di.dcram_pos[(base & 0xffff) + bt[0]].x;
		  cram[pos >> 3] &= ~(1 << (pos & 7));
		  bt ++;
		}
	      }
	      break;
	    }
	    }
	    break;
	  }
	  bmux_val_set(base, mux, idx, mode, def);
	  break;
	}
	}
      }
    }
    mux++;
  }
}

std::vector<mistral::CycloneV::bmux_setting_t> mistral::CycloneV::bmux_get() const
{
  std::vector<bmux_setting_t> res;
  for(pos_t p : lab_pos)
    bmux_get_any(LAB, p, pos2bit(p), bm_lab, BM_CRAM, res);
  for(pos_t p : mlab_pos)
    bmux_get_any(MLAB, p, pos2bit(p), bm_mlab, BM_CRAM, res);
  for(pos_t p : m10k_pos)
    bmux_get_any(M10K, p, pos2bit(p), bm_m10k, BM_CRAM, res);
  for(pos_t p : dsp_pos)
    bmux_get_any(DSP, p, pos2bit(p), bm_dsp, BM_CRAM, res);
  if(di.hps_blocks)
    bmux_get_any(HPS_CLOCKS, di.hps_blocks[I_HPS_CLOCKS], pos2bit(di.hps_blocks[I_HPS_CLOCKS]), bm_hps_clocks, BM_CRAM, res);

  for(uint32_t i = 0; i != di.ioblocks_count; i++)
    if(di.ioblocks[i].idx == 0 && di.ioblocks[i].btype == GPIO)
      bmux_get_any(GPIO, di.ioblocks[i].pos, di.ioblocks[i].pram, bm_gpio, BM_PRAM, res, (di.ioblocks[i].pram & (1 << 24)) ? 1 : 0);

  for(uint32_t i = 0; i != di.dqs16_count; i++)
    bmux_get_any(DQS16, di.dqs16s[i].pos, di.dqs16s[i].pram, bm_dqs16, BM_PRAM, res);

  for(pos_t p : fpll_pos)
    bmux_get_any(FPLL,   p, fpll2pram(p),   bm_fpll, BM_PRAM, res);
  for(pos_t p : cmuxc_pos)
    bmux_get_any(CMUXCR, p, cmuxc2pram(p),  bm_cmuxcr, BM_PRAM, res);
  for(pos_t p : cmuxv_pos)
    bmux_get_any(CMUXVR, p, cmuxv2pram(p),  bm_cmuxvr, BM_PRAM, res);
  for(pos_t p : cmuxv_pos)
    bmux_get_any(CMUXVG, p, cmuxv2pram(p),  bm_cmuxvg, BM_PRAM, res);
  for(pos_t p : cmuxh_pos)
    bmux_get_any(CMUXHR, p, cmuxh2pram(p),  bm_cmuxhr, BM_PRAM, res);
  for(pos_t p : cmuxh_pos)
    bmux_get_any(CMUXHG, p, cmuxh2pram(p),  bm_cmuxhg, BM_PRAM, res);
  for(pos_t p : dll_pos)
    bmux_get_any(DLL,    p, dll2pram(p),    bm_dll,    BM_PRAM, res);
  for(pos_t p : hssi_pos)
    bmux_get_any(HSSI,   p, hssi2pram(p),   bm_hssi,   BM_PRAM, res);
  for(pos_t p : cbuf_pos)
    bmux_get_any(CBUF,   p, cbuf2pram(p),   bm_cbuf,   BM_PRAM, res);
  for(pos_t p : lvl_pos)
    bmux_get_any(LVL,    p, lvl2pram(p),    bm_lvl,    BM_PRAM, res);
  for(pos_t p : pma3_pos) {
    uint32_t pr = pma32pram(p);
    bmux_get_any(PMA3,   p, pr,             bm_pma3,   BM_PRAM, res, (pr >> 24) & 1);
    bmux_get_any(PMA3,   p, 18*((pr >> 24) & 3), bm_pma3c,  BM_DCRAM, res);
  }
  for(pos_t p : serpar_pos)
    bmux_get_any(SERPAR, p, serpar2pram(p), bm_serpar, BM_PRAM, res);
  for(pos_t p : term_pos)
    bmux_get_any(TERM,   p, term2pram(p),   bm_term,   BM_PRAM, res);
  for(pos_t p : hip_pos)
    bmux_get_any(HIP,    p, hip2pram(p),    bm_hip,    BM_PRAM, res);
  for(pos_t p : hmc_pos)
    bmux_get_any(HMC,    p, hmc2pram(p),    bm_hmc,    BM_PRAM, res);
  return res;
}

void mistral::CycloneV::bmux_set_defaults()
{
  for(pos_t p : lab_pos)
    bmux_set_default(LAB,  p, pos2bit(p), bm_lab,  BM_CRAM);
  for(pos_t p : mlab_pos)
    bmux_set_default(MLAB, p, pos2bit(p), bm_mlab, BM_CRAM);
  for(pos_t p : m10k_pos)
    bmux_set_default(M10K, p, pos2bit(p), bm_m10k, BM_CRAM);
  for(pos_t p : dsp_pos)
    bmux_set_default(DSP,  p, pos2bit(p), bm_dsp,  BM_CRAM);
  if(di.hps_blocks)
    bmux_set_default(HPS_CLOCKS, di.hps_blocks[I_HPS_CLOCKS], pos2bit(di.hps_blocks[I_HPS_CLOCKS]), bm_hps_clocks, BM_CRAM);

  for(uint32_t i = 0; i != di.ioblocks_count; i++)
    if(di.ioblocks[i].idx == 0 && di.ioblocks[i].btype == GPIO)
      bmux_set_default(GPIO, di.ioblocks[i].pos, di.ioblocks[i].pram, bm_gpio, BM_PRAM, (di.ioblocks[i].pram & (1 << 24)) ? 1 : 0);

  for(uint32_t i = 0; i != di.dqs16_count; i++)
    bmux_set_default(DQS16, di.dqs16s[i].pos, di.dqs16s[i].pram, bm_dqs16, BM_PRAM);

  for(pos_t p : fpll_pos)
    bmux_set_default(FPLL,   p, fpll2pram(p),   bm_fpll,   BM_PRAM);
  for(pos_t p : cmuxc_pos)
    bmux_set_default(CMUXCR, p, cmuxc2pram(p),  bm_cmuxcr, BM_PRAM);
  for(pos_t p : cmuxv_pos)
    bmux_set_default(CMUXVR, p, cmuxv2pram(p),  bm_cmuxvr, BM_PRAM);
  for(pos_t p : cmuxv_pos)
    bmux_set_default(CMUXVG, p, cmuxv2pram(p),  bm_cmuxvg, BM_PRAM);
  for(pos_t p : cmuxh_pos)
    bmux_set_default(CMUXHR, p, cmuxh2pram(p),  bm_cmuxhr, BM_PRAM);
  for(pos_t p : cmuxh_pos)
    bmux_set_default(CMUXHG, p, cmuxh2pram(p),  bm_cmuxhg, BM_PRAM);
  for(pos_t p : dll_pos)
    bmux_set_default(DLL,    p, dll2pram(p),    bm_dll,    BM_PRAM);
  for(pos_t p : hssi_pos)
    bmux_set_default(HSSI,   p, hssi2pram(p),   bm_hssi,   BM_PRAM);
  for(pos_t p : cbuf_pos)
    bmux_set_default(CBUF,   p, cbuf2pram(p),   bm_cbuf,   BM_PRAM);
  for(pos_t p : lvl_pos)
    bmux_set_default(LVL,    p, lvl2pram(p),    bm_lvl,    BM_PRAM);
  for(pos_t p : pma3_pos) {
    uint32_t pr = pma32pram(p);
    bmux_set_default(PMA3,   p, pr,             bm_pma3,   BM_PRAM, (pr >> 24) & 1);
    bmux_set_default(PMA3,   p, 18*((pr >> 24) & 3), bm_pma3c,  BM_DCRAM);
  }
  for(pos_t p : serpar_pos)
    bmux_set_default(SERPAR, p, serpar2pram(p), bm_serpar, BM_PRAM);
  for(pos_t p : term_pos)
    bmux_set_default(TERM,   p, term2pram(p),   bm_term,   BM_PRAM);
  for(pos_t p : hip_pos)
    bmux_set_default(HIP,    p, hip2pram(p),    bm_hip,    BM_PRAM);
  for(pos_t p : hmc_pos)
    bmux_set_default(HMC,    p, hmc2pram(p),    bm_hmc,    BM_PRAM);
}

const mistral::CycloneV::bmux *mistral::CycloneV::bmux_find(const bmux *pmux, bmux_type_t mux, int variant) const
{
  while(pmux->mux) {
    if(pmux->mux == mux && (pmux->variant == -1 || pmux->variant == variant))
      return pmux;
    pmux ++;
  }
  return nullptr;
}

void mistral::CycloneV::bmux_find(block_type_t btype, pos_t pos, bmux_type_t mux, uint32_t &base, const bmux *&pmux, bmux_ram_t &mode) const
{
  base = 0;
  pmux = nullptr;
  mode = BM_CRAM;
  switch(btype) {
  case LAB:
    if(tile_types[pos] != T_LAB)
      break;
    base = pos2bit(pos);
    pmux = bmux_find(bm_lab, mux);
    mode = BM_CRAM;
    break;

  case MLAB:
    if(tile_types[pos] != T_MLAB)
      break;
    base = pos2bit(pos);
    pmux = bmux_find(bm_mlab, mux);
    mode = BM_CRAM;
    break;

  case M10K:
    if(tile_types[pos] != T_M10K)
      break;
    base = pos2bit(pos);
    pmux = bmux_find(bm_m10k, mux);
    mode = BM_CRAM;
    break;

  case DSP:
    if(tile_types[pos] != T_DSP)
      break;
    base = pos2bit(pos);
    pmux = bmux_find(bm_dsp, mux);
    mode = BM_CRAM;
    break;

  case HPS_CLOCKS:
    if(!di.hps_blocks || pos != di.hps_blocks[I_HPS_CLOCKS])
      break;
    base = pos2bit(pos);
    pmux = bmux_find(bm_hps_clocks, mux);
    mode = BM_CRAM;
    break;

  case GPIO:
    for(uint32_t i = 0; i != di.ioblocks_count; i++)
      if(di.ioblocks[i].pos == pos) {
	base = di.ioblocks[i].pram;
	pmux = bmux_find(bm_gpio, mux, (di.ioblocks[i].pram & (1 << 24)) ? 1 : 0);
	mode = BM_PRAM;
	break;
      }
    break;

  case DQS16:
    for(uint32_t i = 0; i != di.dqs16_count; i++)
      if(di.dqs16s[i].pos == pos) {
	base = di.dqs16s[i].pram;
	pmux = bmux_find(bm_dqs16, mux);
	mode = BM_PRAM;
	break;
      }
    break;

  case FPLL:
    base = fpll2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_fpll, mux);
    mode = BM_PRAM;
    break;

  case CMUXCR:
    base = cmuxc2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cmuxcr, mux);
    mode = BM_PRAM;
    break;

  case CMUXVR:
    base = cmuxv2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cmuxvr, mux);
    mode = BM_PRAM;
    break;

  case CMUXVG:
    base = cmuxv2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cmuxvg, mux);
    mode = BM_PRAM;
    break;

  case CMUXHR:
    base = cmuxh2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cmuxhr, mux);
    mode = BM_PRAM;
    break;

  case CMUXHG:
    base = cmuxh2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cmuxhg, mux);
    mode = BM_PRAM;
    break;

  case DLL:
    base = dll2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_dll, mux);
    mode = BM_PRAM;
    break;

  case HSSI:
    base = hssi2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_hssi, mux);
    mode = BM_PRAM;
    break;

  case CBUF:
    base = cbuf2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_cbuf, mux);
    mode = BM_PRAM;
    break;

  case LVL:
    base = lvl2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_lvl, mux);
    mode = BM_PRAM;
    break;

  case PMA3:
    base = pma32pram(pos);
    if(base == 0xffffffff)
      break;
    if(mux == PCLK0_SEL || mux == PCLK1_SEL) {
      pmux = bmux_find(bm_pma3c, mux);
      mode = BM_DCRAM;
    } else {
      pmux = bmux_find(bm_pma3, mux);
      mode = BM_PRAM;
    }
    break;

  case SERPAR:
    base = serpar2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_serpar, mux);
    mode = BM_PRAM;
    break;

  case TERM:
    base = term2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_term, mux);
    mode = BM_PRAM;
    break;

  case HIP:
    base = hip2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_hip, mux);
    mode = BM_PRAM;
    break;

  case HMC:
    base = hmc2pram(pos);
    if(base == 0xffffffff)
      break;
    pmux = bmux_find(bm_hmc, mux);
    mode = BM_PRAM;
    break;

  default:
    break;
  }
}

int mistral::CycloneV::bmux_type(block_type_t btype, pos_t pos, bmux_type_t mux, int midx) const
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span)
    return -1;
  return pmux->stype;
}

bool mistral::CycloneV::bmux_m_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bmux_type_t s)
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span || pmux->stype != MT_MUX)
    return false;

  const bmux_sel_entry *sel = bmux_sel_entries + pmux->entries_offset;
  for(int e = 0; e != pmux->entries; e++)
    if(sel[e].sel == s) {
      bmux_val_set(base, pmux, midx, mode, sel[e].mask);
      return true;
    }

  return false;
}

bool mistral::CycloneV::bmux_n_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, uint32_t s)
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span || pmux->stype != MT_NUM)
    return false;

  const bmux_num_entry *sel = bmux_num_entries + pmux->entries_offset;
  for(int e = 0; e != pmux->entries; e++)
    if(sel[e].num == s) {
      bmux_val_set(base, pmux, midx, mode, sel[e].mask);
      return true;
    }

  return false;
}

bool mistral::CycloneV::bmux_b_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bool s)
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span || pmux->stype != MT_BOOL)
    return false;

  bmux_val_set(base, pmux, midx, mode, s);
  return true;
}

bool mistral::CycloneV::bmux_r_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, uint64_t s)
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span || pmux->stype != MT_RAM)
    return false;

  bmux_val_set(base, pmux, midx, mode, s);
  return true;
}

bool mistral::CycloneV::bmux_r_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, const std::vector<uint8_t> &s)
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span || pmux->stype != MT_RAM)
    return false;

  bmux_val_set(base, pmux, midx, mode, s);
  return true;
}

bool mistral::CycloneV::bmux_get(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bmux_setting_t &s) const
{
  uint32_t base;
  const bmux *pmux;
  bmux_ram_t mode;

  bmux_find(btype, pos, mux, base, pmux, mode);
  if(!pmux || midx < 0 || midx >= pmux->span)
    return false;

  s.btype = btype;
  s.pos = pos;
  s.mux = mux;
  s.midx = midx;
  s.type = pmux->stype;

  switch(pmux->stype) {
  case MT_MUX: {
    auto r = bmux_m_read(btype, pos, base, pmux, midx, mode);
    s.s = r.first;
    s.def = r.second;
    break;
  }
  case MT_NUM: {
    auto r = bmux_n_read(btype, pos, base, pmux, midx, mode);
    s.s = r.first;
    s.def = r.second;
    break;
  }
  case MT_BOOL: {
    auto r = bmux_b_read(btype, pos, base, pmux, midx, mode);
    s.s = r.first;
    s.def = r.second;
    break;
  }
  case MT_RAM: {
    s.s = pmux->bits;
    s.def = bmux_r_read(btype, pos, base, pmux, midx, mode, s.r);
    break;
  }
  }

  return true;
}
