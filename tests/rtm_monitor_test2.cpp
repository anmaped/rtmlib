
#if !defined NO_THREADS && !defined NO_RTM_MONITOR_TESTS

#include "custom-rtm-monitor/Rtm_monitor_62be.h"

namespace test2 {

#include "custom-rtm-monitor/Rtm_instrument_62be.h"

RTML_BUFFER0_SETUP();

void test2() {

  Writer_rtm__62be writer;
  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__62be::b);
  nanosleep((const struct timespec[]){{3, 0L}}, NULL);

  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__62be::b);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__62be::b);
  nanosleep((const struct timespec[]){{10, 0L}}, NULL);

  return;
}

int main() {

  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  __buffer_rtm_monitor_62be.debug();
  //nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  rtm_mon0.enable();

  test2(); // unknown unknown ... true

  rtm_mon0.disable();

  assert(!rtm_mon0.join());

  auto v = rtm_mon0.getVeredict();

  if (strcmp(out_p(v), "true") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}

} // namespace test2

extern "C" int rtm_monitor_test2();

int rtm_monitor_test2() {

  test2::main();
  return 0;
}

#else

#include <cstdio>

extern "C" int rtm_monitor_test2();

int rtm_monitor_test2() {
  printf("%s \033[0;33mnot applicable.\e[0m\n", __FILE__);

  return 0;
}

#endif