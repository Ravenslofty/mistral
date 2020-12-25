#include "cyclonev.h"

std::vector<mistral::CycloneV::inv_setting_t> mistral::CycloneV::inv_get() const
{
    std::vector<inv_setting_t> res;
    for(uint32_t i = 0; i != di.inverters_count; i++) {
	const auto &inf = di.inverters[i];
	uint32_t pos = inf.cram_y * di.cram_sx + inf.cram_x;
	bool value = (cram[pos >> 3] >> (pos & 7)) & 1;
	bool def = value == inf.def;
	res.emplace_back(inv_setting_t{inf.node, value, def});
    }
    return res;
}

bool mistral::CycloneV::inv_set(rnode_t node, bool value)
{
    for(uint32_t i = 0; i != di.inverters_count; i++) {
	const auto &inf = di.inverters[i];
	if(inf.node == node) {
	    uint32_t pos = inf.cram_y * di.cram_sx + inf.cram_x;
	    if(value)
		cram[pos >> 3] |= 1 << (pos & 7);
	    else
		cram[pos >> 3] &= ~(1 << (pos & 7));
	    return true;
	}
    }
    return false;
}

