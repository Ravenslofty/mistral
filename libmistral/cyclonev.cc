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
  if(model_name == "ap")
    model_name = "5CEBA4F23C8";

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

const char *const mistral::CycloneV::timing_slot_names[] = {
  "n55", "n40", "0", "85", "100", "125",
  nullptr
};

const double mistral::CycloneV::timing_slot_temperature[T_COUNT] = {
  -55, -40, 0, 85, 100, 125
};

const char *const mistral::CycloneV::edge_names[] = {
  "fall", "rise",
  nullptr
};

const char *const mistral::CycloneV::delay_type_names[] = {
  "max", "min",
  nullptr
};

const char *const mistral::CycloneV::cmux_link_names[] = {
  "CLKIN",
  "CLKPIN",
  "CLKPIN_SEL",
  "CLKPIN_SEL_0",
  "CLKPIN_SEL_1",
  "CLKPIN_SEL_2",
  "CLKPIN_SEL_3",
  "DEFAULT",
  "ICLK_SEL",
  "NCLKPIN",
  "NCLKPIN_SEL",
  "NCLKPIN_SEL_0",
  "NCLKPIN_SEL_1",
  "NCLKPIN_SEL_2",
  "NCLKPIN_SEL_3",
  "OFF",
  "PLLIN",
  "PLL_SEL_0",
  "PLL_SEL_1",
  "SWITCH",
  nullptr
};

const std::pair<uint8_t, uint8_t> mistral::CycloneV::cmuxhg_link_table[4][64] = {
  {{ CMUX_CLKPIN_SEL_0, 0 }, { CMUX_CLKPIN_SEL_1, 0 }, { CMUX_CLKPIN_SEL_2, 0 }, { CMUX_CLKPIN_SEL_3, 0 }, { CMUX_NCLKPIN_SEL_0, 0 }, { CMUX_NCLKPIN_SEL_1, 0 }, { CMUX_NCLKPIN_SEL_2, 0 }, { CMUX_NCLKPIN_SEL_3, 0 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_PLL_SEL_0, 0 }, { CMUX_PLL_SEL_1, 0 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_SWITCH, 0 }, { CMUX_CLKIN, 1 }, { CMUX_OFF, 1 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN_SEL_0, 1 }, { CMUX_CLKPIN_SEL_1, 1 }, { CMUX_CLKPIN_SEL_2, 1 }, { CMUX_CLKPIN_SEL_3, 1 }, { CMUX_NCLKPIN_SEL_0, 1 }, { CMUX_NCLKPIN_SEL_1, 1 }, { CMUX_NCLKPIN_SEL_2, 1 }, { CMUX_NCLKPIN_SEL_3, 1 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_PLL_SEL_0, 1 }, { CMUX_PLL_SEL_1, 1 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_SWITCH, 1 }, { CMUX_CLKIN, 1 }, { CMUX_OFF, 1 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN_SEL_0, 2 }, { CMUX_CLKPIN_SEL_1, 2 }, { CMUX_CLKPIN_SEL_2, 2 }, { CMUX_CLKPIN_SEL_3, 2 }, { CMUX_NCLKPIN_SEL_0, 2 }, { CMUX_NCLKPIN_SEL_1, 2 }, { CMUX_NCLKPIN_SEL_2, 2 }, { CMUX_NCLKPIN_SEL_3, 2 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_PLL_SEL_0, 2 }, { CMUX_PLL_SEL_1, 2 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_SWITCH, 2 }, { CMUX_CLKIN, 3 }, { CMUX_OFF, 1 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN_SEL_0, 3 }, { CMUX_CLKPIN_SEL_1, 3 }, { CMUX_CLKPIN_SEL_2, 3 }, { CMUX_CLKPIN_SEL_3, 3 }, { CMUX_NCLKPIN_SEL_0, 3 }, { CMUX_NCLKPIN_SEL_1, 3 }, { CMUX_NCLKPIN_SEL_2, 3 }, { CMUX_NCLKPIN_SEL_3, 3 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_PLL_SEL_0, 3 }, { CMUX_PLL_SEL_1, 3 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_SWITCH, 3 }, { CMUX_CLKIN, 3 }, { CMUX_OFF, 1 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
};

const std::pair<uint8_t, uint8_t> mistral::CycloneV::cmuxvg_link_table[4][64] = {
  {{ CMUX_CLKPIN, 1 }, { CMUX_CLKPIN, 3 }, { CMUX_CLKPIN, 0 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 1 }, { CMUX_NCLKPIN, 3 }, { CMUX_NCLKPIN, 0 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_SWITCH, 0 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN, 1 }, { CMUX_CLKPIN, 3 }, { CMUX_CLKPIN, 0 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 1 }, { CMUX_NCLKPIN, 3 }, { CMUX_NCLKPIN, 0 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_CLKIN, 1 }, { CMUX_SWITCH, 1 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN, 1 }, { CMUX_CLKPIN, 3 }, { CMUX_CLKPIN, 0 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 1 }, { CMUX_NCLKPIN, 3 }, { CMUX_NCLKPIN, 0 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_CLKIN, 2 }, { CMUX_SWITCH, 2 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_CLKPIN, 1 }, { CMUX_CLKPIN, 3 }, { CMUX_CLKPIN, 0 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 1 }, { CMUX_NCLKPIN, 3 }, { CMUX_NCLKPIN, 0 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 15 }, { CMUX_CLKIN, 3 }, { CMUX_SWITCH, 3 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
};

const std::pair<uint8_t, uint8_t> mistral::CycloneV::cmuxhr_link_table[12][32] = {
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 14 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 15 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 2 }, { CMUX_NCLKPIN_SEL, 2 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 16 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 3 }, { CMUX_NCLKPIN_SEL, 3 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 17 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 4 }, { CMUX_NCLKPIN_SEL, 4 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 18 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 5 }, { CMUX_NCLKPIN_SEL, 5 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 19 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 6 }, { CMUX_NCLKPIN_SEL, 6 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 14 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 7 }, { CMUX_NCLKPIN_SEL, 7 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 15 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 8 }, { CMUX_NCLKPIN_SEL, 8 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 16 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 9 }, { CMUX_NCLKPIN_SEL, 9 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 17 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN_SEL, 10 }, { CMUX_NCLKPIN_SEL, 10 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 18 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
  {{ CMUX_PLLIN, 13 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 7 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN_SEL, 11 }, { CMUX_NCLKPIN_SEL, 11 }, { CMUX_ICLK_SEL, 4 }, { CMUX_ICLK_SEL, 5 }, { CMUX_ICLK_SEL, 6 }, { CMUX_ICLK_SEL, 7 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 19 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }, { CMUX_OFF, 0 }},
};

const std::pair<uint8_t, uint8_t> mistral::CycloneV::cmuxvr_link_table[20][16] = {
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 0 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 15 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 9 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 26 }},
  {{ CMUX_PLLIN, 7 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 1 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 10 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 27 }, { CMUX_PLLIN, 28 }},
  {{ CMUX_PLLIN, 8 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 2 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 0 }, { CMUX_NCLKPIN, 0 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 11 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 29 }, { CMUX_PLLIN, 30 }},
  {{ CMUX_PLLIN, 7 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 31 }},
  {{ CMUX_PLLIN, 0 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 4 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 13 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 26 }, { CMUX_PLLIN, 27 }},
  {{ CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 28 }, { CMUX_PLLIN, 29 }},
  {{ CMUX_PLLIN, 2 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 6 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 0 }, { CMUX_NCLKPIN, 0 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 15 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 30 }, { CMUX_PLLIN, 31 }},
  {{ CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 7 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 16 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 26 }},
  {{ CMUX_PLLIN, 4 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 8 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 15 }, { CMUX_PLLIN, 17 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 27 }, { CMUX_PLLIN, 28 }},
  {{ CMUX_PLLIN, 5 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 0 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 9 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 29 }, { CMUX_PLLIN, 30 }},
  {{ CMUX_PLLIN, 6 }, { CMUX_PLLIN, 8 }, { CMUX_PLLIN, 0 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 15 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 9 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 26 }},
  {{ CMUX_PLLIN, 7 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 1 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 10 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 27 }, { CMUX_PLLIN, 28 }},
  {{ CMUX_PLLIN, 8 }, { CMUX_PLLIN, 0 }, { CMUX_PLLIN, 2 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 0 }, { CMUX_NCLKPIN, 0 }, { CMUX_PLLIN, 17 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 11 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 29 }, { CMUX_PLLIN, 30 }},
  {{ CMUX_PLLIN, 7 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 31 }},
  {{ CMUX_PLLIN, 0 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 4 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 9 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 13 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 26 }, { CMUX_PLLIN, 27 }},
  {{ CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 28 }, { CMUX_PLLIN, 29 }},
  {{ CMUX_PLLIN, 2 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 6 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 0 }, { CMUX_NCLKPIN, 0 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 15 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 24 }, { CMUX_PLLIN, 30 }, { CMUX_PLLIN, 31 }},
  {{ CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 7 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 2 }, { CMUX_NCLKPIN, 2 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 16 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 18 }, { CMUX_PLLIN, 19 }, { CMUX_PLLIN, 25 }, { CMUX_PLLIN, 26 }},
  {{ CMUX_PLLIN, 4 }, { CMUX_PLLIN, 6 }, { CMUX_PLLIN, 8 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_CLKPIN, 1 }, { CMUX_NCLKPIN, 1 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 15 }, { CMUX_PLLIN, 17 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 20 }, { CMUX_PLLIN, 21 }, { CMUX_PLLIN, 27 }, { CMUX_PLLIN, 28 }},
  {{ CMUX_PLLIN, 5 }, { CMUX_PLLIN, 7 }, { CMUX_PLLIN, 0 }, { CMUX_OFF, 0 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_CLKPIN, 3 }, { CMUX_NCLKPIN, 3 }, { CMUX_PLLIN, 14 }, { CMUX_PLLIN, 16 }, { CMUX_PLLIN, 9 }, { CMUX_DEFAULT, 0 }, { CMUX_PLLIN, 22 }, { CMUX_PLLIN, 23 }, { CMUX_PLLIN, 29 }, { CMUX_PLLIN, 30 }},
};

const std::pair<uint8_t, uint8_t> mistral::CycloneV::cmuxcr_link_table[6][16] = {
  {{ CMUX_PLLIN, 0 }, { CMUX_PLLIN, 2 }, { CMUX_PLLIN, 4 }, { CMUX_PLLIN, 6 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
  {{ CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_PLLIN, 7 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
  {{ CMUX_PLLIN, 8 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
  {{ CMUX_PLLIN, 9 }, { CMUX_PLLIN, 11 }, { CMUX_PLLIN, 13 }, { CMUX_PLLIN, 15 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
  {{ CMUX_PLLIN, 16 }, { CMUX_PLLIN, 10 }, { CMUX_PLLIN, 12 }, { CMUX_PLLIN, 14 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 0 }, { CMUX_CLKIN, 1 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
  {{ CMUX_PLLIN, 17 }, { CMUX_PLLIN, 1 }, { CMUX_PLLIN, 3 }, { CMUX_PLLIN, 5 }, { CMUX_ICLK_SEL, 0 }, { CMUX_ICLK_SEL, 1 }, { CMUX_ICLK_SEL, 2 }, { CMUX_ICLK_SEL, 3 }, { CMUX_CLKPIN_SEL, 0 }, { CMUX_CLKPIN_SEL, 1 }, { CMUX_NCLKPIN_SEL, 0 }, { CMUX_NCLKPIN_SEL, 1 }, { CMUX_CLKIN, 2 }, { CMUX_CLKIN, 3 }, { CMUX_OFF, 0 }, { CMUX_DEFAULT, 0 }},
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

mistral::CycloneV::timing_slot_t mistral::CycloneV::timing_slot_lookup(const std::string &n) const
{
  for(int i = 0; i != T_COUNT; i++)
    if(n == timing_slot_names[i])
      return timing_slot_t(i);
  fprintf(stderr, "Incorrect timing slot name %s\n", n.c_str());
  exit(1);
}

mistral::CycloneV::edge_t mistral::CycloneV::edge_lookup(const std::string &n) const
{
  for(int i = 0; edge_names[i]; i++)
    if(n == edge_names[i])
      return edge_t(i);
  fprintf(stderr, "Incorrect edge name %s\n", n.c_str());
  exit(1);
}

mistral::CycloneV::delay_type_t mistral::CycloneV::delay_type_lookup(const std::string &n) const
{
  for(int i = 0; delay_type_names[i]; i++)
    if(n == delay_type_names[i])
      return delay_type_t(i);
  fprintf(stderr, "Incorrect delay type name %s\n", n.c_str());
  exit(1);
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
  inv_default_set();
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
      std::vector<uint16_t> *posvec = nullptr;
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

void mistral::CycloneV::validate_fw_bw() const
{
  for(const rnode_base *rnb = reinterpret_cast<const rnode_base *>(rnode_info); rnb != reinterpret_cast<const rnode_base *>(rnode_info_end); rnb = rnode_next(rnb)) {
    rnode_t rn = rnb->node;

    if(rnb->pattern == 0xff && rnb->target_count == 0) {
      printf("%s: unconnected node.\n", rn2s(rn).c_str());
      continue;
    }

    if(rnb->drivers[0] == 0xff && rn2t(rn) != GCLK && rn2t(rn) != RCLK && rn2t(rn) != GCLKFB && rn2t(rn) != RCLKFB) {
      printf("%s: missing driver information.\n", rn2s(rn).c_str());
    }

    {    
      // fw -> bw
      const rnode_target *rt = rnode_targets(rnb);
      const uint16_t *rtp = rnode_target_positions(rnb);
      for(int i=0; i != rnb->target_count; i++)
	if(!(rtp[i] & 0x8000)) {
	  rnode_t rnt = rt[i].rn;
	  const rnode_base *rntb = rnode_lookup(rnt);
	  if(!rntb) {
	    printf("%s: %s - forward node missing.\n", rn2s(rn).c_str(), rn2s(rnt).c_str());
	    continue;
	  }
	  const rnode_t *rs = rnode_sources(rntb);
	  int span = rntb->pattern == 0xff ? 0 : rntb->pattern == 0xfe ? 1 : rmux_patterns[rntb->pattern].span;
	  bool ok = false;
	  for(int j=0; !ok && j != span; j++)
	    ok = rs[j] == rn;
	  if(!ok)
	    printf("%s: %s - forward not found in backward.\n", rn2s(rn).c_str(), rn2s(rnt).c_str());
	}
    }

    {
      // bw -> fw
      const rnode_t *rs = rnode_sources(rnb);
      int span = rnb->pattern == 0xff ? 0 : rnb->pattern == 0xfe ? 1 : rmux_patterns[rnb->pattern].span;
      for(int i=0; i != span; i++) {
	rnode_t rns = rs[i];
	if(!rns)
	  continue;
	const rnode_base *rnsb = rnode_lookup(rns);
	if(!rnsb) {
	  printf("%s: %s - backward node missing.\n", rn2s(rn).c_str(), rn2s(rns).c_str());
	  continue;
	}
	const rnode_target *rst = rnode_targets(rnsb);
	const uint16_t *rstp = rnode_target_positions(rnsb);
	bool ok = false;
	for(int j=0; !ok && j != rnsb->target_count; j++)
	  ok = !(rstp[j] & 0x8000) && rst[j].rn == rn;
	if(!ok)
	    printf("%s: %s - backward not found in forward.\n", rn2s(rn).c_str(), rn2s(rns).c_str());
      }
    }
  }
}
