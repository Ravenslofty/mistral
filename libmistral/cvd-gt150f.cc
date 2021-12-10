#include "cyclonev.h"

// gt150f die for the Cyclone V

// Declined in four models, gt150f, e150b, e150f, e150fsc

const uint8_t mistral::CycloneV::gt150f_bel_spans_info[] = {
   1,  9,   3,  1, 14,  32, 55,  73, 80, 
  10, 19,   1,  1, 80,
  20, 20,   2,  2, 30,  33, 79,
  21, 31,   1,  1, 80,
  32, 32,   2,  2, 30,  33, 79,
  33, 43,   1,  1, 80,
  45, 53,   1,  1, 80,
  54, 54,   2,  2, 30,  33, 79,
  55, 85,   1,  1, 80,
  86, 86,   2,  2, 30,  33, 79,
  87, 88,   1,  1, 80,
  0xff
};

extern const uint8_t _binary_gt150f_r_bin_start[];
extern const uint8_t _binary_gt150f_r_bin_end[];

const mistral::CycloneV::die_info mistral::CycloneV::gt150f = {
  "gt150f", GT150F,

  7605, 7024,              // cram size
  90, 82,                  // tiles size
  90, 82,                  // hps (arm) processor core position
  543,                     // IO blocks count
  30,                      // dqs16 count
  12862,                   // inverters count
  0,                       // count of forced-1 bits
  xy2pos(0, 2),            // control block position

  // default options
  { 0x28c0e180d0, 0x24141e3c32, 0xa001858f7f, 0x9bffffffff, 0xfffffffe81, 0xac817d803f, 0xf000181602, 0xffffffff6a, 0x00000030c2, 0x3b030ecc0a, 0x0c50b98814, 0xcf82a014ce },
  
  // frame size
  227,

  // pram sizes
  { 5806, 7434, 9669, 8169, 4862, 7518, 7204, 4767, 13012, 5106, 7204, 5751, 1984, 9500, 6800, 9136 },

  // no edcrc zones
  { 318, 1121, 2099, 3059, 3491, 4174, 4940, 5862, 6530, 7605 },

  // postambles
  190, 412,

  // x to bit x
  {
       0,   65,  124,  183,  259,  315,  615,  691,  750,  826,
     885,  944, 1003, 1062, 1118, 1418, 1499, 1558, 1617, 1676,
    1737, 1769, 1845, 1904, 1963, 2023, 2096, 2396, 2455, 2531,
    2590, 2649, 2710, 2742, 2806, 2882, 2941, 3000, 3056, 3356,
    3432, 3488, 3788, 3847, 3906, 3921, 3980, 4039, 4115, 4171,
    4471, 4530, 4589, 4665, 4726, 4758, 4822, 4881, 4937, 5237,
    5313, 5372, 5431, 5490, 5549, 5608, 5684, 5744, 5803, 5859,
    6159, 6218, 6277, 6353, 6412, 6471, 6527, 6827, 6891, 6967,
    7026, 7085, 7144, 7220, 7279, 7355, 7416, 7448, 7524, 7583
  },

  // column types
  {
    T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_LAB,
    T_DSP,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_MLAB,  T_M10K,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_DSP,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_MLAB,
    T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_MLAB,
    T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_M10K,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_MLAB,  T_LAB,   T_EMPTY,
  },

  {
      nullptr, gt150f_package_f23, gt150f_package_f27, gt150f_package_f31, nullptr,
      nullptr, gt150f_package_u19, nullptr,
      nullptr, nullptr, gt150f_package_m15,
  },

  _binary_gt150f_r_bin_start, _binary_gt150f_r_bin_end,
  gt150f_bel_spans_info,
  gt150f_ioblocks_info,
  gt150f_dqs16_info,
  gt150f_p2p_info,
  gt150f_fixed_blocks_info,
  gt150f_inverters_info,
  gt150f_dcram_pos,
  nullptr,
  nullptr,
};
