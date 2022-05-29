#include "cyclonev.h"

const mistral::CycloneV::pin_info_t *mistral::CycloneV::pin_find_pos(xycoords pos, int index) const
{
    uint16_t key = pos.v | (index << 14);
    int np = package_infos[model->package].pin_count;
    const pin_info_t *pins = di.packages[model->package]->pins;
    for(int i=0; i != np; i++)
	if(pins[i].pad == key)
	    return pins+i;
    return nullptr;
}

const mistral::CycloneV::pin_info_t *mistral::CycloneV::pin_find_pnode(mistral::CycloneV::pnode_coords pn) const
{
  if(!pn)
    return nullptr;

  // Direct connection
  if(pn.bt() == GPIO && pn.bi() != -1)
    return pin_find_pos(pn.p(), pn.bi());

  // Connection through HMC bypass.  TODO: detect if bypass is active
  if(pn.bt() == HMC) {
    pn = hmc_get_bypass(pn);
    auto gpio = p2p_to(pn);
    if(!gpio) {
      auto gpiol = p2p_from(pn);
      for(pnode_coords gp : gpiol)
	if(gp.bt() == GPIO) {
	  gpio = gp;
	  break;
	}
    }
    if(!gpio || gpio.bt() != GPIO || gpio.bi() == -1)
      return nullptr;
    return pin_find_pos(gpio.p(), gpio.bi());
  }

  return nullptr;
}

const mistral::CycloneV::pin_info_t *mistral::CycloneV::pin_find_name(const std::string &name) const
{
  int np = package_infos[model->package].pin_count;
  const pin_info_t *pins = di.packages[model->package]->pins;
  for(int i=0; i != np; i++)
    if(std::string(pins[i].name) == name)
      return pins+i;
  return nullptr;
}
