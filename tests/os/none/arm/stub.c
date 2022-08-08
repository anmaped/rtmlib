
#include "stub.h"
#include "uart_pl011.h"

void *_sbrk(int increment) {
  extern unsigned char _heap_top;
  static unsigned char *heap_end = &_heap_top;
  unsigned char *current_heap_end = heap_end;
  heap_end += increment;
  return current_heap_end;
}

int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int _read(int file, char *ptr, int len) { return 0; }

int _write(int file, char *ptr, int len) {
  int i;
  for (i = 0; i < len; ++i) {
    if (ptr[i] == '\n') {
      uart_putchar('\r');
    }
    uart_putchar(ptr[i]);
  }
  return len;
}

int _close(int file) { return -1; }

int _isatty(int fd) {
  errno = ENOTTY;
  return 0;
}

int _lseek(int file, int ptr, int dir) {
  errno = ESPIPE;
  return -1;
}

int _getpid(void) { return 1; }

int _kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}

void _exit(int return_value) {
  while (1) {
  };
}
