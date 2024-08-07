
#if !defined NO_THREADS && !defined NO_RTM_MONITOR_TESTS

#include "custom-rtm-monitor/Rtm_monitor_62be.h"

namespace test1 {

#include "custom-rtm-monitor/Rtm_instrument_62be.h"

RTML_BUFFER0_SETUP();

void test1() {

  Writer_rtm__62be writer;

  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__62be::b);
  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{4, 0L}}, NULL);

  writer.push(Writer_rtm__62be::a);
  nanosleep((const struct timespec[]){{12, 0L}}, NULL);

  return;
}

int main() {

  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  __buffer_rtm_monitor_62be.debug();
  // nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  rtm_mon0.enable();

  test1(); // unknown unknown ... false

  rtm_mon0.disable();

  assert(!rtm_mon0.join());

  auto v = rtm_mon0.getVeredict();

  if (strcmp(out_p(v), "false") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}

} // namespace test1

extern "C" int rtm_monitor_test1();

int rtm_monitor_test1() {
  test1::main();
  return 0;
}

#else

#include <cstdio>

extern "C" int rtm_monitor_test1();

int rtm_monitor_test1() {
  printf("%s \033[0;33mnot applicable.\e[0m\n", __FILE__);

  return 0;
}

#endif