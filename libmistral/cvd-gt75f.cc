#include "cyclonev.h"

// gt75f die for the Cyclone V

// Declined in six models, gx75b, gt75f, gt75f_sc, gx50b, gx50f and gx50fsc

const uint8_t mistral::CycloneV::gt75f_bel_spans_info[] = {
   1,  9,   3,  1, 13,  31, 35,  53, 60, 
  10, 25,   1,  1, 60,
  27, 67,   1,  1, 60,

  0xff
};

extern const uint8_t _binary_gt75f_r_bin_start[];
extern const uint8_t _binary_gt75f_r_bin_end[];

const mistral::CycloneV::die_info mistral::CycloneV::gt75f = {
  "gt75f", GT75F,

  6006, 5304,              // cram size
  69, 62,                  // tiles size
  69, 62,                  // hps (arm) processor core position
  401,                     // IO blocks count
  22,                      // dqs16 count
  20,                      // count of forced-1 bits
  xy2pos(0, 3),            // control block position

  // default options
  { 0x18a08100d0, 0x1ace153c32, 0xa001858f7f, 0x9bffffffff, 0xfffffffe81, 0x03108d803f, 0xf000100e02, 0xffffffff6a, 0x00000030c2, 0x3b030ecc07, 0x0c40910514, 0xcf82c00c8a },
  
  // frame size
  173,

  // pram sizes
  { 3433, 5071, 9669, 4786, 3383, 2853, 4791, 7235, 3383, 9629, 5106, 4786, 3433, 6304, 6800, 9500 },

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
      nullptr, gt75f_package_f23, gt75f_package_f27, nullptr, nullptr,
      nullptr, gt75f_package_u19, nullptr,
      gt75f_package_m11, gt75f_package_m13, nullptr,
  },

  _binary_gt75f_r_bin_start, _binary_gt75f_r_bin_end,
  gt75f_bel_spans_info,
  gt75f_ioblocks_info,
  gt75f_dqs16_info,
  gt75f_fixed_blocks_info,
  gt75f_dcram_pos,
  gt75f_forced_1_info,
  nullptr,
};
