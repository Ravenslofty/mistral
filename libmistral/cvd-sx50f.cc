#include "cyclonev.h"

// sx50f die for the Cyclone V

// Declined in thirteen variants, se30b, se30b_sc, se30bs, se30bs_sc,
// se50b, se50b_sc, se50bs, se50bs_sc, se50m, sx30f, sx30f_sc, sx50f
// and sx50f_sc


const uint8_t mistral::CycloneV::sx50f_bel_spans_info[] = {
   1,  9,   3,  1, 13,  31, 35,  53, 60, 
  10, 25,   1,  1, 60,
  27, 30,   1,  1, 60,
  31, 67,   1,  1, 16,

  0xff
};

extern const uint8_t _binary_sx50f_r_bin_start[];
extern const uint8_t _binary_sx50f_r_bin_end[];

const mistral::CycloneV::die_info mistral::CycloneV::sx50f = {
  "sx50f", SX50F,

  6006, 5304,              // cram size
  69, 62,                  // tiles size
  31, 17,                  // hps (arm) processor core position
  560293,                  // routing muxes count
  438,                     // IO blocks count
  19,                      // dqs16 count
  9863,                    // inverters count
  0,                       // count of forced-1 bits
  xy2pos(0, 3),            // control block position

  // default options
  { 0x18a08100d0, 0x1ace153c32, 0xa001858f7f, 0x9bffffffff, 0xfffffffe81, 0x03108d803f, 0xf000080e02, 0xffffffff6a, 0x00000030c2, 0x3b030ecc07, 0x0c40910514, 0xcb03e00c8a },

  // frame size
  173,

  // pram sizes
  { 3433, 5071, 9669, 4786, 3383, 2853, 7162, 9607, 3438, 5066, 3433, 6304, 6800, 9500 },

  // no edcrc zones
  { 318, 1003, 1727, 2444, 3281, 3831, 4867, 5417, 6006 },

  // postambles
  150, 452,

  // x to bit x
  {
       0,   65,  124,  183,  259,  315,  615,  691,  750,  826,
     885,  944, 1000, 1300, 1359, 1440, 1501, 1533, 1592, 1651,
    1724, 2024, 2083, 2142, 2203, 2235, 2311, 2326, 2385, 2441,
    2741, 2800, 2859, 2935, 2996, 3028, 3087, 3146, 3222, 3278,
    3578, 3637, 3696, 3772, 3828, 4128, 4187, 4246, 4327, 4386,
    4445, 4521, 4582, 4614, 4673, 4732, 4808, 4864, 5164, 5223,
    5282, 5358, 5414, 5714, 5773, 5832, 5893, 5925, 5984
  },

  // column types
  {
    T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_M10K,  T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,
    T_M10K,  T_LAB,   T_LAB,   T_LAB,   T_DSP,   T_MLAB,  T_EMPTY, T_LAB,   T_LAB,   T_M10K,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,
  
    T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,
    T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_LAB,   T_DSP,   T_LAB,   T_EMPTY,
  },

  {
      nullptr, nullptr, nullptr, nullptr, nullptr,
      nullptr, sx50f_package_u19, sx120f_package_u23,
      nullptr, nullptr, nullptr,
  },

  _binary_sx50f_r_bin_start, _binary_sx50f_r_bin_end,
  sx50f_bel_spans_info,
  sx50f_ioblocks_info,
  sx50f_dqs16_info,
  sx50f_p2r_info,
  sx50f_p2p_info,
  sx50f_fixed_blocks_info,
  sx50f_inverters_info,
  sx50f_dcram_pos,
  nullptr,
  sx50f_hps_info,
};
