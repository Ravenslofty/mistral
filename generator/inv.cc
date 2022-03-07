#include "inv.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

InvLoader::InvLoader(const NodesReader &_nr, const std::vector<uint8_t> &_data, uint32_t width, const P2RLoader &p2r, const P2PLoader &p2p) : nr(_nr)
{
  const uint8_t *p = _data.data();
  const uint8_t *e = _data.data() + _data.size();

  while(p != e) {
    const uint8_t *st = p;
    rnode_t node = nr.lookup_r(p);
    if(!node)
      error(st, "Incorrect inverter rnode");
    skipsp(p);
    int x = lookup_int(p);
    if(x == -1 || *p++ != '.')
      error(st, "Incorrect inverter x position");
    int y = lookup_int(p);
    if(y == -1)
      error(st, "Incorrect inverter y position");

    uint32_t pos = x + y*width;

    skipsp(p);
    if(*p == '0') {
      pos |= inverter_info::DEF_0;
      p++;
    } else if(*p == '1') {
      pos |= inverter_info::DEF_1;
      p++;
    }

    if(*p == '\r')
      p++;
    if(*p != '\n')
      error(st, "WTF? non-ended line?");
    p++;
    add(p2r, p2p, node, pos);
  }
  std::sort(data.begin(), data.end(), [](const inverter_info &a, const inverter_info &b) { return a.node < b.node; });
}

void InvLoader::error(const uint8_t *st, const char *err) const
{
  if(err)
    fprintf(stderr, "%s\n", err);
  const uint8_t *en = st;
  while(*en != '\n' && *en != '\r')
    en++;
  std::string line(st, en);
  fprintf(stderr, "line: %s\n", line.c_str());
  exit(1);
}

void InvLoader::add(const P2RLoader &p2r, const P2PLoader &p2p, rnode_t node, uint32_t pos)
{
  if(!(pos & inverter_info::DEF_MASK)) {
    pnode_t pn = p2r.find_r(node);
    auto pt = pn2pt(pn);
    if(pn) {
      switch(pn2bt(pn)) {
      case CTRL: pos |= inverter_info::DEF_0; break;
      case CMUXCR: pos |= inverter_info::DEF_0; break;
      case CMUXHG: pos |= inverter_info::DEF_0; break;
      case CMUXHR: pos |= inverter_info::DEF_0; break;
      case CMUXVG: pos |= inverter_info::DEF_0; break;
      case CMUXVR: pos |= inverter_info::DEF_0; break;
      case DLL: pos |= inverter_info::DEF_0; break;
      case DQS16: pos |= inverter_info::DEF_0; break;
      case HIP: pos |= inverter_info::DEF_0; break;
      case HPS_BOOT: pos |= inverter_info::DEF_1; break;
      case HPS_CLOCKS_RESETS: pos |= inverter_info::DEF_0; break;
      case HPS_DBG_APB: pos |= inverter_info::DEF_0; break;
      case HPS_DMA: pos |= inverter_info::DEF_0; break;
      case HPS_FPGA2HPS: pos |= inverter_info::DEF_0; break;
      case HPS_FPGA2SDRAM: pos |= inverter_info::DEF_0; break;
      case HPS_HPS2FPGA: pos |= inverter_info::DEF_0; break;
      case HPS_HPS2FPGA_LIGHT_WEIGHT: pos |= inverter_info::DEF_0; break;
      case HPS_INTERRUPTS: pos |= inverter_info::DEF_0; break;
      case HPS_LOAN_IO: pos |= inverter_info::DEF_0; break;
      case HPS_MPU_EVENT_STANDBY: pos |= inverter_info::DEF_0; break;
      case HPS_MPU_GENERAL_PURPOSE: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_CAN: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_EMAC: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_I2C: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_NAND: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_QSPI: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_SDMMC: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_SPI_MASTER: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_SPI_SLAVE: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_UART: pos |= inverter_info::DEF_0; break;
      case HPS_PERIPHERAL_USB: pos |= inverter_info::DEF_0; break;
      case HPS_STM_EVENT: pos |= inverter_info::DEF_0; break;
      case HPS_TPIU_TRACE: pos |= inverter_info::DEF_0; break;
      case LVL: pos |= inverter_info::DEF_0; break;
      case SERPAR: pos |= inverter_info::DEF_0; break;
      case TERM: pos |= inverter_info::DEF_0; break;

      case FPLL: pos |= (pt == CLKEN || pt == EXTSWITCH0 ? inverter_info::DEF_1 : inverter_info::DEF_0); break;
      case HPS_CROSS_TRIGGER: pos |= pt == CLK_EN ? inverter_info::DEF_1 : inverter_info::DEF_0; break;
      case HPS_TEST: pos |= pos |= pt == CFG_DFX_BYPASS_ENABLE ? inverter_info::DEF_1 : inverter_info::DEF_0; break;
      case HSSI: pos |= ((pt == PMA_PMA_RESERVED_IN && pn2pi(pn) == 0) || pt == SMRT_PACK_PLD_8G_TXELECIDLE) ? inverter_info::DEF_1 : inverter_info::DEF_0; break;

      case GPIO:
	pos |= inverter_info::DEF_GP;
	break;
      
      case HMC:
	pos |= inverter_info::DEF_HMC;
	break;

      default: break;
      }

    } else {
      if(rn2t(node) == DCMUX)
	pos |= inverter_info::DEF_0;
      else if(rn2t(node) == GOUT)
	pos |= inverter_info::DEF_0;
    }
  }

  data.emplace_back(inverter_info{node, pos});
}

void InvLoader::remap(std::function<int (rnode_t)> rn2ri)
{
  for(auto &inf : data)
    inf.node = rn2ri(inf.node);
}
