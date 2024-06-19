
#if !defined NO_THREADS && !defined NO_RTM_MONITOR_TESTS

#include "custom-rtm-monitor/Rtm_monitor_6ea3.h"

namespace test3 {

#include "custom-rtm-monitor/Rtm_instrument_6ea3.h"

RTML_BUFFER0_SETUP();

int test(Rtm_monitor_6ea3_0<RMTLD3_reader<RTML_reader<
             RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>>> &rtm_mon0) {
  three_valued_type _out;

  Writer_rtm__6ea3 writer;

  writer.push(Writer_rtm__6ea3::c);

  writer.push(Writer_rtm__6ea3::c);

  _out = rtm_mon0.getVeredict();
  assert(_out == T_UNKNOWN);

  nanosleep((const struct timespec[]){{4, 0L}}, NULL);

  _out = rtm_mon0.getVeredict();
  assert(_out == T_UNKNOWN);

  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__6ea3::a);

  _out = rtm_mon0.getVeredict();
  assert(_out == T_UNKNOWN);

  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__6ea3::b);

  _out = rtm_mon0.getVeredict();
  assert(_out == T_FALSE);

  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__6ea3::b);

  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  _out = rtm_mon0.getVeredict();
  assert(_out == T_FALSE);

  nanosleep((const struct timespec[]){{2, 0L}}, NULL);
  __buffer_rtm_monitor_6ea3.debug();
  return 0;
}

int main() {

  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  __buffer_rtm_monitor_6ea3.debug();
  // nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  rtm_mon0.enable();

  test(rtm_mon0); // unknown unknown ... true

  rtm_mon0.disable();

  assert(!rtm_mon0.join());

  auto v = rtm_mon0.getVeredict();

  if (strcmp(out_p(v), "false") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}

} // namespace test3

extern "C" int rtm_monitor_test3();

int rtm_monitor_test3() {

  test3::main();
  return 0;
}

#else

#include <cstdio>

extern "C" int rtm_monitor_test3();

int rtm_monitor_test3() {
  printf("%s \033[0;33mnot applicable.\e[0m\n", __FILE__);

  return 0;
}

#endif