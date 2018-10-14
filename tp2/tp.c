/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>

extern info_t *info;

void printInterruptDescriptor(int_desc_t * interrupt, int numberOfInterrupt)
{
    uint64_t computed_offset = ((interrupt -> offset_2 & 0xFFFF) << 16) | (interrupt -> offset_1 & 0xFFFF);
    uint64_t computed_zero = ((interrupt -> zero_1 & 0x1F) << 1) | (interrupt -> zero_2 & 0x1);
    debug("\n");
    debug("Interrupt %d :\n", numberOfInterrupt);
    debug("------------\n");
    debug("Adress : %p\n", (int_desc_t *) interrupt);
    debug("Offset : 0x%llx\n", computed_offset & 0xFFFFFFFF);
    debug("Selector : 0x%llx\n", interrupt -> selector & 0xFFFF);
    debug("Stack table : 0x%llx\n", interrupt -> ist & 0x7);
    debug("Zero : 0x%llx\n", computed_zero & 0x3F);
    debug("Type : %ld\n", interrupt -> type & 0xF);
    debug("Privilege level : %ld\n", interrupt -> dpl & 0x3);
    debug("Present : %ld\n", interrupt -> p & 0x1);
    debug("\n");
}

void printIDT(idt_reg_t idtr)
{
  debug("\n");
  debug("IDT address : %p\n", (void *) idtr.addr);
  debug("\n");
}

void bp_handler()
{
  asm volatile ("pusha");
  debug("Breakpoint interruption handling.\n");

  uint32_t eip;
  asm volatile ("mov 4(%%ebp), %0" : "=r"(eip));
  debug("EIP address : %p\n", (void *) eip);

  asm volatile ("popa");
  asm volatile ("leave");
  asm volatile ("iret");
}

void bp_trigger()
{
  debug("Breakpoint interruption will be raised.\n");
  asm volatile ("INT3");
  debug("Breakpoint interruption has been raised.\n");
}

void tp()
{
  idt_reg_t idtr;
  get_idtr(idtr);
  printIDT(idtr);
  idtr.desc[3].offset_1 = ((uint32_t) &bp_handler) & 0xFFFF;
  idtr.desc[3].offset_2 = ((uint32_t) &bp_handler >> 16) & 0xFFFF;
  
  bp_trigger();
  debug("Breakpoint interruption has been succesfully handled.\n");
}
