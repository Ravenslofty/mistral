#include "cyclonev.h"

// gt300f die for the Cyclone V

// Declined in five models, gt300f, gt300f_sc, gx300b, gx300f and gx300fsc

const uint8_t mistral::CycloneV::gt300f_bel_spans_info[] = {
   1,   9,   3,  1, 21,  39, 63,  81, 114, 
  10,  59,   1,  1, 114,
  61, 120,   1,  1, 114,

  0xff
};

extern const uint8_t _binary_gt300f_r_bin_start[];
extern const uint8_t _binary_gt300f_r_bin_end[];

const mistral::CycloneV::die_info mistral::CycloneV::gt300f = {
  "gt300f", GT300F,

  10038, 9948,             // cram size
  122, 116,                // tiles size
  122, 116,                // hps (arm) processor core position
  xycoords(0, 3),          // control block position

  // default options
  { 0x2900e180d0, 0x25161e3c32, 0xa001858f7f, 0x9bffffffff, 0xfffffffe83, 0x10d4ad803f, 0xf000200e02, 0xffffffff6a, 0x00000030c2, 0x3b030ecc0a, 0x0c50b98814, 0xcf82a014ce },

  // frame size
  317,

  // pram sizes
  { 8224, 7434, 9669, 10587, 7280, 7518, 11971, 5801, 9629, 5106, 7204, 8169, 6800, 9500, 6800, 9136 },

  // no edcrc zones
  { 318, 1003, 2039, 3172, 4132, 4699, 5787, 6958, 7644, 8447, 9584, 10038 },

  // postambles
  206, 396,

  // x to bit x
  {
       0,   65,  124,  183,  259,  315,  615,  691,  750,  826,
     885,  944, 1000, 1300, 1376, 1435, 1499, 1558, 1617, 1693,
    1754, 1786, 1845, 1904, 1980, 2036, 2336, 2395, 2454, 2513,
    2589, 2649, 2708, 2769, 2801, 2860, 2919, 2995, 3054, 3113,
    3169, 3469, 3545, 3604, 3663, 3722, 3786, 3862, 3921, 3982,
    4014, 4073, 4129, 4429, 4505, 4564, 4640, 4696, 4996, 5055,
    5114, 5129, 5188, 5247, 5323, 5384, 5416, 5475, 5534, 5610,
    5669, 5728, 5784, 6084, 6160, 6219, 6300, 6359, 6418, 6494,
    6553, 6612, 6673, 6705, 6764, 6823, 6899, 6955, 7255, 7331,
    7390, 7449, 7509, 7585, 7641, 7941, 8000, 8059, 8135, 8194,
    8253, 8312, 8388, 8444, 8744, 8803, 8862, 8926, 8985, 9044,
    9120, 9179, 9238, 9299, 9331, 9407, 9466, 9525, 9581, 9881,
    9957, 10016
  },

  // column types
  {
    T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,
    T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_LAB,   T_MLAB,
    T_LAB,   T_LAB,   T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,
    T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_DSP,
  
    T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,
    T_EMPTY, T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,
  
    T_LAB,   T_MLAB,  T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_LAB,   T_MLAB,
    T_LAB,   T_LAB,   T_LAB,   T_DSP,   T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_M10K,  T_MLAB,
    T_LAB,   T_EMPTY,
  },

  {
      nullptr, &gt300f_package_f23, &gt300f_package_f27, &gt300f_package_f31, &gt300f_package_f35,
      nullptr, &gt300f_package_u19, nullptr,
      nullptr, nullptr, nullptr,
  },

  _binary_gt300f_r_bin_start, _binary_gt300f_r_bin_end,
  gt300f_bel_spans_info,
};
