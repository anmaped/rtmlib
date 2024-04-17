/* This file was automatically generated from rmtld3synth tool version
v0.3-alpha2-35-g8a06be4 (8a06be4993438cc33c2c7e07d57523efb74088a8)
x86_64 GNU/Linux 2023-08-11 23:46. */

#ifndef RTM_COMPUTE_0EE2_H_
#define RTM_COMPUTE_0EE2_H_

#include "rmtld3/formulas.h"
#include "rmtld3/rmtld3.h"

// Propositions
const proposition PROP_1 = 1;

template <class T>
three_valued_type _rtm_compute_0ee2_0(T &trace, timespan &t) {
  return prop<T>(trace, PROP_1, t);
};

#ifdef USE_MAP_SORT
#include <string>
#include <unordered_map>

// Create an unordered_map of sorts (that map to integers)
std::unordered_map<std::string, int> _mapsorttostring = {
    {"a", 1},
};

// Create an unordered_map of sorts (that map to strings)
std::unordered_map<int, std::string> _mapsorttoint = {
    {1, "a"},
};
#endif

#endif // RTM_COMPUTE_0EE2_H_

