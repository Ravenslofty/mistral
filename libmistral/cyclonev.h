#ifndef MISTRAL_CYCLONEV_H
#define MISTRAL_CYCLONEV_H

#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <array>
#include <iterator>

#include "mistral-analogsim.h"

namespace mistral {
  class CycloneV {
  public:
    enum rnode_type_t {
#define P(x) x
#include "cv-rnodetypes.ipp"
#undef P
    };

    enum block_type_t {
#define P(x) x
#include "cv-blocktypes.ipp"
#undef P
    };

    enum port_type_t {
#define P(x) x
#include "cv-porttypes.ipp"
#undef P
    };

    enum bmux_type_t {
#define P(x) x
#include "cv-bmuxtypes.ipp"
#undef P
    };

    enum driver_type_t {
#define P(x) DRV_ ## x
#include "cv-drivertypes.ipp"
#undef P
      DRV_COUNT
    };

    enum shape_type_t {
#define P(x) SHP_ ## x
#include "cv-shapetypes.ipp"
#undef P
    };

    enum invert_t {
      INV_NO,
      INV_YES,
      INV_PROGRAMMABLE,
      INV_UNKNOWN
    };

    enum timing_slot_t {
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

    enum edge_t {
      RF_FALL,
      RF_RISE,
      RF_COUNT
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

    enum delay_type_t {
      DELAY_MAX,
      DELAY_MIN
    };

    static const char *const rnode_type_names[];
    static const char *const block_type_names[];
    static const char *const port_type_names[];
    static const char *const bmux_type_names[];
    static const char *const driver_type_names[];
    static const char *const shape_type_names[];
    static const char *const timing_slot_names[];
    static const char *const edge_names[];
    static const char *const delay_type_names[];

    static const double timing_slot_temperature[];

    rnode_type_t  rnode_type_lookup (const std::string &n) const;
    block_type_t  block_type_lookup (const std::string &n) const;
    port_type_t   port_type_lookup  (const std::string &n) const;
    bmux_type_t   bmux_type_lookup  (const std::string &n) const;
    timing_slot_t timing_slot_lookup(const std::string &n) const;
    edge_t        edge_lookup       (const std::string &n) const;
    delay_type_t  delay_type_lookup (const std::string &n) const;

    enum tile_type_t {
      T_EMPTY,
      T_LAB,
      T_MLAB,
      T_M10K,
      T_DSP,
      T_DSP2
    };

    enum package_type_t {
      PKG_F17,  // Fineline BGA 256 pins
      PKG_F23,  // Fineline BGA 484 pins
      PKG_F27,  // Fineline BGA 672 pins
      PKG_F31,  // Fineline BGA 896 pins
      PKG_F35,  // Fineline BGA 1152 pins

      PKG_U15,  // Ultra Fineline BGA 324 pins
      PKG_U19,  // Ultra Fineline BGA 484 pins
      PKG_U23,  // Ultra Fineline BGA 672 pins

      PKG_M11,  // Micro Fineline BGA 301 pins
      PKG_M13,  // Micro Fineline BGA 383 pins
      PKG_M15   // Micro Fineline BGA 484 pins
    };

    enum die_type_t {
      E50F,
      GX25F,
      GT75F,
      GT150F,
      GT300F,
      SX50F,
      SX120F
    };

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

    enum rnode_timing_mode_t {
      RTM_NO_DELAY,
      RTM_CIRCUIT,
      RTM_P2P,
      RTM_UNSUPPORTED
    };

    struct package_info_t {
      int pin_count;
      char type; // 'f', 'u' or 'm'
      int width_in_pins;
      int height_in_pins;
      int width_in_mm;
      int height_in_mm;
    };

    enum pin_flags_t : uint32_t {
      PIN_IO_MASK    = 0x00000007,
      PIN_DPP        = 0x00000001, // Dedicated Programming Pin
      PIN_HSSI       = 0x00000002, // High Speed Serial Interface input
      PIN_JTAG       = 0x00000003, // JTAG
      PIN_GPIO       = 0x00000004, // General-Purpose I/O

      PIN_HPS        = 0x00000008, // Hardware Processor System

      PIN_DIFF_MASK  = 0x00000070,
      PIN_DM         = 0x00000010,
      PIN_DQS        = 0x00000020,
      PIN_DQS_DIS    = 0x00000030,
      PIN_DQSB       = 0x00000040,
      PIN_DQSB_DIS   = 0x00000050,

      PIN_TYPE_MASK  = 0x00000f00,
      PIN_DO_NOT_USE = 0x00000100,
      PIN_GXP_RREF   = 0x00000200,
      PIN_NC         = 0x00000300,
      PIN_VCC        = 0x00000400,
      PIN_VCCL_SENSE = 0x00000500,
      PIN_VCCN       = 0x00000600,
      PIN_VCCPD      = 0x00000700,
      PIN_VREF       = 0x00000800,
      PIN_VSS        = 0x00000900,
      PIN_VSS_SENSE  = 0x00000a00,
    };

    struct pin_info_t {
      uint8_t x;
      uint8_t y;	
      uint16_t pad;
      uint32_t flags;
      const char *name;
      const char *function;
      const char *io_block;
      double r, c, l, length;
      int delay_ps;
      int index;
    };

    struct pkg_info_t {
      const pin_info_t *pins;
      uint16_t dqs_pos;
      uint8_t dqs_index;
    };

    enum hps_index_t {
      I_HPS_BOOT,
      I_HPS_CLOCKS,
      I_HPS_CLOCKS_RESETS,
      I_HPS_CROSS_TRIGGER,
      I_HPS_DBG_APB,
      I_HPS_DMA,
      I_HPS_FPGA2HPS,
      I_HPS_FPGA2SDRAM,
      I_HPS_HPS2FPGA,
      I_HPS_HPS2FPGA_LIGHT_WEIGHT,
      I_HPS_INTERRUPTS,
      I_HPS_JTAG,
      I_HPS_LOAN_IO,
      I_HPS_MPU_EVENT_STANDBY,
      I_HPS_MPU_GENERAL_PURPOSE,
      I_HPS_PERIPHERAL_CAN,
      I_HPS_PERIPHERAL_EMAC = I_HPS_PERIPHERAL_CAN + 2,
      I_HPS_PERIPHERAL_I2C = I_HPS_PERIPHERAL_EMAC + 2,
      I_HPS_PERIPHERAL_NAND = I_HPS_PERIPHERAL_I2C + 4,
      I_HPS_PERIPHERAL_QSPI,
      I_HPS_PERIPHERAL_SDMMC,
      I_HPS_PERIPHERAL_SPI_MASTER,
      I_HPS_PERIPHERAL_SPI_SLAVE = I_HPS_PERIPHERAL_SPI_MASTER + 2,
      I_HPS_PERIPHERAL_UART = I_HPS_PERIPHERAL_SPI_SLAVE + 2,
      I_HPS_PERIPHERAL_USB = I_HPS_PERIPHERAL_UART + 2,
      I_HPS_STM_EVENT = I_HPS_PERIPHERAL_USB + 2,
      I_HPS_TEST,
      I_HPS_TPIU_TRACE,
      I_HPS_COUNT
    };

    enum cmux_link_t {
      CMUX_CLKIN,
      CMUX_CLKPIN,
      CMUX_CLKPIN_SEL,
      CMUX_CLKPIN_SEL_0,
      CMUX_CLKPIN_SEL_1,
      CMUX_CLKPIN_SEL_2,
      CMUX_CLKPIN_SEL_3,
      CMUX_DEFAULT,
      CMUX_ICLK_SEL,
      CMUX_NCLKPIN,
      CMUX_NCLKPIN_SEL,
      CMUX_NCLKPIN_SEL_0,
      CMUX_NCLKPIN_SEL_1,
      CMUX_NCLKPIN_SEL_2,
      CMUX_NCLKPIN_SEL_3,
      CMUX_OFF,
      CMUX_PLLIN,
      CMUX_PLL_SEL_0,
      CMUX_PLL_SEL_1,
      CMUX_SWITCH
    };

    static const char *const cmux_link_names[];

    static const block_type_t hps_index_to_type[I_HPS_COUNT];

    static const package_info_t package_infos[5+3+3];

    using pos_t = uint16_t;          // Tile position
    using rnode_coords = uint32_t;        // Route node id
    using pnode_coords = uint64_t;        // Port node id

    static constexpr uint32_t pos2x(pos_t xy) { return (xy >> 7) & 0x7f; }
    static constexpr uint32_t pos2y(pos_t xy) { return xy & 0x7f; }
    static constexpr pos_t xy2pos(uint32_t x, uint32_t y) { return (x << 7) | y; }

    static constexpr rnode_coords rnode(rnode_type_t type, pos_t pos, uint32_t z) { return (type << 24) | (pos << 10) | z; }
    static constexpr rnode_coords rnode(rnode_type_t type, uint32_t x, uint32_t y, uint32_t z) { return (type << 24) | (x << 17) | (y << 10) | z; }
    static constexpr rnode_type_t rn2t(rnode_coords rn) { return rnode_type_t(rn >> 24); }
    static constexpr pos_t rn2p(rnode_coords rn) { return (rn >> 10) & 0x3fff; }
    static constexpr uint32_t rn2x(rnode_coords rn) { return (rn >> 17) & 0x7f; }
    static constexpr uint32_t rn2y(rnode_coords rn) { return (rn >> 10) & 0x7f; }
    static constexpr uint32_t rn2z(rnode_coords rn) { return rn & 0x3ff; }

    static constexpr pnode_coords pnode(block_type_t bt, pos_t pos, port_type_t pt, int8_t bindex, int16_t pindex) {
      return (uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (pos << 16) | (pindex & 0xffff);
    }

    static constexpr pnode_coords pnode(block_type_t bt, uint32_t x, uint32_t y, port_type_t pt, int8_t bindex, int16_t pindex) {
      return (uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (x << 23) | (y << 16)| (pindex & 0xffff);
    }

    static constexpr block_type_t pn2bt(pnode_coords pn) { return block_type_t((pn >> 52) & 0xff); }
    static constexpr port_type_t  pn2pt(pnode_coords pn) { return port_type_t((pn >> 40) & 0xfff); }
    static constexpr pos_t        pn2p (pnode_coords pn) { return (pn >> 16) & 0x3fff; }
    static constexpr uint32_t     pn2x (pnode_coords pn) { return (pn >> 23) & 0x7f; }
    static constexpr uint32_t     pn2y (pnode_coords pn) { return (pn >> 16) & 0x7f; }
    static constexpr int8_t       pn2bi(pnode_coords pn) { return (pn >> 32) & 0xff; }
    static constexpr int16_t      pn2pi(pnode_coords pn) { return  pn        & 0xffff; }

    static std::string rn2s(rnode_coords rn);
    static std::string pn2s(pnode_coords pn);

    struct Model;

    CycloneV(const Model *m);
    ~CycloneV() = default;

    // Chosen model
    const Model *current_model() const { return model; }

    // Sizes
    int get_tile_sx() const { return di.tile_sx; }
    int get_tile_sy() const { return di.tile_sy; }

    // State clearing, loading and saving
    void clear();
    void rbf_load(const void *data, uint32_t size);
    void rbf_save(std::vector<uint8_t> &data);

    // Routing
    rnode_coords pnode_to_rnode(pnode_coords pn) const;
    pnode_coords rnode_to_pnode(rnode_coords rn) const;
    invert_t rnode_is_inverting(rnode_coords rn) const;

    std::vector<pnode_coords> p2p_from(pnode_coords pn) const;
    pnode_coords p2p_to(pnode_coords pn) const;
    pnode_coords hmc_get_bypass(pnode_coords pn) const;

    std::vector<std::pair<pnode_coords, rnode_coords>> get_all_p2r() const;
    std::vector<std::pair<pnode_coords, pnode_coords>> get_all_p2p() const;
    std::vector<std::pair<pnode_coords, rnode_coords>> get_all_p2ri() const;

    void rnode_link(rnode_coords n1, rnode_coords n2);
    void rnode_link(pnode_coords p1, rnode_coords n2);
    void rnode_link(rnode_coords n1, pnode_coords p2);
    void rnode_link(pnode_coords p1, pnode_coords p2);
    void rnode_unlink(rnode_coords n2);
    void rnode_unlink(pnode_coords p2);

    std::vector<std::pair<rnode_coords, rnode_coords>> route_all_active_links() const;
    std::vector<std::pair<rnode_coords, rnode_coords>> route_frontier_links() const;
    std::vector<std::vector<rnode_coords>> route_frontier_links_with_path() const;

    // Clock muxes
    static const std::pair<uint8_t, uint8_t> cmuxhg_link_table[4][64];
    static const std::pair<uint8_t, uint8_t> cmuxvg_link_table[4][64];
    static const std::pair<uint8_t, uint8_t> cmuxcr_link_table[6][16];
    static const std::pair<uint8_t, uint8_t> cmuxhr_link_table[12][32];
    static const std::pair<uint8_t, uint8_t> cmuxvr_link_table[20][16];

    // Blocks positions 
    const std::vector<block_type_t> &pos_get_bels(pos_t pos) const { return tile_bels[pos]; }

    const std::vector<pos_t> &lab_get_pos()    const { return lab_pos;    }
    const std::vector<pos_t> &mlab_get_pos()   const { return mlab_pos;   }
    const std::vector<pos_t> &m10k_get_pos()   const { return m10k_pos;   }
    const std::vector<pos_t> &dsp_get_pos()    const { return dsp_pos;    }
    const std::vector<pos_t> &hps_get_pos()    const { return hps_pos;    }
    const std::vector<pos_t> &gpio_get_pos()   const { return gpio_pos;   }
    const std::vector<pos_t> &dqs16_get_pos()  const { return dqs16_pos;  }
    const std::vector<pos_t> &fpll_get_pos()   const { return fpll_pos;   }
    const std::vector<pos_t> &cmuxc_get_pos()  const { return cmuxc_pos;  }
    const std::vector<pos_t> &cmuxv_get_pos()  const { return cmuxv_pos;  }
    const std::vector<pos_t> &cmuxh_get_pos()  const { return cmuxh_pos;  }
    const std::vector<pos_t> &dll_get_pos()    const { return dll_pos;    }
    const std::vector<pos_t> &hssi_get_pos()   const { return hssi_pos;   }
    const std::vector<pos_t> &cbuf_get_pos()   const { return cbuf_pos;   }
    const std::vector<pos_t> &lvl_get_pos()    const { return lvl_pos;    }
    const std::vector<pos_t> &ctrl_get_pos()   const { return ctrl_pos;   }
    const std::vector<pos_t> &pma3_get_pos()   const { return pma3_pos;   }
    const std::vector<pos_t> &serpar_get_pos() const { return serpar_pos; }
    const std::vector<pos_t> &term_get_pos()   const { return term_pos;   }
    const std::vector<pos_t> &hip_get_pos()    const { return hip_pos;    }
    const std::vector<pos_t> &hmc_get_pos()    const { return hmc_pos;    }

    // Block muxes
    enum { MT_MUX, MT_NUM, MT_BOOL, MT_RAM };

    struct bmux_setting_t {
      block_type_t btype;
      pos_t pos;
      bmux_type_t mux;
      int midx;
      int type;
      bool def;
      uint32_t s; // bmux_type_t, or number, or bool value, or count of bits for ram
      std::vector<uint8_t> r;
    };

    int bmux_type(block_type_t btype, pos_t pos, bmux_type_t mux, int midx) const;
    bool bmux_get(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bmux_setting_t &s) const;
    bool bmux_set(const bmux_setting_t &s);
    bool bmux_m_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bmux_type_t s);
    bool bmux_n_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, uint32_t s);
    bool bmux_b_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, bool s);
    bool bmux_r_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, uint64_t s);
    bool bmux_r_set(block_type_t btype, pos_t pos, bmux_type_t mux, int midx, const std::vector<uint8_t> &s);

    std::vector<bmux_setting_t> bmux_get() const;

    // Options
    uint32_t compute_default_jtag_id() const;

    struct opt_setting_t {
      bmux_type_t mux;
      bool def;
      int type;
      uint32_t s; // bmux_type_t, or number, or bool value, or count of bits for ram
      std::vector<uint8_t> r;
    };

    int opt_type(bmux_type_t mux) const;
    bool opt_get(bmux_type_t mux, opt_setting_t &s) const;
    bool opt_set(const opt_setting_t &s);
    bool opt_m_set(bmux_type_t mux, bmux_type_t s);
    bool opt_n_set(bmux_type_t mux, uint32_t s);
    bool opt_b_set(bmux_type_t mux, bool s);
    bool opt_r_set(bmux_type_t mux, uint64_t s);
    bool opt_r_set(bmux_type_t mux, const std::vector<uint8_t> &s);

    std::vector<opt_setting_t> opt_get() const;

    // Programmable inverters
    struct inv_setting_t {
      rnode_coords node;
      bool value;
      bool def; // Is the current value the default?
    };

    // Returns (active, default) pairs
    std::vector<inv_setting_t> inv_get() const;
    bool inv_set(rnode_coords node, bool value);


    // Package/pins/pads related functions
    const pin_info_t *pin_find_pos(pos_t pos, int index) const;
    const pin_info_t *pin_find_pnode(pnode_coords pn) const;
    const pin_info_t *pin_find_name(const std::string &name) const;

    // Debug stuff
    void diff(const CycloneV *m) const;
    void validate_fw_bw() const;

  private:
    struct ioblock_info;
    struct dqs16_info;
    struct p2r_info;
    struct p2p_info;
    struct fixed_block_info;
    struct inverter_info;

  public:
    // Die information structure
    struct die_info {
      const char *name;
      die_type_t type;

      uint32_t cram_sx, cram_sy;
      uint8_t tile_sx, tile_sy;
      uint8_t hps_x, hps_y;
      pos_t    ctrl;

      uint64_t default_oram[12];

      uint16_t frame_size;
      uint16_t pram_sizes[32];
      uint16_t noedcrc_zones[12];
      uint16_t postamble_1, postamble_2;
      uint16_t x_to_bx[122];
      tile_type_t column_types[122];

      const pkg_info_t *const packages[5+3+3];

      const uint8_t *const routing_data_start;
      const uint8_t *const routing_data_end;
      const uint8_t *const bel_spans;
    };

    struct variant_info {
      const char *name;
      const die_info &die;
      uint16_t idcode;
      int alut, alm, memory, dsp, dpll, dll, hps;
    };

    struct Model {
      const char *name;
      const variant_info &variant;
      package_type_t package;
      char temperature; // (C)ommercial, (I)ndustrial or (A)utomotive
      uint8_t speed, speed_grade;
      uint8_t pcie, gxb, hmc;
      uint16_t io, gpio;
    };

    static const die_info e50f;
    static const die_info gt75f;
    static const die_info gt150f;
    static const die_info gt300f;
    static const die_info gx25f;
    static const die_info sx50f;
    static const die_info sx120f;

    static const variant_info v_e25b, v_e25f, v_e50b, v_e50f;
    static const variant_info v_gx25b, v_gx25f;
    static const variant_info v_e75b, v_e75f, v_gt75f, v_gx50b, v_gx50f, v_gx75b, v_gx75f;
    static const variant_info v_e150b, v_e150f, v_gt150f, v_gx150b, v_gx150f;
    static const variant_info v_e300b, v_e300f, v_gt300f, v_gx300b, v_gx300f;
    static const variant_info v_se30b, v_se30bs, v_se30m, v_se50b, v_se50bs, v_se50m, v_sx30f, v_sx50f;
    static const variant_info v_se120b, v_se120bs, v_se120m, v_se90b, v_se90bs, v_se90m, v_st120f, v_st90f, v_sx120f, v_sx90f;
    
    static const Model models[];
    static CycloneV *get_model(std::string model_name);

  private:
    enum bmux_ram_t { BM_CRAM, BM_PRAM, BM_ORAM, BM_DCRAM };

    using fpos_t = uint32_t;

    struct data_header {
      uint32_t off_rnode;
      uint32_t off_rnode_end;
      uint32_t off_rnode_hash;
      uint32_t off_line_info;
      uint32_t off_p2r_info;
      uint32_t off_p2p_info;
      uint32_t off_inv_info;
      uint32_t off_1_info;
      uint32_t off_dcram_info;
      uint32_t off_hps_info;
      uint32_t off_fixed_info;
      uint32_t off_dqs16_info;
      uint32_t off_iob_info;

      uint32_t size_rnode_opaque_hash;
      uint32_t count_rnode;
      uint32_t count_p2r;
      uint32_t count_p2p;
      uint32_t count_inv;
      uint32_t count_1;
      uint32_t count_dcram;
      uint32_t count_dqs16;
      uint32_t count_iob;
    };

    struct global_data_header {
      uint32_t off_dnode_lookup;
      uint32_t off_dnode_table2;
      uint32_t off_dnode_table3;
      uint32_t off_dnode_drivers;
    };
    
    struct rnode_object {
      rnode_coords node;
      uint8_t pattern;
      uint8_t target_count;
      uint8_t drivers[2];
      uint16_t line_info_index;
      uint16_t driver_position;
      uint32_t fw_pos;
    }; // Followed by up to 44 sources and up to 64 targets and up to 64 target_positions.  Aligned to 4 bytes.

    union rnode_target {
      rnode_coords rn;
      float caps;
    };
    
    struct rnode_line_information {
      float tc1;
      float tc2;
      float r85;
      float c;
      uint32_t length;

      double tcomp(double t) const { return 1 + tc1*(t-25) + tc2*(t-25)*(t-25); }
    };

    struct rmux_pattern {
      uint8_t bits;
      uint8_t span;
      uint16_t hashdiv;
      uint32_t def;
      uint16_t o_xy;
      uint16_t o_vals;
      uint16_t o_vhash;
    };

    struct dnode_table2 {
      float value[11*11];
    };

    struct dnode_table3 {
      float start;
      float value[11*11*11];
    };

    struct rf_t {
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
      rf_t cg0_pass;
      rf_t cgd_buff;
      rf_t cgd_drive;
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

  public:
    class rnode_source_iterator : public std::iterator<std::bidirectional_iterator_tag, rnode_coords> {
    public:
      rnode_source_iterator(const rnode_coords *_rn) : rn(_rn) {}
      rnode_source_iterator(const rnode_source_iterator &i) : rn(i.rn) {}

      rnode_source_iterator &operator++() {
	while(!*rn)
	  rn++;
	rn++;
	return *this;
      }

      rnode_source_iterator operator++(int) {
	const rnode_coords *rn1 = rn;
	while(!*rn1)
	  rn1++;
	return rnode_source_iterator(rn+1);
      }

      rnode_source_iterator &operator--() {
	rn--;
	while(!rn[-1])
	  rn--;
	return *this;
      }

      rnode_source_iterator operator--(int) {
	const rnode_coords *rn1 = rn-1;
	while(!rn1[-1])
	  rn1--;
	return rnode_source_iterator(rn1);
      }

      rnode_coords operator*() {
	const rnode_coords *rn1 = rn;
	while(!*rn1)
	  rn1++;
	return *rn1;
      }

      bool operator==(const rnode_source_iterator &rhs) const {
	return rn == rhs.rn;
      }

      bool operator!=(const rnode_source_iterator &rhs) const {
	return rn != rhs.rn;
      }

    private:
      const rnode_coords *rn;
    };
      
    class rnode_source_container_proxy {
    public:
      rnode_source_container_proxy(const rnode_object *_rn) : rn(_rn) {}
      rnode_source_iterator begin() const {
	const rnode_coords *start = rnode_sources(rn);
	if(rn->pattern == 0xff)
	  return rnode_source_iterator(start);
	int span = rn->pattern == 0xfe ? 1 : rmux_patterns[rn->pattern].span;
	const rnode_coords *end = start + span;
	while(start != end && !*start)
	  start++;
	if(start == end)
	  start = rnode_sources(rn);
	return rnode_source_iterator(start);
      }

      rnode_source_iterator end() const {
	const rnode_coords *start = rnode_sources(rn);
	if(rn->pattern == 0xff)
	  return rnode_source_iterator(start);
	int span = rn->pattern == 0xfe ? 1 : rmux_patterns[rn->pattern].span;
	const rnode_coords *end = start + span;
	while(end > start && !end[-1])
	  end --;
	return rnode_source_iterator(end);
      }

    private:
      const rnode_object *rn;
    };
    

    class rnode_proxy {
      friend class CycloneV;

    public:
      rnode_proxy(const rnode_object *_rn) : rn(_rn) {}
      rnode_coords id() const { return rn->node; }
      int pattern() const { return rn->pattern; }
      rnode_source_container_proxy sources() const { return rnode_source_container_proxy(rn); }

    private:
      const rnode_object *rn;

      operator const rnode_object&() const { return *rn; }
    };

    class rnode_iterator : public std::iterator<std::input_iterator_tag, rnode_proxy> {
    public:
      rnode_iterator(const rnode_object *_rn) : rn(_rn) {}
      rnode_iterator(const rnode_iterator &i) : rn(i.rn) {}

      rnode_iterator &operator++() {
	rn = rnode_next(rn);
	return *this;
      }

      rnode_iterator operator++(int) {
	return rnode_iterator(rnode_next(rn));
      }

      rnode_proxy operator*() {
	return rnode_proxy(rn);
      }

      bool operator==(const rnode_iterator &rhs) const {
	return rn == rhs.rn;
      }

      bool operator!=(const rnode_iterator &rhs) const {
	return rn != rhs.rn;
      }

    private:
      const rnode_object *rn;
    };
      
    class rnode_container_proxy {
    public:
      rnode_container_proxy(const CycloneV *_data) : data(_data) {}
      rnode_iterator begin() const { return rnode_iterator(reinterpret_cast<const rnode_object *>(data->rnode_info)); }
      rnode_iterator end() const { return rnode_iterator(reinterpret_cast<const rnode_object *>(data->rnode_info_end)); }

    private:
      const CycloneV *data;
    };

    rnode_container_proxy rnodes() const { return rnode_container_proxy(this); }

    rnode_timing_mode_t rnode_timing_get_mode(rnode_coords rn) const;
    void rnode_timing_build_circuit(rnode_coords rn, timing_slot_t temp, delay_type_t delay, edge_t edge,
				    AnalogSim &sim, int &input, std::vector<std::pair<rnode_coords, int>> &outputs) const;
    void rnode_timing_build_circuit_si(rnode_coords rn, timing_slot_t temp, speed_info_t si, edge_t edge,
				       AnalogSim &sim, int &input, std::vector<std::pair<rnode_coords, int>> &outputs) const;
    
    void rnode_timing_build_input_wave(rnode_coords rn, timing_slot_t temp, delay_type_t delay, edge_t edge, edge_speed_type est, AnalogSim::wave &w) const;
    void rnode_timing_build_input_wave_si(rnode_coords rn, timing_slot_t temp, speed_info_t si, edge_t edge, edge_speed_type est, AnalogSim::wave &w) const;

    void rnode_timing_trim_wave(timing_slot_t temp, delay_type_t delay, const AnalogSim::wave &sw, AnalogSim::wave &dw) const;
    void rnode_timing_trim_wave_si(timing_slot_t temp, speed_info_t si, const AnalogSim::wave &sw, AnalogSim::wave &dw) const;

  private:
    struct bmux_sel_entry {
      uint32_t mask;
      bmux_type_t sel;
    };

    struct bmux_num_entry {
      uint32_t mask;
      uint32_t num;
    };

    struct bmux {
      bmux_type_t mux;
      uint16_t span;
      uint8_t bits;
      uint8_t stype;
      uint8_t entries;
      int8_t def;
      uint16_t bit_offset;
      uint16_t entries_offset;
      int8_t variant;
    };

    const static uint16_t bmux_oram_bpos[];
    const static uint16_t bmux_pram_bpos[];
    const static uint16_t bmux_cram_bpos[];

    const static bmux_sel_entry bmux_sel_entries[];
    const static bmux_num_entry bmux_num_entries[];

    const static bmux bm_opt[];

    const static bmux bm_gpio[];
    const static bmux bm_dqs16[];
    const static bmux bm_fpll[];
    const static bmux bm_cmuxcr[];
    const static bmux bm_cmuxvr[];
    const static bmux bm_cmuxvg[];
    const static bmux bm_cmuxhr[];
    const static bmux bm_cmuxhg[];
    const static bmux bm_dll[];
    const static bmux bm_hssi[];
    const static bmux bm_cbuf[];
    const static bmux bm_lvl[];
    const static bmux bm_pma3[];
    const static bmux bm_pma3c[];
    const static bmux bm_serpar[];
    const static bmux bm_term[];
    const static bmux bm_hip[];
    const static bmux bm_hmc[];

    const static bmux bm_lab[];
    const static bmux bm_mlab[];
    const static bmux bm_m10k[];
    const static bmux bm_dsp[];
    const static bmux bm_hps_clocks[];

    static const uint32_t rmux_vals[622];
    static const int8_t rmux_vhash[1688];
    static const uint8_t rmux_xy[2*780];
    static const rmux_pattern rmux_patterns[70+4];

    struct p2r_info {
      pnode_coords p;
      rnode_coords r;
      uint32_t padding;
    };

    struct p2p_info {
      pnode_coords s;
      pnode_coords d;
    };

    struct ioblock_info {
      pos_t pos;
      uint8_t idx;
      uint8_t tidx;
      block_type_t btype;
      uint32_t pram;
    };

    struct dqs16_info {
      pos_t pos;
      uint32_t pram;
    };

    struct fixed_block_info {
      pos_t pos;
      uint32_t pram;
    };

    struct inverter_info {
      enum {
	DEF_UNK  = 0x00000000, // must be zero or things will break

	DEF_0    = 0x10000000,
	DEF_1    = 0x20000000,

	DEF_GP   = 0x30000000, // Algorithmic, GPIO
	DEF_HMC  = 0x40000000, // Algorithmic, bypassable HMC

	DEF_MASK = 0xf0000000,
      };
      
      rnode_coords node;
      uint32_t pos_and_def;
    };

    enum {
      FB_FPLL   = 0,
      FB_CMUXC  = FB_FPLL   +  8,
      FB_CMUXV  = FB_CMUXC  +  4,
      FB_CMUXH  = FB_CMUXV  +  2,
      FB_DLL    = FB_CMUXH  +  2,
      FB_HSSI   = FB_DLL    +  4,
      FB_CBUF   = FB_HSSI   +  4,
      FB_LVL    = FB_CBUF   +  8,
      FB_PMA3   = FB_LVL    + 17,
      FB_SERPAR = FB_PMA3   +  4,
      FB_TERM   = FB_SERPAR + 10,
      FB_HIP    = FB_TERM   +  4,
      FB_HMC    = FB_HIP    +  2,
      FB_COUNT  = FB_HMC    +  2
    };

    static const uint8_t e50f_bel_spans_info[];
    static const pin_info_t e50f_pins_f17[256];
    static const pin_info_t e50f_pins_f23[484];
    static const pin_info_t e50f_pins_u15[324];
    static const pin_info_t e50f_pins_u19[484];
    static const pin_info_t e50f_pins_m13[383];
    static const pkg_info_t e50f_package_f17;
    static const pkg_info_t e50f_package_f23;
    static const pkg_info_t e50f_package_u15;
    static const pkg_info_t e50f_package_u19;
    static const pkg_info_t e50f_package_m13;

    static const uint8_t gx25f_bel_spans_info[];
    static const pin_info_t gx25f_pins_f23[484];
    static const pin_info_t gx25f_pins_u15[324];
    static const pin_info_t gx25f_pins_u19[484];
    static const pkg_info_t gx25f_package_f23;
    static const pkg_info_t gx25f_package_u15;
    static const pkg_info_t gx25f_package_u19;

    static const uint8_t gt75f_bel_spans_info[];
    static const pin_info_t gt75f_pins_f23[484];
    static const pin_info_t gt75f_pins_f27[672];
    static const pin_info_t gt75f_pins_u19[484];
    static const pin_info_t gt75f_pins_m11[301];
    static const pin_info_t gt75f_pins_m13[383];
    static const pkg_info_t gt75f_package_f23;
    static const pkg_info_t gt75f_package_f27;
    static const pkg_info_t gt75f_package_u19;
    static const pkg_info_t gt75f_package_m11;
    static const pkg_info_t gt75f_package_m13;

    static const uint8_t gt150f_bel_spans_info[];
    static const pin_info_t gt150f_pins_f23[484];
    static const pin_info_t gt150f_pins_f27[672];
    static const pin_info_t gt150f_pins_f31[896];
    static const pin_info_t gt150f_pins_u19[484];
    static const pin_info_t gt150f_pins_m15[484];
    static const pkg_info_t gt150f_package_f23;
    static const pkg_info_t gt150f_package_f27;
    static const pkg_info_t gt150f_package_f31;
    static const pkg_info_t gt150f_package_u19;
    static const pkg_info_t gt150f_package_m15;

    static const uint8_t gt300f_bel_spans_info[];
    static const pin_info_t gt300f_pins_f23[484];
    static const pin_info_t gt300f_pins_f27[672];
    static const pin_info_t gt300f_pins_f31[896];
    static const pin_info_t gt300f_pins_f35[1152];
    static const pin_info_t gt300f_pins_u19[484];
    static const pkg_info_t gt300f_package_f23;
    static const pkg_info_t gt300f_package_f27;
    static const pkg_info_t gt300f_package_f31;
    static const pkg_info_t gt300f_package_f35;
    static const pkg_info_t gt300f_package_u19;

    static const uint8_t sx50f_bel_spans_info[];
    static const pin_info_t sx50f_pins_u19[484];
    static const pin_info_t sx50f_pins_u23[672];
    static const pkg_info_t sx50f_package_u19;
    static const pkg_info_t sx50f_package_u23;

    static const uint8_t sx120f_bel_spans_info[];
    static const pin_info_t sx120f_pins_f31[896];
    static const pin_info_t sx120f_pins_u19[484];
    static const pin_info_t sx120f_pins_u23[672];
    static const pkg_info_t sx120f_package_f31;
    static const pkg_info_t sx120f_package_u19;
    static const pkg_info_t sx120f_package_u23;

    static const die_info *const die_infos[7];

    const Model *model;
    const die_info &di;
    
    uint64_t oram[32];
    std::vector<bool> pram[32];
    std::vector<uint8_t> cram;
    std::array<tile_type_t, 0x4000> tile_types;
    std::array<std::vector<block_type_t>, 0x4000> tile_bels;

    std::vector<pos_t> lab_pos;
    std::vector<pos_t> mlab_pos;
    std::vector<pos_t> m10k_pos;
    std::vector<pos_t> dsp_pos;
    std::vector<pos_t> hps_pos;

    std::vector<pos_t> gpio_pos;
    std::vector<pos_t> dqs16_pos;
    std::vector<pos_t> fpll_pos;
    std::vector<pos_t> cmuxc_pos;
    std::vector<pos_t> cmuxv_pos;
    std::vector<pos_t> cmuxh_pos;
    std::vector<pos_t> dll_pos;
    std::vector<pos_t> hssi_pos;
    std::vector<pos_t> cbuf_pos;
    std::vector<pos_t> lvl_pos;
    std::vector<pos_t> ctrl_pos;
    std::vector<pos_t> pma3_pos;
    std::vector<pos_t> serpar_pos;
    std::vector<pos_t> term_pos;
    std::vector<pos_t> hip_pos;
    std::vector<pos_t> hmc_pos;

    std::unordered_map<pnode_coords, rnode_coords> p2r_map;
    std::unordered_map<rnode_coords, pnode_coords> r2p_map;

    std::vector<std::unique_ptr<uint8_t[]>> decompressed_data_storage;
    const data_header *dhead;
    const uint8_t *rnode_info;
    const uint8_t *rnode_info_end;
    const uint8_t *rnode_hash;
    const uint32_t *rnode_hash_lookup;
    const rnode_line_information *rli_data;

    const p2r_info *p2r_infos;
    const p2p_info *p2p_infos;
    const inverter_info *inverter_infos;
    const uint32_t *one_infos;
    const uint32_t *dcram_infos;
    const pos_t *hps_infos;
    const fixed_block_info *fixed_infos;
    const dqs16_info *dqs16_infos;
    const ioblock_info *iob_infos;

    const global_data_header *gdhead;
    const dnode_lookup *dn_lookup;
    const dnode_table2 *dn_table2;
    const dnode_table3 *dn_table3;
    const dnode_info *dn_info;

    void rbf_load_oram(const void *data, uint32_t size);

    void rmux_load();
    void add_cram_blocks();
    void add_pram_blocks();
    void add_pram_fixed(std::vector<pos_t> &pos, block_type_t block, int start, int count);
    uint32_t find_pram_fixed(pos_t p, int start, int count) const;

    uint32_t max_pram_block_size() const;

    uint64_t oram_load(uint8_t strip, uint8_t off, uint8_t size = 1) const;
    void oram_save(uint64_t v, uint8_t strip, uint8_t off, uint8_t size = 1);

    void oram_clear();
    void oram_load(const uint8_t *data);
    void oram_save(uint8_t *data) const;

    static uint16_t crc16(const uint8_t *src, uint32_t len);
    uint32_t crc32(const uint8_t *src) const;

    const rnode_object *rnode_lookup(rnode_coords rn) const;

    static inline const rnode_object *rnode_next(const rnode_object *r) {
      const uint8_t *p = reinterpret_cast<const uint8_t *>(r);
      p += sizeof(rnode_object);
      p += r->pattern == 0xff ? 0 : r->pattern == 0xfe ? 4 : 4*rmux_patterns[r->pattern].span;
      p += 4*r->target_count;
      p += 2*((r->target_count+1) & ~1);
      return reinterpret_cast<const rnode_object *>(p);
    }

    static inline const rnode_coords *rnode_sources(const rnode_object *r) {
      return reinterpret_cast<const uint32_t *>(reinterpret_cast<const uint8_t *>(r) + sizeof(rnode_object));
    }

    static inline const rnode_coords *rnode_sources(const rnode_object &r) {
      return rnode_sources(&r);
    }

    static inline const rnode_target *rnode_targets(const rnode_object *r) {
      const uint8_t *p = reinterpret_cast<const uint8_t *>(r);
      p += sizeof(rnode_object);
      p += r->pattern == 0xff ? 0 : r->pattern == 0xfe ? 4 : 4*rmux_patterns[r->pattern].span;
      return reinterpret_cast<const rnode_target *>(p);
    }

    static inline const rnode_target *rnode_targets(const rnode_object &r) {
      return rnode_targets(&r);
    }

    static inline const uint16_t *rnode_target_positions(const rnode_object *r) {
      const uint8_t *p = reinterpret_cast<const uint8_t *>(r);
      p += sizeof(rnode_object);
      p += r->pattern == 0xff ? 0 : r->pattern == 0xfe ? 4 : 4*rmux_patterns[r->pattern].span;
      p += 4*r->target_count;
      return reinterpret_cast<const uint16_t *>(p);
    }

    static inline const uint16_t *rnode_target_positions(const rnode_object &r) {
      return rnode_target_positions(&r);
    }

    uint32_t rmux_get_val(const rnode_object &r) const;
    void rmux_set_val(const rnode_object &r, uint32_t val);
    int rmux_get_slot(const rnode_object &r) const;
    rnode_coords rmux_get_source(const rnode_object &r) const;
    inline rnode_coords rmux_get_source(const rnode_object *r) const {
      return rmux_get_source(*r);
    }
    bool rmux_is_default(rnode_coords node) const;
    bool rnode_do_link(rnode_coords n1, rnode_coords n2);
    void route_set_defaults();

    void init_p2r_maps();

    inline uint32_t pos2bit(pos_t pos) const {
      uint16_t x = di.x_to_bx[pos2x(pos)];
      uint16_t y = 2 + 86 * pos2y(pos);
      return y * di.cram_sx + x;
    }

    uint32_t fpll2pram(pos_t p) const;
    uint32_t cmuxc2pram(pos_t p) const;
    uint32_t cmuxv2pram(pos_t p) const;
    uint32_t cmuxh2pram(pos_t p) const;
    uint32_t dll2pram(pos_t p) const;
    uint32_t hssi2pram(pos_t p) const;
    uint32_t cbuf2pram(pos_t p) const;
    uint32_t lvl2pram(pos_t p) const;
    uint32_t pma32pram(pos_t p) const;
    uint32_t serpar2pram(pos_t p) const;
    uint32_t term2pram(pos_t p) const;
    uint32_t hip2pram(pos_t p) const;
    uint32_t hmc2pram(pos_t p) const;

    static void bmux_dqs16_adjust(uint32_t &pos, uint32_t offset, bool up);

    void bmux_b_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const;
    void bmux_m_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const;
    void bmux_r_solve_default(block_type_t btype, pos_t pos, int idx, const bmux *mux, uint32_t base, int &def) const;
    uint64_t bmux_val_read(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const;
    std::pair<bmux_type_t, bool> bmux_m_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const;
    std::pair<int, bool> bmux_n_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const;
    std::pair<bool, bool> bmux_b_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode) const;
    bool bmux_r_read(block_type_t btype, pos_t pos, uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, std::vector<uint8_t> &r) const;
    void bmux_get_any(block_type_t btype, pos_t pos, uint32_t base, const bmux *muxes, bmux_ram_t mode, std::vector<bmux_setting_t> &res, int variant = 0) const;

    void bmux_val_set(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, uint64_t val);
    void bmux_val_set(uint32_t base, const bmux *mux, int idx, bmux_ram_t mode, const std::vector<uint8_t> &val);
    void bmux_set_default(block_type_t btype, pos_t pos, uint32_t base, const bmux *muxes, bmux_ram_t mode, int variant = 0);
    void bmux_set_defaults();
    void bmux_find(block_type_t btype, pos_t pos, bmux_type_t mux, uint32_t &base, const bmux *&pmux, bmux_ram_t &mode) const;
    const bmux *bmux_find(const bmux *pmux, bmux_type_t mux, int variant = 0) const;

    int inv_get_default(const inverter_info &inf) const;
    void forced_1_set();
    void inv_default_set();

    void opt_get_one(const bmux *muxes, opt_setting_t &bms) const;

    struct sh {
      size_t operator()(const char *word) const noexcept {
	size_t v = 5381;
	unsigned char c;
	while((c = *word++) != 0)
	  v = v*33 + toupper(c);
	return v;
      }
    };

    struct eq {
      bool operator()(const char *a, const char *b) const noexcept {
	return !strcasecmp(a, b);
      }
    };

    template<typename T> T any_type_lookup(const std::string &n, const std::unordered_map<const char *, T, sh, eq> &h) const {
      auto i = h.find(n.c_str());
      return i == h.end() ? T(0) : i->second;
    }

    template<typename T> void any_type_hash_init(std::unordered_map<const char *, T, sh, eq> &h, const char *const *table)
    {
      for(int i = 1; table[i]; i++)
	h[table[i]] = T(i);
    }

    std::tuple<const uint8_t *, size_t> get_bin(const uint8_t *start, const uint8_t *end);

    struct t2_lookup : public AnalogSim::table2_lookup {
      const dnode_table2 *table;
      t2_lookup(std::string _name, const dnode_table2 *_table) : AnalogSim::table2_lookup(_name), table(_table) {}
      virtual ~t2_lookup() = default;
      virtual void lookup(double x, double y, double &v, double &dvx, double &dvy) const;
      virtual double lookup(double x, double y) const;
    };

    struct t3_lookup : public AnalogSim::table3_lookup {
      const dnode_table3 *table;
      t3_lookup(std::string _name, const dnode_table3 *_table) : AnalogSim::table3_lookup(_name), table(_table) {}
      virtual ~t3_lookup() = default;
      virtual void lookup(double x, double y, double z, double &v, double &dvx, double &dvy, double &dvz) const;
      virtual double lookup(double x, double y, double z) const;
    };

    static void table_pos_to_index(double v, size_t &p, double &pf, double &pf1);
    std::unique_ptr<t2_lookup> dn_t2(int driver_id, const char *slot, uint16_t index) const;
    std::unique_ptr<t3_lookup> dn_t3(int driver_id, const char *slot, uint16_t index) const;
    void rnode_timing_generate_line(const rnode_target *targets,
				    const uint16_t *target_pos,
				    int split_edge, int target_count,
				    uint16_t split_pos,
				    bool second_span,
				    uint16_t line_coalescing,
				    double &caps, int &node,
				    double line_r, edge_t edge,
				    double dev,
				    const rnode_line_information &rli,
				    rnode_coords rn,
				    const dnode_driver *driver_bank,
				    AnalogSim &sim, std::vector<std::pair<rnode_coords, int>> &outputs) const;
    void rnode_timing_build_circuit(int didx, rnode_coords rn, timing_slot_t temp, edge_t edge, AnalogSim &sim, int &input, std::vector<std::pair<rnode_coords, int>> &outputs) const;
    void rnode_timing_build_input_wave(int didx, rnode_coords rn, edge_t edge, edge_speed_type est, AnalogSim::wave &w) const;
    void rnode_timing_trim_wave(int didx, const AnalogSim::wave &sw, AnalogSim::wave &dw) const;

    bool rnode_active(const rnode_object *rn, rnode_coords previous) const;

    std::unordered_map<const char *, rnode_type_t, sh, eq> rnode_type_hash;
    std::unordered_map<const char *, block_type_t, sh, eq> block_type_hash;
    std::unordered_map<const char *, port_type_t, sh, eq>  port_type_hash;
    std::unordered_map<const char *, bmux_type_t, sh, eq>  bmux_type_hash;
  };
}

#endif
