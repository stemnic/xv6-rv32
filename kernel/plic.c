#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

void
plicinit(void)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(volatile uint32*)(PLIC + UART0_IRQ*4) = 7;
}

void
plicinithart(void)
{
  int hart = 0; // cpuid();
  
  // set uart's enable bit for this hart's S-mode. 
  *(uint32*)PLIC_MENABLE(hart) = (1 << UART0_IRQ); // | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(uint32*)PLIC_MPRIORITY(hart) = 0;
}

// return a bitmap of which IRQs are waiting
// to be served.
uint32
plic_pending(void)
{
  uint32 mask;

  //mask = *(uint32*)(PLIC + 0x1000);
  //mask |= (uint64)*(uint32*)(PLIC + 0x1004) << 32;
  mask = *(uint32*)PLIC_PENDING;

  return mask;
}

// ask the PLIC what interrupt we should serve.
int
plic_claim(void)
{
  int hart = cpuid();
  // int irq = *(uint32*)(PLIC + 0x201004);
  int irq = *(uint32*)PLIC_MCLAIM(hart);
  return irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(int irq)
{
  int hart = cpuid();
  //*(uint32*)(PLIC + 0x201004) = irq;
  *(uint32*)PLIC_MCLAIM(hart) = irq;
}
