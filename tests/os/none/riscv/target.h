#include <sys/stat.h>

void *_sbrk(int increment) __attribute__((no_instrument_function));

int _fstat(int file, struct stat *st) __attribute__((no_instrument_function));

int _write(int file, char *ptr, int len) __attribute__((no_instrument_function));