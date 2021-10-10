#undef _FORTIFY_SOURCE

/*
e50   5CEBA4F23C7
gt75  5CGTFD5C5F23I7
gt150 5CGTFD7C5F23I7
gt300 5CGTFD9C5F23I7
gx25  5CGXFC3B6F23C6
sx50  5CSEBA4U23I7
sx120 5CSEBA6U23I7
*/

#include "cyclonev.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void file_load(const char *fname, uint8_t *&data, uint32_t &size)
{
  char msg[4096];
  sprintf(msg, "Open %s", fname);
  FILE *fd = fopen(fname, "r");
  if(!fd) {
    perror(msg);
    exit(2);
  }

  fseek(fd, 0, SEEK_END);
  size = ftell(fd);
  rewind(fd);
  data = (uint8_t *)malloc(size);
  fread(data, 1, size, fd);
  fclose(fd);
}

void file_save(const char *fname, const uint8_t *data, uint32_t size)
{
  char msg[4096];
  sprintf(msg, "Open %s", fname);
  FILE *fd = fopen(fname, "w");
  if(!fd) {
    perror(msg);
    exit(2);
  }

  fwrite(data, 1, size, fd);
  fclose(fd);
}

static void show_models(char **args)
{
  printf("Model                Idcode  Die      Variant  Package    Temp.  Speed\n");
  for(const auto *m = mistral::CycloneV::models; m->name; m++) {
    const auto &pkg = mistral::CycloneV::package_infos[m->package];
    printf("%-20s %04x    %-8s %-8s %c%2d %4d   %c      %d\n",
	   m->name,
	   m->variant.idcode, m->variant.die.name, m->variant.name,
	   pkg.type, pkg.width_in_mm, pkg.pin_count,
	   m->temperature, m->speed);
  }
}

static void show_cycle(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *rbf;
  uint32_t rbfsize;
  file_load(args[1], rbf, rbfsize);

  model->rbf_load(rbf, rbfsize);
  free(rbf);

  std::vector<uint8_t> res;
  model->rbf_save(res);

  delete model;

  file_save(args[2], res.data(), res.size());
}

static void show_routes(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *rbf;
  uint32_t rbfsize;
  file_load(args[1], rbf, rbfsize);

  model->rbf_load(rbf, rbfsize);
  free(rbf);

  auto links = model->route_frontier_links();
  for(const auto &l : links) {
    auto s = model->rnode_to_pnode(l.first);
    auto d = model->rnode_to_pnode(l.second);
    printf("%s %s",
	   s ? mistral::CycloneV::pn2s(s).c_str() : mistral::CycloneV::rn2s(l.first).c_str(),
	   d ? mistral::CycloneV::pn2s(d).c_str() : mistral::CycloneV::rn2s(l.second).c_str());
    bool has_comment = false;
    const mistral::CycloneV::pin_info_t *pin;
    pin = model->pin_find_pnode(s);
    if(pin) {
      printf(" ; %s", pin->name);
      has_comment = true;
    }
    pin = model->pin_find_pnode(d);
    if(pin) {
      if(!has_comment) {
	printf(" ;");
	has_comment = true;
      }
      printf(" %s", pin->name);
    }
    printf("\n");
  }

  delete model;
}

static void show_routesp(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *rbf;
  uint32_t rbfsize;
  file_load(args[1], rbf, rbfsize);

  model->rbf_load(rbf, rbfsize);
  free(rbf);

  auto links = model->route_frontier_links_with_path();
  for(const auto &path : links) {
    std::string p;
    for(auto rn : path) {
      if(!p.empty())
	p += ' ';
      auto pn = model->rnode_to_pnode(rn);
      p += pn ? mistral::CycloneV::pn2s(pn) : mistral::CycloneV::rn2s(rn);
    }
    printf("%s", p.c_str());
    auto s = model->rnode_to_pnode(path.front());
    auto d = model->rnode_to_pnode(path.back());
    bool has_comment = false;
    const mistral::CycloneV::pin_info_t *pin;
    pin = model->pin_find_pnode(s);
    if(pin) {
      printf(" ; %s", pin->name);
      has_comment = true;
    }
    pin = model->pin_find_pnode(d);
    if(pin) {
      if(!has_comment) {
	printf(" ;");
	has_comment = true;
      }
      printf(" %s", pin->name);
    }
    printf("\n");
  }

  delete model;
}

static void show_routes2(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *rbf;
  uint32_t rbfsize;
  file_load(args[1], rbf, rbfsize);

  model->rbf_load(rbf, rbfsize);
  free(rbf);

  auto links = model->route_frontier_links();
  for(const auto &l : links) {
    auto s = model->rnode_to_pnode(l.first);
    auto d = model->rnode_to_pnode(l.second);
    if(!(s&&d))
    printf("%s %s\n",
	   s ? mistral::CycloneV::pn2s(s).c_str() : mistral::CycloneV::rn2s(l.first).c_str(),
	   d ? mistral::CycloneV::pn2s(d).c_str() : mistral::CycloneV::rn2s(l.second).c_str());
  }

  delete model;
}

static void show_bels_1(const std::vector<uint16_t> &pos, const char *name)
{
  for(uint16_t p : pos)
    printf("%2d %2d %s\n", mistral::CycloneV::pos2x(p), mistral::CycloneV::pos2y(p), name);
}

static const char *const hps_blocks[mistral::CycloneV::I_HPS_COUNT] = {
  "hps_boot",
  "hps_clocks",
  "hps_clocks_resets",
  "hps_cross_trigger",
  "hps_dbg_apb",
  "hps_dma",
  "hps_fpga2hps",
  "hps_fpga2sdram",
  "hps_hps2fpga",
  "hps_hps2fpga_light_weight",
  "hps_interrupts",
  "hps_jtag",
  "hps_loan_io",
  "hps_mpu_event_standby",
  "hps_mpu_general_purpose",
  "hps_peripheral_can",
  "hps_peripheral_can",
  "hps_peripheral_emac",
  "hps_peripheral_emac",
  "hps_peripheral_i2c",
  "hps_peripheral_i2c",
  "hps_peripheral_i2c",
  "hps_peripheral_i2c",
  "hps_peripheral_nand",
  "hps_peripheral_qspi",
  "hps_peripheral_sdmmc",
  "hps_peripheral_spi_master",
  "hps_peripheral_spi_master",
  "hps_peripheral_spi_slave",
  "hps_peripheral_spi_slave",
  "hps_peripheral_uart",
  "hps_peripheral_uart",
  "hps_peripheral_usb",
  "hps_peripheral_usb",
  "hps_stm_event",
  "hps_test",
  "hps_tpiu_trace",
};

static void show_bels(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  show_bels_1(model->lab_get_pos(),    "lab");
  show_bels_1(model->mlab_get_pos(),   "mlab");
  show_bels_1(model->m10k_get_pos(),   "m10k");
  show_bels_1(model->dsp_get_pos(),    "dsp");
  show_bels_1(model->gpio_get_pos(),   "gpio");
  show_bels_1(model->dqs16_get_pos(),  "dqs16");
  show_bels_1(model->fpll_get_pos(),   "fpll");
  show_bels_1(model->cmuxc_get_pos(),  "cmuxc");
  show_bels_1(model->cmuxv_get_pos(),  "cmuxv");
  show_bels_1(model->cmuxh_get_pos(),  "cmuxh");
  show_bels_1(model->dll_get_pos(),    "dll");
  show_bels_1(model->hssi_get_pos(),   "hssi");
  show_bels_1(model->cbuf_get_pos(),   "cbuf");
  show_bels_1(model->lvl_get_pos(),    "lvl");
  show_bels_1(model->ctrl_get_pos(),   "ctrl");
  show_bels_1(model->pma3_get_pos(),   "pma3");
  show_bels_1(model->serpar_get_pos(), "serpar");
  show_bels_1(model->term_get_pos(),   "term");
  show_bels_1(model->hip_get_pos(),    "hip");
  show_bels_1(model->hmc_get_pos(),    "hmc");

  const auto &hps = model->hps_get_pos();

  if(!hps.empty())
    for(int i=0; i != mistral::CycloneV::I_HPS_COUNT; i++)
      printf("%2d %2d %s\n", mistral::CycloneV::pos2x(hps[i]), mistral::CycloneV::pos2y(hps[i]), hps_blocks[i]);

  delete model;
}

static void show_p2r(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  auto r = model->get_all_p2r();
  for(const auto &e : r)
    printf("%s %s\n", mistral::CycloneV::pn2s(e.first).c_str(), mistral::CycloneV::rn2s(e.second).c_str());

  delete model;
}

static void show_p2p(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  auto r = model->get_all_p2p();
  for(const auto &e : r)
    printf("%s %s\n", mistral::CycloneV::pn2s(e.first).c_str(), mistral::CycloneV::pn2s(e.second).c_str());

  delete model;
}

static void show_p2ri(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  auto r = model->get_all_p2ri();
  for(const auto &e : r) {
    if(model->pnode_to_rnode(e.first) != e.second || model->rnode_to_pnode(e.second) != e.first) {
      printf("ERROR ");
      int br = model->pnode_to_rnode(e.first);
      if(br)
	printf("%s ", mistral::CycloneV::rn2s(br).c_str());
      else
	printf("- ");
    }
    printf("%s %s\n", mistral::CycloneV::pn2s(e.first).c_str(), mistral::CycloneV::rn2s(e.second).c_str());
  }

  delete model;
}

static void decompile(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *rbf;
  uint32_t rbfsize;
  file_load(args[1], rbf, rbfsize);

  model->rbf_load(rbf, rbfsize);
  free(rbf);

  char msg[4096];
  sprintf(msg, "Open %s", args[2]);
  FILE *fd = fopen(args[2], "w");
  if(!fd) {
    perror(msg);
    exit(2);
  }

  fprintf(fd, "m %s\n", model->current_model()->name);

  for(const auto &s : model->opt_get())
    if(!s.def) {
      fprintf(fd, "o %s ", mistral::CycloneV::bmux_type_names[s.mux]);
      switch(s.type) {
      case mistral::CycloneV::MT_MUX: fprintf(fd, "%s", mistral::CycloneV::bmux_type_names[s.s]); break;
      case mistral::CycloneV::MT_NUM: fprintf(fd, "%d", s.s); break;
      case mistral::CycloneV::MT_BOOL: fprintf(fd, "%d", s.s); break;
      case mistral::CycloneV::MT_RAM: {
	if(((s.s-1) & 7) >= 4)
	  fprintf(fd, "%02x", s.r[(s.s-1) >> 3]);
	else
	  fprintf(fd, "%x", s.r[(s.s-1) >> 3]);
	for(int b = ((s.s - 1) >> 3) - 1; b >= 0; b--) {
	  if((b & 3) == 3)
	    fprintf(fd, ".");
	  fprintf(fd, "%02x", s.r[b]);
	}
	break;
      }
      }
      fprintf(fd, "\n");
    }

  auto links = model->route_all_active_links();
  for(const auto &l : links) {
    auto s = model->rnode_to_pnode(l.first);
    auto d = model->rnode_to_pnode(l.second);
    fprintf(fd, "r %s %s",
	    s ? mistral::CycloneV::pn2s(s).c_str() : mistral::CycloneV::rn2s(l.first).c_str(),
	    d ? mistral::CycloneV::pn2s(d).c_str() : mistral::CycloneV::rn2s(l.second).c_str());
    bool has_comment = false;
    const mistral::CycloneV::pin_info_t *pin;
    pin = model->pin_find_pnode(s);
    if(pin) {
      fprintf(fd, " ; %s", pin->name);
      has_comment = true;
    }      
    pin = model->pin_find_pnode(d);
    if(pin) {
      if(!has_comment) {
	fprintf(fd, " ;");
	has_comment = true;
      }
      fprintf(fd, " %s", pin->name);
    }      
    fprintf(fd, "\n");
  }
  
  for(const auto &s : model->bmux_get())
    if(!s.def) {
      fprintf(fd, "s %s.%03d.%03d:", mistral::CycloneV::block_type_names[s.btype], mistral::CycloneV::pos2x(s.pos), mistral::CycloneV::pos2y(s.pos));
      if(s.midx == -1)
	fprintf(fd, "%s ", mistral::CycloneV::bmux_type_names[s.mux]);
      else
	fprintf(fd, "%s.%d ", mistral::CycloneV::bmux_type_names[s.mux], s.midx);
      switch(s.type) {
      case mistral::CycloneV::MT_MUX: fprintf(fd, "%s", mistral::CycloneV::bmux_type_names[s.s]); break;
      case mistral::CycloneV::MT_NUM: fprintf(fd, "%d", s.s); break;
      case mistral::CycloneV::MT_BOOL: fprintf(fd, "%d", s.s); break;
      case mistral::CycloneV::MT_RAM: {
	if(((s.s-1) & 7) >= 4)
	  fprintf(fd, "%02x", s.r[(s.s-1) >> 3]);
	else
	  fprintf(fd, "%x", s.r[(s.s-1) >> 3]);
	for(int b = ((s.s - 1) >> 3) - 1; b >= 0; b--) {
	  if((b & 3) == 3)
	    fprintf(fd, ".");
	  fprintf(fd, "%02x", s.r[b]);
	}
	break;
      }
      }
      if(s.btype == mistral::CycloneV::GPIO && s.midx != -1) {
	auto pin = model->pin_find_pos(s.pos, s.midx);
	if(pin)
	  fprintf(fd, " ; %s", pin->name);
      }
      fprintf(fd, "\n");
    }

  for(const auto &s : model->inv_get())
    if(!s.def || !model->rnode_to_pnode(s.node)) {
      auto p = model->rnode_to_pnode(s.node);
      fprintf(fd, "i %s %d",
	      p ? mistral::CycloneV::pn2s(p).c_str() : mistral::CycloneV::rn2s(s.node).c_str(), s.value);
      auto pin = model->pin_find_pnode(p);
      if(pin)
	fprintf(fd, " ; %s", pin->name);
      fprintf(fd, "\n");
    }
  
  delete model;
}

static std::vector<std::string> dotsplit(std::string::const_iterator s, std::string::const_iterator e)
{
  std::vector<std::string> res;
  while(s != e) {
    auto c = s;
    while(s != e && *s != '.')
      s++;
    res.emplace_back(std::string(c, s));
    if(s != e)
      s++;
  }
  return res;
}

static mistral::CycloneV::rnode_t get_rnode(mistral::CycloneV *model, std::string s, const char *file, int line)
{
  auto pp = s.find(':');
  if(pp == std::string::npos) {
    // rnode
    std::vector<std::string> sp = dotsplit(s.begin(), s.end());
    if(sp.size() != 4) {
      fprintf(stderr, "%s:%d: rnode %s has incorrect structure, RTYPE.x.y.z expected\n", file, line, s.c_str());
      exit(1);
    }
    auto rt = model->rnode_type_lookup(sp[0]);
    if(!rt) {
      fprintf(stderr, "%s:%d: rnode %s has unknown route node type\n", file, line, s.c_str());
      exit(1);
    }
    uint32_t x = strtol(sp[1].c_str(), 0, 10);
    uint32_t y = strtol(sp[2].c_str(), 0, 10);
    uint32_t z = strtol(sp[3].c_str(), 0, 10);

    return mistral::CycloneV::rnode(rt, x, y, z);

  } else {
    // pnode
    std::vector<std::string> sp = dotsplit(s.begin(), s.begin() + pp);
    if(sp.size() != 3 && sp.size() != 4) {
      fprintf(stderr, "%s:%d: pnode %s has incorrect structure, BLOCK.x.y:* or BLOCK.x.y.z:* expected\n", file, line, s.c_str());
      exit(1);
    }
    auto bt = model->block_type_lookup(sp[0]);
    if(!bt) {
      fprintf(stderr, "%s:%d: pnode %s has unknown block type\n", file, line, s.c_str());
      exit(1);
    }
    uint32_t x = strtol(sp[1].c_str(), 0, 10);
    uint32_t y = strtol(sp[2].c_str(), 0, 10);
    int8_t bi = sp.size() == 4 ? strtol(sp[3].c_str(), 0, 10) : -1;

    sp = dotsplit(s.begin() + pp + 1, s.end());
    if(sp.size() != 1 && sp.size() != 2) {
      fprintf(stderr, "%s:%d: pnode %s has incorrect structure, *:PORT or *:PORT.id expected\n", file, line, s.c_str());
      exit(1);
    }
    auto pt = model->port_type_lookup(sp[0]);
    if(!pt) {
      fprintf(stderr, "%s:%d: pnode %s has unknown port type\n", file, line, s.c_str());
      exit(1);
    }
    int16_t pi = sp.size() == 2 ? strtol(sp[1].c_str(), 0, 10) : -1;

    auto p = mistral::CycloneV::pnode(bt, x, y, pt, bi, pi);
    auto r = model->pnode_to_rnode(p);
    if(!r) {
      fprintf(stderr, "%s:%d: pnode %s has no associated rnode\n", file, line, s.c_str());
      exit(1);
    }
    return r;
  }
}

static void compile(char **args)
{
  uint8_t *src;
  uint32_t srcsize;
  file_load(args[0], src, srcsize);

  mistral::CycloneV *model = nullptr;

  uint8_t *p = src;
  uint8_t *e = src + srcsize;
  int line = 1;
  while(p != e) {
    std::vector<std::string> segments;
    while(p != e) {
      while(p != e && (*p == ' ' || *p == '\t' || *p == '\r'))
	p++;
      if(p != e && *p == ';')
	while(p != e && *p != '\n')
	  p++;
      if(p != e && *p != '\n') {
	const char *s = (const char *)p;
	while(p != e && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ';')
	  p++;
	segments.emplace_back(std::string(s, (const char *)p));
      }
      if(p != e && *p == '\n') {
	p++;
	break;
      }
    }
    if(segments.empty()) {
      line++;
      continue;
    }
    if(segments[0] == "m") {
      if(model) {
	fprintf(stderr, "%s:%d: Error: Model already set\n", args[0], line);
	exit(1);
      }
      if(segments.size() != 2) {
	fprintf(stderr, "%s:%d: Error: 'm' takes only one parameter, the model name\n", args[0], line);
	exit(1);
      }	
      model = mistral::CycloneV::get_model(segments[1]);
      if(!model) {
	fprintf(stderr, "%s:%d: Error: Model %s unsupported\n", args[0], line, segments[1].c_str());
	exit(1);
      }
    } else if(segments[0] == "o") {
      if(!model) {
	fprintf(stderr, "%s:%d: Error: 'o' without a model previously set\n", args[0], line);
	exit(1);
      }

      if(segments.size() != 3) {
	fprintf(stderr, "%s:%d: Error: 'o' takes two parameters, option and value\n", args[0], line);
	exit(1);
      }	

      auto mux = model->bmux_type_lookup(segments[1]);
      if(!mux) {
	fprintf(stderr, "%s:%d: option %s does not exist\n", args[0], line, segments[1].c_str());
	exit(1);
      }

      int mtype = model->opt_type(mux);
      if(mtype < 0) {
	fprintf(stderr, "%s:%d: option %s does not exist\n", args[0], line, segments[1].c_str());
	exit(1);
      }

      switch(mtype) {
      case mistral::CycloneV::MT_MUX: {
	auto mval = model->bmux_type_lookup(segments[2]);
	if(!mval) {
	  fprintf(stderr, "%s:%d: option %s has unknown choice %s\n", args[0], line, segments[1].c_str(), segments[2].c_str());
	  exit(1);
	}
	if(!model->opt_m_set(mux, mval)) {
	  fprintf(stderr, "%s:%d: option %s has invalid choice %s\n", args[0], line, segments[1].c_str(), segments[2].c_str());
	  exit(1);
	}
	break;
      }

      case mistral::CycloneV::MT_NUM: {
	auto mval = strtol(segments[2].c_str(), nullptr, 0);
	if(!model->opt_n_set(mux, mval)) {
	  fprintf(stderr, "%s:%d: option %s has invalid numeric value %s\n", args[0], line, segments[1].c_str(), segments[2].c_str());
	  exit(1);
	}
	break;
      }

      case mistral::CycloneV::MT_BOOL: {
	if(segments[2] != "0" && segments[2] != "1") {
	  fprintf(stderr, "%s:%d: option %s has invalid bool %s\n", args[0], line, segments[1].c_str(), segments[2].c_str());
	  exit(1);
	}
	model->opt_b_set(mux, segments[2] == "1");
	break;
      }

      case mistral::CycloneV::MT_RAM: {
	std::string nibbles;
	for(char c : segments[2]) {
	  if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
	    nibbles += c;
	  else if(c != '.') {
	    fprintf(stderr, "%s:%d: option %s has invalid ram contents %s\n", args[0], line, segments[1].c_str(), segments[2].c_str());
	    exit(1);
	  }
	}
	int len = nibbles.size();
	if(len & 1) {
	  nibbles = '0' + nibbles;
	  len ++;
	}
	len >>= 1;
	char h[3];
	h[2] = 0;
	std::vector<uint8_t> s(len);
	for(int i=0; i != len; i++) {
	  h[0] = nibbles[(len-i-1)*2];
	  h[1] = nibbles[(len-i-1)*2+1];
	  s[i] = strtol(h, nullptr, 16);
	}
	model->opt_r_set(mux, s);
	break;
      }	
      }

    } else if(segments[0] == "r") {
      if(!model) {
	fprintf(stderr, "%s:%d: Error: 'r' without a model previously set\n", args[0], line);
	exit(1);
      }

      if(segments.size() != 3) {
	fprintf(stderr, "%s:%d: Error: 'r' takes two parameters, source and destination\n", args[0], line);
	exit(1);
      }	

      auto s = get_rnode(model, segments[1], args[0], line);
      auto d = get_rnode(model, segments[2], args[0], line);
      model->rnode_link(s, d);

    } else if(segments[0] == "s") {
      if(!model) {
	fprintf(stderr, "%s:%d: Error: 's' without a model previously set\n", args[0], line);
	exit(1);
      }

      if(segments.size() != 3) {
	fprintf(stderr, "%s:%d: Error: 's' takes two parameters, mux and value\n", args[0], line);
	exit(1);
      }	

      // bmux
      const std::string &s = segments[1];
      auto pp = s.find(':');
      if(pp == std::string::npos) {
	fprintf(stderr, "%s:%d: block mux %s has incorrect structure, BLOCK.x.y:MUX(.i) expected\n", args[0], line, s.c_str());
	exit(1);
      }

      std::vector<std::string> sp = dotsplit(s.begin(), s.begin() + pp);
      if(sp.size() != 3) {
	fprintf(stderr, "%s:%d: block mux %s has incorrect structure, BLOCK.x.y:* expected\n", args[0], line, s.c_str());
	exit(1);
      }

      auto bt = model->block_type_lookup(sp[0]);
      if(!bt) {
	fprintf(stderr, "%s:%d: block mux %s has unknown block type\n", args[0], line, s.c_str());
	exit(1);
      }
      uint32_t x = strtol(sp[1].c_str(), 0, 10);
      uint32_t y = strtol(sp[2].c_str(), 0, 10);

      sp = dotsplit(s.begin() + pp + 1, s.end());
      if(sp.size() != 1 && sp.size() != 2) {
	fprintf(stderr, "%s:%d: block mux %s has incorrect structure, *:MUX or *:MUX.id expected\n", args[0], line, s.c_str());
	exit(1);
      }
      auto mux = model->bmux_type_lookup(sp[0]);
      if(!mux) {
	fprintf(stderr, "%s:%d: block mux %s has unknown mux type\n", args[0], line, s.c_str());
	exit(1);
      }
      int midx = sp.size() == 2 ? strtol(sp[1].c_str(), 0, 10) : 0;

      auto pos = mistral::CycloneV::xy2pos(x, y);
      int mtype = model->bmux_type(bt, pos, mux, midx);
      if(mtype < 0) {
	fprintf(stderr, "%s:%d: block mux %s does not exist\n", args[0], line, s.c_str());
	exit(1);
      }

      switch(mtype) {
      case mistral::CycloneV::MT_MUX: {
	auto mval = model->bmux_type_lookup(segments[2]);
	if(!mval) {
	  fprintf(stderr, "%s:%d: block mux %s has unknown choice %s\n", args[0], line, s.c_str(), segments[2].c_str());
	  exit(1);
	}
	if(!model->bmux_m_set(bt, pos, mux, midx, mval)) {
	  fprintf(stderr, "%s:%d: block mux %s has invalid choice %s\n", args[0], line, s.c_str(), segments[2].c_str());
	  exit(1);
	}
	break;
      }

      case mistral::CycloneV::MT_NUM: {
	auto mval = strtol(segments[2].c_str(), nullptr, 0);
	if(!model->bmux_n_set(bt, pos, mux, midx, mval)) {
	  fprintf(stderr, "%s:%d: block mux %s has invalid numeric value %s\n", args[0], line, s.c_str(), segments[2].c_str());
	  exit(1);
	}
	break;
      }

      case mistral::CycloneV::MT_BOOL: {
	if(segments[2] != "0" && segments[2] != "1") {
	  fprintf(stderr, "%s:%d: block mux %s has invalid bool %s\n", args[0], line, s.c_str(), segments[2].c_str());
	  exit(1);
	}
	model->bmux_b_set(bt, pos, mux, midx, segments[2] == "1");
	break;
      }

      case mistral::CycloneV::MT_RAM: {
	std::string nibbles;
	for(char c : segments[2]) {
	  if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
	    nibbles += c;
	  else if(c != '.') {
	    fprintf(stderr, "%s:%d: block mux %s has invalid ram contents %s\n", args[0], line, s.c_str(), segments[2].c_str());
	    exit(1);
	  }
	}
	int len = nibbles.size();
	if(len & 1) {
	  nibbles = '0' + nibbles;
	  len ++;
	}
	len >>= 1;
	char h[3];
	h[2] = 0;
	std::vector<uint8_t> s(len);
	for(int i=0; i != len; i++) {
	  h[0] = nibbles[(len-i-1)*2];
	  h[1] = nibbles[(len-i-1)*2+1];
	  s[i] = strtol(h, nullptr, 16);
	}
	model->bmux_r_set(bt, pos, mux, midx, s);
	break;
      }	
      }

    } else if(segments[0] == "i") {
      if(!model) {
	fprintf(stderr, "%s:%d: Error: 'i' without a model previously set\n", args[0], line);
	exit(1);
      }

      if(segments.size() != 3) {
	fprintf(stderr, "%s:%d: Error: 'i' takes two parameters, node and setting\n", args[0], line);
	exit(1);
      }	

      if(segments[2] != "0" && segments[2] != "1") {
	fprintf(stderr, "%s:%d: Error: 'i' value has invalid bool %s\n", args[0], line, segments[2].c_str());
	exit(1);
      }

      auto s = get_rnode(model, segments[1], args[0], line);
      bool value = segments[2][0] == '1';
      if(!model->inv_set(s, value)) {
	fprintf(stderr, "%s:%d: Error: node %s has no associated inverter\n", args[0], line, segments[1].c_str());
	exit(1);	
      }

    } else {
      fprintf(stderr, "%s:%d: Error: Keyword %s unknown\n", args[0], line, segments[0].c_str());
      exit(1);
    }

    line++;
  }

  free(src);

  std::vector<uint8_t> res;
  model->rbf_save(res);

  delete model;

  file_save(args[1], res.data(), res.size());
}

static void diff(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  auto model2 = mistral::CycloneV::get_model(args[0]);

  uint8_t *rbf;
  uint32_t rbfsize;

  file_load(args[1], rbf, rbfsize);
  model->rbf_load(rbf, rbfsize);
  free(rbf);

  file_load(args[2], rbf, rbfsize);
  model2->rbf_load(rbf, rbfsize);
  free(rbf);

  model->diff(model2);
  delete model;
  delete model2;
}

static void missing(char **args)
{
  auto model = mistral::CycloneV::get_model(args[0]);
  if(!model) {
    fprintf(stderr, "Error: model %s unsupported\n", args[0]);
    exit(1);
  }

  uint8_t *links;
  uint32_t linkssize;
  file_load(args[1], links, linkssize);

  uint8_t *p = links;
  uint8_t *e = links + linkssize;
  int line = 1;
  while(p != e) {
    uint8_t *s = p;
    while(p != e && *p != '\r' && *p != '\n')
      p++;
    if(p != s) {
      uint8_t *le = p;
      uint8_t *pp = s;
      while(pp != le && *pp != ' ')
	pp++;
      auto n = get_rnode(model, std::string(s, pp), args[1], line);
      if(!model->rnode_to_pnode(n))
	printf("%s\n", std::string(s, le).c_str());
    }
    while(p != e && (*p == '\r' || *p == '\n'))
      p++;
    line++;
  }

  delete model;
}

struct fct {
  const char *name;
  int pmin, pmax;
  void (*f)(char **args);
  const char *help;
};

static const fct fcts[] = {
  { "models",   0, 0, show_models,   "models                          -- Dump the list of known CycloneV models" },
  { "routes",   2, 2, show_routes,   "routes   model file.rbf         -- Dump the information of all active routes" },
  { "routes2",  2, 2, show_routes2,  "routes2  model file.rbf         -- Dump the information of all unresolved active routes" },
  { "routesp",  2, 2, show_routesp,  "routesp  model file.rbf         -- Dump the information of all active routes with intermediate pips" },
  { "cycle",    3, 3, show_cycle,    "cycle    model file.rbf n.rbf   -- Load a rbf and save it again" },
  { "bels",     1, 1, show_bels,     "bels     model                  -- Dump the list of logic blocks for a given model" },
  { "p2r",      1, 1, show_p2r,      "p2r      model                  -- Dump the list of block port/routing nodes connections for peripheral blocks" },
  { "p2ri",     1, 1, show_p2ri,     "p2ri     model                  -- Dump the list of block port/routing nodes connections for one tile per inner block" },
  { "p2p",      1, 1, show_p2p,      "p2p      model                  -- Dump the list of block port/block port connections" },
  { "decomp",   3, 3, decompile,     "decomp   model file.rbf out.bt  -- Decompile the bitstream" },
  { "comp",     2, 2, compile,       "comp     file.bt out.rbf        -- Compile to a bitstream" },
  { "diff",     3, 3, diff,          "diff     model f1.rbf f2.rbf    -- Compare two bitstrems" },
  { "missing",  2, 2, missing,       "missing  model list.txt         -- List missing pnodes" },
  { }
};

static void show_help(const char *progname)
{
  fprintf(stderr, "Usage:\n");
  for(const fct *f = fcts; f->name; f++)
    fprintf(stderr, "  %s %s\n", progname, f->help);
}


int main(int argc, char **argv)
{
  if(argc == 1) {
    show_help(argv[0]);
    exit(0);
  }

  const char *command = argv[1];
  for(const fct *f = fcts; f->name; f++)
    if(!strcmp(command, f->name)) {
      if(argc - 2 < f->pmin || argc - 2 > f->pmax) {
	fprintf(stderr, "Error on command %s, wrong number of arguments (%d, expected %d-%d)\n\n",
		command, argc - 2, f->pmin, f->pmax);
	show_help(argv[0]);
	exit(1);
      }
      f->f(argv+2);
      exit(0);
    }

  fprintf(stderr, "Unknown command %s\n\n", command);
  show_help(argv[0]);
  exit(1);
}
