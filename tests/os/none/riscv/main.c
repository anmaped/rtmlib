#include <stdio.h>

#include "uart.h"

int main(void) {
  uart_init();

  printf("Hello World!\n");
}