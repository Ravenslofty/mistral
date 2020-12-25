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
