#include "cyclonev.h"

#include <string.h>


int mistral::CycloneV::opt_type(bmux_type_t mux) const
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux)
    return -1;
  return pmux->stype;
}

bool mistral::CycloneV::opt_m_set(bmux_type_t mux, bmux_type_t s)
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux || pmux->stype != MT_MUX)
    return false;

  const bmux_sel_entry *sel = bmux_sel_entries + pmux->entries_offset;
  for(int e = 0; e != pmux->entries; e++)
    if(sel[e].sel == s) {
      bmux_val_set(0, pmux, 0, BM_ORAM, sel[e].mask);
      return true;
    }

  return false;
}

bool mistral::CycloneV::opt_n_set(bmux_type_t mux, uint32_t s)
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux || pmux->stype != MT_NUM)
    return false;

  const bmux_num_entry *sel = bmux_num_entries + pmux->entries_offset;
  for(int e = 0; e != pmux->entries; e++)
    if(sel[e].num == s) {
      bmux_val_set(0, pmux, 0, BM_ORAM, sel[e].mask);
      return true;
    }

  return false;
}

bool mistral::CycloneV::opt_b_set(bmux_type_t mux, bool s)
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux || pmux->stype != MT_BOOL)
    return false;

  bmux_val_set(0, pmux, 0, BM_ORAM, s);
  return true;
}

bool mistral::CycloneV::opt_r_set(bmux_type_t mux, uint64_t s)
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux || pmux->stype != MT_RAM)
    return false;

  bmux_val_set(0, pmux, 0, BM_ORAM, s);
  return true;
}

bool mistral::CycloneV::opt_r_set(bmux_type_t mux, const std::vector<uint8_t> &s)
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux || pmux->stype != MT_RAM)
    return false;

  bmux_val_set(0, pmux, 0, BM_ORAM, s);
  return true;
}

void mistral::CycloneV::opt_get_one(const bmux *muxes, opt_setting_t &bms) const
{
    bms.mux = muxes->mux;
    bms.type = muxes->stype;
    switch(muxes->stype) {
    case MT_MUX: {
      auto r = bmux_m_read(BNONE, 0, 0, muxes, 0, BM_ORAM);
      bms.s = r.first;
      bms.def = r.second;
      break;
    }
    case MT_NUM: {
      auto r = bmux_n_read(BNONE, 0, 0, muxes, 0, BM_ORAM);
      bms.s = r.first;
      bms.def = r.second;
      break;
    }
    case MT_BOOL: {
      auto r = bmux_b_read(BNONE, 0, 0, muxes, 0, BM_ORAM);
      bms.s = r.first;
      bms.def = r.second;
      break;
    }
    case MT_RAM: {
      bms.s = muxes->bits;
      bms.def = bmux_r_read(BNONE, 0, 0, muxes, 0, BM_ORAM, bms.r);
      break;
    }
    }
}

bool mistral::CycloneV::opt_get(bmux_type_t mux, opt_setting_t &s) const
{
  const bmux *pmux = bmux_find(bm_opt, mux);
  if(!pmux)
    return false;

  opt_get_one(pmux, s);
  return true;
}

std::vector<mistral::CycloneV::opt_setting_t> mistral::CycloneV::opt_get() const
{
  std::vector<opt_setting_t> res;
  auto muxes = bm_opt;
  while(muxes->mux) {
    opt_setting_t bms;
    opt_get_one(muxes, bms);
    res.push_back(bms);
    muxes++;
  }
  return res;
}

uint32_t mistral::CycloneV::compute_default_jtag_id() const
{
  uint32_t jtag_id = 0;
  for(uint8_t c : cram)
    jtag_id += c;
  //  for(uint8_t c : cff)
  //    jtag_id += c;
  return jtag_id;
}

uint64_t mistral::CycloneV::oram_load(uint8_t strip, uint8_t off, uint8_t count) const
{
  return (oram[strip] >> off) & ((uint64_t(1) << count) - 1);
}

void mistral::CycloneV::oram_save(uint64_t v, uint8_t strip, uint8_t off, uint8_t count)
{
  uint64_t mask = ((uint64_t(1) << count) - 1) << off;
  oram[strip] = (oram[strip] & ~mask) | ((v << off) & mask);
}

void mistral::CycloneV::oram_clear()
{
  for(int i = 0x00; i != 0x08; i++)
    oram[i] = di.default_oram[i];
  for(int i = 0x08; i != 0x0c; i++)
    oram[i] = 0xffffffffff;
  for(int i = 0x0c; i != 0x10; i++)
    oram[i] = di.default_oram[i-4];
  for(int i = 0x10; i != 0x20; i++)
    oram[i] = 0xffffffffff;

  opt_r_set(JTAG_ID, model->variant.idcode);
}

void mistral::CycloneV::oram_load(const uint8_t *data)
{
  memset(oram, 0, sizeof(oram));
  for(int strip = 0; strip != 32; strip++)
    for(int bit = 0; bit != 40; bit++)
      if(data[0x84 + (strip >> 3) + (bit << 2)] & (0x80 >> (strip & 7)))
	oram[strip] |= uint64_t(1) << bit;
}

void mistral::CycloneV::oram_save(uint8_t *data) const
{
  memset(data, 0xff, 0x80);
  memset(data+0x80, 'j', 4);

  memset(data + 0x84, 0x00, 0xa0);

  for(int strip = 0; strip != 32; strip++)
    for(int bit = 0; bit != 40; bit++)
      if((oram[strip] >> bit) & 1)
	data[0x84 + (strip >> 3) + (bit << 2)] |= 0x80 >> (strip & 7);

  data[0x124] = 0x00;
  data[0x125] = 0x00;

  uint16_t checksum = crc16(data + 0x84, 0x126 - 0x84);
  data[0x126] = checksum;
  data[0x127] = checksum >> 8;

  memset(data + 0x128, 0xff, di.frame_size*4 - 0xb8);
}
