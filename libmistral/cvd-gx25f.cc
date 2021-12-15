#include "cyclonev.h"

// gx25f die for the Cyclone V

// Declined in three models, gx25b, gx25f and gx25b_sc

const uint8_t mistral::CycloneV::gx25f_bel_spans_info[] = {
   1,  9,   2,  1, 13,  31, 38, 
  10, 22,   1,  1, 38,
  24, 47,   1,  1, 38,

  0xff
};

extern const uint8_t _binary_gx25f_r_bin_start[];
extern const uint8_t _binary_gx25f_r_bin_end[];

const mistral::CycloneV::die_info mistral::CycloneV::gx25f = {
  "gx25f", GX25F,

  3856, 3412,              // cram size
  49, 40,                  // tiles size
  49, 40,                  // hps (arm) processor core position
  259,                     // IO blocks count
  14,                      // dqs16 count
  20,                      // count of forced-1 bits
  xy2pos(0, 2),            // control block position

  // default options
  { 0x10504080d0, 0x0f080c2620, 0xa041858f7f, 0x9bffffffff, 0xfffffffe80, 0x6eb0dd803f, 0xf000080e02, 0xffffffff6a, 0x00000030c2, 0x3b030ecc04, 0x0630488310, 0xcf82a00848 },
  
  // frame size
  113,

  // pram sizes
  { 2511, 5071, 4826, 3383, 2858, 2368, 2449, 8226, 5106, 2418, 3383, 1984, 9500 },

  // no edcrc zones
  { 318, 1371, 2417, 3166, 3856 },

  // postambles
  86, 340,

  // x to bit x
  {
       0,   65,  124,  183,  259,  315,  615,  691,  750,  826,
     885,  944, 1005, 1037, 1101, 1177, 1236, 1295, 1368, 1668,
    1727, 1788, 1820, 1879, 1894, 1953, 2012, 2088, 2149, 2181,
    2240, 2299, 2358, 2414, 2714, 2795, 2854, 2913, 2989, 3048,
    3107, 3163, 3463, 3539, 3598, 3657, 3716, 3775, 3834,
  },

  // column types
  {
    T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_DSP,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_MLAB,  T_M10K,  T_LAB,
    T_LAB,   T_DSP,   T_LAB,   T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,
    T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,
    T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_EMPTY,
  },

  {
      nullptr, gx25f_package_f23, nullptr, nullptr, nullptr,
      gx25f_package_u15, gx25f_package_u19, nullptr,
      nullptr, nullptr, nullptr,
  },

  _binary_gx25f_r_bin_start, _binary_gx25f_r_bin_end,
  gx25f_bel_spans_info,
  gx25f_ioblocks_info,
  gx25f_dqs16_info,
  gx25f_fixed_blocks_info,
  gx25f_dcram_pos,
  gx25f_forced_1_info,
  nullptr,
};
