#ifndef MISTRAL_ANALOGSIM_H
#define MISTRAL_ANALOGSIM_H

#include <vector>
#include <functional>
#include <string>

namespace mistral {
  class AnalogSim {
  public:
    using table2_lookup = std::function<double (double, double)>;
    using table3_lookup = std::function<double (double, double, double)>;

    int gn(const char *name = nullptr);
    void gn(int &node, const char *name = nullptr) {
      if(node == -1)
	node = gn(name);
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

    void add_gnd_vdd(double vdd);

    void add_r(int n1, int n2, double r);
    void add_c(int n1, int n2, double c);

    void add_pass(int n1, int n2, table2_lookup pass);
    void add_buff(int n1, int n2, table2_lookup driver);
    void add_2port(int n1, int n2, table2_lookup pullup, table2_lookup output);
    void add_noqpg(int n1, int n2, int n3, table3_lookup pass);

    void set_node_name(int n, std::string name) {
      nodes[n].name = name;
    }

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
    };

    struct component {
      int type;
      int nodes[3];
      double param;
      table2_lookup t2a, t2b;
      table3_lookup t3a;
    };

    std::vector<node> nodes;
    std::vector<component> components;

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
  };
}


#endif
