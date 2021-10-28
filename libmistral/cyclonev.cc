#include "cyclonev.h"
#include <lzma.h>

const mistral::CycloneV::package_info_t mistral::CycloneV::package_infos[5+3+3] = {
  {  256, 'f', 16, 16, 17, 17 },
  {  484, 'f', 22, 22, 23, 23 },
  {  672, 'f', 26, 26, 27, 27 },
  {  896, 'f', 30, 30, 31, 31 },
  { 1152, 'f', 34, 34, 35, 35 },

  {  324, 'u', 18, 18, 15, 15 },
  {  484, 'u', 22, 22, 19, 19 },
  {  672, 'u', 28, 28, 23, 23 },

  {  301, 'm', 21, 21, 11, 11 },
  {  383, 'm', 25, 25, 13, 13 },
  {  484, 'm', 28, 28, 15, 15 },
};

mistral::CycloneV *mistral::CycloneV::get_model(std::string model_name)
{
  if(model_name == "ms")
    model_name = "5CSEBA6U23I7";

  for(const Model *m = models; m->name; m++)
    if(model_name == m->name)
      return new CycloneV(m);

  return nullptr;
}

const char *const mistral::CycloneV::rnode_type_names[] = {
#define P(x) #x
#include "cv-rnodetypes.ipp"
#undef P
  nullptr
};

const char *const mistral::CycloneV::block_type_names[] = {
#define P(x) #x
#include "cv-blocktypes.ipp"
#undef P
  nullptr
};

const char *const mistral::CycloneV::port_type_names[] = {
#define P(x) #x
#include "cv-porttypes.ipp"
#undef P
  nullptr
};

const char *const mistral::CycloneV::bmux_type_names[] = {
#define P(x) #x
#include <cv-bmuxtypes.ipp>
#undef P
  nullptr
};

const char *const mistral::CycloneV::driver_type_names[] = {
#define P(x) #x
#include <cv-drivertypes.ipp>
#undef P
  nullptr
};

const char *const mistral::CycloneV::shape_type_names[] = {
#define P(x) #x
#include <cv-shapetypes.ipp>
#undef P
  nullptr
};

mistral::CycloneV::rnode_type_t mistral::CycloneV::rnode_type_lookup(const std::string &n) const
{
  return any_type_lookup(n, rnode_type_hash);
}

mistral::CycloneV::block_type_t mistral::CycloneV::block_type_lookup(const std::string &n) const
{
  return any_type_lookup(n, block_type_hash);
}

mistral::CycloneV::port_type_t mistral::CycloneV::port_type_lookup(const std::string &n) const
{
  return any_type_lookup(n, port_type_hash);
}

mistral::CycloneV::bmux_type_t mistral::CycloneV::bmux_type_lookup(const std::string &n) const
{
  return any_type_lookup(n, bmux_type_hash);
}

const mistral::CycloneV::block_type_t mistral::CycloneV::hps_index_to_type[I_HPS_COUNT] = {
  HPS_BOOT,
  HPS_CLOCKS,
  HPS_CLOCKS_RESETS,
  HPS_CROSS_TRIGGER,
  HPS_DBG_APB,
  HPS_DMA,
  HPS_FPGA2HPS,
  HPS_FPGA2SDRAM,
  HPS_HPS2FPGA,
  HPS_HPS2FPGA_LIGHT_WEIGHT,
  HPS_INTERRUPTS,
  HPS_JTAG,
  HPS_LOAN_IO,
  HPS_MPU_EVENT_STANDBY,
  HPS_MPU_GENERAL_PURPOSE,
  HPS_PERIPHERAL_CAN,
  HPS_PERIPHERAL_CAN,
  HPS_PERIPHERAL_EMAC,
  HPS_PERIPHERAL_EMAC,
  HPS_PERIPHERAL_I2C,
  HPS_PERIPHERAL_I2C,
  HPS_PERIPHERAL_I2C,
  HPS_PERIPHERAL_I2C,
  HPS_PERIPHERAL_NAND,
  HPS_PERIPHERAL_QSPI,
  HPS_PERIPHERAL_SDMMC,
  HPS_PERIPHERAL_SPI_MASTER,
  HPS_PERIPHERAL_SPI_MASTER,
  HPS_PERIPHERAL_SPI_SLAVE,
  HPS_PERIPHERAL_SPI_SLAVE,
  HPS_PERIPHERAL_UART,
  HPS_PERIPHERAL_UART,
  HPS_PERIPHERAL_USB,
  HPS_PERIPHERAL_USB,
  HPS_STM_EVENT,
  HPS_TEST,
  HPS_TPIU_TRACE,
};

mistral::CycloneV::CycloneV(const Model *m) : model(m), di(m->variant.die)
{
  rmux_load();

  any_type_hash_init(rnode_type_hash, rnode_type_names);
  any_type_hash_init(block_type_hash, block_type_names);
  any_type_hash_init(port_type_hash,  port_type_names);
  any_type_hash_init(bmux_type_hash,  bmux_type_names);

  add_cram_blocks();
  add_pram_blocks();
  ctrl_pos.push_back(di.ctrl);
  tile_bels[di.ctrl].push_back(CTRL);
  init_p2r_maps();

  cram.resize((di.cram_sx*di.cram_sy + 7) / 8, 0);
  for(int i=0; i != 32; i++)
    pram[i].resize(di.pram_sizes[i], false);

  clear();
}

void mistral::CycloneV::clear()
{
  std::fill(cram.begin(), cram.end(), 0);
  for(int i=0; i != 32; i++)
    std::fill(pram[i].begin(), pram[i].end(), false);
  bmux_set_defaults();
  route_set_defaults();
  forced_1_set();
  oram_clear();
}

void mistral::CycloneV::forced_1_set()
{
  for(uint32_t i=0; i != di.forced_1_count; i++) {
    uint32_t pos = di.forced_1_pos[i].y * di.cram_sx + di.forced_1_pos[i].x;
    cram[pos >> 3] |= 1 << (pos & 7);
  }
}

void mistral::CycloneV::add_cram_blocks()
{
  tile_types.fill(T_EMPTY);
  const uint8_t *pos = di.bel_spans;
  while(*pos != 0xff) {
    uint8_t xs = *pos++;
    uint8_t xe = *pos++;
    uint8_t spans_count = *pos++;
    const uint8_t *spans_start = pos;
    pos += 2*spans_count;

    for(uint8_t x = xs; x <= xe; x++) {
      const uint8_t *spans = spans_start;
      std::vector<uint16_t> *posvec;
      bool is_dsp = false;
      tile_type_t tp = di.column_types[x];
      switch(tp) {
      case T_EMPTY:
	abort();
      case T_LAB:
	posvec = &lab_pos;
	break;
      case T_MLAB:
	posvec = &mlab_pos;
	break;
      case T_M10K:
	posvec = &m10k_pos;
	break;
      case T_DSP:
	posvec = &dsp_pos;
	is_dsp = true;
	break;
      case T_DSP2: abort();
      };

      for(uint8_t si = 0; si != spans_count; si++) {
	uint8_t ys = *spans++;
	uint8_t ye = *spans++;
	for(uint8_t y = ys; y <= ye; y++) {
	  pos_t pos = xy2pos(x, y);
	  if(!is_dsp || !((y - ys) & 1)) {
	    tile_types[pos] = tp;
	    tile_bels[pos].push_back(tp == T_LAB ? LAB : tp == T_MLAB ? MLAB : tp == T_M10K ? M10K : DSP);
	    posvec->push_back(pos);
	  } else
	    tile_types[pos] = T_DSP2;
	}	
      }
    }
  }

  if(di.hps_blocks) {
    for(int i=0; i != I_HPS_COUNT; i++) {
      tile_bels[di.hps_blocks[i]].push_back(hps_index_to_type[i]);
      hps_pos.push_back(di.hps_blocks[i]);
    }
  }
}

void mistral::CycloneV::diff(const CycloneV *m) const
{
  for(int i=0; i != 32; i++)
    if(oram[i] != m->oram[i]) {
      uint64_t dt = oram[i] ^ m->oram[i];
      for(int j=0; j != 40; j++)
	if((dt >> j) & 1)
	  printf("oram %02d.%02d: %d -> %d\n", i, j, int((oram[i] >> j) & 1), int((m->oram[i] >> j) & 1));
    }

  for(int i=0; i != 32; i++)
    for(unsigned int j=0; j != pram[i].size(); j++)
      if(pram[i][j] != m->pram[i][j])
	printf("pram %02d.%05d: %d -> %d\n", i, j, pram[i][j], m->pram[i][j]);

  for(uint32_t i = 0; i != cram.size(); i++)
    if(cram[i] != m->cram[i]) {
      uint8_t v = cram[i] ^ m->cram[i];
      for(uint32_t j = 0; j != 8; j++)
	if((v >> j) & 1) {
	  uint32_t pos = i*8+j;
	  uint32_t x = pos % di.cram_sx;
	  uint32_t y = pos / di.cram_sx;
	  uint32_t tx;
	  for(tx=0; int(tx) != di.tile_sx-1; tx++)
	    if(x < di.x_to_bx[tx+1])
	      break;
	  uint32_t xx = x - di.x_to_bx[tx];
	  uint32_t ty = (y - 2) / 86;
	  uint32_t yy = (y - 2) % 86;
	      
	  printf("cram %8d %05d.%05d (%03d.%03d+%03d.%02d): %d -> %d\n", pos, x, y, tx, ty, xx, yy, (cram[i] >> j) & 1, (m->cram[i] >> j) & 1);
	}
    }
}

std::tuple<const uint8_t *, size_t> mistral::CycloneV::get_bin(const uint8_t *start, const uint8_t *end)
{
  const uint8_t *data = start;
  size_t dsize = end - start;

  if(!memcmp(data, "\xfd" "7zXZ", 6)) {
    const uint8_t *fptr = data + dsize - 12;
    lzma_stream_flags stream_flags;
    lzma_ret ret;

    ret = lzma_stream_footer_decode(&stream_flags, fptr);
    const uint8_t *iptr = fptr - stream_flags.backward_size;
    lzma_index *index = nullptr;
    uint64_t memlimit = UINT64_MAX;
    size_t pos = 0;
    lzma_index_buffer_decode(&index, &memlimit, nullptr, iptr, &pos, fptr - iptr);
    size_t size = lzma_index_uncompressed_size(index);
    lzma_index_end(index, nullptr);

    decompressed_data_storage.emplace_back(std::make_unique<uint8_t[]>(size));

    lzma_stream strm = LZMA_STREAM_INIT;
    if ((ret = lzma_stream_decoder(&strm, UINT64_MAX, 0)) != LZMA_OK) {
      fprintf(stderr, "failed to initialise liblzma: %d\n", ret);
      exit(1);
    }

    strm.next_in = data;
    strm.avail_in = dsize;
    strm.next_out = decompressed_data_storage.back().get();
    strm.avail_out = size;
  
    if((ret = lzma_code(&strm, LZMA_RUN)) != LZMA_STREAM_END) {
      fprintf(stderr, "rmux data decompression failure: %d\n", ret);
      exit(1);
    }

    data = decompressed_data_storage.back().get();
    dsize = size;
  }

  return std::make_tuple(data, dsize);
}

