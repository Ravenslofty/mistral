#include "drivers.h"
#include "io.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *const DriversParser::driver_type_names[] = {
#define P(x) #x
#include <cv-drivertypes.ipp>
#undef P
  "globals",
  "table2",
  "table3",
  "wave",
  "edges",
  nullptr
};

const char *const DriversParser::shape_type_names[] = {
#define P(x) #x
#include <cv-shapetypes.ipp>
#undef P
  nullptr
};

enum info_type_t {
  IT_cg0_pass,
  IT_cgd_buff,
  IT_cgd_drive,
  IT_cgd_pass,
  IT_cgs_pass,
  IT_cnand,
  IT_cint,
  IT_coff,
  IT_con,
  IT_cout,
  IT_cstage1,
  IT_cstage2,
  IT_cwire,
  IT_cor_factor,
  IT_min_cor_factor,
  IT_rnor_pup,
  IT_rwire,
  IT_rmult,
  IT_first_c = IT_cg0_pass,
  IT_last_c = IT_min_cor_factor,
  IT_first_r = IT_rnor_pup,
  IT_last_r = IT_rmult,
};

enum table_type_t {
  TT_driver,
  TT_output,
  TT_pass1,
  TT_pass2,
  TT_pullup
};

const char *const DriversParser::speed_info[] = { "6", "7", "8", "m", "ms", "ss", "tt", "ff", nullptr };

const char *const DriversParser::temp_info[] = { "n55", "n40", "0", "85", "100", "125", nullptr };

const char *const DriversParser::lab_output_type_info[] = { "comb", "ff", nullptr };

const char *const DriversParser::lab_output_connectivity_info[] = { "global", "local", nullptr };

const char *const DriversParser::edge_type_info[] = {
  "io",
  "mlab",
  "jtag",
  "osc",
  "crc",
  "rublock",
  "tsd",
  "biasgen",
  "oct",
  "dsp",
  "gclk",
  "rclk",
  nullptr
};

const char *const DriversParser::edge_speed_info[] = { "fast", "slow", nullptr };

const char *const DriversParser::rise_fall_info[] = { "fall", "rise", nullptr };

const char *const DriversParser::sg_info[] = {
  "6",
  "6_h6",
  "7",
  "7_h5",
  "7_h5s",
  "7_h6",
  "8",
  "8_h6",
  "8_h7",
  nullptr
};
  
const char *const DriversParser::table_types[] = {
  "driver",
  "output",
  "pass1",
  "pass2",
  "pullup",
  nullptr
};

const char *const DriversParser::globals_types[] = {
  "timing_scale",
  "vdd",
  "vcch",
  nullptr
};

const char *const DriversParser::info_types[] = {
  "cg0_pass",
  "cgd_buff",
  "cgd_drive",
  "cgd_pass",
  "cgs_pass",
  "cnand",
  "cint",
  "coff",
  "con",
  "cout",
  "cstage1",
  "cstage2",
  "cwire",
  "cor_factor",
  "min_cor_factor",
  "rnor_pup",
  "rwire",
  "rmult",
  nullptr
};


void DriversParser::error(const uint8_t *st, const char *err) const
{
  if(err)
    fprintf(stderr, "%s\n", err);
  const uint8_t *en = st;
  while(*en != '\n' && *en != '\r')
    en++;
  std::string line(st, en);
  fprintf(stderr, "line: %s\n", line.c_str());
  exit(1);
};

dnode_info &DriversParser::di_get(int speed, int temp)
{
  uint32_t idx = lookup.index_si[speed][temp];
  if(idx == 0xffffffff) {
    idx = drivers.size();
    drivers.resize(idx+1);
    memset(&drivers.back(), 0, sizeof(dnode_info));
    for(int i = 0; i != DRV_COUNT; i++) {
      auto &dn = drivers.back().drivers[i];
      dn.shape = 0xff;
      dn.rmult = 1.0;
      dn.driver = dn.output = dn.pass1 = dn.pass2 = dn.pullup = 0xffff;
    }
    lookup.index_si[speed][temp] = idx;

    switch(speed) {
    case SI_6:
      lookup.index_sg[SG_6][temp][0] = idx;
      lookup.index_sg[SG_6_H6][temp][0] = idx;
      break;
    case SI_7:
      lookup.index_sg[SG_7][temp][0] = idx;
      lookup.index_sg[SG_7_H5][temp][0] = idx;
      lookup.index_sg[SG_7_H5S][temp][0] = idx;
      lookup.index_sg[SG_7_H6][temp][0] = idx;
      break;
    case SI_8:
      lookup.index_sg[SG_8][temp][0] = idx;
      lookup.index_sg[SG_8_H6][temp][0] = idx;
      lookup.index_sg[SG_8_H7][temp][0] = idx;
      break;
    case SI_M:
      lookup.index_sg[SG_6][temp][1] = idx;
      lookup.index_sg[SG_6_H6][temp][1] = idx;
      lookup.index_sg[SG_7][temp][1] = idx;
      lookup.index_sg[SG_7_H5][temp][1] = idx;
      lookup.index_sg[SG_7_H6][temp][1] = idx;
      lookup.index_sg[SG_8][temp][1] = idx;
      lookup.index_sg[SG_8_H6][temp][1] = idx;
      lookup.index_sg[SG_8_H7][temp][1] = idx;
      break;
    case SI_MS:
      lookup.index_sg[SG_7_H5S][temp][1] = idx;
      break;
    default:
      break;
    }
  }
  return drivers[idx];
}

dnode_driver &DriversParser::dd_get(int speed, int temp, int driver)
{
  return di_get(speed, temp).drivers[driver];
}


uint16_t *DriversParser::tidx_get(int speed, int temp, const uint8_t *st, const uint8_t *&p)
{
  int driver = drivermatch.lookup(p);
  if(driver == -1 || *p++ != '.')
    error(st, "Bad driver name on table");
  int slot = tablematch.lookup(p);
  if(driver == -1 || *p != ' ')
    error(st, "Bad subtable name on table");
  
  auto &dd = dd_get(speed, temp, driver);
  uint16_t *ptidx = nullptr;
  switch(slot) {
  case TT_driver: ptidx = &dd.driver; break;
  case TT_output: ptidx = &dd.output; break;
  case TT_pass1: ptidx = &dd.pass1; break;
  case TT_pass2: ptidx = &dd.pass2; break;
  case TT_pullup: ptidx = &dd.pullup; break;
  default: fprintf(stderr, "table failure\n"); exit(1);
  }
  if(*ptidx != 0xffff)
    error(st, "Table collision");
  return ptidx;
}

DriversParser::DriversParser(const std::vector<uint8_t> &data) : 
  speedmatch(speed_info),
  tempmatch(temp_info),
  drivermatch(driver_type_names),
  shapematch(shape_type_names),
  infomatch(info_types),
  tablematch(table_types),
  globalsmatch(globals_types),
  lotmatch(lab_output_type_info),
  locmatch(lab_output_connectivity_info),
  etmatch(edge_type_info),
  esmatch(edge_speed_info),
  rfmatch(rise_fall_info)
{
  memset(&lookup, 0xff, sizeof(lookup));

  const uint8_t *p = data.data();
  const uint8_t *e = data.data() + data.size();

  while(p != e) {
    const uint8_t *st = p;
    skipsp(p);
    int speed = speedmatch.lookup(p);
    if(speed == -1 || *p != ' ')
      error(st, "Incorrect speed info");
    skipsp(p);
    int temp = tempmatch.lookup(p);
    if(temp == -1 || *p != ' ')
      error(st, "Incorrect temperature info");
    skipsp(p);
    int driver = drivermatch.lookup(p);
    if(driver == -1 || *p != ' ')
      error(st, "Incorrect driver info");
    skipsp(p);

    switch(driver) {
    case DRV_GLOBALS: {
      dnode_info &di = di_get(speed, temp);
      int global = globalsmatch.lookup(p);
      if(global == -1)
	error(st, "unknown global");
      skipsp(p);
      double value = lookup_float(p);
      skipsp(p);
      switch(global) {
      case DRVG_TIMING_SCALE: di.timing_scale = value; break;
      case DRVG_VDD: di.vdd = value; break;
      case DRVG_VCCH: di.vcch = value; break;
      }
      break;
    }

    case DRV_TABLE2: {
      *tidx_get(speed, temp, st, p) = table2.size();
      table2.resize(table2.size()+1);
      dnode_table2 &t = table2.back();
      memset(&t, 0, sizeof(t));
      for(int i=0; i != 11*11; i++) {
	if(*p != ' ')
	  error(st, "Missing numbers");
	skipsp(p);
	t.value[i] = lookup_float(p);
      }
      if(*p != '\r' && *p != '\n')
	error(st, "Extra stuff at the end");
      break;
    }
    case DRV_TABLE3: {
      *tidx_get(speed, temp, st, p) = table3.size();
      table3.resize(table3.size()+1);
      dnode_table3 &t = table3.back();
      memset(&t, 0, sizeof(t));
      skipsp(p);
      t.start = lookup_float(p);
      for(int i=0; i != 11*11*11; i++) {
	if(*p != ' ')
	  error(st, "Missing numbers");
	skipsp(p);
	t.value[i] = lookup_float(p);
      }
      if(*p != '\r' && *p != '\n')
	error(st, "Extra stuff at the end");
      break;
    }
    case DRV_WAVE: {
      dnode_info &di = di_get(speed, temp);
      int lot = lotmatch.lookup(p);
      if(lot == -1)
	error(st, "unknown lab output type");
      skipsp(p);
      int rf = rfmatch.lookup(p);
      if(rf == -1)
	error(st, "unknown rise/fall type");
      skipsp(p);
      int loc = locmatch.lookup(p);
      if(loc == -1)
	error(st, "unknown lab connectivity type");
      auto &wave = di.input_waveforms[lot][rf][loc];
      for(int i=0; i != 10; i++) {
	skipsp(p);
	wave.wave[i].time = lookup_float(p);
	skipsp(p);
	wave.wave[i].vdd = lookup_float(p);
      }
      if(*p != '\r' && *p != '\n')
	error(st, "Extra stuff at the end");
      break;
    }
    case DRV_EDGES: {
      dnode_info &di = di_get(speed, temp);
      int et = etmatch.lookup(p);
      if(et == -1)
	error(st, "unknown edge type");
      skipsp(p);
      di.edges[et][EST_SLOW].rf[RF_RISE] = lookup_float(p);
      skipsp(p);
      di.edges[et][EST_SLOW].rf[RF_FALL] = lookup_float(p);
      skipsp(p);
      di.edges[et][EST_FAST].rf[RF_RISE] = lookup_float(p);
      skipsp(p);
      di.edges[et][EST_FAST].rf[RF_FALL] = lookup_float(p);
      if(*p != '\r' && *p != '\n')
	error(st, "Extra stuff at the end");
      break;
    }
    default: {
      dnode_driver &dd = dd_get(speed, temp, driver);
      skipsp(p);
      if((*p != 'i' && *p != 'n') || p[1] != ' ')
	error(st, "Bad inversion flag");
      dd.invert = *p++ == 'i';
      skipsp(p);
      int line_coalescing = lookup_int(p);
      if(line_coalescing == -1 || *p != ' ')
	error(st, "Bad coalescing");
      dd.line_coalescing = line_coalescing;
      skipsp(p);
      int shape = shapematch.lookup(p);
      if(shape == -1 || *p != ' ')
	error(st, "Bad shape");
      dd.shape = shape;
      for(;;) {
	skipsp(p);
	int key = infomatch.lookup(p);
	if(key == -1 || *p != '=')
	  error(st, "unknown key");
	p++;
	if(key >= IT_first_c && key <= IT_last_c) {
	  rf_t cc;
	  cc.rf[RF_RISE] = lookup_float(p);
	  if(*p == '/') {
	    p++;
	    cc.rf[RF_FALL] = lookup_float(p);
	  } else
	    cc.rf[RF_FALL] = cc.rf[RF_RISE];

	  switch(key) {
	  case IT_cg0_pass: dd.cg0_pass = cc; break;
	  case IT_cgd_buff: dd.cgd_buff = cc; break;
	  case IT_cgd_drive: dd.cgd_drive = cc; break;
	  case IT_cgd_pass: dd.cgd_pass = cc; break;
	  case IT_cgs_pass: dd.cgs_pass = cc; break;
	  case IT_cnand: dd.cnand = cc; break;
	  case IT_cint: dd.cint = cc; break;
	  case IT_coff: dd.coff = cc; break;
	  case IT_con: dd.con = cc; break;
	  case IT_cout: dd.cout = cc; break;
	  case IT_cstage1: dd.cstage1 = cc; break;
	  case IT_cstage2: dd.cstage2 = cc; break;
	  case IT_cwire: dd.cwire = cc; break;
	  case IT_cor_factor: dd.cor_factor = cc; break;
	  case IT_min_cor_factor: dd.min_cor_factor = cc; break;
	  default: fprintf(stderr, "caps failure\n"); exit(1);
	  }
	} else {
	  float r = lookup_float(p);
	  switch(key) {
	  case IT_rnor_pup: dd.rnor_pup = r; break;
	  case IT_rwire: dd.rwire = r; break;
	  case IT_rmult: dd.rmult = r; break;
	  default: fprintf(stderr, "res failure\n"); exit(1);
	  }
	}
	if(*p != ' ' && *p != '\r' && *p != '\n')
	  error(st, "Crap in the line");
	if(*p != ' ')
	  break;
      }
      break;
    }
    }

    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "Broken EOL");
    p++;
  }

  // seu die 125 min info is missing for a part of the drivers, use the non-seu info instead
  const dnode_info &dis = drivers[lookup.index_sg[SG_7][T_125][1]];
  dnode_info &did = drivers[lookup.index_sg[SG_7_H5S][T_125][1]];
  for(int l = 0; l != DRV_COUNT; l++)
    if(did.drivers[l].shape == 0xff)
      memcpy(&did.drivers[l], &dis.drivers[l], sizeof(dnode_driver));

  // seu die -55 min info is fully missing, use non-seu instead
  lookup.index_sg[SG_7_H5S][T_N55][1] = lookup.index_sg[SG_7][T_N55][1];
  lookup.index_si[SI_MS][T_N55] = lookup.index_si[SI_M][T_N55];

  bool bad = false;
  for(int i=0; i != SG_COUNT; i++)
    for(int j=0; j != T_COUNT; j++)
      for(int k=0; k != 2; k++)
	if(lookup.index_sg[i][j][k] == 0xffffffff) {
	  fprintf(stderr, "Missing %s %s %s\n", sg_info[i], temp_info[j], k ? "min" : "max");
	  bad = true;
	} else {
	  const dnode_info &di = drivers[lookup.index_sg[i][j][k]];
	  for(int l = 0; l != DRV_COUNT; l++)
	    if(di.drivers[l].shape == 0xff) {
	      fprintf(stderr, "Missing %s %s %s %s\n", sg_info[i], temp_info[j], k ? "min" : "max", driver_type_names[l]);
	      bad = true;		
	    }
	}

  if(bad)
    exit(1);		  
}

