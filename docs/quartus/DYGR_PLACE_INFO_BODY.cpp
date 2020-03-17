#include <cstdint>

#include <string>
#include <unordered_map>
#include <vector>

struct DYGR_PLACE_INFO_BODY {
    UNKNOWN1*              m_clock_region_info;
    UNKNOWN2*              m_blk_interfaces_info;
    std::vector<UNKNOWN3>  m_place_block_list;
    std::vector<UNKNOWN4*> m_blk_type_info_list;
    std::vector<UNKNOWN5*> m_phys_impl_list;
    std::vector<UNKNOWN6>  m_place_block_type_info;
    std::vector<UNKNOWN7*> m_blk_type_info_lookup;
    std::unordered_map<std::string, UNKNOWN8>
};

struct UNKNOWN1;

struct UNKNOWN2;

struct UNKNOWN3 {
    uint16_t m_phys_impl_index;
    uint16_t m_x;
    uint16_t m_y;
    uint16_t m_z;
    uint32_t m_container_blk_id;
    uint16_t m_level;
};

struct UNKNOWN4;

struct UNKNOWN5;

struct UNKNOWN6 {
    uint32_t start_index;
    uint32_t num;
};

struct UNKNOWN7;

struct UNKNOWN8 {
    uint16_t m_block_type;
    uint16_t m_x_loc;
    uint16_t m_y_loc;
    uint16_t m_z_loc;
}