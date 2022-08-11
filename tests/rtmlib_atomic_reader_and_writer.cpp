/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2022 André Pedro
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
 * rtmlib atomic read and write
 */
#ifndef NO_THREADS

#include <task_compat.h>

#include <assert.h>

#include <circularbuffer.h>
#include <reader.h>
#include <writer.h>

#if defined(__FREERTOS__)
#define PRIO 4
#else
#define PRIO 0
#endif

extern "C" int rtmlib_atomic_reader_and_writer();

int rtmlib_atomic_reader_and_writer() {

  static RTML_buffer<Event<int>, 100> buf;

  // deploy one task for reader_1
  auto consumer1 = [](void *) -> void * {
    // printf("c1\n");

    static RTML_reader<RTML_buffer<Event<int>, 100>> reader_1 =
        RTML_reader<RTML_buffer<Event<int>, 100>>(buf);

    static unsigned int count = 0;

    Event<int> node1;

    int gap = reader_1.synchronize();

    int __out = reader_1.pull(node1);

    printf("c1: %x %x ret:%d |%x| |%x| (%x) %d\n", (uint32_t)node1.getData(),
           (uint32_t)node1.getTime(), (uint32_t)__out, (uint32_t)buf.length(),
           (size_t)reader_1.length(), (uint32_t)count, (uint32_t)gap);

    if (__out == reader_1.AVAILABLE) {
      // assert(node1.getData() == count);
      count++;
    }

    return NULL;
  };

  __attribute__((unused)) __task consumer_1 =
      __task("consumer1", consumer1, PRIO, SCHED_OTHER, 1000);

  // deploy one task for reader_2
  auto consumer2 = [](void *) -> void * {
    // printf("c2\n");

    static RTML_reader<RTML_buffer<Event<int>, 100>> reader_2 =
        RTML_reader<RTML_buffer<Event<int>, 100>>(buf);

    static unsigned int count = 0;

    Event<int> node1;

    // check that out
    int gap = reader_2.synchronize();

    int __out = reader_2.pull(node1);

    printf("c2: %x %x ret:%d |%x| |%x| (%x) %d\n", (uint32_t)node1.getData(),
           (uint32_t)node1.getTime(), (uint32_t)__out, (uint32_t)buf.length(),
           (size_t)reader_2.length(), (uint32_t)count, (uint32_t)gap);

    if (__out == reader_2.AVAILABLE) {
      assert(node1.getData() == count);
      count++;
    }

    return NULL;
  };

  __attribute__((unused)) __task consumer_2 =
      __task("consumer2", consumer2, PRIO, SCHED_OTHER, 3000);

  // deploy one task for writer
  auto producer1 = [](void *) -> void * {
    printf("p1\n");

    static RTML_writer<RTML_buffer<Event<int>, 100>> writer =
        RTML_writer<RTML_buffer<Event<int>, 100>>(buf);

    static unsigned int x = 0;

    Event<int> node1 = Event<int>(x, 0);

    x++;

    writer.push(node1);

    buf.debug();

    assert(buf.length() == x || buf.length() == 100);

    return NULL;
  };

  __attribute__((unused)) __task producer_1 =
      __task("producer1", producer1, PRIO, SCHED_OTHER, 6000);

  // random load
  nanosleep((const struct timespec[]){{30, 0L}}, NULL);

  consumer_1.st = ABORT;
  consumer_2.st = ABORT;
  producer_1.st = ABORT;

  printf("Waiting running tasks...\n");

  // wait till all tasks are finished (threads are not destroyed)
  while (consumer_1.st != ABORTED || consumer_2.st != ABORTED ||
         producer_1.st != ABORTED) {
  };

  printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);

  return 0;
}

#else

#include <stdio.h>

extern "C" int rtmlib_atomic_reader_and_writer();

int rtmlib_atomic_reader_and_writer() {
  printf("%s \033[0;33mnot applicable.\e[0m\n", __FILE__);

  return 0;
}

#endif
