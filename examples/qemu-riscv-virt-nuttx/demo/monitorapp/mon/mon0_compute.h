
  #ifndef _MON0_COMPUTE_H_973490313
  #define _MON0_COMPUTE_H_973490313

  #include "rmtld3/rmtld3.h"
  #include "rmtld3/macros.h"
  
  // Propositions
  const proposition PROP_1 = 1;
  
  template<class T>
  three_valued_type _mon0_compute (T &trace, timespan &t) { return prop<T>(trace, PROP_1, t); };

#ifdef USE_MAP_SORT
  #include <string>
  #include <unordered_map>

  // Create an unordered_map of sorts (that map to integers)
  std::unordered_map<std::string, int> _mapsorttostring = {
  {"a",1},
  };

  // Create an unordered_map of sorts (that map to strings)
  std::unordered_map<int, std::string> _mapsorttoint = {
  {1,"a"},
  };
#endif

  #endif //_MON0_COMPUTE_H_973490313
    
