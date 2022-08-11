#include <sys/types.h>

typedef struct UART_t {
  volatile uint32_t DATA;
  volatile uint32_t STATE;
  volatile uint32_t CTRL;
  volatile uint32_t INTSTATUS;
  volatile uint32_t BAUDDIV;
} UART_t;

#define UART0_ADDR ((UART_t *)(0x40004000))

#define UART_STATE_TXFULL (1 << 0)
#define UART_CTRL_TX_EN (1 << 0)
#define UART_CTRL_RX_EN (1 << 1)

static void uart_print(const char *s) {
  UART0_ADDR->BAUDDIV = 16;
  UART0_ADDR->CTRL = UART_CTRL_TX_EN;

  while (*s != '\0') {
    UART0_ADDR->DATA = *s;
    s++;
  }
}

void main(void) {
  uart_print("Hello, World!\n");

  /* exit qemu */
  register int r0 __asm__("r0");
  r0 = 0x18;
  register int r1 __asm__("r1");
  r1 = 0x20026;
  __asm__ volatile("bkpt #0xAB");

}
