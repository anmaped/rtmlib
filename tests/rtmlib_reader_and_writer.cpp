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
 * rtmlib reader and writer
 */

#include <assert.h>

#include <circularbuffer.h>
#include <reader.h>
#include <writer.h>

extern "C" int rtmlib_reader_and_writer();

int rtmlib_reader_and_writer() {

  RTML_buffer<Event<int>, 100> buf;
  int ID = 0x01;

  RTML_reader<RTML_buffer<Event<int>, 100>> reader =
      RTML_reader<RTML_buffer<Event<int>, 100>>(buf);

  RTML_writer<RTML_buffer<Event<int>, 100>> writer =
      RTML_writer<RTML_buffer<Event<int>, 100>>(buf);

  Event<int> node0 = Event<int>();

  assert(reader.pull(node0) == reader.UNAVAILABLE);

  assert(reader.pop(node0) == reader.UNAVAILABLE);

  // directly fill and overload to buffer (+11 overload)
  Event<int> nodex = Event<int>();
  long int i;
  for (i = 0; i < 111; i++) {
    nodex.set(ID, i);

    if (i >= 100)
      assert(buf.push(nodex) == buf.BUFFER_OVERFLOW);
    else
      assert(buf.push(nodex) == buf.OK);
  }

  Event<int> node1 = Event<int>();

  assert(reader.pull(node1) == reader.READER_OVERFLOW);

  assert(reader.pop(node1) == reader.READER_OVERFLOW);

  assert(reader.gap() == true);

  assert(reader.synchronize() == true);

  assert(reader.gap() == false);

  // pop all before use atomic write and avoid an infinite loop
  for (i = 0; i < 111; i++) {

    if (i >= 100)
      assert(buf.pop(nodex) == buf.EMPTY);
    else
      assert(buf.pop(nodex) == buf.OK);
  }

  Event<int> erase;
  buf.write(erase, 0); // do null sincronization with the atomic writer
                       // (buffer.push and writer.push should not be combined!)

  // now introduce some events with the writer
  Event<int> nodey = Event<int>();
  long int j;
  for (j = 0; j < 111; j++) {
    nodey.set(ID, j);

    if (j >= 100)
      assert(writer.push(nodey) == buf.BUFFER_OVERFLOW);

    if (j < 100)
      assert(writer.push(nodey) == buf.OK);
  }

  printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);

  return 0;
}
