//
// low-level driver routines for 16550a UART.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((volatile uint32 *)(UART0 + 4 * reg))

// the UART control registers.
// FE310 SoC UART (https://static.dev.sifive.com/FE310-G000.pdf)
#define TDR 0 // transmit data
#define RDR 1 // receive data
#define TCR 2 // transmit control
#define RCR 3 // receive control
#define IE  4 // interrupt enable
#define IP  5 // interrupt pending
#define DIV 6 // baud rate divisor

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

void
uartinit(void)
{
  // Enable TX/RX, watermark level = 1
  WriteReg(TCR, 0x10001);
  WriteReg(RCR, 0x10001);

  // enable RX interrupt.
  WriteReg(IE, 0x02);
}

// write one output character to the UART.
void
uartputc(int c)
{
  // wait for Transmit Holding Empty to be set in LSR.
  while((ReadReg(TDR) & (1 << 31))) 
    ;
  WriteReg(TDR, c);
}

// read one input character from the UART.
// return -1 if none is waiting.
int
uartgetc(void)
{
  int c;
  if(((c = ReadReg(RDR)) & (1<<31)) == 0) {
    // input data is ready.
    return c & 0xff;
  } else {
    return -1;
  }
}

// trap.c calls here when the uart interrupts.
void
uartintr(void)
{
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }
}
