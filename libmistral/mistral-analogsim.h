#ifndef MISTRAL_ANALOGSIM_H
#define MISTRAL_ANALOGSIM_H

#include <vector>
#include <functional>
#include <string>
#include <memory>

namespace mistral {
  class AnalogSim {
  public:
    struct time_slot {
      double t, v;
      time_slot(double _t, double _v) : t(_t), v(_v) {}
    };

    struct time_interval {
      double mi, mx;
    };

    // Lookup functors return the value and the partial derivatives
    struct table2_lookup {
      std::string name;

      table2_lookup(std::string _name) : name(_name) {}

      virtual ~table2_lookup() = default;
      virtual void lookup(double x, double y, double &v, double &dvx, double &dvy) const = 0;
      virtual double lookup(double x, double y) const = 0;
    };

    struct table3_lookup {
      std::string name;

      table3_lookup(std::string _name) : name(_name) {}

      virtual ~table3_lookup() = default;
      virtual void lookup(double x, double y, double z, double &v, double &dvx, double &dvy, double &dvz) const = 0;
      virtual double lookup(double x, double y, double z) const = 0;
    };

    using wave = std::vector<time_slot>;

    int gn(const char *name = nullptr);
    void gn(int &node, const char *name = nullptr) {
      if(node == -1)
	node = gn(name);
    }

    int gn_g(double v, const char *name = nullptr);
    void gn_g(int &node, double v, const char *name = nullptr) {
      if(node == -1)
	node = gn_g(v, name);
    }

    int gn_v(double v, const char *name = nullptr);
    void gn_v(int &node, double v, const char *name = nullptr) {
      if(node == -1)
	node = gn_v(v, name);
    }

    int gn_input(const char *name = "input");
    void gn_input(int &node, const char *name = "input") {
      if(node == -1)
	node = gn_input(name);
    }

    AnalogSim();
    void set_timing_scale(double scale_min, double scale_max);
    void set_max_dv(double max_dv);
    void set_min_dv(double min_dv);

    void add_gnd_vdd(double vdd);

    void add_r(int n1, int n2, double r);
    void add_c(int n1, int n2, double c);

    void add_pass(int n1, int n2, std::unique_ptr<table2_lookup> pass);
    void add_buff(int n1, int n2, std::unique_ptr<table2_lookup> driver);
    void add_2port(int n1, int n2, std::unique_ptr<table2_lookup> pullup, std::unique_ptr<table2_lookup> output);
    void add_noqpg(int n1, int n2, int n3, std::unique_ptr<table3_lookup> pass);

    void set_node_name(int n, std::string name) {
      nodes[n].name = name;
    }

    std::string get_node_name(int n) const {
      return nodes[n].name;
    }

    void set_input_wave(int node, const wave &w);
    void set_output_wave(int node, wave &w, time_interval &transition_delay);

    void show() const;
    void run();

  private:
    enum {
      N_STD,
      N_INPUT,
      N_V,
    };

    enum {
      C_R,
      C_C,

      C_PASS,
      C_BUFF,
      C_2PORT,
      C_NOQPG,
    };

    struct node {
      int type;
      double value;
      std::string name;
      int w;

      node(int _type, std::string _name) : type(_type), value(0), name(_name), w(-1) {}
      node(int _type, double _value, std::string _name) : type(_type), value(_value), name(_name), w(-1) {}
    };

    struct component {
      int type;
      int nodes[3], onodes[3];
      double param;
      std::unique_ptr<table2_lookup> t2a, t2b;
      std::unique_ptr<table3_lookup> t3a;
    };

    double config_vdd;
    double config_max_dv;
    double config_min_dv;
    double config_timing_scale_min, config_timing_scale_max;

    double input_start_time, input_end_time;

    std::vector<node> nodes;
    std::vector<component> components;
    std::vector<std::pair<wave, double>> input_waves;
    std::vector<std::pair<wave *, time_interval *>> output_waves;

    std::vector<bool> output_wave_is_rising;
    std::vector<int> nodes_order, inverse_nodes_order;
    int non_linear_nodes_count, first_fixed_node, node_count;

    std::vector<double> voltages[4], voltages_filtered, voltage_offsets, voltage_offsets_hold;
    std::vector<double> matrix_r, matrix_c, linear_matrix, inverse_linear_matrix;
    std::vector<double> non_linear_current;
    std::vector<double> non_linear_current_deriv;
    std::vector<double> node_functions, node_currents;
    std::vector<double> jacobian, inverse_jacobian;
    std::array<double, 3> timestep;

    const char *nn(int id) const {
      return nodes[id].name.c_str();
    }

    static std::string mkname(const char *name, int id) {
      if(name)
	return name;
      char buf[32];
      sprintf(buf, "#%d", id);
      return std::string(buf);
    }


    static void vector_alloc(std::vector<double> &vect, int size);
    static void matrix_alloc(std::vector<double> &matrix, int sx, int sy);
    static void vector_clear(std::vector<double> &vect);
    static void matrix_clear(std::vector<double> &matrix);
    static void matrix_identity(std::vector<double> &matrix, int size);
    static void matrix_invert(std::vector<double> &matrix, std::vector<double> &inverse, int size);
    static void matrix_mul(std::vector<double> &dest, const std::vector<double> &s1, const std::vector<double> &s2, int s1x, int s1y, int s2x, int s2y, int s1stride = 0, int s2stride = 0);
    static void matrix_combine(std::vector<double> &dest, const std::vector<double> &s1, const std::vector<double> &s2, int sx, int sy, double s2_mul);
    void vector_show(const std::vector<double> &vect, int size);
    static void matrix_show(const std::vector<double> &matrix, int sx, int sy, bool full = true, int stride = 0);

    bool node_fixed_voltage(int node) const;
    bool node_fixed_voltage_ordered(int node) const;

    void order_nodes();
    void compute_time_range();
    double input_voltage_at_time(int input, double time);
    bool output_record(double time, bool test_end);
    void init_voltages();
    void update_input_voltages(double time);
    void add_linear_matrix_entry(std::vector<double> &matrix, int n0, int n1, double v);
    void build_resistor_matrix(std::vector<double> &matrix);
    void build_capacitor_matrix(std::vector<double> &matrix);
    void build_non_linear_connection_matrix();
    void compute_non_linear_currents();
    void add_non_linear_current(int n0, double v);
    void add_non_linear_current_deriv(int n0, int n1, double v);
    void compute_node_functions();
    void compute_jacobian();
    double step_new_dc_voltages();
    void compute_linear_voltages();
    void compute_linear_voltages_2();
    void add_voltage_offset(int node, double v);
    void recompute_voltage_offsets(double scale);
    void apply_inverse_linear_matrix_to_voltage_offsets();
    void converge_voltages();
    double compute_max_delta_voltage() const;
    void voltages_integration();
    void voltages_step_history();

    void show_nodes_order() const;
  };
}


#endif
