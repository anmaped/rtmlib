#include "target.h"

#include "uart.h"

void *_sbrk(int increment) {
  extern char end;
  static char *heap_end = &end;
  char *current_heap_end = heap_end;
  heap_end += increment;
  return current_heap_end;
}

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _write(int file, char *ptr, int len) {
  int i;
  for (i = 0; i < len; ++i) {
    if (ptr[i] == '\n') {
      uart_send('\r');
    }
    uart_send(ptr[i]);
  }

  return len;
}