#include "cyclonev.h"

int mistral::CycloneV::inv_get_default(const inverter_info &inf) const
{
  switch(inf.pos_and_def & inverter_info::DEF_MASK) {
  case inverter_info::DEF_0: return 0; break;
  case inverter_info::DEF_1: return 1; break;

  case inverter_info::DEF_GP: {
    pnode_coords pnode = rnode_to_pnode(inf.node);
    if(pnode.pt() != DATAOUT && pnode.pt() != OEIN)
      return 0;

    bool is_wired = pin_find_pos(pnode.p(), pnode.bi());
    if(pnode.pt() == DATAOUT || pnode.pi() == 0)
      return is_wired ? 0 : 1;
    return is_wired ? 1 : 0;
  }

  case inverter_info::DEF_HMC: {
    pnode_coords pnode = rnode_to_pnode(inf.node);
    pnode = hmc_get_bypass(pnode);
    if(!pnode)
      return 0;

    auto gpio = p2p_to(pnode);
    if(!gpio) {
      auto gpiol = p2p_from(pnode);
      for(pnode_coords gp : gpiol)
	if(gp.bt() == GPIO) {
	  gpio = gp;
	  break;
	}
    }
    if(!gpio)
      return 0;

    if(gpio.pt() != OEIN && gpio.pt() != DATAOUT)
      return 0;

    bool is_wired = pin_find_pos(gpio.p(), gpio.bi());
    if(gpio.pt() == DATAOUT || !gpio)
      return is_wired ? 0 : 1;
    return is_wired ? 1 : 0;
  }
  }

  return -1;
}

std::vector<mistral::CycloneV::inv_setting_t> mistral::CycloneV::inv_get() const
{
  std::vector<inv_setting_t> res;
  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    uint32_t pos = inf.pos_and_def & ~inverter_info::DEF_MASK;
    bool value = (cram[pos >> 3] >> (pos & 7)) & 1;
    int def = inv_get_default(inf);
    res.emplace_back(inv_setting_t{inf.node, value, int(value) == def});
  }
  return res;
}

void mistral::CycloneV::inv_default_set()
{
  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    uint32_t pos = inf.pos_and_def & ~inverter_info::DEF_MASK;
    bool def = inv_get_default(inf) == 1;

    if(def)
      cram[pos >> 3] |= 1 << (pos & 7);
    else
      cram[pos >> 3] &= ~(1 << (pos & 7));
  }
}

bool mistral::CycloneV::inv_set(rnode_index node, bool value)
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

mistral::CycloneV::invert_t mistral::CycloneV::rnode_is_inverting(rnode_index ri) const
{
  const rnode_object *ro = ri2ro(ri);
  rnode_coords rn = ro->rc();
  if(rn.t() == WM)
    return INV_NO;

  for(uint32_t i = 0; i != dhead->count_inv; i++) {
    const auto &inf = inverter_infos[i];
    if(inf.node == ri)
      return INV_PROGRAMMABLE;
  }

  if(ro->driver(0) == 0xff)
    return INV_UNKNOWN;

  return dn_info[dn_lookup->index_si[SI_TT][T_85]].drivers[ro->driver(0)].invert ? INV_YES : INV_NO;
}

