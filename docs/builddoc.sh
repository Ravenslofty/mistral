#!/bin/sh

for b in cbuf dll dqs16 dsp fpll gpio hip hmc hps_clocks hssi lab lvl m10k mlab opt serpar term; do
  ./mkmuxdoc.py ../data/${b}-mux.txt srcdoc/${b}-cfg.txt srcdoc/${b}-doc.txt > gendoc/${b}-dmux.rst
done

b=cmuxvr; ./mkmuxdoc.py ../data/cmuxv-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxvg; ./mkmuxdoc.py ../data/cmuxv-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxhr; ./mkmuxdoc.py ../data/cmuxh-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxhg; ./mkmuxdoc.py ../data/cmuxh-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst
b=cmuxcr; ./mkmuxdoc.py ../data/cmuxc-mux.txt srcdoc/${b}-cfg.txt srcdoc/cmux-doc.txt > gendoc/${b}-dmux.rst

b=pma3; ./mkmuxdoc.py ../data/${b}-mux.txt,../data/${b}-c-mux.txt srcdoc/${b}-cfg.txt srcdoc/${b}-doc.txt > gendoc/${b}-dmux.rst

for b in cmuxp cmuxcr cmuxhg cmuxhr cmuxvg cmuxvr ctrl dll fpll gpio hmc hps_boot hps_clocks_resets hps_cross_trigger hps_dbg_apb hps_dma hps_fpga2hps hps_fpga2sdram hps_hps2fpga hps_hps2fpga_light_weight hps_interrupts hps_jtag hps_loan_io hps_mpu_event_standby hps_mpu_general_purpose hps_peripheral_can hps_peripheral_emac hps_peripheral_i2c hps_peripheral_nand hps_peripheral_qspi hps_peripheral_sdmmc hps_peripheral_spi_master hps_peripheral_spi_slave hps_peripheral_uart hps_peripheral_usb hps_stm_event hps_test hps_tpiu_trace; do
  ./mkp2rdoc.py $b srcdoc/%s-p2r.txt srcdoc/p2r-doc.txt > gendoc/${b}-dp2r.rst
done

for b in lab m10k dsp; do
  ./mkp2rdoc.py $b srcdoc/%s-p2ri.txt srcdoc/p2r-doc.txt > gendoc/${b}-dp2r.rst
done

for b in cmuxcr cmuxhg cmuxhr cmuxvg cmuxvr dll dqs16 fpll gpio hmc hps_clocks lvl; do
  ./mkp2pdoc.py $b srcdoc/%s-p2p.txt srcdoc/p2p-doc.txt > gendoc/${b}-dp2p.rst
done

cd ../docs
rm -rf ../docs-html ../docs-pdf
mkdir ../docs-html ../docs-pdf

sphinx-build -Eb singlehtml . ../docs-html
sphinx-build -Eb latex . ../docs-pdf
make -C ../docs-pdf
