#include "hps.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>

const struct {
  int block;
  int hps;
} block_table[] = {
  { HPS_BOOT, I_HPS_BOOT },
  { HPS_CLOCKS, I_HPS_CLOCKS },
  { HPS_CLOCKS_RESETS, I_HPS_CLOCKS_RESETS },
  { HPS_CROSS_TRIGGER, I_HPS_CROSS_TRIGGER },
  { HPS_DBG_APB, I_HPS_DBG_APB },
  { HPS_DMA, I_HPS_DMA },
  { HPS_FPGA2HPS, I_HPS_FPGA2HPS },
  { HPS_FPGA2SDRAM, I_HPS_FPGA2SDRAM },
  { HPS_HPS2FPGA, I_HPS_HPS2FPGA },
  { HPS_HPS2FPGA_LIGHT_WEIGHT, I_HPS_HPS2FPGA_LIGHT_WEIGHT },
  { HPS_INTERRUPTS, I_HPS_INTERRUPTS },
  { HPS_JTAG, I_HPS_JTAG },
  { HPS_LOAN_IO, I_HPS_LOAN_IO },
  { HPS_MPU_EVENT_STANDBY, I_HPS_MPU_EVENT_STANDBY },
  { HPS_MPU_GENERAL_PURPOSE, I_HPS_MPU_GENERAL_PURPOSE },
  { HPS_PERIPHERAL_CAN, I_HPS_PERIPHERAL_CAN },
  { HPS_PERIPHERAL_EMAC, I_HPS_PERIPHERAL_EMAC },
  { HPS_PERIPHERAL_I2C, I_HPS_PERIPHERAL_I2C },
  { HPS_PERIPHERAL_NAND, I_HPS_PERIPHERAL_NAND },
  { HPS_PERIPHERAL_QSPI, I_HPS_PERIPHERAL_QSPI },
  { HPS_PERIPHERAL_SDMMC, I_HPS_PERIPHERAL_SDMMC },
  { HPS_PERIPHERAL_SPI_MASTER, I_HPS_PERIPHERAL_SPI_MASTER },
  { HPS_PERIPHERAL_SPI_SLAVE, I_HPS_PERIPHERAL_SPI_SLAVE },
  { HPS_PERIPHERAL_UART, I_HPS_PERIPHERAL_UART },
  { HPS_PERIPHERAL_USB, I_HPS_PERIPHERAL_USB },
  { HPS_STM_EVENT, I_HPS_STM_EVENT },
  { HPS_TEST, I_HPS_TEST },
  { HPS_TPIU_TRACE, I_HPS_TPIU_TRACE },
  { BNONE, 0 },
};


HPSLoader::HPSLoader(const P2PLoader &p2p, const P2RLoader &p2r)
{
  for(int i=0; block_table[i].block; i++)
    hpsmap[block_table[i].block] = block_table[i].hps;

  data.resize(I_HPS_COUNT, 0);

  for(const auto &d : p2r.data)
    lookup(d.p);

  for(const auto &d : p2p.data) {
    lookup(d.s);
    lookup(d.d);
  }

  if(!data[0])
    data.clear();
}

void HPSLoader::lookup(pnode_coords p)
{
  int t = pn2bt(p);
  auto i = hpsmap.find(t);
  if(i == hpsmap.end())
    return;

  pos_t pos = pn2p(p);
  int ix = i->second;
  while(ix < I_HPS_COUNT && data[ix] && data[ix] != pos)
    ix++;
  assert(ix != I_HPS_COUNT); // Overflow?
  if(!data[ix]) {
    data[ix] = pos;
    //    printf("%s %3d.%03d\n", block_type_names[t], pos2x(pos), pos2y(pos));
  }
}
