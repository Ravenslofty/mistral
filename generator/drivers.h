#ifndef DRIVERS_H
#define DRIVERS_H

#include "prefixtree.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <stdint.h>

enum speed_grade_t {
  SG_6,
  SG_6_H6,
  SG_7,
  SG_7_H5,
  SG_7_H5S,
  SG_7_H6,
  SG_8,
  SG_8_H6,
  SG_8_H7,
  SG_COUNT 
};

enum timing_slot {
  T_N55,
  T_N40,
  T_0,
  T_85,
  T_100,
  T_125,
  T_COUNT
};

enum {
  CRF_FALL,
  CRF_RISE
};

enum driver_type_t {
#define P(x) DRV_ ## x
#include <cv-drivertypes.ipp>
#undef P
  DRV_COUNT,
  DRV_GLOBALS = DRV_COUNT,
  DRV_TABLE2,
  DRV_TABLE3
};

enum shape_type_t {
#define P(x) SHP_ ## x
#include <cv-shapetypes.ipp>
#undef P
};

enum speed_info_t {
  SI_6,
  SI_7,
  SI_8,
  SI_M,
  SI_MS,
  SI_COUNT
};

enum driver_globals_t {
  DRVG_TIMING_SCALE,
  DRVG_VDD,
  DRVG_VCCH
};

struct dnode_table2 {
  float value[11*11];
};

struct dnode_table3 {
  float start;
  float value[11*11*11];
};

struct caps_t {
  float rf[2];
};

struct dnode_driver {
  uint8_t shape;
  uint8_t invert;
  uint16_t line_coalescing;
  uint16_t driver;
  uint16_t output;
  uint16_t pass1;
  uint16_t pass2;
  uint16_t pullup;
  uint16_t padding;
  caps_t cbuff;
  caps_t cg0_pass;
  caps_t cgd_buff;
  caps_t cgd_drive;
  caps_t cgd_pass;
  caps_t cgs_pass;
  caps_t cint;
  caps_t coff;
  caps_t con;
  caps_t cout;
  caps_t cstage1;
  caps_t cstage2;
  caps_t cwire;
  caps_t cor_factor;     // Not a caps, but rise/fall dependant
  caps_t min_cor_factor; // Same
  float rnor_pup;
  float rwire;
  float rmult;
};

struct dnode_info {
  double timing_scale;
  double vdd;
  double vcch;
  dnode_driver drivers[DRV_COUNT];
};

struct dnode_lookup {
  uint32_t index[SG_COUNT][T_COUNT][2]; // [speed grade][temperature][max=0,min=1]
};

class DriversParser {
public:
  static const char *const driver_type_names[];
  static const char *const shape_type_names[];
  static const char *const speed_info[];
  static const char *const sg_info[];
  static const char *const temp_info[];
  static const char *const info_types[];
  static const char *const table_types[];
  static const char *const globals_types[];

  std::vector<dnode_table2> table2;
  std::vector<dnode_table3> table3;
  std::vector<dnode_info> drivers;

  dnode_lookup lookup;

  DriversParser(const std::vector<uint8_t> &data);

private:
  PrefixTree speedmatch, tempmatch, drivermatch, shapematch, infomatch, tablematch, globalsmatch;

  void error(const uint8_t *st, const char *err = nullptr) const;

  dnode_info &di_get(int speed, int temp);
  dnode_driver &dd_get(int speed, int temp, int driver);
  uint16_t *tidx_get(int speed, int temp, const uint8_t *st, const uint8_t *&p);
};

#endif
