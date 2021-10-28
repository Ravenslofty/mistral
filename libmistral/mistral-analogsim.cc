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
  for(size_t i=0; i != nodes.size(); i++)
    switch(nodes[i].type) {
    case N_STD:   printf("node %3d: %s\n", i, nn(i));                             break;
    case N_INPUT: printf("node %3d: %s input\n", i, nn(i));                     break;
    case N_V:     printf("node %3d: %s voltage(%g)\n", i, nn(i), nodes[i].value); break;
    }

  for(size_t i=0; i != components.size(); i++) {
    const auto &c = components[i];
    switch(c.type) {
    case C_R: printf("comp %3d: R(%s, %s, %g (%g))\n", i, nn(c.nodes[0]), nn(c.nodes[1]), c.param, 1/c.param); break;
    case C_C: printf("comp %3d: C(%s, %s, %g)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), c.param); break;
    case C_PASS: printf("comp %3d: Pass(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_BUFF: printf("comp %3d: Buff(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_2PORT: printf("comp %3d: 2Port(%s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1])); break;
    case C_NOQPG: printf("comp %3d: NoQPg(%s, %s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), nn(c.nodes[2])); break;
    }
  }
}

/*
+Debug: NLSPC: dev=0 name=dev000 type='DEV_C' num_terms=2 [ wire_node gnd ] params [ 5.106e-15 ]
+Debug: NLSPC: dev=1 name=dev001 type='DEV_R' num_terms=2 [ wire_node buff ] params [ 0.0104493 ]
+Debug: NLSPC: dev=2 name=dev002 type='DEV_PWL2_PASS' num_terms=2 [ input_node pass1 ] params [ 1 ]
+Debug: NLSPC: dev=3 name=dev003 type='DEV_PWL2_PASS' num_terms=2 [ pass1 pass2 ] params [ 1 ]
+Debug: NLSPC: dev=4 name=dev004 type='DEV_C' num_terms=2 [ pass1 gnd ] params [ 1.54775e-15 ]
+Debug: NLSPC: dev=5 name=dev005 type='DEV_PWL2_2PORT' num_terms=2 [ pass2 out ] params [ 1 ]
+Debug: NLSPC: dev=6 name=dev006 type='DEV_C' num_terms=2 [ pass2 out ] params [ 7.4085e-16 ]
+Debug: NLSPC: dev=7 name=dev007 type='DEV_C' num_terms=2 [ pass2 gnd ] params [ 3.3028e-15 ]
+Debug: NLSPC: dev=8 name=dev008 type='DEV_PWL2_BUFF' num_terms=2 [ out buff ] params [ 1 ]
+Debug: NLSPC: dev=9 name=dev009 type='DEV_R' num_terms=2 [ pass1 vdd ] params [ 1e-09 ]
+Debug: NLSPC: dev=10 name=dev010 type='DEV_R' num_terms=2 [ pass2 vdd ] params [ 1e-09 ]
+Debug: NLSPC: dev=11 name=dev011 type='DEV_C' num_terms=2 [ out gnd ] params [ 5.8167e-15 ]
+Debug: NLSPC: dev=12 name=dev012 type='DEV_R' num_terms=2 [ out gnd ] params [ 1e-09 ]
+Debug: NLSPC: dev=13 name=dev013 type='DEV_C' num_terms=2 [ buff gnd ] params [ 3.262e-15 ]
+Debug: NLSPC: dev=14 name=dev014 type='DEV_R' num_terms=2 [ buff gnd ] params [ 1e-09 ]
+Debug: NLSPC: dev=15 name=dev015 type='DEV_C' num_terms=2 [ out buff ] params [ 1.2215e-15 ]
+Debug: NLSPC: dev=16 name=dev016 type='DEV_R' num_terms=2 [ wire_node n008 ] params [ 0.00656789 ]
+Debug: NLSPC: dev=17 name=dev017 type='DEV_C' num_terms=2 [ n008 gnd ] params [ 9.09455e-15 ]
+Debug: NLSPC: dev=18 name=dev018 type='DEV_R' num_terms=2 [ n008 n009 ] params [ 0.0050041 ]
+Debug: NLSPC: dev=19 name=dev019 type='DEV_C' num_terms=2 [ n009 gnd ] params [ 1.29927e-14 ]
+Debug: NLSPC: dev=20 name=dev020 type='DEV_R' num_terms=2 [ n009 n010 ] params [ 0.00412103 ]
+Debug: NLSPC: dev=21 name=dev021 type='DEV_C' num_terms=2 [ n010 gnd ] params [ 1.14886e-14 ]
+Debug: NLSPC: dev=22 name=dev022 type='DEV_R' num_terms=2 [ n010 n011 ] params [ 0.00412103 ]
+Debug: NLSPC: dev=23 name=dev023 type='DEV_C' num_terms=2 [ n011 gnd ] params [ 1.55071e-14 ]
+Debug: NLSPC: dev=24 name=dev024 type='DEV_R' num_terms=2 [ n011 n012 ] params [ 0.00214462 ]
+Debug: NLSPC: dev=25 name=dev025 type='DEV_C' num_terms=2 [ n012 gnd ] params [ 2.3373e-14 ]
+Debug: NLSPC: dev=26 name=dev026 type='DEV_R' num_terms=2 [ n012 n013 ] params [ 0.00146974 ]
+Debug: NLSPC: dev=27 name=dev027 type='DEV_C' num_terms=2 [ n013 gnd ] params [ 2.26446e-14 ]
+Debug: NLSPC: dev=28 name=dev028 type='DEV_R' num_terms=2 [ n013 n014 ] params [ 0.00350287 ]
+Debug: NLSPC: dev=29 name=dev029 type='DEV_C' num_terms=2 [ n014 gnd ] params [ 1.34889e-14 ]

Debug: NLSPC: dev=0 name=dev000 type='DEV_C' num_terms=2 [ wire_node gnd ] params [ 5.106e-15 ]
+Debug: NLSPC: dev=1 name=dev001 type='DEV_R' num_terms=2 [ wire_node buff ] params [ 0.0104493 ]
+Debug: NLSPC: dev=2 name=dev002 type='DEV_PWL2_PASS' num_terms=2 [ input_node pass1 ] params [ 1 ]
+Debug: NLSPC: dev=3 name=dev003 type='DEV_PWL2_PASS' num_terms=2 [ pass1 pass2 ] params [ 1 ]
+Debug: NLSPC: dev=4 name=dev004 type='DEV_C' num_terms=2 [ pass1 gnd ] params [ 1.54615e-15 ]
+Debug: NLSPC: dev=5 name=dev005 type='DEV_PWL2_2PORT' num_terms=2 [ pass2 out ] params [ 1 ]
+Debug: NLSPC: dev=6 name=dev006 type='DEV_C' num_terms=2 [ pass2 out ] params [ 7.4085e-16 ]
+Debug: NLSPC: dev=7 name=dev007 type='DEV_C' num_terms=2 [ pass2 gnd ] params [ 3.301e-15 ]
+Debug: NLSPC: dev=8 name=dev008 type='DEV_PWL2_BUFF' num_terms=2 [ out buff ] params [ 1 ]
+Debug: NLSPC: dev=9 name=dev009 type='DEV_R' num_terms=2 [ pass1 vdd ] params [ 1e-09 ]
+Debug: NLSPC: dev=10 name=dev010 type='DEV_R' num_terms=2 [ pass2 vdd ] params [ 1e-09 ]
+Debug: NLSPC: dev=11 name=dev011 type='DEV_C' num_terms=2 [ out gnd ] params [ 5.8167e-15 ]
+Debug: NLSPC: dev=12 name=dev012 type='DEV_R' num_terms=2 [ out gnd ] params [ 1e-09 ]
+Debug: NLSPC: dev=13 name=dev013 type='DEV_C' num_terms=2 [ buff gnd ] params [ 3.262e-15 ]
+Debug: NLSPC: dev=14 name=dev014 type='DEV_R' num_terms=2 [ buff gnd ] params [ 1e-09 ]
+Debug: NLSPC: dev=15 name=dev015 type='DEV_C' num_terms=2 [ out buff ] params [ 1.2215e-15 ]
Debug: NLSPC: dev=16 name=dev016 type='DEV_R' num_terms=2 [ wire_node n008 ] params [ 0.00656789 ]
Debug: NLSPC: dev=17 name=dev017 type='DEV_C' num_terms=2 [ n008 gnd ] params [ 9.0942e-15 ]
Debug: NLSPC: dev=18 name=dev018 type='DEV_R' num_terms=2 [ n008 n009 ] params [ 0.0050041 ]
Debug: NLSPC: dev=19 name=dev019 type='DEV_C' num_terms=2 [ n009 gnd ] params [ 1.29919e-14 ]
Debug: NLSPC: dev=20 name=dev020 type='DEV_R' num_terms=2 [ n009 n010 ] params [ 0.00412103 ]
Debug: NLSPC: dev=21 name=dev021 type='DEV_C' num_terms=2 [ n010 gnd ] params [ 1.14882e-14 ]
Debug: NLSPC: dev=22 name=dev022 type='DEV_R' num_terms=2 [ n010 n011 ] params [ 0.00412103 ]
Debug: NLSPC: dev=23 name=dev023 type='DEV_C' num_terms=2 [ n011 gnd ] params [ 1.55067e-14 ]
Debug: NLSPC: dev=24 name=dev024 type='DEV_R' num_terms=2 [ n011 n012 ] params [ 0.00214462 ]
Debug: NLSPC: dev=25 name=dev025 type='DEV_C' num_terms=2 [ n012 gnd ] params [ 2.33727e-14 ]
Debug: NLSPC: dev=26 name=dev026 type='DEV_R' num_terms=2 [ n012 n013 ] params [ 0.00146974 ]
Debug: NLSPC: dev=27 name=dev027 type='DEV_C' num_terms=2 [ n013 gnd ] params [ 2.26439e-14 ]
Debug: NLSPC: dev=28 name=dev028 type='DEV_R' num_terms=2 [ n013 n014 ] params [ 0.00350287 ]
Debug: NLSPC: dev=29 name=dev029 type='DEV_C' num_terms=2 [ n014 gnd ] params [ 1.34854e-14 ]
*/
