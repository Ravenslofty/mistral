#include "bdz-ph.h"
#include <functional>
#include <math.h>
#include <assert.h>

namespace {
  using index_t = uint32_t;

  struct edge {
    std::array<index_t, 3> vert, next;
  };

  struct graph {
    static constexpr index_t null_edge = index_t(-1);

    index_t ne, r, vertices, nedges;
    std::vector<edge> edges;
    std::vector<index_t> queue;
    std::vector<index_t> first_edge;
    std::vector<uint8_t> vert_degree;
    std::vector<uint8_t> g;

    graph(index_t _ne);
    static index_t calc_r(index_t ne);

    void clear();

    void build_graph(std::function<std::array<uint32_t, 3> (index_t)> hash_lookup);
    bool generate_queue();
    void assign();
    void pack(uint8_t *dest);

    void set_g(uint32_t vertice, uint32_t val);
    uint32_t get_g(uint32_t vertice) const;
  };

}

constexpr index_t graph::null_edge;

graph::graph(index_t _ne) :
  ne(_ne), r(calc_r(ne)), vertices(3*r), nedges(0), edges(ne), queue(ne), first_edge(vertices, null_edge), vert_degree(vertices, 0), g((vertices+3+5) >> 2, 0)
{
}

index_t graph::calc_r(index_t ne)
{
  index_t r = index_t(ceil(1.23*ne/3));
  
  if(!(r & 1))
    r ++;
  
  if(r < 3)
    r = 3;
  return r;
}

void graph::clear()
{
  std::fill(first_edge.begin(), first_edge.end(), null_edge);
  std::fill(vert_degree.begin(), vert_degree.end(), 0);
}

void graph::build_graph(std::function<std::array<uint32_t, 3> (index_t)> hash_lookup)
{
  clear();
  for(index_t i = 0; i != ne; i++) {
    auto h = hash_lookup(i);
    index_t e0 = (h[0] % r) + 0;
    index_t e1 = (h[1] % r) + r;
    index_t e2 = (h[2] % r) + 2*r;

    edges[i].vert[0] = e0;
    edges[i].vert[1] = e1;
    edges[i].vert[2] = e2;
    edges[i].next[0] = first_edge[e0];
    edges[i].next[1] = first_edge[e1];
    edges[i].next[2] = first_edge[e2];
    first_edge[e0] = i;
    first_edge[e1] = i;
    first_edge[e2] = i;
    vert_degree[e0] ++;
    vert_degree[e1] ++;
    vert_degree[e2] ++;
  }
}

bool graph::generate_queue()
{
  index_t qh = 0, qt = 0;
  std::vector<bool> marked_edge(ne, false);

  for(index_t i = 0; i != ne; i++) {
    auto v = edges[i].vert;
    if(vert_degree[v[0]] == 1 ||
       vert_degree[v[1]] == 1 ||
       vert_degree[v[2]] == 1) {
      queue[qh++] = i;
      marked_edge[i] = true;
    }
  }

  while(qt != qh) {
    index_t ce = queue[qt++];
    for(index_t i = 0; i != 3; i++) {
      index_t v = edges[ce].vert[i];
      index_t e1 = first_edge[v];
      index_t e2 = null_edge;
      index_t sel;
      while(e1 != ce && e1 != null_edge) {
	for(sel = 0; sel != 2; sel++)
	  if(edges[e1].vert[sel] == v)
	    break;
	e2 = e1;
	e1 = edges[e1].next[sel];
      }
      assert(e1 != null_edge);
      if(e2 != null_edge)
	edges[e2].next[sel] = edges[e1].next[i];
      else
	first_edge[v] = edges[e1].next[i];
      vert_degree[v] --;
    }

    auto v = edges[ce].vert;
    for(index_t i = 0; i != 3; i++)
      if(vert_degree[v[i]] == 1) {
	index_t te = first_edge[v[i]];
	if(!marked_edge[te]) {
	  queue[qh++] = te;
	  marked_edge[te] = true;
	}
      }
  }

  return qh == ne;
}

void graph::set_g(uint32_t vertice, uint32_t val)
{
  // Note: it is assumed values are set only once and initialization is zero
  g[vertice >> 2] |= val << ((vertice & 3) << 1);
}

uint32_t graph::get_g(uint32_t vertice) const
{
  return (g[vertice >> 2] >> ((vertice & 3) << 1)) & 3;
}


void graph::assign()
{
  std::vector<bool> marked_vert(vertices, false);
  for(auto i = queue.crbegin(); i != queue.crend(); i++) {
    index_t ce = *i;
    auto v = edges[ce].vert;
    if(!marked_vert[v[0]]) {
      marked_vert[v[0]] = true;
      marked_vert[v[1]] = true;
      marked_vert[v[2]] = true;
      set_g(v[0], (6 - get_g(v[1]) - get_g(v[2])) % 3);
    } else if(!marked_vert[v[1]]) {
      marked_vert[v[1]] = true;
      marked_vert[v[2]] = true;
      set_g(v[1], (7 - get_g(v[0]) - get_g(v[2])) % 3);
    } else {
      marked_vert[v[2]] = true;
      set_g(v[2], (8 - get_g(v[0]) - get_g(v[1])) % 3);
    }
  }
}

void graph::pack(uint8_t *dest)
{
  // number of values has been rounded up to at least a multiple of 5
  for(index_t i = 0; i < vertices; i += 5)
    *dest++ = get_g(i) + 3*get_g(i+1) + 3*3*get_g(i+2) + 3*3*3*get_g(i+3) + 3*3*3*3*get_g(i+4);
}
      
      
template<typename T> std::array<uint32_t, 3> bdz_ph_hash::jenkins_hash(T val, uint32_t seed)
{
  std::array<uint32_t, 3> h = {
    0x9e3779b9 + val,
    0x9e3779b9 + (sizeof(val) > 4 ? val >> 32 : 0),
    seed + uint32_t(sizeof(T))
  };

  h[0] -= h[1]; h[0] -= h[2]; h[0] ^= (h[2]>>13);
  h[1] -= h[2]; h[1] -= h[0]; h[1] ^= (h[0]<< 8);
  h[2] -= h[0]; h[2] -= h[1]; h[2] ^= (h[1]>>13);
  h[0] -= h[1]; h[0] -= h[2]; h[0] ^= (h[2]>>12);
  h[1] -= h[2]; h[1] -= h[0]; h[1] ^= (h[0]<<16);
  h[2] -= h[0]; h[2] -= h[1]; h[2] ^= (h[1]>> 5);
  h[0] -= h[1]; h[0] -= h[2]; h[0] ^= (h[2]>> 3);
  h[1] -= h[2]; h[1] -= h[0]; h[1] ^= (h[0]<<10);
  h[2] -= h[0]; h[2] -= h[1]; h[2] ^= (h[1]>>15);

  return h;
}

template<typename T> std::vector<uint8_t> bdz_ph_hash::make(const T *data, size_t ne)
{
  graph gr(ne);

  uint32_t seed;
  for(seed = 0; seed < 65536; seed++) {
    gr.build_graph([seed, data] (index_t index) { return jenkins_hash(data[index], seed); });
    if(gr.generate_queue())
      break;
  }

  assert(seed != 100);

  gr.assign();

  std::vector<uint8_t> res(2*4 + (gr.vertices+4)/5);

  res[0] = seed >> 0;
  res[1] = seed >> 8;
  res[2] = seed >> 16;
  res[3] = seed >> 24;
  res[4] = gr.r >> 0;
  res[5] = gr.r >> 8;
  res[6] = gr.r >> 16;
  res[7] = gr.r >> 24;

  gr.pack(&res[8]);

  return res;
}

size_t bdz_ph_hash::output_range(const uint8_t *data)
{
  uint32_t r = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
  return 3*r;
}

template<typename T> size_t bdz_ph_hash::lookup(const uint8_t *hdata, T key)
{
  // Only values 0..242 (3**5) are used
  static const uint8_t pow3_lookup[5][256] = {
    {
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,

      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,

      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,
      0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,  0, 1, 2, 0, 1, 2, 0, 1, 2,

      0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    {
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,

      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,

      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,
      0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,  0, 0, 0, 1, 1, 1, 2, 2, 2,

      0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1, 1, 1, 1,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,

      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1,

      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 2, 2, 2,

      0,0,0,0,0,0,0,0,0,0,0,0,0
    }
  };

  uint32_t s = hdata[0] | (hdata[1] << 8) | (hdata[2] << 16) | (hdata[3] << 24);
  uint32_t r = hdata[4] | (hdata[5] << 8) | (hdata[6] << 16) | (hdata[7] << 24);

  auto h = jenkins_hash(key, s);

  h[0] = (h[0] % r) + 0;
  h[1] = (h[1] % r) + r;
  h[2] = (h[2] % r) + 2*r;

  uint32_t i0 = pow3_lookup[h[0] % 5][hdata[8 + (h[0]/5)]];
  uint32_t i1 = pow3_lookup[h[1] % 5][hdata[8 + (h[1]/5)]];
  uint32_t i2 = pow3_lookup[h[2] % 5][hdata[8 + (h[2]/5)]];

  return h[(i0 + i1 + i2) % 3];
}

template std::vector<uint8_t> bdz_ph_hash::make<uint32_t>(const uint32_t *data, size_t ne);
template size_t bdz_ph_hash::lookup<uint32_t>(const uint8_t *hdata, uint32_t key);
