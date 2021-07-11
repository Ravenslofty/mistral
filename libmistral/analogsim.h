#ifndef MISTRAL_ANALOGSIM_H
#define MISTRAL_ANALOGSIM_H

#include <vector>

namespace mistral {
  class AnalogSim {
  public:
    struct lookup_table_2 {
      char name[64];
      int size[2];
      double v0[2], vstep[2];
      double data[];

      double eval(double c1, double c2);
      double eval(double c1, double c2, double &dc1, double &dc2);
    };

    int gn();
    void gn(int &node) {
      if(node == -1)
	node = gn();
    }

    int gn_v(double v);
    void gn_v(int &node, double v) {
      if(node == -1)
	node = gn_v(v);
    }

    int gn_input();
    void gn_input(int &node) {
      if(node == -1)
	node = gn_input();
    }

    void add_resistor(int n1, int n2, double r);
    void add_capacitor(int n1, int n2, double c);

    void add_tran3_noq_pg(int n1, int n2, int n3, const lookup_table_2 *table);
    void add_pwl2_2port(int n1, int n2, const lookup_table_2 *table1, const lookup_table_2 *table2);

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

      C_TRAN3_NOQ_PG,
      C_PWL2_2PORT,
    };

    struct node {
      int type;
      double value;
    };

    struct component {
      int type;
      int nodes[3];
      double param;
      const lookup_table_2 *tab2_1, *tab2_2;
    };

    std::vector<node> nodes;
    std::vector<component> components;
  };
}


#endif
