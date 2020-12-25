#include "cyclonev.h"

#include <string.h>

uint32_t mistral::CycloneV::max_pram_block_size() const
{
  uint32_t m = 0;
  for(int i=0; i != 32; i++)
    if(di.pram_sizes[i] > m)
      m = di.pram_sizes[i];
  return m;
}

uint16_t mistral::CycloneV::crc16(const uint8_t *src, uint32_t len)
{
  uint16_t accum = 0xffff;
  for(uint32_t pos = 0; pos != len*8; pos ++) {
    bool bit = ((src[pos >> 3] >> (pos & 7)) ^ accum) & 1;
    accum >>= 1;
    if(bit)
      accum ^= 0xa001;
  }
  return accum;
}

uint32_t mistral::CycloneV::crc32(const uint8_t *src) const
{
  uint32_t cram_frame_bits = di.frame_size - 7;
  uint32_t accum = 0x00000001;
  for(uint32_t bitid = 0; bitid != 32; bitid++) {
    for(uint32_t block = 0; block != cram_frame_bits; block++) {
      uint32_t pos = 224 + (31 - bitid) + 32 * block;
      bool bit = ((src[pos >> 3] >> (pos & 7)) ^ (accum >> 31)) & 1;
      accum <<= 1;
      if(bit)
	accum ^= 0xf4acfb13;
    }
  }
  return accum;
}

void mistral::CycloneV::rbf_load_oram(const void *data, uint32_t)
{
  oram_load(static_cast<const uint8_t *>(data));
}

void mistral::CycloneV::rbf_load(const void *data, uint32_t size)
{
  uint32_t mpram = max_pram_block_size();
  uint32_t pram_frame_bytes = (di.frame_size+2)*4;
  uint32_t oram_frame_bytes = pram_frame_bytes + 104;
  uint32_t pram_blocks = (mpram + di.frame_size - 1)/di.frame_size;

  oram_load(static_cast<const uint8_t *>(data));

  for(int bit=0; bit != 32; bit++)
    std::fill(pram[bit].begin(), pram[bit].end(), false);
  for(int bit = 31; bit >= 0; bit--) {
    uint8_t mask = 1 << (bit & 7);
    uint32_t off = bit >> 3;
    uint32_t count = di.pram_sizes[bit];
    for(uint32_t i=0; i != count; i++) {
      uint32_t base = mpram - i - 1;
      if(static_cast<const uint8_t *>(data)[oram_frame_bytes + 4 + off + base * 4 + (base / di.frame_size)*(4+4)] & mask)
	pram[bit][i] = true;
    }
  }

  uint32_t padding = (64 - (di.cram_sy & 63)) ^ 32;
  uint32_t cram_frame_size = di.cram_sy + 256 + padding;
  uint32_t cram_frame_bytes = cram_frame_size / 8;

  opt_setting_t comp;
  opt_get(COMPRESSION_DIS, comp);

  std::vector<uint8_t> framed_cram;
  const uint8_t *framed;
  uint32_t framed_bytes = di.cram_sx * cram_frame_bytes;
  if(!comp.s) {
    framed_cram.resize(framed_bytes, 0x00);
    framed = framed_cram.data();

    const uint8_t *comp = static_cast<const uint8_t *>(data) + oram_frame_bytes + pram_frame_bytes * pram_blocks;
    uint8_t *uncomp = framed_cram.data();
    bool high = false;
    auto r = [&comp, &high]() -> uint8_t { if(high) { high = false; return *comp++ >> 4; } else { high = true; return *comp & 0xf; }};

    while(uncomp < &framed_cram[framed_bytes]) {
      uint8_t key = r();
      uint8_t v;
      v = key & 1 ? r() : 0;
      v |= key & 2 ? r() << 4 : 0;
      *uncomp++ = v;
      v = key & 4 ? r() : 0;
      v |= key & 8 ? r() << 4 : 0;
      *uncomp++ = v;
    }

  } else
    framed = static_cast<const uint8_t *>(data) + oram_frame_bytes + pram_frame_bytes * pram_blocks;

  std::fill(cram.begin(), cram.end(), 0);
  uint32_t cram_frame_bits = di.frame_size - 7;
  int32_t offset = padding - 32;
  for(uint32_t x = 0; x != di.cram_sx; x ++) {
    const uint8_t *d = framed + x*cram_frame_bytes;
    for(uint32_t y = 32; y != di.cram_sy; y ++) {
      uint32_t ya = (y + offset) % cram_frame_bits;
      uint32_t yb = (y + offset) / cram_frame_bits;
      uint32_t pos = 224 + (31^yb) + 32*ya;
      if((d[pos >> 3] >> (pos & 7)) & 1) {
	uint32_t cpos = x + di.cram_sx * y;
	cram[cpos >> 3] |= 0x01 << (cpos & 7);
      }
    }
  }
}

void mistral::CycloneV::rbf_save(std::vector<uint8_t> &data)
{
  uint32_t mpram = max_pram_block_size();
  uint32_t pram_frame_bytes = di.frame_size*4 + 8;
  uint32_t oram_frame_bytes = pram_frame_bytes + 104;
  uint32_t pram_blocks = (mpram + di.frame_size - 1)/di.frame_size;
  data.clear();
  data.resize(oram_frame_bytes + pram_frame_bytes * pram_blocks);

  oram_save(data.data());

  for(int bit = 31; bit >= 0; bit--) {
    uint8_t mask = 1 << (bit & 7);
    uint32_t off = bit >> 3;
    uint32_t count = di.pram_sizes[bit];
    for(uint32_t i=0; i != count; i++) {
      if(pram[bit][i]) {
	uint32_t base = mpram - i - 1;	
	data[oram_frame_bytes + 4 + off + base * 4 + (base / di.frame_size)*(4+4)] |= mask;
      }
    }
  }

  uint32_t coff = oram_frame_bytes + (pram_blocks-1)*pram_frame_bytes;

  data[coff  ] = 0x2a;
  data[coff+1] = 0xc5;
  data[coff+2] = 0x01;

  coff = oram_frame_bytes;
  for(uint32_t page = 0; page != pram_blocks; page++) {
    uint16_t crc = crc16(&data[coff], pram_frame_bytes - 2);
    data[coff + pram_frame_bytes - 2] = crc;
    data[coff + pram_frame_bytes - 1] = crc >> 8;
    coff += pram_frame_bytes;
  }    

  uint32_t padding = (64 - (di.cram_sy & 63)) ^ 32;
  uint32_t cram_frame_size = di.cram_sy + 256 + padding;
  uint32_t cram_frame_bytes = cram_frame_size / 8;

  opt_setting_t comp;
  opt_get(COMPRESSION_DIS, comp);

  std::vector<uint8_t> framed_cram;
  uint8_t *framed;
  uint32_t framed_bytes = di.cram_sx * cram_frame_bytes;
  if(!comp.s) {
    framed_cram.resize(framed_bytes, 0x00);
    framed = framed_cram.data();
  } else {
    data.resize(data.size() + framed_bytes, 0x00);
    framed = &data[oram_frame_bytes + pram_frame_bytes * pram_blocks];
  }

  int idx = 0;
  framed[             0*cram_frame_bytes + 0] = 0x84;
  framed[             0*cram_frame_bytes + 1] = 0x3e;
  framed[             0*cram_frame_bytes + 2] = 0x01;
  framed[(di.cram_sx-1)*cram_frame_bytes + 0] = 0x42;
  framed[(di.cram_sx-1)*cram_frame_bytes + 1] = 0x9f;
  framed[(di.cram_sx-1)*cram_frame_bytes + 2] = 0x00;
  
  uint32_t cram_frame_bits = di.frame_size - 7;
  int32_t offset = padding - 32;
  for(uint32_t x = 0; x != di.cram_sx; x ++) {
    uint8_t *d = framed + x*cram_frame_bytes;
    for(uint32_t y = 32; y != di.cram_sy; y ++) {
      uint32_t cpos = x + di.cram_sx * y;
      if((cram[cpos >> 3] >> (cpos & 7)) & 1) {
	uint32_t ya = (y + offset) % cram_frame_bits;
	uint32_t yb = (y + offset) / cram_frame_bits;
	uint32_t pos = 224 + (31^yb) + 32*ya;
	d[pos >> 3] |= 0x01 << (pos & 7);
      }
    }

    uint32_t edcrc = x >= di.noedcrc_zones[idx] ? 0 : crc32(d);
    d[cram_frame_bytes - 8] = edcrc;
    d[cram_frame_bytes - 7] = edcrc >> 8;
    d[cram_frame_bytes - 6] = edcrc >> 16;
    d[cram_frame_bytes - 5] = edcrc >> 24;

    if(x == uint32_t(di.noedcrc_zones[idx]) + 255)
      idx++;

    uint16_t crc = crc16(d, cram_frame_bytes - 2);
    d[cram_frame_bytes - 2] = crc;
    d[cram_frame_bytes - 1] = crc >> 8;
  }

  if(!comp.s) {
    bool high = false;
    auto w = [&data, &high](uint8_t nib) { if(high) { data.back() |= nib << 4; high = false; } else { data.push_back(nib); high = true; }};
                
    for(uint32_t i = 0; i < framed_bytes; i += 2) {
      uint8_t sv1 = *framed++;
      uint8_t sv2 = *framed++;
      uint8_t mask = 0;
      if(sv1 & 0x0f) mask |= 1;
      if(sv1 & 0xf0) mask |= 2;
      if(sv2 & 0x0f) mask |= 4;
      if(sv2 & 0xf0) mask |= 8;
      w(mask);
      if(mask & 1) w(sv1 & 0xf);
      if(mask & 2) w(sv1 >> 4);
      if(mask & 4) w(sv2 & 0xf);
      if(mask & 8) w(sv2 >> 4);
    }
    while(high)
      w(0xf);
  }

  coff = data.size();
  data.resize(coff + di.postamble_1, 0x00);
  data[coff] = 0xec;
  data[coff+1] = 0x64;
  uint16_t crc = crc16(&data[coff], di.postamble_1);
  data.push_back(crc);
  data.push_back(crc >> 8);

  coff = data.size();
  data.resize(coff + 10, 0x00);
  data[coff] = 0xae;
  data[coff+1] = 0xfb;
  crc = crc16(&data[coff], 10);
  data.push_back(crc);
  data.push_back(crc >> 8);

  coff = data.size();
  data.resize(coff + di.postamble_2, 0xff);
}
