/**
 * rmtlib_buffer push and pop
 */

#include <assert.h>

#include <RTML_buffer.h>
#include <RTML_reader.h>

int rtmlib_reader_and_writer() {

  RTML_buffer<int, 100> buf;
  int ID = 0x01;

  RTML_reader<RTML_buffer<int, 100>> reader =
      RTML_reader<RTML_buffer<int, 100>>(buf);

  // fill and overload +11
  Event<int> nodex = Event<int>();
  long int i;
  for (i = 0; i < 111; i++) {
    nodex.set(ID, i);

    if (i >= 100)
      assert(buf.push(nodex) == buf.OVERFLOW);
    else
      assert(buf.push(nodex) == buf.OK);
  }

  assert(reader.gap() == true);

  Event<int> node0 = Event<int>();
  reader.pull(node0);
  reader.pull(node0);

  // reader

  printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);

  return 0;
}
