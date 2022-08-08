#include <sys/stat.h>
#include <sys/errno.h>

void *_sbrk(int increment);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);
int _fstat(int file, struct stat *st);
int _close(int file);
int _isatty(int fd);
int _lseek(int file, int ptr, int dir);
int _getpid(void);
int _kill(int pid, int sig);
void _exit(int return_value);
