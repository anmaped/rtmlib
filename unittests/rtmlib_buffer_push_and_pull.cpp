/**
 * rmtlib_buffer push and pull
 */

#include <assert.h>

#include <RTML_buffer.h>

int rtmlib_buffer_push_and_pull() {

  RTML_buffer<int, 100> buf;
  int ID = 1;

  assert(buf.length() == 0);

  Event<int> node0 = Event<int>(ID, 0);

  assert(buf.pull(node0) == buf.EMPTY);
  buf.push(node0);

  Event<int> node1 = Event<int>(ID, 1);
  buf.push(node1);

  Event<int> node2 = Event<int>(ID, 2);
  buf.push(node2);

  assert(buf.length() == 3);

  assert([&]() {
    Event<int> _ev;
    buf.pull(_ev);
    return _ev.getTime();
  }() == 0 &&
         [&]() {
           Event<int> _ev;
           buf.pull(_ev);
           return _ev.getTime();
         }() == 1);

  assert(buf.length() == 1);

  Event<int> node3 = Event<int>(ID, 3);
  buf.push(node3);

  assert([&]() {
    Event<int> _ev;
    buf.read(_ev, 0);
    return _ev.getTime();
  }() == 0 &&
         [&]() {
           Event<int> _ev;
           buf.read(_ev, 1);
           return _ev.getTime();
         }() == 1);

  // fill and overload
  Event<int> nodex = Event<int>();
  long int i;
  for (i = 0; i < 101; i++) {
    nodex.set(ID, i);

    if (i >= 98)
      assert(buf.push(nodex) == buf.OVERFLOW);
    else
      assert(buf.push(nodex) == buf.OK);
  }

  assert([&]() {
    Event<int> _ev;
    buf.read(_ev, 0);
    return _ev.getTime();
  }() == 97);

  for (i = 0; i < 110; i++) {
    if (i >= 100)
      assert(buf.pull(nodex) == buf.EMPTY);
    else
      assert(buf.pull(nodex) == buf.OK);
  }

  printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);

  return 0;
}
