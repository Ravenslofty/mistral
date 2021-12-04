#include "mistral-analogsim.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <utility>

void mistral::AnalogSim::vector_alloc(std::vector<double> &vect, int size)
{
  vect.resize(size);
}

void mistral::AnalogSim::matrix_alloc(std::vector<double> &matrix, int sx, int sy)
{
  if(!sy)
    sy = sx;
  matrix.resize(sx*sy);
}

void mistral::AnalogSim::vector_clear(std::vector<double> &vect)
{
  std::fill(vect.begin(), vect.end(), 0.0);
}

void mistral::AnalogSim::matrix_clear(std::vector<double> &matrix)
{
  std::fill(matrix.begin(), matrix.end(), 0.0);
}

void mistral::AnalogSim::matrix_identity(std::vector<double> &matrix, int size)
{
  std::fill(matrix.begin(), matrix.end(), 0.0);
  for(int i=0; i != size; i++)
    matrix[i*size+i] = 1.0;
}

void mistral::AnalogSim::matrix_invert(std::vector<double> &matrix, std::vector<double> &inverse, int size)
{
  matrix_clear(inverse);
  for(int y=0; y != size; y++)
    inverse[y*size + y] = 1.0;
  for(int y = 0; y != size; y++) {
    if(!matrix[y*size+y]) {
      // Need to swap the line with one with a non-zero value in the slot
      int y1;
      for(y1 = y+1; y1 != size && !matrix[y1*size+y]; y1++);
      if(y1 == size) {
	fprintf(stderr, "Noninvertible matrix\n");
	exit(1);
      }
      for(int x = 0; x != size; x++)
	std::swap(matrix[y*size+x], matrix[y1*size+x]);
      for(int x = 0; x != size; x++)
	std::swap(inverse[y*size+x], inverse[y1*size+x]);
    }

    // Normalize the diagonal to 1
    double mult = 1/matrix[y*size+y];
    matrix[y*size+y] = 1.0;
    for(int x = y+1; x != size; x++)
      matrix[y*size+x] *= mult;
    for(int x = 0; x != size; x++)
      inverse[y*size+x] *= mult;

    // Clear the column on the other lines
    for(int y1 = 0; y1 != size; y1++)
      if(y1 != y && matrix[y1*size+y]) {
	double mult2 = -matrix[y1*size+y];
	for(int x = 0; x != size; x++)
	  if(x == y)
	    matrix[y1*size+x] = 0;
	  else
	    matrix[y1*size+x] += mult2 * matrix[y*size+x];
	for(int x = 0; x != size; x++)
	  inverse[y1*size+x] += mult2 * inverse[y*size+x];
      }
  }
}


void mistral::AnalogSim::matrix_mul(std::vector<double> &dest, const std::vector<double> &s1, const std::vector<double> &s2, int s1x, int s1y, int s2x, int s2y, int s1stride, int s2stride)
{
  assert(s1x == s2y);

  if(!s1stride)
    s1stride = s1x;
  if(!s2stride)
    s2stride = s2x;

  for(int i = 0; i != s2x; i++)
    for(int j = 0; j != s1y; j++) {
      double v = 0;
      for(int k = 0; k != s2y; k++)
	v += s1[j*s1stride + k] * s2[k*s2stride + i];
      dest[j*s2x + i] = v;
    }
}

void mistral::AnalogSim::matrix_combine(std::vector<double> &dest, const std::vector<double> &s1, const std::vector<double> &s2, int sx, int sy, double s2_mul)
{
  for(int i = 0; i != sx*sy; i++)
    dest[i] = s1[i] + s2_mul * s2[i];
}

void mistral::AnalogSim::vector_show(const std::vector<double> &vect, int size)
{
  for(int i = 0; i != size; i++)
    printf("%2d %-20s: %12g\n", i, nodes[nodes_order[i]].name.c_str(), vect[i]);
}


void mistral::AnalogSim::matrix_show(const std::vector<double> &matrix, int sx, int sy, bool full, int stride)
{
  if(!stride)
    stride = sx;

  if(full) {
    for(int y = 0; y != sy; y++) {
      printf("%2d:", y);
      for(int x = 0; x != sx; x++)
	printf(" %12g", matrix[y*stride + x]);
      printf("\n");
    }

  } else {
    for(int y = 0; y != sy; y++) {
      printf("row %2d:", y);
      for(int x = 0; x != sx; x++)
	if(matrix[y*stride + x])
	  printf(" %d:%g", x, matrix[y*stride + x]);
      printf("\n");
    }
  }
}

int mistral::AnalogSim::gn(const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node(N_STD, mkname(name, id)));
  return id;
}

int mistral::AnalogSim::gn_v(double v, const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node(N_V, v, mkname(name, id)));
  return id;
}

int mistral::AnalogSim::gn_input(const char *name)
{
  int id = nodes.size();
  nodes.emplace_back(node(N_INPUT, mkname(name, id)));
  return id;
}

void mistral::AnalogSim::add_gnd_vdd(double vdd)
{
  if(!nodes.empty()) {
    fprintf(stderr, "add_gnd_vdd called on non-empty circuit\n");
    abort();
  }
  nodes.emplace_back(node(N_V, 0,   "gnd"));
  nodes.emplace_back(node(N_V, vdd, "vdd"));
}

void mistral::AnalogSim::add_r(int n1, int n2, double r)
{
  components.emplace_back(component{ C_R, { n1, n2, -1 }, {}, r, nullptr, nullptr, nullptr });
}

void mistral::AnalogSim::add_c(int n1, int n2, double c)
{
  components.emplace_back(component{ C_C, { n1, n2, -1 }, {}, c, nullptr, nullptr, nullptr });
}

void mistral::AnalogSim::add_pass(int n1, int n2, std::unique_ptr<table2_lookup> pass)
{
  components.emplace_back(component{ C_PASS, { n1, n2, -1 }, {}, 0, std::move(pass), nullptr, nullptr });
}

void mistral::AnalogSim::add_buff(int n1, int n2, std::unique_ptr<table2_lookup> driver)
{
  components.emplace_back(component{ C_BUFF, { n1, n2, -1 }, {}, 0, std::move(driver), nullptr, nullptr });
}

void mistral::AnalogSim::add_2port(int n1, int n2, std::unique_ptr<table2_lookup> pullup, std::unique_ptr<table2_lookup> output)
{
  components.emplace_back(component{ C_2PORT, { n1, n2, -1 }, {}, 0, std::move(pullup), std::move(output), nullptr });
}

void mistral::AnalogSim::add_noqpg(int n1, int n2, int n3, std::unique_ptr<table3_lookup> pass)
{
  components.emplace_back(component{ C_NOQPG, { n1, n2, n3 }, {}, 0, nullptr, nullptr, std::move(pass) });
}

void mistral::AnalogSim::show() const
{
  if(true) {
    for(size_t i=0; i != nodes.size(); i++)
      switch(nodes[i].type) {
      case N_STD:   printf("node %3zu: %s\n", i, nn(i));                             break;
      case N_INPUT: printf("node %3zu: %s input\n", i, nn(i));                       break;
      case N_V:     printf("node %3zu: %s voltage(%g)\n", i, nn(i), nodes[i].value); break;
      }
  }

  for(size_t i=0; i != components.size(); i++) {
    const auto &c = components[i];
    switch(c.type) {
    case C_R: printf("comp %3zu: R(%s, %s, %g (%g))\n",        i, nn(c.nodes[0]), nn(c.nodes[1]), c.param, 1/c.param); break;
    case C_C: printf("comp %3zu: C(%s, %s, %g)\n",             i, nn(c.nodes[0]), nn(c.nodes[1]), c.param); break;
    case C_PASS: printf("comp %3zu: Pass(%s, %s, %s)\n",       i, nn(c.nodes[0]), nn(c.nodes[1]), c.t2a->name.c_str()); break;
    case C_BUFF: printf("comp %3zu: Buff(%s, %s, %s)\n",       i, nn(c.nodes[0]), nn(c.nodes[1]), c.t2a->name.c_str()); break;
    case C_2PORT: printf("comp %3zu: 2Port(%s, %s, %s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), c.t2a->name.c_str(), c.t2b->name.c_str()); break;
    case C_NOQPG: printf("comp %3zu: NoQPg(%s, %s, %s, %s)\n", i, nn(c.nodes[0]), nn(c.nodes[1]), nn(c.nodes[2]), c.t3a->name.c_str()); break;
    }
  }
}

void mistral::AnalogSim::set_input_wave(int node, const wave &w)
{
  nodes[node].w = input_waves.size();
  input_waves.push_back(w);
}

void mistral::AnalogSim::set_output_wave(int node, wave &w)
{
  assert(nodes[node].type == N_STD);
  nodes[node].w = output_waves.size();
  output_waves.push_back(&w);
}

bool mistral::AnalogSim::node_fixed_voltage(int node) const
{
  return nodes[node].type != N_STD;
}

bool mistral::AnalogSim::node_fixed_voltage_ordered(int node) const
{
  return node >= first_fixed_node;
}

void mistral::AnalogSim::order_nodes()
{
  node_count = nodes.size();
  nodes_order.resize(node_count);
  inverse_nodes_order.resize(node_count);

  // Identify nodes connected to non-linear components
  std::vector<bool> nl_nodes(node_count, false);
  for(const auto &c : components) {
    int nn = 0;
    switch(c.type) {
    case C_R:
    case C_C:
      nn = 0;
      break;

    case C_PASS:
    case C_BUFF:
    case C_2PORT:
      nn = 2;
      break;

    case C_NOQPG:
      nn = 3;
      break;
    }
    for(int n1 = 0; n1 != nn; n1 ++)
      nl_nodes[c.nodes[n1]] = true;
  }

  // First in order are the non-constant non-linear nodes
  int ni = 0;
  for(int i = 0; i != node_count; i++)
    if(nl_nodes[i] && !node_fixed_voltage(i))
      nodes_order[ni++] = i;
  non_linear_nodes_count = ni;

  // Then the rest of the non-constant nodes
  for(int i = 0; i != node_count; i++)
    if(!nl_nodes[i] && !node_fixed_voltage(i))
      nodes_order[ni++] = i;

  // Then the constant nodes
  first_fixed_node = ni;
  for(int i = 0; i != node_count; i++)
    if(node_fixed_voltage(i))
      nodes_order[ni++] = i;

  // Generate the inverse order too
  for(int i = 0; i != node_count; i++)
    inverse_nodes_order[nodes_order[i]] = i;

  // Add the mapping in the components
  for(auto &c : components)
    for(int i = 0; i != 3; i++)
      c.onodes[i] = c.nodes[i] == -1 ? -1 : inverse_nodes_order[c.nodes[i]];
}

void mistral::AnalogSim::show_nodes_order() const
{
  for(int i=0; i != node_count; i++)
    printf("%d => %s (%g)\n", i, nodes[nodes_order[i]].name.c_str(), voltages[0][i]);
}

void mistral::AnalogSim::compute_time_range()
{
  input_start_time = input_end_time = 0;
  for(const wave &w : input_waves) {
    double start = w.front().t;
    double end = w.back().t;
    if(!input_end_time || start < input_start_time)
      input_start_time = start;
    if(end > input_end_time)
      input_end_time = end;
  }
}

double mistral::AnalogSim::input_voltage_at_time(int input, double time)
{
  const node &n = nodes[nodes_order[input]];
  assert(n.type == N_INPUT);
  assert(n.w != -1);
  const wave &w = input_waves[n.w];
  if(time <= w.front().t)
    return w.front().v;
  if(time >= w.back().t)
    return w.back().v;
  size_t index;
  for(index = 1; time > w[index].t; index++);
  double ratio = (time - w[index-1].t) / (w[index].t - w[index-1].t);
  return w[index-1].v * (1-ratio) + w[index].v * ratio;
}

bool mistral::AnalogSim::output_record(double time, bool test_end)
{
  if(!test_end) {
    output_wave_is_rising.resize(output_waves.size());
    for(int i = 0; i != first_fixed_node; i++) {
      const auto &n = nodes[nodes_order[i]];
      if(n.w != -1) {
	double v = voltages[0][i];
	if(v <= 0.1*config_vdd)
	  output_wave_is_rising[n.w] = true;
	else if(v >= 0.9*config_vdd)
	  output_wave_is_rising[n.w] = false;
	else {
	  fprintf(stderr, "Initial voltage on output %s is outside of 10%%-90%% range (%g)\n", n.name.c_str(), v);
	}
      }
    }
  }

  for(int i = 0; i != first_fixed_node; i++) {
    const auto &n = nodes[nodes_order[i]];
    if(n.w != -1) {
      printf("%8.2fps: %s = %5.3f\n", time*1e12, n.name.c_str(), voltages[0][i]);
      output_waves[n.w]->emplace_back(time_slot(time, voltages[0][i]));
    }
  }

  if(test_end) {
    bool done = true;
    for(int i = 0; i != first_fixed_node; i++) {
      const auto &n = nodes[nodes_order[i]];
      if(n.w != -1) {
	double v = voltages[0][i];
	if(output_wave_is_rising[n.w]) {
	  if(v < 0.9*config_vdd)
	    done = false;
	} else {
	  if(v > 0.1*config_vdd)
	    done = false;
	}
      }
    }
    return done;
  } else
    return false;
}

void mistral::AnalogSim::init_voltages()
{
  vector_clear(voltages[0]);
  vector_clear(voltage_offsets);

  for(int i = first_fixed_node; i != node_count; i++) {
    const auto &n = nodes[nodes_order[i]];
    if(n.type == N_V)
      voltages[0][i] = n.value;
    else
      voltages[0][i] = input_voltage_at_time(i, input_start_time);
    voltage_offsets[i] = -voltages[0][i];
  }

  for(const auto &c : components)
    if(c.type == C_R) {
      int n0 = c.onodes[0];
      int n1 = c.onodes[1];
      bool f0 = node_fixed_voltage_ordered(n0);
      bool f1 = node_fixed_voltage_ordered(n1);
      if(f0 && !f1) {
	voltage_offsets[n1] = -voltages[0][n0];
	if(c.param == 1e9)
	  voltages[0][n1] = voltages[0][n0];
      }
      if(!f0 && f1) {
	voltage_offsets[n0] = -voltages[0][n1];
	if(c.param == 1e9)
	  voltages[0][n0] = voltages[0][n1];
      }
    }
}

void mistral::AnalogSim::update_input_voltages(double time)
{
  for(int i = first_fixed_node; i != node_count; i++) {
    const auto &n = nodes[nodes_order[i]];
    if(n.type == N_INPUT) {
      voltages[0][i] = input_voltage_at_time(i, time);
      voltage_offsets[i] = -voltages[0][i];
      //      printf("%8.2fps: %s = %5.3f\n", time*1e12, n.name.c_str(), voltages[0][i]);
    }
  }
}

void mistral::AnalogSim::add_linear_matrix_entry(std::vector<double> &matrix, int n0, int n1, double v)
{
  if(!node_fixed_voltage_ordered(n0))
    matrix[n0*node_count + n1] += v;
}

void mistral::AnalogSim::build_resistor_matrix(std::vector<double> &matrix)
{
  matrix_clear(matrix);
  for(int i=first_fixed_node; i != node_count; i++)
    matrix[i*node_count+i] = 1.0;

  for(const auto &c : components)
    if(c.type == C_R) {
      add_linear_matrix_entry(matrix, c.onodes[0], c.onodes[0],  1/c.param);
      add_linear_matrix_entry(matrix, c.onodes[0], c.onodes[1], -1/c.param);
      add_linear_matrix_entry(matrix, c.onodes[1], c.onodes[1],  1/c.param);
      add_linear_matrix_entry(matrix, c.onodes[1], c.onodes[0], -1/c.param);
    }
}

void mistral::AnalogSim::build_capacitor_matrix(std::vector<double> &matrix)
{
  matrix_clear(matrix);
  for(const auto &c : components)
    if(c.type == C_C) {
      add_linear_matrix_entry(matrix, c.onodes[0], c.onodes[0],  c.param);
      add_linear_matrix_entry(matrix, c.onodes[0], c.onodes[1], -c.param);
      add_linear_matrix_entry(matrix, c.onodes[1], c.onodes[1],  c.param);
      add_linear_matrix_entry(matrix, c.onodes[1], c.onodes[0], -c.param);
    }
}

void mistral::AnalogSim::add_non_linear_current(int n0, double v)
{
  if(n0 < non_linear_nodes_count)
    non_linear_current[n0] += v;
}

void mistral::AnalogSim::add_non_linear_current_deriv(int n0, int n1, double v)
{
  if(n0 < non_linear_nodes_count && n1 < non_linear_nodes_count)
    non_linear_current_deriv[n0*non_linear_nodes_count + n1] += v;
}

void mistral::AnalogSim::compute_non_linear_currents()
{
  vector_clear(non_linear_current);
  matrix_clear(non_linear_current_deriv);

  for(const auto &c : components)
    switch(c.type) {
    case C_R:
    case C_C:
      break;

    case C_2PORT: {
      double va, dvxa, dvya, vb, dvxb, dvyb;
      c.t2a->lookup(voltages[0][c.onodes[0]], voltages[0][c.onodes[1]], va, dvxa, dvya);
      c.t2b->lookup(voltages[0][c.onodes[0]], voltages[0][c.onodes[1]], vb, dvxb, dvyb);
      add_non_linear_current(c.onodes[0], va);
      add_non_linear_current(c.onodes[1], vb);
      add_non_linear_current_deriv(c.onodes[0], c.onodes[0], dvxa);
      add_non_linear_current_deriv(c.onodes[1], c.onodes[0], dvxb);
      add_non_linear_current_deriv(c.onodes[0], c.onodes[1], dvya);
      add_non_linear_current_deriv(c.onodes[1], c.onodes[1], dvyb);
      break;
    }

    case C_NOQPG: {
      double v, dvx, dvy, dvz;
      c.t3a->lookup(voltages[0][c.onodes[0]], voltages[0][c.onodes[1]], voltages[0][c.onodes[2]], v, dvx, dvy, dvz);

      add_non_linear_current(c.onodes[0],  v);
      add_non_linear_current(c.onodes[2], -v);
      add_non_linear_current_deriv(c.onodes[0], c.onodes[0],  dvx);
      add_non_linear_current_deriv(c.onodes[2], c.onodes[0], -dvx);
      add_non_linear_current_deriv(c.onodes[0], c.onodes[1],  dvy);
      add_non_linear_current_deriv(c.onodes[2], c.onodes[1], -dvy);
      add_non_linear_current_deriv(c.onodes[0], c.onodes[2],  dvz);
      add_non_linear_current_deriv(c.onodes[2], c.onodes[2], -dvz);
      break;
    }

    default:
      printf("Type %d\n", c.type);
      break;
    }
}

void mistral::AnalogSim::compute_node_functions()
{
  for(int i = 0; i != non_linear_nodes_count; i++) {
    node_functions[i] = voltages[0][i] + voltage_offsets[i];
    for(int j = 0; j != non_linear_nodes_count; j++)
      node_functions[i] += non_linear_current[j] * inverse_linear_matrix[i*node_count + j];
  }
}

void mistral::AnalogSim::compute_jacobian()
{
  matrix_mul(jacobian, inverse_linear_matrix, non_linear_current_deriv, non_linear_nodes_count, non_linear_nodes_count, non_linear_nodes_count, non_linear_nodes_count, node_count, non_linear_nodes_count);

  for(int i = 0; i != non_linear_nodes_count; i++)
    jacobian[i*non_linear_nodes_count + i] += 1.0;
}

double mistral::AnalogSim::step_new_dc_voltages()
{
  double max_dv = 0;
  for(int i = 0; i != non_linear_nodes_count; i++) {
    double abs_dv = fabs(node_currents[i]);
    if(abs_dv > max_dv)
      max_dv = abs_dv;
  }

  double ratio = max_dv > config_max_dv ? config_max_dv / max_dv : 1.0;

  for(int i = 0; i != non_linear_nodes_count; i++)
    voltages[0][i] -= ratio * node_currents[i];

  return max_dv;
}

void mistral::AnalogSim::compute_linear_voltages()
{
  for(int i = non_linear_nodes_count; i != first_fixed_node; i++) {
    double v = -voltage_offsets[i];
    for(int j = 0; j != non_linear_nodes_count; j++)
      v -= non_linear_current[j] * inverse_linear_matrix[i * node_count + j];
    voltages[0][i] = v;
  }
}

void mistral::AnalogSim::add_voltage_offset(int node, double v)
{
  if(node < first_fixed_node)
    voltage_offsets[node] += v;
}

void mistral::AnalogSim::recompute_voltage_offsets(double scale)
{
  for(int i = 0; i != first_fixed_node; i++) {
    double vr = 0, vc = 0;
    for(int j = 0; j != node_count; j++) {
      vr += voltages[1][j] * matrix_r[i * node_count + j];
      vc += voltages[1][j] * matrix_c[i * node_count + j];
    }
    voltage_offsets[i] = vr - scale * vc;
  }

  for(const auto &c : components) {
    switch(c.type) {
    case C_R:
    case C_C:
      break;

    case C_2PORT: {
      double va = c.t2a->lookup(previous_voltages[c.onodes[0]], previous_voltages[c.onodes[1]]);
      double vb = c.t2b->lookup(previous_voltages[c.onodes[0]], previous_voltages[c.onodes[1]]);
      add_voltage_offset(c.onodes[0], va);
      add_voltage_offset(c.onodes[1], vb);
      break;
    }

    case C_NOQPG: {
      double v = c.t3a->lookup(previous_voltages[c.onodes[0]], previous_voltages[c.onodes[1]], previous_voltages[c.onodes[2]]);
      add_voltage_offset(c.onodes[0],  v);
      add_voltage_offset(c.onodes[2], -v);
      break;
    }

    default:
      printf("Type %d\n", c.type);
      break;
    }
  }
}

void mistral::AnalogSim::apply_inverse_linear_matrix_to_voltage_offsets()
{
  std::copy(voltage_offsets.begin(), voltage_offsets.end(), voltage_offsets_hold.begin());
  matrix_mul(voltage_offsets, inverse_linear_matrix, voltage_offsets_hold, node_count, node_count, 1, node_count);
}

void mistral::AnalogSim::converge_voltages()
{
  for(;;) {
    compute_non_linear_currents();
    compute_node_functions();
    compute_jacobian();
    matrix_invert(jacobian, inverse_jacobian, non_linear_nodes_count);
    matrix_mul(node_currents, inverse_jacobian, node_functions, non_linear_nodes_count, non_linear_nodes_count, 1, non_linear_nodes_count);

    double max_dv = step_new_dc_voltages();

    if(max_dv < config_min_dv)
      break;
  }
}

double mistral::AnalogSim::compute_max_delta_voltage() const
{
  double dv = 0;
  for(int i = 0; i != first_fixed_node; i++) {
    double dv1 = fabs(voltages[0][i] - previous_voltages[i]);
    printf("%2d %-20s: %12g %12g %12g\n", i, nodes[nodes_order[i]].name.c_str(), voltages[0][i], previous_voltages[i], dv1);
    if(dv1 > dv)
      dv = dv1;
  }
  return dv;
}

void mistral::AnalogSim::voltages_integration()
{
  for(int i=0; i != first_fixed_node; i++)
    voltages[0][i] = 3*voltages[1][i] - 3*voltages[2][i] + voltages[3][i];
}

void mistral::AnalogSim::voltages_step_history()
{
  std::vector<double> v(std::move(voltages[3]));
  voltages[3] = std::move(voltages[2]);
  voltages[2] = std::move(voltages[1]);
  voltages[1] = std::move(voltages[0]);
  voltages[0] = std::move(v);
}

void mistral::AnalogSim::run()
{
  printf("Running analog sim\n");

  config_vdd = 1.0;
  config_max_dv = 0.1;
  config_min_dv = 1e-9;

  order_nodes();

  for(int i=0; i != 4; i++)
    vector_alloc(voltages[i], node_count);
  vector_alloc(previous_voltages, node_count);
  vector_alloc(voltage_offsets, node_count);
  vector_alloc(voltage_offsets_hold, node_count);
  matrix_alloc(matrix_r, node_count, node_count);
  matrix_alloc(matrix_c, node_count, node_count);
  matrix_alloc(linear_matrix, node_count, node_count);
  matrix_alloc(inverse_linear_matrix, node_count, node_count);
  vector_alloc(non_linear_current, non_linear_nodes_count);
  matrix_alloc(non_linear_current_deriv, non_linear_nodes_count, non_linear_nodes_count);
  vector_alloc(node_functions, non_linear_nodes_count);
  matrix_alloc(jacobian, non_linear_nodes_count, non_linear_nodes_count);
  matrix_alloc(inverse_jacobian, non_linear_nodes_count, non_linear_nodes_count);
  vector_alloc(node_currents, non_linear_nodes_count);

  init_voltages();

  show_nodes_order();

  build_resistor_matrix(linear_matrix);
  matrix_invert(linear_matrix, inverse_linear_matrix, node_count);
  converge_voltages();
  compute_linear_voltages();

  for(int i=1; i != 4; i++)
    std::copy(voltages[0].begin(), voltages[0].end(), voltages[i].begin());

  double current_time = input_start_time;
  output_record(current_time, false);
  double delta_t = 2e-11;

  for(;;) {
    update_input_voltages(current_time + delta_t);
    voltages_integration();
    std::copy(voltages[0].begin(), voltages[0].end(), previous_voltages.begin());

    build_resistor_matrix(matrix_r);
    build_capacitor_matrix(matrix_c);
    recompute_voltage_offsets(2.0 / delta_t);

    matrix_combine(linear_matrix, matrix_r, matrix_c, node_count, node_count, 2.0 / delta_t);
    matrix_invert(linear_matrix, inverse_linear_matrix, node_count);
    apply_inverse_linear_matrix_to_voltage_offsets();
    converge_voltages();
    compute_linear_voltages();

    double dv = compute_max_delta_voltage();
    double next_delta_t = delta_t * 0.8 * pow(config_max_dv / dv, 0.3333);

    printf("dv = %g\n", dv);

    if(dv >= config_max_dv) {
      delta_t = next_delta_t;
      continue;
    }

    current_time += delta_t;
    bool done = output_record(current_time, true);
    if(done)
      break;

    delta_t = next_delta_t;

    voltages_step_history();
  }
}
