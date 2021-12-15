#include "cyclonev.h"

std::vector<mistral::CycloneV::inv_setting_t> mistral::CycloneV::inv_get() const
{
  std::vector<inv_setting_t> res;
  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    uint32_t pos = inf.pos_and_def & ~inverter_info::DEF_MASK;
    bool value = (cram[pos >> 3] >> (pos & 7)) & 1;
    bool def = false;
    switch(inf.pos_and_def & inverter_info::DEF_MASK) {
    case inverter_info::DEF_0: def = value == false; break;
    case inverter_info::DEF_1: def = value == true; break;

    case inverter_info::DEF_GP_0:
    case inverter_info::DEF_GP_1: {
      bool base = (inf.pos_and_def & inverter_info::DEF_MASK) == inverter_info::DEF_GP_0;
      pnode_t pnode = rnode_to_pnode(inf.node);
      if(pin_find_pnode(pnode))
	base = !base;
      def = value == base;
      break;
    }
    }
      
    res.emplace_back(inv_setting_t{inf.node, value, def});
  }
  return res;
}

bool mistral::CycloneV::inv_set(rnode_t node, bool value)
{
  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    if(inf.node == node) {
      uint32_t pos = inf.pos_and_def & ~inverter_info::DEF_MASK;
      if(value)
	cram[pos >> 3] |= 1 << (pos & 7);
      else
	cram[pos >> 3] &= ~(1 << (pos & 7));
      return true;
    }
  }
  return false;
}

mistral::CycloneV::invert_t mistral::CycloneV::rnode_is_inverting(rnode_t rn) const
{
  if(rn2t(rn) == WM)
    return INV_NO;

  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    if(inf.node == rn)
      return INV_PROGRAMMABLE;
  }
  const rnode_base *rb = rnode_lookup(rn);
  if(!rb)
    return INV_UNKNOWN;

  if(rb->drivers[0] == 0xff)
    return INV_UNKNOWN;
  int driver = rb->drivers[0];
  return dn_info[dn_lookup->index[model->speed_grade][T_85][DELAY_MAX]].drivers[driver].invert ? INV_YES : INV_NO;
}

