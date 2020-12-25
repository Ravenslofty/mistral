#!/usr/bin/python

import sys

hmc_map = {
    'AFISEQBUSY': [True, 0, 2],
    'AVLADDRESS': [True, 0, 16],
    'AVLREAD': [True, 0, 0],
    'AVLRESETN': [True, 0, 0],
    'AVLWRITE': [True, 0, 0],
    'AVLWRITEDATA': [True, 0, 32],
    'BONDINGIN': [True, 3, 6],
    'GLOBALRESETN': [True, 0, 0],
    'IAVSTCMDDATA': [True, 6, 42],
    'IAVSTCMDRESETN': [True, 6, 0],
    'IAVSTRDCLK': [True, 4, 0],
    'IAVSTRDREADY': [True, 4, 0],
    'IAVSTRDRESETN': [True, 4, 0],
    'IAVSTWRACKREADY': [True, 6, 0],
    'IAVSTWRCLK': [True, 0, 4],
    'IAVSTWRDATA': [True, 4, 90],
    'IAVSTWRRESETN': [True, 4, 0],
    'IOINTADDRACLR': [True, 0, 16],
    'IOINTADDRDOUT': [True, 0, 64],
    'IOINTBAACLR': [True, 0, 3],
    'IOINTBADOUT': [True, 0, 12],
    'IOINTCASNACLR': [True, 0, 0],
    'IOINTCASNDOUT': [True, 0, 4],
    'IOINTCKDOUT': [True, 0, 4],
    'IOINTCKEACLR': [True, 0, 2],
    'IOINTCKEDOUT': [True, 0, 8],
    'IOINTCKNDOUT': [True, 0, 4],
    'IOINTCSNACLR': [True, 0, 2],
    'IOINTCSNDOUT': [True, 0, 8],
    'IOINTDMDOUT': [True, 0, 20],
    'IOINTDQDOUT': [True, 0, 180],
    'IOINTDQOE': [True, 0, 90],
    'IOINTDQSBDOUT': [True, 0, 20],
    'IOINTDQSBOE': [True, 0, 10],
    'IOINTDQSDOUT': [True, 0, 20],
    'IOINTDQSLOGICACLRFIFOCTRL': [True, 0, 5],
    'IOINTDQSLOGICACLRPSTAMBLE': [True, 0, 5],
    'IOINTDQSLOGICDQSENA': [True, 0, 10],
    'IOINTDQSLOGICFIFORESET': [True, 0, 5],
    'IOINTDQSLOGICINCRDATAEN': [True, 0, 10],
    'IOINTDQSLOGICINCWRPTR': [True, 0, 10],
    'IOINTDQSLOGICOCT': [True, 0, 10],
    'IOINTDQSLOGICREADLATENCY': [True, 0, 25],
    'IOINTDQSOE': [True, 0, 10],
    'IOINTODTACLR': [True, 0, 2],
    'IOINTODTDOUT': [True, 0, 8],
    'IOINTRASNACLR': [True, 0, 0],
    'IOINTRASNDOUT': [True, 0, 4],
    'IOINTRESETNACLR': [True, 0, 0],
    'IOINTRESETNDOUT': [True, 0, 4],
    'IOINTWENACLR': [True, 0, 0],
    'IOINTWENDOUT': [True, 0, 4],
    'LOCALDEEPPOWERDNCHIP': [True, 0, 2],
    'LOCALDEEPPOWERDNREQ': [True, 0, 0],
    'LOCALREFRESHCHIP': [True, 0, 2],
    'LOCALREFRESHREQ': [True, 0, 0],
    'LOCALSELFRFSHCHIP': [True, 0, 2],
    'LOCALSELFRFSHREQ': [True, 0, 0],
    'MMRADDR': [True, 0, 10],
    'MMRBE': [True, 0, 0],
    'MMRBURSTBEGIN': [True, 0, 0],
    'MMRBURSTCOUNT': [True, 0, 2],
    'MMRCLK': [True, 0, 0],
    'MMRREADREQ': [True, 0, 0],
    'MMRRESETN': [True, 0, 0],
    'MMRWDATA': [True, 0, 8],
    'MMRWRITEREQ': [True, 0, 0],
    'PLLLOCKED': [True, 0, 0],
    'PORTCLK': [True, 6, 0],
    'SCADDR': [True, 0, 10],
    'SCANEN': [True, 0, 0],
    'SCBE': [True, 0, 0],
    'SCBURSTBEGIN': [True, 0, 0],
    'SCBURSTCOUNT': [True, 0, 2],
    'SCCLK': [True, 0, 0],
    'SCREADREQ': [True, 0, 0],
    'SCRESETN': [True, 0, 0],
    'SCWDATA': [True, 0, 8],
    'SCWRITEREQ': [True, 0, 0],
    'SOFTRESETN': [True, 0, 0],
    'AFICTLLONGIDLE': [True, 0, 2],
    'AFICTLREFRESHDONE': [True, 0, 2],
    'AVLREADDATA[32]': [True, 0, 0],
    'AVLWAITREQUEST': [True, 0, 0],
    'BONDINGOUT1': [True, 0, 4],
    'BONDINGOUT2': [True, 0, 6],
    'BONDINGOUT3': [True, 0, 6],
    'CTLCALREQ': [True, 0, 0],
    'IOINTAFICALFAIL': [True, 0, 0],
    'IOINTAFICALSUCCESS': [True, 0, 0],
    'IOINTAFIRLAT': [True, 0, 5],
    'IOINTAFIWLAT': [True, 0, 4],
    'IOINTDQDIN': [True, 0, 180],
    'IOINTDQSLOGICRDATAVALID': [True, 0, 5],
    'LOCALDEEPPOWERDNACK': [True, 0, 0],
    'LOCALINITDONE': [True, 0, 0],
    'LOCALPOWERDOWNACK': [True, 0, 0],
    'LOCALREFRESHACK': [True, 0, 0],
    'LOCALSELFRFSHACK': [True, 0, 0],
    'MMRRDATA': [True, 0, 8],
    'MMRRDATAVALID': [True, 0, 0],
    'MMRWAITREQUEST': [True, 0, 0],
    'OAMMREADY': [True, 6, 0],
    'ORDAVSTDATA': [True, 4, 80],
    'ORDAVSTVALID': [True, 4, 0],
    'OWRACKAVSTDATA': [True, 6, 0],
    'OWRACKAVSTVALID': [True, 6, 0],
    'PHYRESETN': [True, 0, 0],
    'SCRDATA': [True, 0, 8],
    'SCRDATAVALID': [True, 0, 0],
    'SCWAITREQUEST': [True, 0, 0],

    'AFICTLLONGIDLE': [True, 0, 2],
    'AFICTLREFRESHDONE': [True, 0, 2],
    'AVLREADDATA': [True, 0, 32],
    'AVLWAITREQUEST': [True, 0, 0],
    'BONDINGOUT': [True, 3, 6],
    'CTLCALREQ': [True, 0, 0],
    'IOINTAFICALFAIL': [True, 0, 0],
    'IOINTAFICALSUCCESS': [True, 0, 0],
    'IOINTAFIRLAT': [True, 0, 5],
    'IOINTAFIWLAT': [True, 0, 4],
    'IOINTDQDIN': [True, 0, 180],
    'IOINTDQSLOGICRDATAVALID': [True, 0, 5],
    'LOCALDEEPPOWERDNACK': [True, 0, 0],
    'LOCALINITDONE': [True, 0, 0],
    'LOCALPOWERDOWNACK': [True, 0, 0],
    'LOCALREFRESHACK': [True, 0, 0],
    'LOCALSELFRFSHACK': [True, 0, 0],
    'MMRRDATA': [True, 0, 8],
    'MMRRDATAVALID': [True, 0, 0],
    'MMRWAITREQUEST': [True, 0, 0],
    'OAMMREADY': [True, 0, 6],
    'ORDAVSTDATA': [True, 4, 80],
    'ORDAVSTVALID': [True, 4, 0],
    'OWRACKAVSTDATA': [True, 6, 0],
    'OWRACKAVSTVALID': [True, 6, 0],
    'PHYRESETN': [True, 0, 0],
    'SCRDATA': [True, 0, 8],
    'SCRDATAVALID': [True, 0, 0],
    'SCWAITREQUEST': [True, 0, 0],

    'DDIOPHYDQDIN': [False, 0, 180],
    'PHYDDIOADDRACLR': [False, 0, 16],
    'PHYDDIOADDRDOUT': [False, 0, 64],
    'PHYDDIOBAACLR': [False, 0, 0],
    'PHYDDIOBADOUT': [False, 0, 4],
    'PHYDDIOCASNACLR': [False, 0, 0],
    'PHYDDIOCASNDOUT': [False, 0, 4],
    'PHYDDIOCKDOUT': [False, 0, 4],
    'PHYDDIOCKEACLR': [False, 0, 2],
    'PHYDDIOCKEDOUT': [False, 0, 8],
    'PHYDDIOCKNDOUT': [False, 0, 4],
    'PHYDDIOCSNACLR': [False, 0, 2],
    'PHYDDIOCSNDOUT': [False, 0, 8],
    'PHYDDIODMDOUT': [False, 0, 20],
    'PHYDDIODQDOUT': [False, 0, 180],
    'PHYDDIODQOE': [False, 0, 90],
    'PHYDDIODQSBDOUT': [False, 0, 20],
    'PHYDDIODQSBOE': [False, 0, 10],
    'PHYDDIODQSDOUT': [False, 0, 20],
    'PHYDDIODQSOE': [False, 0, 10],
    'PHYDDIOODTACLR': [False, 0, 2],
    'PHYDDIOODTDOUT': [False, 0, 8],
    'PHYDDIORASNACLR': [False, 0, 0],
    'PHYDDIORASNDOUT': [False, 0, 4],
    'PHYDDIORESETNACLR': [False, 0, 0],
    'PHYDDIORESETNDOUT': [False, 0, 4],
    'PHYDDIOWENACLR': [False, 0, 0],
    'PHYDDIOWENDOUT': [False, 0, 4],
    }

fpll_map = {
    'atpgmode': [True, 1],
    'clken': [True, 2],
    'cnt_sel': [True, 5],
    'reg_byte_en': [True, 2],
    'reg_clk': [True, 1],
    'reg_mdio_dis': [True, 1],
    'reg_read': [True, 1],
    'reg_reg_addr': [True, 6],
    'reg_rst_n': [True, 1],
    'reg_ser_shift_load': [True, 1],
    'reg_write': [True, 1],
    'reg_writedata': [True, 16],
    'extswitch': [True, 1],
    'fbclk_in_l': [True, 1],
    'fbclk_in_r': [True, 1],
    'csr_test': [True, 1],
    'nreset': [True, 1],
    'pfden': [True, 1],
    'phase_en': [True, 1],
    'scanen': [True, 1],
    'up_dn': [True, 1],
    'clkin': [False, 4],
    'zdb_in': [False, 1],
    'clk0_bad': [True, 1],
    'clk1_bad': [True, 1],
    'clksel': [True, 1],
    'reg_readdata': [True, 16],
    'lock': [True, 1],
    'phase_done': [True, 1],
    'extclk': [False, 1],
}

cmux_map = {
    'gclk': [True, 'g', 4, True, 'CLKOUT'],
    'rclk': [True, 'r', 20, True, 'CLKOUT'],
    'pclk': [False, 'p', 2, True, 'CLKOUT'],
    'gclkfb': [False, 'g', 3, True, 'CLKFBOUT'],
    'rclkfb': [False, 'r', 2, True, 'CLKFBOUT'],
    'switchclk': [True, 'g', 4, True, 'SWITCHCLK'],
    'syn_en': [True, 'g', 4, True, 'SYN_EN'],
    'clkinp': [False, 'p', 1, True, 'CLKIN'],
    'clkinr': [False, 'r', 4, True, 'CLKIN'],
    'clking': [False, 'g', 4, True, 'CLKIN'],
    'switchin': [True, 'G', 8, True, 'SWITCHIN'],
    'burstcnt': [False, 'g', 3, True, 'BURSTCNT'],
    'enoutg': [True, 'g', 4, True, 'ENABLE'],
    'enoutr': [True, 'r', 20, True, 'ENABLE'],
    'fbclkin': [False, 'r', 4, True, 'CLKFBIN'],

    'clkpin': [False, 'x', 8, False, 'CLKPIN'],
    'nclkpin': [False, 'x', 8, False, 'NCLKPIN'],
    'pllg': [False, 'g', 16, False, 'PLLIN'],
    'pllr': [False, 'r', 32, False, 'PLLIN'],
}

ctrl_map = {
    'data': [True, 16],
    'dft_in': [True, 6],
    'dft_out': [True, 25],
    'spidataint': [False, 4],
    'spidataout': [False, 4],
    'spidclk': [False, 1],
    'spisce': [False, 1],
}

hps_map = {
    'BONDING_OUT_1': ['BONDING_OUT', 0],
    'BONDING_OUT_2': ['BONDING_OUT', 1],
    'CHANNEL0_REQ': ['REQ', 0],
    'CHANNEL0_SINGLE': ['SINGLE', 0],
    'CHANNEL0_XX_ACK': ['ACK', 0],
    'CHANNEL1_REQ': ['REQ', 1],
    'CHANNEL1_SINGLE': ['SINGLE', 1],
    'CHANNEL1_XX_ACK': ['ACK', 1],
    'CHANNEL2_REQ': ['REQ', 2],
    'CHANNEL2_SINGLE': ['SINGLE', 2],
    'CHANNEL2_XX_ACK': ['ACK', 2],
    'CHANNEL3_REQ': ['REQ', 3],
    'CHANNEL3_SINGLE': ['SINGLE', 3],
    'CHANNEL3_XX_ACK': ['ACK', 3],
    'CHANNEL4_REQ': ['REQ', 4],
    'CHANNEL4_SINGLE': ['SINGLE', 4],
    'CHANNEL4_XX_ACK': ['ACK', 4],
    'CHANNEL5_REQ': ['REQ', 5],
    'CHANNEL5_SINGLE': ['SINGLE', 5],
    'CHANNEL5_XX_ACK': ['ACK', 5],
    'CHANNEL6_REQ': ['REQ', 6],
    'CHANNEL6_SINGLE': ['SINGLE', 6],
    'CHANNEL6_XX_ACK': ['ACK', 6],
    'CHANNEL7_REQ': ['REQ', 7],
    'CHANNEL7_SINGLE': ['SINGLE', 7],
    'CHANNEL7_XX_ACK': ['ACK', 7],
    'CMD_DATA_0': ['CMD_DATA', 0],
    'CMD_DATA_1': ['CMD_DATA', 1],
    'CMD_DATA_2': ['CMD_DATA', 2],
    'CMD_DATA_3': ['CMD_DATA', 3],
    'CMD_DATA_4': ['CMD_DATA', 4],
    'CMD_DATA_5': ['CMD_DATA', 5],
    'CMD_PORT_CLK_0': ['CMD_PORT_CLK', 0],
    'CMD_PORT_CLK_1': ['CMD_PORT_CLK', 1],
    'CMD_PORT_CLK_2': ['CMD_PORT_CLK', 2],
    'CMD_PORT_CLK_3': ['CMD_PORT_CLK', 3],
    'CMD_PORT_CLK_4': ['CMD_PORT_CLK', 4],
    'CMD_PORT_CLK_5': ['CMD_PORT_CLK', 5],
    'CMD_READY_0': ['CMD_READY', 0],
    'CMD_READY_1': ['CMD_READY', 1],
    'CMD_READY_2': ['CMD_READY', 2],
    'CMD_READY_3': ['CMD_READY', 3],
    'CMD_READY_4': ['CMD_READY', 4],
    'CMD_READY_5': ['CMD_READY', 5],
    'CMD_VALID_0': ['CMD_VALID', 0],
    'CMD_VALID_1': ['CMD_VALID', 1],
    'CMD_VALID_2': ['CMD_VALID', 2],
    'CMD_VALID_3': ['CMD_VALID', 3],
    'CMD_VALID_4': ['CMD_VALID', 4],
    'CMD_VALID_5': ['CMD_VALID', 5],
    'DFT_IN_FPGA_BIST_PERI_SI_0': ['DFT_IN_FPGA_BIST_PERI_SI', 0],
    'DFT_IN_FPGA_BIST_PERI_SI_1': ['DFT_IN_FPGA_BIST_PERI_SI', 1],
    'DFT_IN_FPGA_BIST_PERI_SI_2': ['DFT_IN_FPGA_BIST_PERI_SI', 2],
    'DFT_IN_FPGA_CAN0TESTEN': ['DFT_IN_FPGA_CANTESTEN', 0],
    'DFT_IN_FPGA_CAN1TESTEN': ['DFT_IN_FPGA_CANTESTEN', 1],
    'DFT_IN_FPGA_EMAC0TESTEN': ['DFT_IN_FPGA_EMACTESTEN', 0],
    'DFT_IN_FPGA_EMAC1TESTEN': ['DFT_IN_FPGA_EMACTESTEN', 1],
    'DFT_IN_FPGA_MEM_PERI_SI_0': ['DFT_IN_FPGA_MEM_PERI_SI', 0],
    'DFT_IN_FPGA_MEM_PERI_SI_1': ['DFT_IN_FPGA_MEM_PERI_SI', 1],
    'DFT_IN_FPGA_MEM_PERI_SI_2': ['DFT_IN_FPGA_MEM_PERI_SI', 2],
    'DFT_IN_FPGA_PLL1_BG_PWRDN': ['DFT_IN_FPGA_PLL_BG_PWRDN', 0],
    'DFT_IN_FPGA_PLL1_BG_RESET': ['DFT_IN_FPGA_PLL_BG_RESET', 0],
    'DFT_IN_FPGA_PLL1_CLK_SELECT': ['DFT_IN_FPGA_PLL_CLK_SELECT', 0],
    'DFT_IN_FPGA_PLL1_OUTRESET': ['DFT_IN_FPGA_PLL_OUTRESET', 0],
    'DFT_IN_FPGA_PLL1_OUTRESETALL': ['DFT_IN_FPGA_PLL_OUTRESETALL', 0],
    'DFT_IN_FPGA_PLL1_PWRDN': ['DFT_IN_FPGA_PLL_PWRDN', 0],
    'DFT_IN_FPGA_PLL1_REG_PWRDN': ['DFT_IN_FPGA_PLL_REG_PWRDN', 0],
    'DFT_IN_FPGA_PLL1_REG_RESET': ['DFT_IN_FPGA_PLL_REG_RESET', 0],
    'DFT_IN_FPGA_PLL1_REG_TEST_SEL': ['DFT_IN_FPGA_PLL_REG_TEST_SEL', 0],
    'DFT_IN_FPGA_PLL1_RESET': ['DFT_IN_FPGA_PLL_RESET', 0],
    'DFT_IN_FPGA_PLL1_TEST': ['DFT_IN_FPGA_PLL_TEST', 0],
    'DFT_IN_FPGA_PLL2_BG_PWRDN': ['DFT_IN_FPGA_PLL_BG_PWRDN', 1],
    'DFT_IN_FPGA_PLL2_BG_RESET': ['DFT_IN_FPGA_PLL_BG_RESET', 1],
    'DFT_IN_FPGA_PLL2_CLK_SELECT': ['DFT_IN_FPGA_PLL_CLK_SELECT', 1],
    'DFT_IN_FPGA_PLL2_OUTRESET': ['DFT_IN_FPGA_PLL_OUTRESET', 1],
    'DFT_IN_FPGA_PLL2_OUTRESETALL': ['DFT_IN_FPGA_PLL_OUTRESETALL', 1],
    'DFT_IN_FPGA_PLL2_PWRDN': ['DFT_IN_FPGA_PLL_PWRDN', 1],
    'DFT_IN_FPGA_PLL2_REG_PWRDN': ['DFT_IN_FPGA_PLL_REG_PWRDN', 1],
    'DFT_IN_FPGA_PLL2_REG_RESET': ['DFT_IN_FPGA_PLL_REG_RESET', 1],
    'DFT_IN_FPGA_PLL2_REG_TEST_SEL': ['DFT_IN_FPGA_PLL_REG_TEST_SEL', 1],
    'DFT_IN_FPGA_PLL2_RESET': ['DFT_IN_FPGA_PLL_RESET', 1],
    'DFT_IN_FPGA_PLL2_TEST': ['DFT_IN_FPGA_PLL_TEST', 1],
    'DFT_IN_FPGA_PLL3_BG_PWRDN': ['DFT_IN_FPGA_PLL_BG_PWRDN', 2],
    'DFT_IN_FPGA_PLL3_BG_RESET': ['DFT_IN_FPGA_PLL_BG_RESET', 2],
    'DFT_IN_FPGA_PLL3_CLK_SELECT': ['DFT_IN_FPGA_PLL_CLK_SELECT', 2],
    'DFT_IN_FPGA_PLL3_OUTRESET': ['DFT_IN_FPGA_PLL_OUTRESET', 2],
    'DFT_IN_FPGA_PLL3_OUTRESETALL': ['DFT_IN_FPGA_PLL_OUTRESETALL', 2],
    'DFT_IN_FPGA_PLL3_PWRDN': ['DFT_IN_FPGA_PLL_PWRDN', 2],
    'DFT_IN_FPGA_PLL3_REG_PWRDN': ['DFT_IN_FPGA_PLL_REG_PWRDN', 2],
    'DFT_IN_FPGA_PLL3_REG_RESET': ['DFT_IN_FPGA_PLL_REG_RESET', 2],
    'DFT_IN_FPGA_PLL3_REG_TEST_SEL': ['DFT_IN_FPGA_PLL_REG_TEST_SEL', 2],
    'DFT_IN_FPGA_PLL3_RESET': ['DFT_IN_FPGA_PLL_RESET', 2],
    'DFT_IN_FPGA_PLL3_TEST': ['DFT_IN_FPGA_PLL_TEST', 2],
    'DFT_OUT_FPGA_BIST_PERI_SO_0': ['DFT_OUT_FPGA_BIST_PERI_SO', 0],
    'DFT_OUT_FPGA_BIST_PERI_SO_1': ['DFT_OUT_FPGA_BIST_PERI_SO', 1],
    'DFT_OUT_FPGA_BIST_PERI_SO_2': ['DFT_OUT_FPGA_BIST_PERI_SO', 2],
    'DFT_OUT_FPGA_MEM_PERI_SO_0': ['DFT_OUT_FPGA_MEM_PERI_SO', 0],
    'DFT_OUT_FPGA_MEM_PERI_SO_1': ['DFT_OUT_FPGA_MEM_PERI_SO', 1],
    'DFT_OUT_FPGA_MEM_PERI_SO_2': ['DFT_OUT_FPGA_MEM_PERI_SO', 2],
    'H2F_CAN0_IRQ': ['H2F_CAN_IRQ', 0],
    'H2F_CAN1_IRQ': ['H2F_CAN_IRQ', 1],
    'H2F_CTI_IRQ0_N': ['H2F_CTI_IRQ_N', 0],
    'H2F_CTI_IRQ1_N': ['H2F_CTI_IRQ_N', 1],
    'H2F_DMA_IRQ0': ['H2F_DMA_IRQ', 0],
    'H2F_DMA_IRQ1': ['H2F_DMA_IRQ', 1],
    'H2F_DMA_IRQ2': ['H2F_DMA_IRQ', 2],
    'H2F_DMA_IRQ3': ['H2F_DMA_IRQ', 3],
    'H2F_DMA_IRQ4': ['H2F_DMA_IRQ', 4],
    'H2F_DMA_IRQ5': ['H2F_DMA_IRQ', 5],
    'H2F_DMA_IRQ6': ['H2F_DMA_IRQ', 6],
    'H2F_DMA_IRQ7': ['H2F_DMA_IRQ', 7],
    'H2F_EMAC0_IRQ': ['H2F_EMAC_IRQ', 0],
    'H2F_EMAC1_IRQ': ['H2F_EMAC_IRQ', 1],
    'H2F_GPIO0_IRQ': ['H2F_GPIO_IRQ', 0],
    'H2F_GPIO1_IRQ': ['H2F_GPIO_IRQ', 1],
    'H2F_GPIO2_IRQ': ['H2F_GPIO_IRQ', 2],
    'H2F_I2C0_IRQ': ['H2F_I2C_IRQ', 0],
    'H2F_I2C1_IRQ': ['H2F_I2C_IRQ', 1],
    'H2F_I2C_EMAC0_IRQ': ['H2F_I2C_EMAC_IRQ', 0],
    'H2F_I2C_EMAC1_IRQ': ['H2F_I2C_EMAC_IRQ', 1],
    'H2F_L4SP0_IRQ': ['H2F_L4SP_IRQ', 0],
    'H2F_L4SP1_IRQ': ['H2F_L4SP_IRQ', 1],
    'H2F_OSC0_IRQ': ['H2F_OSC_IRQ', 0],
    'H2F_OSC1_IRQ': ['H2F_OSC_IRQ', 1],
    'H2F_SPI0_IRQ': ['H2F_SPI_IRQ', 0],
    'H2F_SPI1_IRQ': ['H2F_SPI_IRQ', 1],
    'H2F_SPI2_IRQ': ['H2F_SPI_IRQ', 2],
    'H2F_SPI3_IRQ': ['H2F_SPI_IRQ', 3],
    'H2F_UART0_IRQ': ['H2F_UART_IRQ', 0],
    'H2F_UART1_IRQ': ['H2F_UART_IRQ', 1],
    'H2F_USB0_IRQ': ['H2F_USB_IRQ', 0],
    'H2F_USB1_IRQ': ['H2F_USB_IRQ', 1],
    'H2F_WDOG0_IRQ': ['H2F_WDOG_IRQ', 0],
    'H2F_WDOG1_IRQ': ['H2F_WDOG_IRQ', 1],
    'MI0': ['MI', 0],
    'MI1': ['MI', 1],
    'MI2': ['MI', 2],
    'MI3': ['MI', 3],
    'MO0': ['MO', 0],
    'MO1': ['MO', 1],
    'MO2_WPN': ['MO', 2],
    'MO3_HOLD': ['MO', 3],
    'OUT1_N': ['OUT_N', 0],
    'OUT2_N': ['OUT_N', 1],
    'RD_CLK_0': ['RD_CLK', 0],
    'RD_CLK_1': ['RD_CLK', 1],
    'RD_CLK_2': ['RD_CLK', 2],
    'RD_CLK_3': ['RD_CLK', 3],
    'RD_DATA_0': ['RD_DATA', 0],
    'RD_DATA_1': ['RD_DATA', 1],
    'RD_DATA_2': ['RD_DATA', 2],
    'RD_DATA_3': ['RD_DATA', 3],
    'RD_READY_0': ['RD_READY', 0],
    'RD_READY_1': ['RD_READY', 1],
    'RD_READY_2': ['RD_READY', 2],
    'RD_READY_3': ['RD_READY', 3],
    'RD_VALID_0': ['RD_VALID', 0],
    'RD_VALID_1': ['RD_VALID', 1],
    'RD_VALID_2': ['RD_VALID', 2],
    'RD_VALID_3': ['RD_VALID', 3],
    'SS_0_N': ['SS_N', 0],
    'SS_1_N': ['SS_N', 1],
    'SS_2_N': ['SS_N', 2],
    'SS_3_N': ['SS_N', 3],
    'WRACK_DATA_0': ['WRACK_DATA', 0],
    'WRACK_DATA_1': ['WRACK_DATA', 1],
    'WRACK_DATA_2': ['WRACK_DATA', 2],
    'WRACK_DATA_3': ['WRACK_DATA', 3],
    'WRACK_DATA_4': ['WRACK_DATA', 4],
    'WRACK_DATA_5': ['WRACK_DATA', 5],
    'WRACK_READY_0': ['WRACK_READY', 0],
    'WRACK_READY_1': ['WRACK_READY', 1],
    'WRACK_READY_2': ['WRACK_READY', 2],
    'WRACK_READY_3': ['WRACK_READY', 3],
    'WRACK_READY_4': ['WRACK_READY', 4],
    'WRACK_READY_5': ['WRACK_READY', 5],
    'WRACK_VALID_0': ['WRACK_VALID', 0],
    'WRACK_VALID_1': ['WRACK_VALID', 1],
    'WRACK_VALID_2': ['WRACK_VALID', 2],
    'WRACK_VALID_3': ['WRACK_VALID', 3],
    'WRACK_VALID_4': ['WRACK_VALID', 4],
    'WRACK_VALID_5': ['WRACK_VALID', 5],
    'WR_CLK_0': ['WR_CLK', 0],
    'WR_CLK_1': ['WR_CLK', 1],
    'WR_CLK_2': ['WR_CLK', 2],
    'WR_CLK_3': ['WR_CLK', 3],
    'WR_DATA_0': ['WR_DATA', 0],
    'WR_DATA_1': ['WR_DATA', 1],
    'WR_DATA_2': ['WR_DATA', 2],
    'WR_DATA_3': ['WR_DATA', 3],
    'WR_READY_0': ['WR_READY', 0],
    'WR_READY_1': ['WR_READY', 1],
    'WR_READY_2': ['WR_READY', 2],
    'WR_READY_3': ['WR_READY', 3],
    'WR_VALID_0': ['WR_VALID', 0],
    'WR_VALID_1': ['WR_VALID', 1],
    'WR_VALID_2': ['WR_VALID', 2],
    'WR_VALID_3': ['WR_VALID', 3],
}

if len(sys.argv) != 3:
    print("%s <data-dir> chip         -- Generate the data source for the die" % sys.argv[0])
    sys.exit(0)

def rnodes(n):
    nn = n.split('.')
    return { 't':nn[0], 'x':int(nn[1]), 'y':int(nn[2]), 'z':int(nn[3]) }

def orn(n):
    return "rnode(%s, %d, %d, %d)" % (n['t'], n['x'], n['y'], n['z'])

def pnode(b, x, y, i, p, ip):
    return { 'b':b, 'x':x, 'y':y, 'i':i, 'p':p, 'ip':ip }

def pnodes(n):
    nn = n.split(":")
    p1 = nn[0].split(".")
    p2 = nn[1].split(".")
    return { 'b':p1[0], 'x':int(p1[1]), 'y':int(p1[2]), 'i':(-1 if len(p1) < 4 else int(p1[3])), 'p':p2[0], 'ip':(-1 if len(p2) < 2 else int(p2[1])) }

def opn(n):
    return "pnode(%s, %d, %d, %s, %d, %d)" % (n['b'], n['x'], n['y'], n['p'], n['i'], n['ip'])

def pr(r):
    if 'instance' in r:
        return '(%d << 24) | (%d << 16) | %d' % (r['instance'], r['block'], r['prams'])
    else:
        return '(%d << 16) | %d' % (r['block'], r['prams'])

def load_pram():
    res = {}
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-pram.txt'):
        ls = l.rstrip('\r\n').split()
        if ls[1] not in res:
            res[ls[1]] = {}
        r = res[ls[1]]
        if ls[1] == 'dqs16':
            if ls[2] not in r:
                r[ls[2]] = [ {}, {}, {}, {} ]
            r = r[ls[2]][int(ls[3])]            
        elif ls[2] != '-':
            if ls[2] not in r:
                r[ls[2]] = {}
            r = r[ls[2]]
        for i in range(4, len(ls)):
            id = int(ls[i])
            if id not in r:
                r[id] = {}
            r = r[id]
        pp = ls[0].split('.')
        pp1 = pp[1].split('-')
        r['block'] = int(pp[0])
        r['prams'] = int(pp1[0])
        r['prame'] = int(pp1[1])
        if len(ls) > 3:
            r['instance'] = int(ls[3])
    return res

def load_dcram():
    res = {}
    for l in open(sys.argv[1] + '/pma3-cram.txt'):
        ls = l.rstrip('\r\n').split()
        if ls[0] not in res:
            res[ls[0]] = []
        r = res[ls[0]]
        for p in ls[2:]:
            pp = p.split('.')
            r.append([int(pp[0]), int(pp[1])])
    return res

def load_f1ram():
    res = []
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-1.txt'):
        pp = l.rstrip('\r\n').split('.')
        res.append([int(pp[0]), int(pp[1])])
    return res

def load_gpio(p2r, p2p):
    res = []
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-iob.txt'):
        ls = l.rstrip('\r\n').split()
        assert(int(ls[0]) == len(res))
        p = ls[1].split('.')
        entry = { 'x':int(p[0]), 'y':int(p[1]), 'idx':int(p[2]), 'type':ls[2], 'tidx':int(ls[3]) }
        if ls[2] == 'gpio':
            for i in range(4, len(ls)):
                e = ls[i].split(':')
                if e[0] == 'serdes':
                    entry['serdes'] = True
                elif e[0][:2] == 'nd':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'DATAIN', int(e[0][2])), rnodes(e[1])])
                elif e[0][:2] == 'cd':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'DATAOUT', int(e[0][2])), rnodes(e[1])])
                elif e[0][:2] == 'oe':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'OEIN', int(e[0][2])), rnodes(e[1])])
                elif e[0] == 'cein':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'CEIN', -1), rnodes(e[1])])
                elif e[0] == 'ceout':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'CEOUT', -1), rnodes(e[1])])
                elif e[0] == 'aclr':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'ACLR', -1), rnodes(e[1])])
                elif e[0] == 'sclr':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'SCLR', -1), rnodes(e[1])])
                elif e[0][:3] == 'cki':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'CLKIN_IN', int(e[0][3])), rnodes(e[1])])
                elif e[0][:3] == 'cko':
                    p2r.append([pnode('GPIO', entry['x'], entry['y'], entry['idx'], 'CLKIN_OUT', int(e[0][3])), rnodes(e[1])])
        res.append(entry)
    return res

def load_hmc(p2r, p2p, pram):
    hmcx = None
    hmcy = None
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-hmc.txt'):
        ls = l.rstrip('\r\n').split()
        if l[0] != ' ':
            assert(ls[0] == 'HMC')
            p = ls[1].split('.')
            hmcx = int(p[0])
            hmcy = int(p[1])
        else:
            if ls[0] == 'dqs':
                assert(len(ls) == 6)
                for i in range(5):
                    if ls[i+1] != '-':
                        p = ls[i+1].split('.')
                        x = int(p[0])
                        y = int(p[1])
                        p2p.append([pnode('HMC', hmcx, hmcy, i, "PNONE", -1), pnode('DQS16', x, y, -1, 'PNONE', -1)])                        
            elif ls[0] == 'leveldc':
                p = ls[1].split('.')
                x = int(p[0])
                y = int(p[1])
                p2p.append([pnode('HMC', hmcx, hmcy, -1, "PNONE", -1), pnode('LEVELING_DELAY_CHAIN', x, y, -1, 'PNONE', -1)])
            elif ls[0] not in hmc_map:
                print("Missing %s in hmc_map" % ls[0], file = sys.stderr)
                sys.exit(1)
            else:
                k = hmc_map[ls[0]]
                assert(len(ls) == 1 + (max(1, k[1])*max(1, k[2])))
                idx = 1
                if k[0]:
                    for i in range(max(1, k[1])):
                        for j in range(max(1, k[2])):
                            if ls[idx] != '-':
                                p2r.append([pnode('HMC', hmcx, hmcy, -1 if k[1] == 0 else i, ls[0], -1 if k[2] == 0 else j), rnodes(ls[idx])])
                            idx += 1
                else:
                    for i in range(max(1, k[1])):
                        for j in range(max(1, k[2])):
                            if ls[idx] != '-':
                                p2p.append([pnode('HMC', hmcx, hmcy, -1 if k[1] == 0 else i, ls[0], -1 if k[2] == 0 else j), pnodes(ls[idx])])
                            idx += 1

def load_fpll(p2r, p2p, pram):
    fpllx = None
    fplly = None
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-fpll.txt'):
        ls = l.rstrip('\r\n').split()
        if l[0] != ' ':
            assert(ls[0] == 'FPLL')
            p = ls[1].split('.')
            fpllx = int(p[0])
            fplly = int(p[1])
        else:
            k = fpll_map[ls[0]]
            if len(ls) != 1 + k[1]:
                print("Error on number of entries, slot %s, expected %d, got 1+%d\n" % (ls[0], k[1], len(ls)-1), file=sys.stderr)
                sys.exit(1)
            if k[0]:
                for i in range(k[1]):
                    p2r.append([pnode('FPLL', fpllx, fplly, -1, ls[0].upper(), i if k[1] > 1 else -1), rnodes(ls[i+1])])
            else:
                for i in range(k[1]):
                    if ls[0] == 'extclk':
                        p2p.append([pnode('FPLL', fpllx, fplly, -1, ls[0].upper(), i if k[1] > 1 else -1), pnodes(ls[i+1])])
                    elif ls[i+1] != '-':
                        p2p.append([pnodes(ls[i+1]), pnode('FPLL', fpllx, fplly, -1, ls[0].upper(), i if k[1] > 1 else -1)])

def load_ctrl(p2r, p2p):
    ctrlx = None
    ctrly = None
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-ctrl.txt'):
        ls = l.rstrip('\r\n').split()
        if l[0] != ' ':
            assert(ls[0] == 'CTRL')
            p = ls[1].split('.')
            ctrlx = int(p[0])
            ctrly = int(p[1])
        else:
            k = ctrl_map[ls[0]] if ls[0] in ctrl_map else None
            if len(ls) != (1 + k[1] if k != None else 2):
                print("Error on number of entries, slot %s, expected %d, got 1+%d\n" % (ls[0], k[1] if k != None else 1 , len(ls)-1), file=sys.stderr)
                sys.exit(1)
            if k == None or k[0]:
                for i in range(k[1] if k != None else 1):
                    p2r.append([pnode('CTRL', ctrlx, ctrly, -1, ls[0].upper(), i if k != None and k[1] > 1 else -1), rnodes(ls[i+1])])
            else:
                for i in range(k[1]):
                    if ls[0] == 'spidatain':
                        p2p.append([pnodes(ls[i+1]), pnode('CTRL', ctrlx, ctrly, -1, ls[0].upper(), i if k[1] > 1 else -1)])
                    elif ls[i+1] != '-':
                        p2p.append([pnode('CTRL', ctrlx, ctrly, -1, ls[0].upper(), i if k[1] > 1 else -1), pnodes(ls[i+1])])

def load_cmux(p2r, p2p, pram):
    cmuxx = None
    cmuxy = None
    cmuxk = None
    cmap = {}
    for e in pram['cmuxc']:
        cmap[e] = 'c'
    for e in pram['cmuxh']:
        cmap[e] = 'h'
    for e in pram['cmuxv']:
        cmap[e] = 'v'
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-cmux.txt'):
        ls = l.rstrip('\r\n').split()
        if l[0] != ' ':
            assert(ls[0] == 'CMUX')
            p = ls[1].split('.')
            cmuxx = int(p[0])
            cmuxy = int(p[1])
            cmuxk = cmap[ls[1]] if ls[1] in cmap else None
        else:
            k = cmux_map[ls[0]]
            if len(ls) != 1 + k[2]:
                print("Error on number of entries, slot %s, expected %d, got 1+%d\n" % (ls[0], k[2], len(ls)-1), file=sys.stderr)
                sys.exit(1)
            idx = 1
            tt = None
            if k[1] == 'p':
                tt = 'CMUXP'
            else:
                tt = 'CMUX' + cmuxk.upper() + k[1].upper()
            if k[3]:
                for i in range(max(1, k[2])):
                    if ls[idx] != '-':
                        if k[1] == 'G':
                            p2r.append([pnode(tt, cmuxx, cmuxy, i % 2, k[4], i // 2), rnodes(ls[idx])])
                        else:
                            p2r.append([pnode(tt, cmuxx, cmuxy, -1 if not k[0] else i, k[4], -1 if k[0] else i), rnodes(ls[idx])])
                    idx += 1
            else:
                for i in range(max(1, k[2])):
                    if ls[idx] != '-':
                        if k[1] == 'x' and cmuxk != 'c':
                            p2p.append([pnode('CMUX' + cmuxk.upper() + 'G', cmuxx, cmuxy, -1 if not k[0] else i, k[4], -1 if k[0] else i), pnodes(ls[idx])])
                            p2p.append([pnode('CMUX' + cmuxk.upper() + 'R', cmuxx, cmuxy, -1 if not k[0] else i, k[4], -1 if k[0] else i), pnodes(ls[idx])])
                        else:
                            if k[1] == 'x':
                                tt = 'CMUXCR'
                            p2p.append([pnode(tt, cmuxx, cmuxy, -1 if not k[0] else i, k[4], -1 if k[0] else i), pnodes(ls[idx])])
                    idx += 1

def load_hps(p2r, p2p):
    blocks = []
    curt = None
    curx = None
    cury = None
    for l in open(sys.argv[1] + '/' + sys.argv[2] + '-hps.txt'):
        ls = l.rstrip('\r\n').split()
        if l[0] != ' ':
            curt = ls[0]
            p = ls[1].split('.')
            curx = int(p[0])
            cury = int(p[1])
            blocks.append([curx, cury])
        else:
            key = ls[0]
            idx = -1
            if key in hps_map:
                k1 = hps_map[key]
                key = k1[0]
                idx = k1[1]
            n = len(ls)-1
            for i in range(n):
                p2r.append([pnode(curt, curx, cury, idx, key, -1 if n == 1 else i), rnodes(ls[i+1])])
    return blocks

def header():
    print("// Generated file, edition is futile")
    print("")
    print("#include \"cyclonev.h\"")
    print("")

def out_fixed(pram):
    def out_fixed_1(px, count):
        ne = 0
        for e in sorted(px.keys()):
            xy = e.split('.')
            print("  { xy2pos(%d, %d), %s }," % (int(xy[0]), int(xy[1]), pr(px[e])))
            ne += 1
        while ne < count:
            print("  { 0xffff, 0xffffffff },")
            ne += 1

    print("const mistral::CycloneV::fixed_block_info mistral::CycloneV::%s_fixed_blocks_info[FB_COUNT] = {" % sys.argv[2])
    out_fixed_1(pram['fpll'],    8)
    out_fixed_1(pram['cmuxc'],   4)
    out_fixed_1(pram['cmuxv'],   2)
    out_fixed_1(pram['cmuxh'],   2)
    out_fixed_1(pram['dll'],     4)
    out_fixed_1(pram['hssi'] if 'hssi' in pram else {}, 4)
    out_fixed_1(pram['cbuf'],    8)
    out_fixed_1(pram['lvl'],    17)
    out_fixed_1(pram['pma3'] if 'pma3' in pram else {}, 4)
    out_fixed_1(pram['serpar'], 10)
    out_fixed_1(pram['term'],    4)
    out_fixed_1(pram['hip']  if 'hip'  in pram else {}, 2)
    print("};")
    
def out_gpio(gpio, pram):
    nmap = { 'dpp':'DEDICATED_PROGRAMMING_PAD', 'jtag':'JTAG', 'hssi_input':'HSSI_INPUT', 'hssi_output':'HSSI_OUTPUT' }
    print("const mistral::CycloneV::ioblock_info mistral::CycloneV::%s_ioblocks_info[] = {" % sys.argv[2])
    for i in gpio:
        if i['type'] == 'gpio':
            ent = pram['gpio']["%03d.%03d" % (i['x'], i['y'])]
            print("  { xy2pos(%d, %d), %d, %d, %d, GPIO, %s }," % (i['x'], i['y'], i['idx'], i['tidx'], 1 if 'serdes' in i else 0, pr(ent)))
        else:
            print("  { xy2pos(%d, %d), %d, %d, 0, %s }," % (i['x'], i['y'], i['idx'], i['tidx'], nmap[i['type']]))
    print("};")

def out_dqs16(cram):
    print("const mistral::CycloneV::dqs16_info mistral::CycloneV::%s_dqs16_info[] = {" % sys.argv[2])
    d = pram['dqs16']
    for e in sorted(d.keys()):
        xy = e.split('.')
        r = d[e][0]
        print("  { xy2pos(%d, %d), (1 << 23) | (%d << 22) | (%d << 16) | %d }," % (int(xy[0]), int(xy[1]), 1 if d[e][1]['prams'] > r['prams'] else 0, r['block'], r['prams']))
    print("};")

def out_hps(hps):
    print("const mistral::CycloneV::pos_t mistral::CycloneV::%s_hps_info[] = {" % sys.argv[2])
    for e in hps:
        print("  xy2pos(%d, %d)," % (e[0], e[1]))
    print("};")

def out_dcram(dcram):
    print("const mistral::CycloneV::dcram_info mistral::CycloneV::%s_dcram_pos[] = {" % sys.argv[2])
    for e in dcram[sys.argv[2]]:
        print("  { %d, %d }," % (e[0], e[1]))
    print("};")

def out_f1ram(f1ram):
    print("const mistral::CycloneV::dcram_info mistral::CycloneV::%s_forced_1_info[%d] = {" % (sys.argv[2], len(f1ram)))
    for e in f1ram:
        print("  { %d, %d }," % (e[0], e[1]))
    print("};")
    
def out_p2r(p2r):
    print("const mistral::CycloneV::p2r_info mistral::CycloneV::%s_p2r_info[] = {" % sys.argv[2])
    for i in p2r:
        print("  { %s, %s }," % (opn(i[0]), orn(i[1])))
    print("  { }")
    print("};")

def out_p2p(p2p):
    print("const mistral::CycloneV::p2p_info mistral::CycloneV::%s_p2p_info[] = {" % sys.argv[2])
    for i in p2p:
        print("  { %s, %s }," % (opn(i[0]), opn(i[1])))
    print("  { }")
    print("};")

p2r = []
p2p = []

pram = load_pram()
dcram = load_dcram()
f1ram = load_f1ram()

gpio = load_gpio(p2r, p2p)

load_hmc(p2r, p2p, pram)
load_cmux(p2r, p2p, pram)
load_fpll(p2r, p2p, pram)
load_ctrl(p2r, p2p)
hps = load_hps(p2r, p2p) if sys.argv[2][:2] == 'sx' else None

header()
out_gpio(gpio, pram)
print("")
out_dqs16(pram)
print("")
out_fixed(pram)
if sys.argv[2][:2] == 'sx':
    print("")
    out_hps(hps)
if sys.argv[2] != 'e50f':
    print("")
    out_dcram(dcram)
if len(f1ram) != 0:
    print("")
    out_f1ram(f1ram)
print("")
out_p2r(p2r)
print("")
out_p2p(p2p)
