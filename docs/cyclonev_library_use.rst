CycloneV library usage
======================

Library structure
-----------------

The library provides a CycloneV class in the mistral namespace.
Information is provided to allow to choose a CycloneV::Model object
which represents a sold FPGA variant.  Then a CycloneV object can be
created from it.  That object stores the state of the FPGA
configuration and allows to read and modify it.

All the types, enums, functions, methods, arrays etc described in the
following paragraph are in the CycloneV class.


Packages
--------

.. code-block:: C++

    enum package_type_t;

    struct CycloneV::package_info_t {
      int pin_count;
      char type;
      int width_in_pins;
      int height_in_pins;
      int width_in_mm;
      int height_in_mm;
    };

    const package_info_t package_infos[5+3+3];

The FPGAs are sold in 11 different packages, which are named by their
type (Fineline BGA, Ultra Fineline BGA or Micro Fineline BGA) and
their width in mm.

+---------+------+------+------------+--------------+
| Enum    | Type | Pins | Size in mm | Size in pins |
+=========+======+======+============+==============+
| PKG_F17 | f    |  256 | 16x16      | 17x17        |
+---------+------+------+------------+--------------+
| PKG_F23 | f    |  484 | 22x22      | 23x23        |
+---------+------+------+------------+--------------+
| PKG_F27 | f    |  672 | 26x26      | 27x27        |
+---------+------+------+------------+--------------+
| PKG_F31 | f    |  896 | 30x30      | 31x31        |
+---------+------+------+------------+--------------+
| PKG_F35 | f    | 1152 | 34x34      | 35x35        |
+---------+------+------+------------+--------------+
| PKG_U15 | u    |  324 | 18x18      | 15x15        |
+---------+------+------+------------+--------------+
| PKG_U19 | u    |  484 | 22x22      | 19x19        |
+---------+------+------+------------+--------------+
| PKG_U23 | u    |  672 | 28x28      | 23x23        |
+---------+------+------+------------+--------------+
| PKG_M11 | m    |  301 | 21x21      | 11x11        |
+---------+------+------+------------+--------------+
| PKG_M13 | m    |  383 | 25x25      | 13x13        |
+---------+------+------+------------+--------------+
| PKG_M15 | m    |  484 | 28x28      | 15x15        |
+---------+------+------+------------+--------------+


Model information
-----------------

.. code-block:: C++

    enum die_type_t { E50F, GX25F, GT75F, GT150F, GT300F, SX50F, SX120F };

    struct Model {
      const char *name;
      const variant_info &variant;
      package_type_t package;
      char temperature;
      char speed;
      char pcie, gxb, hmc;
      uint16_t io, gpio;
    };

    struct variant_info {
      const char *name;
      const die_info &die;
      uint16_t idcode;
      int alut, alm, memory, dsp, dpll, dll, hps;
    };

    struct die_info {
      const char *name;
      die_type_t type;
      uint8_t tile_sx, tile_sy;
      // ...
    };

    const Model models[];
    CycloneV *get_model(std::string model_name);

A Model is built from a package, a variant and a temperature/speed
grade.  A variant selects a die and which hardware is active on it.

The Model fields are:

* name - the SKU, for instance 5CSEBA6U23I7
* variant - its associated variant_info
* package - the packaging used
* temperature - the temperature grade, 'A' for automotive (-45..125C), 'I' for industrial (-40..100C), 'C' for commercial (0..85C)
* speed - the speed grade, 6-8, smaller is faster
* pcie - number of PCIe interfaces (depends on both variant and number of available pins)
* gxb - ??? (same)
* hmc - number of Memory interfaces (same)
* io - number of i/os
* gpio - number of fpga-usable gpios


The Variant fields are:

* name - name of the variant, for instance se120b
* die - its associated die_info
* idcode - the IDCODE associated to this variant (not unique per variant at all)
* alut - number of LUTs
* alm - number of logic elements
* memory - bits of memory
* dsp - number of dsp blocks
* dpll - number of plls
* dll - number of delay-locked loops
* hps - number of arm cores

The Die usable fields are:

* name - name of the die, for instance sx120f
* type - the enum value for the die type
* tile_sx, tile_sy - size of the tile grid

The limits indicated in the variant structure may be lower than the
theoretical die capabilities.  We have no idea what happens if these
limits are not respected.

To create a CycloneV object, the constructor requires a Model \*.
Either choose one from the models array, or, in the usual case of
selection by sku, the CycloneV::get_model function looks it up and
allocates one.  The models array ends with a nullptr name pointer.

The get_model function implements the alias "ms" for the 5CSEBA6U23I7
used in the de10-nano, a.k.a MiSTer.


pos, rnode and pnode
--------------------

.. code-block:: C++

    using pos_t = uint16_t;          // Tile position

    static constexpr uint32_t pos2x(pos_t xy);
    static constexpr uint32_t pos2y(pos_t xy);
    static constexpr pos_t xy2pos(uint32_t x, uint32_t y);

The type pos_t represents a position in the grid.  xy2pos allows to create one, pos2x and pos2y extracts the coordinates.

.. code-block:: C++

    using rnode_t = uint32_t;        // Route node id

    enum rnode_type_t;
    const char *const rnode_type_names[];
    rnode_type_t rnode_type_lookup(const std::string &n) const;

    constexpr rnode_t rnode(rnode_type_t type, pos_t pos, uint32_t z);
    constexpr rnode_t rnode(rnode_type_t type, uint32_t x, uint32_t y, uint32_t z);
    constexpr rnode_type_t rn2t(rnode_t rn);
    constexpr pos_t rn2p(rnode_t rn);
    constexpr uint32_t rn2x(rnode_t rn);
    constexpr uint32_t rn2y(rnode_t rn);
    constexpr uint32_t rn2z(rnode_t rn);

    std::string rn2s(rnode_t rn);

A rnode_t represents a note in the routing network.  It is
characterized by its type (rnode_type_t) and its coordinates (x, y for
the tile, z for the instance number in the tile).  Those functions
allow to create one and extract the different components.
rnode_types_names gives the string representation for every
rnode_type_t value, and rnode_type_lookup finds the rnode_type_t for a
given name.  rn2s provides a string representation of the rnode
(TYPE.xxx.yyy.zzzz).

The rnode_type_t value 0 is NONE, and a rnode_t of 0 is guaranteed
invalid.

.. code-block:: C++

    using pnode_t = uint64_t;        // Port node id

    enum block_type_t;
    const char *const block_type_names[];
    block_type_t block_type_lookup(const std::string &n) const;

    enum port_type_t;
    const char *const port_type_names[];
    port_type_t  port_type_lookup (const std::string &n) const;

    constexpr pnode_t pnode(block_type_t bt, pos_t pos, port_type_t pt, int8_t bindex, int16_t pindex);
    constexpr pnode_t pnode(block_type_t bt, uint32_t x, uint32_t y, port_type_t pt, int8_t bindex, int16_t pindex);
    constexpr block_type_t pn2bt(pnode_t pn);
    constexpr port_type_t  pn2pt(pnode_t pn);
    constexpr pos_t        pn2p (pnode_t pn);
    constexpr uint32_t     pn2x (pnode_t pn);
    constexpr uint32_t     pn2y (pnode_t pn);
    constexpr int8_t       pn2bi(pnode_t pn);
    constexpr int16_t      pn2pi(pnode_t pn);

    std::string pn2s(pnode_t pn);

A pnode_t represents a port of a logical block.  It is characterized
by the block type (block_type_t), the block tile position, the block
number instance (when appropriate, -1 when not), the port type
(port_type_t) and the bit number in the port (when appropriate, -1
when not).  pn2s provides the string representation
BLOCK.xxx.yyy(.instance):PORT(.bit)

The block_type_t value 0 is BNONE, the port_type_t value 0 is PNONE,
and pnode_t 0 is guaranteed invalid.


.. code-block:: C++

    rnode_t pnode_to_rnode(pnode_t pn) const;
    pnode_t rnode_to_pnode(rnode_t rn) const;

These two methods allow to find the connections between the logic
block ports and the routing nodes.  It is always 1:1 when there is
one.



Routing network management
--------------------------

.. code-block:: C++

    void rnode_link(rnode_t n1, rnode_t n2);
    void rnode_link(pnode_t p1, rnode_t n2);
    void rnode_link(rnode_t n1, pnode_t p2);
    void rnode_link(pnode_t p1, pnode_t p2);
    void rnode_unlink(rnode_t n2);
    void rnode_unlink(pnode_t p2);


The method rnode_link links two nodes together with n1 as source and
n2 as destination, automatically converting from pnode_t to rnode_t
when needed.  rnode_unlink disconnects anything connected to the
destination n2.

There are two special cases.  DCMUX is a 2:1 mux which selects between
a data and a clock signal and has no disconnected state.  Unlinking it
puts in in the default clock position.  Most SCLK muxes use a 5-bit
vertical configuration where up to 5 inputs can be connected and the
all-off configuration is not allowed.  Usually at least one input goes
to vcc, but in some cases all five are used and unlinking selects the
4th input (the default in that case).

.. code-block:: C++

    std::vector<std::pair<rnode_t, rnode_t>> route_all_active_links() const;
    std::vector<std::pair<rnode_t, rnode_t>> route_frontier_links() const;

route_all_active_links gives all current active connections.
route_frontier_links solves these connections to keep only the
extremities, giving the inter-logic-block connections directly.


Logic block management
----------------------

.. code-block:: C++

    const std::vector<pos_t> &lab_get_pos()    const
    [etc]

    cosst std::vector<block_type_t> &pos_get_bels(pos_t pos) const

The numerous xxx_get_pos() methods gives the list of positions of
logic blocks of a given type.  The known types are lab, mlab, m10k,
dsp, hps, gpio, dqs16, fpll, cmuxc, cmuxv, cmuxh, dll, hssi, cbuf,
lvl, ctrl, pma3, serpar, term and hip.  A vector is empty when a block
type doesn't exist in the given die.

In the hps case the 37 blocks can be indexed by hps_index_t enum.

Alternatively the pos_get_bels() method gives the (possibly empty)
list of logic blocks present in a given tile.


.. code-block:: C++

    enum { MT_MUX, MT_NUM, MT_BOOL, MT_RAM };

    enum bmux_type_t;
    const char *const bmux_type_names[];
    bmux_type_t bmux_type_lookup(const std::string &n) const;

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

These methods allow to manage the logic blocks muxes configurations.
A mux is characterized by its block (type and position), its type
(bmux_type_t) and its instance number (0 if there is only one).  There
are four kinds of muxes, symbolic (MT_MUX), numeric (MT_NUM), booolean
(MT_BOOL) and ram (MT_RAM).

bmux_type looks up a mux and returns its MT_* type, or -1 if it
doesn't exist.  bmux_get reads the state of a mux and returns it in s
and true when found, false otherwise.  The def field indicates whether
the value is the default.  The bmux_set sets a mux generically, and
the bmux_*_set sets it per-type.

The no-parameter bmux_get version returns the state of all muxes of the FPGA.


Inverters management
--------------------

.. code-block:: C++

    struct inv_setting_t {
      rnode_t node;
      bool value;
      bool def;
    };

    std::vector<inv_setting_t> inv_get() const;
    bool inv_set(rnode_t node, bool value);

inv_get() returns the state of the programmable inverters, and inv_set
sets the state of one.  The field def is currently very incorrect.


Pin/package management
----------------------

.. code-block:: C++

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

    const pin_info_t *pin_find_pos(pos_t pos, int index) const;

The pin_info_t structure describes a pin with:

* x, y - its coordinates in the package grid (not the fpga grid, the pins one)
* pad - either 0xffff (no associated gpio) or (index << 14) | tile_pos, where index indicates which pad of the gpio is connected to the pin
* flags - flags describing the pin function
* name - pin name, like A1
* function - pin function as text, like "GND"
* io_block - name of the I/O block for power purposes, like 9A
* r, c, l - electrical characteristics of the pin-pad connection wire
* length - length of the wire
* delay_ps - usual signal transmission delay is ps
* index - pin sub-index for hssi_input, hssi_output, dedicated programming pins and jtag

The pin_find_pos method looks up a pin from a gpio tile/index combination.


Options
-------

.. code-block:: C++

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

The options work like the block muxes without a block, tile or
instance number.  They're otherwise the same.


Bitstream management
--------------------

.. code-block:: C++

    void clear();
    void rbf_load(const void *data, uint32_t size);
    void rbf_save(std::vector<uint8_t> &data);

The clear method returns the FPGA state to all defaults.  rbf_load
parses a raw bitstream file from memory and loads the state from it.
rbf_save generats a rbf from the current state.
