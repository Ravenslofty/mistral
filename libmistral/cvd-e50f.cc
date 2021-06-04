#include "cyclonev.h"

// e50f die for the Cyclone V

// Declined in three models, e25b, e25f and e50b

const uint8_t mistral::CycloneV::e50f_bel_spans_info[] = {
   1, 26,   1,  1, 44,
  28, 53,   1,  1, 44,
  0xff
};

const mistral::CycloneV::die_info mistral::CycloneV::e50f = {
  "e50f", E50F,

  4958, 3928,              // cram size
  55, 46,                  // tiles size
  55, 46,                  // hps (arm) processor core position
  612514,                  // routing muxes count
  293,                     // IO blocks count
  17,                      // dqs16 count
  5845,                    // inverters count
  12,                      // count of forced-1 bits
  xy2pos(0, 4),            // control block position

  // default options
  { 0x10706080d0, 0x128a0f3024, 0xa001858f7f, 0x9bffffffff, 0xfffffffe80, 0xa0aa8d803f, 0xf000280e02, 0xffffffff6a, 0x00000030c2, 0x3b030ecc05, 0x0730508412, 0xcf82a00848 },

  // frame size
  129,

  // pram sizes
  { 3383, 5071, 4826, 3383, 2494, 2732, 2449, 8226, 5106, 2418, 3383, 2449, 2732, 2449 },

  // no edcrc zones
  { 183, 902, 1803, 2503, 3222, 3971, 4521, 4958 },

  // postambles
  102, 388,

  // x to bit x
  {
       0,   65,  124,  180,  480,  556,  615,  674,  735,  767,
     843,  899, 1199, 1263, 1339, 1400, 1432, 1491, 1550, 1626,
    1685, 1744, 1800, 2100, 2176, 2235, 2294, 2353, 2368, 2444,
    2500, 2800, 2859, 2920, 2952, 3028, 3087, 3146, 3219, 3519,
    3583, 3642, 3701, 3777, 3836, 3895, 3968, 4268, 4327, 4386,
    4462, 4518, 4818, 4877, 4936,
  },

  // column types
  {
    T_EMPTY, T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_DSP,   T_MLAB,
    T_LAB,   T_M10K,  T_LAB,   T_MLAB,  T_LAB,   T_DSP,   T_LAB,   T_LAB,   T_MLAB,  T_LAB,
    T_LAB,   T_LAB,   T_M10K,  T_MLAB,  T_LAB,   T_LAB,   T_LAB,   T_EMPTY, T_MLAB,  T_LAB,
    T_M10K,  T_LAB,   T_LAB,   T_DSP,   T_MLAB,  T_LAB,   T_LAB,   T_MLAB,  T_M10K,  T_LAB,
    T_LAB,   T_LAB,   T_MLAB,  T_LAB,   T_LAB,   T_MLAB,  T_M10K,  T_LAB,   T_LAB,   T_MLAB,
    T_LAB,   T_M10K,  T_LAB,   T_LAB,   T_EMPTY,
  },

  {
      e50f_package_f17, e50f_package_f23, nullptr, nullptr, nullptr,
      e50f_package_u15, e50f_package_u19, nullptr,
      nullptr, e50f_package_m13, nullptr,
  },

  e50f_bel_spans_info,
  e50f_ioblocks_info,
  e50f_dqs16_info,
  e50f_p2r_info,
  e50f_p2p_info,
  e50f_fixed_blocks_info,
  e50f_inverters_info,
  nullptr,
  e50f_forced_1_info,
  nullptr,
};
