
#ifndef _RMTLD3_PATTERN_H_
#define _RMTLD3_PATTERN_H_

#include "rmtld3.h"

template <typename B, size_t N> class RMTLD3_Pattern {

  B memory[N];

public:
  RMTLD3_Pattern();

  /**
   * Get value from memory
   */
  three_valued_type getValue(size_t, size_t);

  /**
   * Map t to memory index
   */
  size_t mapt(timespan &);
};

template <typename B, size_t N> RMTLD3_Pattern<B, N>::RMTLD3_Pattern() {}

template <typename B, size_t N>
three_valued_type RMTLD3_Pattern<B, N>::getValue(size_t i, size_t id) {
  three_valued_type value;
  memory[id].read(value, i);
  return value;
}

template <typename B, size_t N> size_t RMTLD3_Pattern<B, N>::mapt(timespan &t) {
  return (size_t)t;
}

#endif //_RMTLD3_PATTERN_H_
