/**
 * rmtlib_buffer push and pop
 */

#include <assert.h>

#include <RTML_buffer.h>

int rtmlib_buffer_push_and_pop() {

  RTML_buffer<int, 100> buf;
  int ID = 0x01;

  assert(buf.length() == 0);

  Event<int> node0 = Event<int>(ID, 0);

  assert(buf.pop(node0) == buf.EMPTY);
  buf.push(node0);

  Event<int> node1 = Event<int>(ID, 1);
  buf.push(node1);

  Event<int> node2 = Event<int>(ID, 2);
  buf.push(node2);

  assert(buf.length() == 3);

  assert([&]() {
    Event<int> _ev;
    buf.pop(_ev);
    return _ev.getTime();
  }() == 2 &&
         [&]() {
           Event<int> _ev;
           buf.pop(_ev);
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
         }() == 3);

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
  }() == 99);

  for (i = 0; i < 110; i++) {
    if (i >= 100)
      assert(buf.pop(nodex) == buf.EMPTY);
    else
      assert(buf.pop(nodex) == buf.OK);
  }

  printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);

  return 0;
}
