/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>

extern info_t *info;

idt_reg_t idtr;

void printIDT()
{
  get_idtr(idtr);
  debug("\n");
  debug("IDT adress : %p\n", (void *) idtr.addr);
}

void bp_handler()
{
  debug("");
}

void tp()
{
  printIDT();
  while(1) {
    asm volatile ("INT3");
  }
}
