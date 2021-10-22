#ifndef BDZ_PH
#define BDZ_PH

// Reimplementation of the bdz-ph algorithm of cmph

#include <stdint.h>
#include <vector>
#include <array>
#include <stdlib.h>

namespace bdz_ph_hash {
  template<typename T> std::vector<uint8_t> make(const T *data, size_t ne);
  template<typename T> static inline std::vector<uint8_t> make(const std::vector<T> &data)
  {
    return make(data.data(), data.size());
  }

  template<typename T> size_t lookup(const uint8_t *hdata, T key);
  template<typename T> static inline size_t lookup(const std::vector<uint8_t> &hdata, T key)
  {
    return lookup(hdata.data(), key);
  }

  size_t output_range(const uint8_t *hdata);
  static inline size_t output_range(const std::vector<uint8_t> &hdata) {
    return output_range(hdata.data());
  }

  template<typename T> std::array<uint32_t, 3> jenkins_hash(T val, uint32_t seed);
}

#endif
