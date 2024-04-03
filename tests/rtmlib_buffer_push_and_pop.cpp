/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2020 André Pedro
 *
 *  This file is part of rtmlib.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * rmtlib_buffer push and pop
 */

#include <assert.h>

#include <circularbuffer.h>

extern "C" int rtmlib_buffer_push_and_pop();

int rtmlib_buffer_push_and_pop() {

  RTML_buffer<Event<int>, 100> buf;
  int ID = 0x01;

  assert(buf.length() == 0);

  Event<int> node0 = Event<int>(ID, 0);

  assert(buf.pop(node0) == buf.EMPTY);
  buf.push(node0);

  Event<int> node1 = Event<int>(ID, 1);
  buf.push(node1);

  Event<int> node2 = Event<int>(ID, 2);
  buf.push(node2);

  // check buffer copy
  RTML_buffer<Event<int>, 100> buf_copy;
  RTML_buffer<Event<int>, 100> *buf_copy_ = &buf_copy;
  *(buf_copy_) = buf;
  
  assert(buf_copy_->length() == 3);

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
      assert(buf.push(nodex) == buf.BUFFER_OVERFLOW);
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
