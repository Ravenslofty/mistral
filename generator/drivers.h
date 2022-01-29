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

enum lab_output_type {
  LAB_OUTPUTT_COMB,
  LAB_OUTPUTT_FF,
  LAB_OUTPUTT_COUNT
};

enum lab_output_connectivity {
  LAB_OUTPUTC_GLOBAL,
  LAB_OUTPUTC_LOCAL,
  LAB_OUTPUTC_COUNT
};

enum edge_type {
  EDGE_IO,
  EDGE_MLAB,
  EDGE_JTAG,
  EDGE_OSC,
  EDGE_CRC,
  EDGE_RUBLOCK,
  EDGE_TSD,
  EDGE_BIASGEN,
  EDGE_OCT,
  EDGE_DSP,
  EDGE_GCLK,
  EDGE_RCLK,
  EDGE_COUNT
};

enum edge_speed_type {
  EST_FAST,
  EST_SLOW,
  EST_COUNT
};

enum edge_t {
  RF_FALL,
  RF_RISE,
  RF_COUNT
};

enum driver_type_t {
#define P(x) DRV_ ## x
#include <cv-drivertypes.ipp>
#undef P
  DRV_COUNT,
  DRV_GLOBALS = DRV_COUNT,
  DRV_TABLE2,
  DRV_TABLE3,
  DRV_WAVE,
  DRV_EDGES
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
  SI_SS,
  SI_TT,
  SI_FF,
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

struct rf_t {
  float rf[RF_COUNT];
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
  rf_t cg0_pass;
  rf_t cgd_buff;
  rf_t cgd_drive;
  rf_t cgd_nand;
  rf_t cgd_pass;
  rf_t cgs_pass;
  rf_t cint;
  rf_t coff;
  rf_t con;
  rf_t cout;
  rf_t cstage1;
  rf_t cstage2;
  rf_t cwire;
  rf_t cor_factor;
  rf_t min_cor_factor;
  float rnor_pup;
  float rwire;
  float rmult;
};

struct input_waveform_info {
  struct {
    double time;
    double vdd;
  } wave[10];
};

struct dnode_info {
  double timing_scale;
  double vdd;
  double vcch;
  
  rf_t edges[EDGE_COUNT][EST_COUNT];
  input_waveform_info input_waveforms[LAB_OUTPUTT_COUNT][RF_COUNT][LAB_OUTPUTC_COUNT];
  dnode_driver drivers[DRV_COUNT];
};

struct dnode_lookup {
  uint32_t index_sg[SG_COUNT][T_COUNT][2]; // [speed grade][temperature][max=0,min=1]
  uint32_t index_si[SI_COUNT][T_COUNT];    // [speed info][temperature]
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
  static const char *const lab_output_type_info[];
  static const char *const lab_output_connectivity_info[];
  static const char *const edge_type_info[];
  static const char *const edge_speed_info[];
  static const char *const rise_fall_info[];

  std::vector<dnode_table2> table2;
  std::vector<dnode_table3> table3;
  std::vector<dnode_info> drivers;

  dnode_lookup lookup;

  DriversParser(const std::vector<uint8_t> &data);

private:
  PrefixTree speedmatch, tempmatch, drivermatch, shapematch, infomatch, tablematch, globalsmatch, lotmatch, locmatch, etmatch, esmatch, rfmatch;

  void error(const uint8_t *st, const char *err = nullptr) const;

  dnode_info &di_get(int speed, int temp);
  dnode_driver &dd_get(int speed, int temp, int driver);
  uint16_t *tidx_get(int speed, int temp, const uint8_t *st, const uint8_t *&p);
};

#endif
