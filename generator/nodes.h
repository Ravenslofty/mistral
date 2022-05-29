#ifndef NODES_H
#define NODES_H

#include "prefixtree.h"
#include <string>

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
#include <cv-porttypes.ipp>
#undef P
};

extern const char *const rnode_type_names[];
extern const char *const block_type_names[];
extern const char *const port_type_names[];

// - tile coordinates
struct xycoords {
  uint16_t v;

  xycoords() : v(0) {}
  xycoords(uint32_t x, uint32_t y) : v((x << 7) | y) {}
  explicit xycoords(uint32_t _v) : v(_v) {}

  uint32_t x() const noexcept { return (v >> 7) & 0x7f; }
  uint32_t y() const noexcept { return v & 0x7f; }

  bool operator == (const xycoords &p) const noexcept { return v == p.v; }
  bool operator != (const xycoords &p) const noexcept { return v != p.v; }
  bool operator <  (const xycoords &p) const noexcept { return v < p.v; }
};


// - routing node label as type and x/y/z coordinates
struct rnode_coords {
  uint32_t v;

  rnode_coords() : v(0) {}
  explicit rnode_coords(uint32_t _v) : v(_v) {}
  rnode_coords(rnode_type_t type, xycoords pos, uint32_t z) : v((type << 24) | (pos.v << 10) | z) {}
  rnode_coords(rnode_type_t type, uint32_t x, uint32_t y, uint32_t z) : v((type << 24) | (x << 17) | (y << 10) | z) {}

  rnode_type_t t() const noexcept { return rnode_type_t(v >> 24); }
  xycoords     p() const noexcept { return xycoords((v >> 10) & 0x3fff); }
  uint32_t     x() const noexcept { return (v >> 17) & 0x7f; }
  uint32_t     y() const noexcept { return (v >> 10) & 0x7f; }
  uint32_t     z() const noexcept { return v & 0x3ff; }

  operator bool() const { return v != 0; }

  bool operator == (const rnode_coords &p) const noexcept { return v == p.v; }
  bool operator != (const rnode_coords &p) const noexcept { return v != p.v; }
  bool operator <  (const rnode_coords &p) const noexcept { return v < p.v; }
  bool operator >  (const rnode_coords &p) const noexcept { return v > p.v; }

  std::string to_string() const;
};

// - port node label as block type, block x/y/index, port type, port index
struct pnode_coords {
  uint64_t v;

  pnode_coords() : v(0) {}
  explicit pnode_coords(uint64_t _v) : v(_v) {}

  pnode_coords(block_type_t bt, xycoords pos, port_type_t pt, int8_t bindex, int16_t pindex) :
    v((uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (pos.v << 16) | (pindex & 0xffff)) {}
  pnode_coords(block_type_t bt, uint32_t x, uint32_t y, port_type_t pt, int8_t bindex, int16_t pindex) :
    v((uint64_t(bt) << 52) | (uint64_t(pt) << 40) | (uint64_t(bindex & 0xff) << 32) | (x << 23) | (y << 16)| (pindex & 0xffff)) {}

  block_type_t bt() const noexcept { return block_type_t((v >> 52) & 0xff); }
  port_type_t  pt() const noexcept { return port_type_t((v >> 40) & 0xfff); }
  xycoords     p () const noexcept { return xycoords((v >> 16) & 0x3fff); }
  uint32_t     x () const noexcept { return (v >> 23) & 0x7f; }
  uint32_t     y () const noexcept { return (v >> 16) & 0x7f; }
  int8_t       bi() const noexcept { return (v >> 32) & 0xff; }
  int16_t      pi() const noexcept { return  v        & 0xffff; }

  operator bool() const noexcept { return v != 0; }

  bool operator == (const pnode_coords &p) const noexcept { return v == p.v; }
  bool operator != (const pnode_coords &p) const noexcept { return v != p.v; }
  bool operator <  (const pnode_coords &p) const noexcept { return v < p.v; }

  std::string to_string() const;
};

// - Routing node as index
using rnode_index = uint32_t;

// - Routing node object
struct rnode_object {
  rnode_coords ro_rc;
  rnode_index ro_ri;
  uint8_t ro_pattern;
  uint8_t ro_targets_count;
  uint8_t ro_targets_caps_count;
  uint8_t ro_drivers[2];
  uint16_t ro_line_info_index;
  uint16_t ro_driver_position;
  uint32_t ro_fw_pos;
};

// Additional std::hash specializations for wrapped integerst
template<> struct std::hash<xycoords> {
  std::size_t operator()(const xycoords &v) const noexcept { return std::hash<uint16_t>()(v.v); }
};
template<> struct std::hash<rnode_coords> {
  std::size_t operator()(const rnode_coords &v) const noexcept { return std::hash<uint32_t>()(v.v); }
};
template<> struct std::hash<pnode_coords> {
  std::size_t operator()(const pnode_coords &v) const noexcept { return std::hash<uint64_t>()(v.v); }
};

class NodesReader {
public:
  NodesReader();

  rnode_coords lookup_r(const uint8_t *&p) const;
  pnode_coords lookup_p(const uint8_t *&p) const;

  block_type_t lookup_block(const uint8_t *&p) const;
  port_type_t lookup_port(const uint8_t *&p) const;

private:
  PrefixTree rnames, bnames, pnames;
};

#endif
