#include "analogsim.h"

int mistral::AnalogSim::gn()
{
  int id = nodes.size();
  nodes.emplace_back(node{N_STD, 0});
  return id;
}

int mistral::AnalogSim::gn_v(double v)
{
  int id = nodes.size();
  nodes.emplace_back(node{ N_V, v });
  return id;
}

int mistral::AnalogSim::gn_input()
{
  int id = nodes.size();
  nodes.emplace_back(node{N_INPUT, 0});
  return id;
}

void mistral::AnalogSim::add_resistor(int n1, int n2, double r)
{
  components.emplace_back(component{ C_R, { n1, n2, -1 }, r, nullptr, nullptr });
}

void mistral::AnalogSim::add_capacitor(int n1, int n2, double c)
{
  components.emplace_back(component{ C_C, { n1, n2, -1 }, c, nullptr, nullptr });
}

void mistral::AnalogSim::add_tran3_noq_pg(int n1, int n2, int n3, const lookup_table_2 *table)
{
  components.emplace_back(component{ C_TRAN3_NOQ_PG, { n1, n2, n3 }, 0, table, nullptr });
}

void mistral::AnalogSim::add_pwl2_2port(int n1, int n2, const lookup_table_2 *table1, const lookup_table_2 *table2)
{
  components.emplace_back(component{ C_PWL2_2PORT, { n1, n2, -1 }, 0, table1, table2 });
}

