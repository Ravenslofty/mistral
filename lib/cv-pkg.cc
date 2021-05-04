#include "cyclonev.h"

const mistral::CycloneV::pin_info_t *mistral::CycloneV::pin_find_pos(pos_t pos, int index) const
{
    uint16_t key = pos | (index << 14);
    int np = package_infos[model->package].pin_count;
    const pin_info_t *pins = di.packages[model->package];
    for(int i=0; i != np; i++)
	if(pins[i].pad == key)
	    return pins+i;
    return nullptr;
}

const mistral::CycloneV::pin_info_t *mistral::CycloneV::pin_find_pnode(mistral::CycloneV::pnode_t pn) const
{
  if(!pn)
    return nullptr;

  // Direct connection
  if(pn2bt(pn) == GPIO && pn2bi(pn) != -1)
    return pin_find_pos(pn2p(pn), pn2bi(pn));

  // Connection through HMC bypass.  TODO: detect if bypass is active
  if(pn2bt(pn) == HMC) {
    pn = hmc_get_bypass(pn);
    auto gpio = p2p_to(pn);
    if(!gpio) {
      auto gpiol = p2p_from(pn);
      for(pnode_t gp : gpiol)
	if(pn2bt(gp) == GPIO) {
	  gpio = gp;
	  break;
	}
    }
    if(!gpio || pn2bt(gpio) != GPIO || pn2bi(gpio) == -1)
      return nullptr;
    return pin_find_pos(pn2p(gpio), pn2bi(gpio));
  }

  return nullptr;
}

