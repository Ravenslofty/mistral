#include "mistral-analogsim.h"

#include <stdio.h>
#include <stdlib.h>

int mistral::AnalogSim::gn(const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node{N_STD, 0, mkname(name, id)});
  return id;
}

int mistral::AnalogSim::gn_v(double v, const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node{N_V, v, mkname(name, id)});
  return id;
}

int mistral::AnalogSim::gn_input(const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node{N_INPUT, 0, mkname(name, id)});
  return id;
}

void mistral::AnalogSim::add_gnd_vdd(double vdd)
{
  if(!nodes.empty()) {
    fprintf(stderr, "add_gnd_vdd called on non-empty circuit\n");
    abort();
  }
  nodes.emplace_back(node{ N_V, 0,   "gnd" });
  nodes.emplace_back(node{ N_V, vdd, "vdd" });
}

void mistral::AnalogSim::add_r(int n1, int n2, double r)
{
  components.emplace_back(component{ C_R, { n1, n2, -1 }, r, nullptr, nullptr, nullptr });
}

void mistral::AnalogSim::add_c(int n1, int n2, double c)
{
  components.emplace_back(component{ C_C, { n1, n2, -1 }, c, nullptr, nullptr, nullptr });
}

void mistral::AnalogSim::add_pass(int n1, int n2, table2_lookup pass)
{
  components.emplace_back(component{ C_PASS, { n1, n2, -1 }, 0, pass, nullptr, nullptr });
}

void mistral::AnalogSim::add_buff(int n1, int n2, table2_lookup driver)
{
  components.emplace_back(component{ C_BUFF, { n1, n2, -1 }, 0, driver, nullptr, nullptr });
}

void mistral::AnalogSim::add_2port(int n1, int n2, table2_lookup pullup, table2_lookup output)
{
  components.emplace_back(component{ C_2PORT, { n1, n2, -1 }, 0, pullup, output, nullptr });
}

void mistral::AnalogSim::add_noqpg(int n1, int n2, int n3, table3_lookup pass)
{
  components.emplace_back(component{ C_NOQPG, { n1, n2, n3 }, 0, nullptr, nullptr, pass });
}

void mistral::AnalogSim::show() const
{
  if(true) {
    for(size_t i=0; i != nodes.size(); i++)
      switch(nodes[i].type) {
      case N_STD:   printf("node %3zu: %s\n", i, nn(i));                             break;
      case N_INPUT: printf("node %3zu: %s input\n", i, nn(i));                     break;
      case N_V:     printf("node %3zu: %s voltage(%g)\n", i, nn(i), nodes[i].value); break;
      }
  }

  for(size_t i=0; i != components.size(); i++) {
    const auto &c = components[i];
    switch(c.type) {
    case C_R: printf("comp %3zu: R(%s, %s, %g (%g))\n", i, nn(c.nodes[0]), nn(c.nodes[1]), c.param, 1/c.param); break;
    case C_C: printf("comp %3zu: C(%s, %s, %g)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), c.param); break;
    case C_PASS: printf("comp %3zu: Pass(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_BUFF: printf("comp %3zu: Buff(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_2PORT: printf("comp %3zu: 2Port(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_NOQPG: printf("comp %3zu: NoQPg(%s, %s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), nn(c.nodes[2])); break;
    }
  }
}
