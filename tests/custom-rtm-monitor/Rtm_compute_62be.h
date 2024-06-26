/* This file was automatically generated from rmtld3synth tool version
v0.4-5-g4f6dc2b (4f6dc2bab216608513653509357ed5d4f99f6546)
x86_64 GNU/Linux 2024-06-25 11:24.

Settings:
out_dir -> './out'
rtm_buffer_size -> 100
rtm_event_subtype -> 'std::underlying_type<_auto_gen_prop>::type'
version -> 'v0.4-5-g4f6dc2b (4f6dc2bab216608513653509357ed5d4f99f6546)
x86_64 GNU/Linux 2024-06-25 11:24'
rtm_monitor_name_prefix -> 'rtm_#_%'
rtm_monitor_time_unit -> 's'
rtm_period -> 200000
rtm_min_inter_arrival_time -> 1
init -> true
rtm_max_period -> 2000000
rtm_event_type -> 'Event'
input_exp_dsl -> (a U[9.] b)
gen_tests -> 'false'

Formula(s):
- (a U[9.] b)

*/

#ifndef RTM_COMPUTE_62BE_H_
#define RTM_COMPUTE_62BE_H_

#include <rmtld3/formulas.h>
#include <rmtld3/rmtld3.h>

#define RTM_TIME_UNIT s

// Propositions
enum _auto_gen_prop
{
  PROP_a = 2,
  PROP_b = 1,
};

template <typename T> class Eval_until_less_222983
{
public:
  static three_valued_type
  eval_phi1 (T &trace, timespan &t)
  {
    auto sf = prop<T> (trace, PROP_a, t);
    return sf;
  };
  static three_valued_type
  eval_phi2 (T &trace, timespan &t)
  {
    auto sf = prop<T> (trace, PROP_b, t);
    return sf;
  };
};

template <class T>
three_valued_type
_rtm_compute_62be_0 (T &trace, timespan &t)
{
  return until_less<T, Eval_until_less_222983<T>, 9> (trace, t);
};

#ifdef RTMLIB_ENABLE_MAP_SORT
#include <string>
#include <unordered_map>

// Create an unordered_map of sorts (that map to integers)
std::unordered_map<std::string, int> _mapsorttostring = {
  { "a", PROP_a },
  { "b", PROP_b },
};

// Create an unordered_map of sorts (that map to strings)
std::unordered_map<int, std::string> _mapsorttoint = {
  { PROP_a, "a" },
  { PROP_b, "b" },
};
#endif

#endif // RTM_COMPUTE_62BE_H_

