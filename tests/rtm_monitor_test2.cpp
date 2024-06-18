
#ifndef NO_THREADS

#include "custom-rtm-monitor/Rtm_compute_8ea7.h"
#include "custom-rtm-monitor/Rtm_monitor_8ea7.h"


namespace test2 {

#include "custom-rtm-monitor/Rtm_instrument_8ea7.h"

RTML_BUFFER0_SETUP();

void test2() {

  Writer_rtm__8ea7 writer;
  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::b);
  nanosleep((const struct timespec[]){{3, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::b);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{2, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::b);
  nanosleep((const struct timespec[]){{10, 0L}}, NULL);

  return;
}

int main() {

  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  __buffer_rtm_monitor_8ea7.debug();
  //nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  rtm_mon0.enable();

  test2(); // unknown unknown ... true

  rtm_mon0.disable();

  rtm_mon0.join();

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

extern "C" int rtm_monitor_test2();

int rtm_monitor_test2() {
  printf("%s \033[0;33mnot applicable.\e[0m\n", __FILE__);

  return 0;
}

#endif