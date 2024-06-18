/* This file was automatically generated from rmtld3synth tool version
v0.4 (184fbccc232e66bfac40de73db696ef28bd99535)
x86_64 GNU/Linux 2024-05-03 18:46.

Settings:
out_dir -> 'out'
rtm_buffer_size -> 100
rtm_event_subtype -> '_auto_gen_prop'
version -> 'v0.4 (184fbccc232e66bfac40de73db696ef28bd99535)
x86_64 GNU/Linux 2024-05-03 18:46'
rtm_monitor_name_prefix -> 'rtm_#_%'
rtm_monitor_time_unit -> 's'
rtm_period -> 200000
rtm_min_inter_arrival_time -> 1
init -> true
rtm_max_period -> 2000000
rtm_event_type -> 'Event'
input_exp_dsl -> ((a U[9.] b) or ~((~(c) or ~(~(c)))))
gen_tests -> 'false'

Formula(s):
- ((a U[9.] b) or ~((~(c) or ~(~(c)))))

*/

#ifndef RTM_COMPUTE_6EA3_H_
#define RTM_COMPUTE_6EA3_H_

#include <rmtld3/formulas.h>
#include <rmtld3/rmtld3.h>

#define RTM_TIME_UNIT s

// Propositions
enum _auto_gen_prop {
  PROP_a = 3,
  PROP_b = 2,
  PROP_c = 1,
};

template <typename T> class Eval_until_less_1_ {
public:
  static three_valued_type eval_phi1(T &trace, timespan &t) {
    auto sf = prop<T>(trace, 3, t);
    return sf;
  };
  static three_valued_type eval_phi2(T &trace, timespan &t) {
    auto sf = prop<T>(trace, 2, t);
    return sf;
  };
};

template <class T>
three_valued_type _rtm_compute_6ea3_0(T &trace, timespan &t) {
  return [](T &trace, timespan &t) {
    auto x = until_less<T, Eval_until_less_1_<T>, 9>(trace, t);
    auto y = [](T &trace, timespan &t) {
      auto x = [](T &trace, timespan &t) {
        auto x = [](T &trace, timespan &t) {
          auto x = prop<T>(trace, 1, t);
          return b3_not(x);
        }(trace, t);
        auto y = [](T &trace, timespan &t) {
          auto x = [](T &trace, timespan &t) {
            auto x = prop<T>(trace, 1, t);
            return b3_not(x);
          }(trace, t);
          return b3_not(x);
        }(trace, t);
        return b3_or(x, y);
      }(trace, t);
      return b3_not(x);
    }(trace, t);
    return b3_or(x, y);
  }(trace, t);
};

#ifdef RTMLIB_ENABLE_MAP_SORT
#include <string>
#include <unordered_map>

// Create an unordered_map of sorts (that map to integers)
std::unordered_map<std::string, int> _mapsorttostring = {
    {"a", PROP_a},
    {"b", PROP_b},
    {"c", PROP_c},
};

// Create an unordered_map of sorts (that map to strings)
std::unordered_map<int, std::string> _mapsorttoint = {
    {PROP_a, "a"},
    {PROP_b, "b"},
    {PROP_c, "c"},
};
#endif

#endif // RTM_COMPUTE_6EA3_H_
