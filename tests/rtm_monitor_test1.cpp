
#include "custom-rtm-monitor/Rtm_compute_8ea7.h"
#include "custom-rtm-monitor/Rtm_monitor_8ea7.h"

namespace test1 {

#include "custom-rtm-monitor/Rtm_instrument_8ea7.h"

RTML_BUFFER0_SETUP();

void test1() {

  Writer_rtm__8ea7 writer;

  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::b);
  nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{4, 0L}}, NULL);

  writer.push(Writer_rtm__8ea7::a);
  nanosleep((const struct timespec[]){{12, 0L}}, NULL);

  return;
}

int main() {

  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  __buffer_rtm_monitor_8ea7.debug();
  //nanosleep((const struct timespec[]){{1, 0L}}, NULL);

  rtm_mon0.enable();

  test1(); // unknown unknown ... false

  rtm_mon0.disable();

  // wait till all tasks are finished (threads are not destroyed)
  while (rtm_mon0.isRunning()) {
    nanosleep((const struct timespec[]){{1, 0L}}, NULL);
  };

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
