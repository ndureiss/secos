/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <types.h>

#define c0_sel gdt_krn_seg_sel(1)
#define d0_sel gdt_krn_seg_sel(2)
#define c3_sel gdt_usr_seg_sel(3)
#define d3_sel gdt_usr_seg_sel(4)
#define ts_sel gdt_krn_seg_sel(5)

extern info_t *info;

void printSegment(seg_desc_t * segment, int numberOfSegment)
{
    uint64_t computed_limit = ((segment -> limit_2 & 0xF) << 16) | (segment -> limit_1 & 0xFFFF);
    uint64_t computed_base = ((segment -> base_3 & 0xFF) << 24) | ((segment -> base_2 & 0xFF) << 16) | ((segment -> base_1 & 0xFFFF));
    
    debug("\n");
    debug("Segment %d :\n", numberOfSegment);
    debug("------------\n");
    debug("Limit : 0x%llx\n", computed_limit & 0xFFFFF);
    debug("Base : 0x%llx\n", computed_base & 0xFFFFFFFF);
    debug("Type : %ld\n", segment -> type & 0xF);
    debug("Descriptor type : %ld\n", segment -> s & 0x1);
    debug("Descriptor privilege level: %ld\n", segment -> dpl & 0x3);
    debug("Segment present flag : %ld\n", segment -> p & 0x1);
    debug("Available for fun and profit : %ld\n", segment -> avl & 0x1);
    debug("Longmode: %ld\n", segment -> l & 0x1);
    debug("Default length: %ld\n", segment -> d & 0x1);
    debug("Granularity: %ld\n", segment -> g & 0x1);
    debug("\n");
}

seg_desc_t set_desc(uint64_t base, uint64_t limit, uint64_t type, uint64_t descriptorType, uint64_t privilegeLevel, uint64_t dispo, uint64_t length, uint64_t granularity)
{
    seg_desc_t segment;
    segment.limit_1 = limit & 0xFFFF;
    segment.base_1 = base & 0xFFFF;
    segment.base_2 = base >> 16 && 0xFF;
    segment.type = type;
    segment.s = descriptorType;
    segment.dpl = privilegeLevel;
    segment.p = dispo;
    segment.limit_2 = limit >> 16 & 0xF;
    segment.avl = 0;
    segment.l = 0;
    segment.d = length;
    segment.g = granularity;
    segment.base_3 = base >> 24 & 0xFF;
    return segment;
}

void userland()
{
   asm volatile ("mov %eax, %cr0"); // Génère un General Protection Fault
}

seg_desc_t gdt[5];

tss_t TSS;

fptr32_t ptr;

void tp()
{
  gdt[0] = set_desc(0x0, 0x0, 0x0, 0, 0x0, 0, 0, 0);
  gdt[1] = set_desc(0x0, 0xFFFFF, SEG_DESC_CODE_XR, 1, 0x0, 1, 1, 1);  // Code ring 0
  gdt[2] = set_desc(0x0, 0xFFFFF, SEG_DESC_DATA_RW, 1, 0x0, 1, 1, 1);  // Data ring 0
  gdt[3] = set_desc(0x0, 0xFFFFF, SEG_DESC_CODE_XR, 1, 0x3, 1, 1, 1);  // Code ring 3
  gdt[4] = set_desc(0x0, 0xFFFFF, SEG_DESC_DATA_RW, 1, 0x3, 1, 1, 1);  // Data ring 3
  
  gdt_reg_t gdtr;
  gdtr.limit = sizeof(gdt) - 1;
  gdtr.desc = gdt;
  
  set_gdtr(gdtr);
  
  set_cs(c0_sel); //utiliser ici les selecteurs
  set_ss(d0_sel);
  set_ds(d3_sel);
  set_es(d3_sel);
  set_fs(d3_sel);
  set_gs(d3_sel);

  // 3.3
  //ptr.offset = (uint32_t) &userland;
  //ptr.segment = c3_sel;
  //farjump(ptr);

  //3.4
  //TSS.s0.esp = get_ebp();
  //TSS.s0.ss = selecteur de data ring 0;
  //init tss tss_sdsc(&GDT[ts_idx], (offset_t) &TSS)
  //set_tr(selecteur de ts);
/*
  asm volatile ("push %0 :: i"(selecteur de ss)); //ss
  asm volatile ("push %%ebp"); //esp
  asm volatile ("pushf"); //eflags
  asm volatile ("push %0 :: i"(selecteur de cs)); //cs
  asm volatile ("push %0" :: "r"(&userland)); //eip
  asm volatile ("iret"); //
  asm volatile (""); //
  */
  for (unsigned int i = 0; i < ((gdtr.limit + 1) / sizeof(seg_desc_t)); i++) {
    printSegment(gdtr.desc + i, i);
  }
}
