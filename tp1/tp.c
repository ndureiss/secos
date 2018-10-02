/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

extern info_t *info;

void printSegment(seg_desc_t * segment, int numberOfSegment)
{
    uint64_t computed_limit = ((segment -> limit_2 & 0xF) << 16) | (segment -> limit_1 & 0xFFFF);
    uint64_t computed_base = ((segment -> base_3 & 0xFF) << 24) | ((segment -> base_2 & 0xFF) << 16) | ((segment -> base_1 & 0xFFFF));

    printf("\n");
    printf("Segment %d :\n", numberOfSegment);
    printf("------------\n");
    printf("Limit : 0x%llx\n", computed_limit & 0xFFFFF);
    printf("Base : 0x%llx\n", computed_base & 0xFFFFFFFF);
    printf("Type : %ld\n", segment -> type & 0xF);
    printf("Descriptor type : %ld\n", segment -> s & 0x1);
    printf("Descriptor privilege level: %ld\n", segment -> dpl & 0x3);
    printf("Segment present flag : %ld\n", segment -> p & 0x1);
    printf("Available for fun and profit : %ld\n", segment -> avl & 0x1);
    printf("Longmode: %ld\n", segment -> l & 0x1);
    printf("Default length: %ld\n", segment -> d & 0x1);
    printf("Granularity: %ld\n", segment -> g & 0x1);
    printf("\n");
}

void q1()
{
    gdt_reg_t gdtr;
    get_gdtr(gdtr);

    for (unsigned int i = 0; i < ((gdtr.limit + 1) / sizeof(seg_desc_t)); i++) {
        printSegment(gdtr.desc + i, i);
    }
}

seg_desc_t set_seg(uint64_t base, uint64_t limit, uint64_t type, uint64_t descriptorType)
{
    seg_desc_t segment;
    segment.limit_1 = limit & 0xFFFF;
    segment.base_1 = base & 0xFFFF;
    segment.base_2 = base >> 16 && 0xFF;
    segment.type = type;
    segment.s = descriptorType;
    segment.dpl = 0x0;
    segment.p = 0;
    segment.limit_2 = limit >> 16 & 0xF;
    segment.avl = 0;
    segment.l = 0;
    segment.d = 0;
    segment.g = 0;
    segment.base_3 = base >> 24 & 0xFF;
    return segment;
}

seg_desc_t gdt[3];

void tp()
{
    gdt[0] = set_seg(0x0, 0x0, 0x0, 0);;
    gdt[1] = set_seg(0x0, 0xFFFFF, SEG_DESC_CODE_XR, 1);;
    gdt[2] = set_seg(0x0, 0xFFFFF, SEG_DESC_DATA_RW, 1);;

    gdt_reg_t gdtr;
    gdtr.limit = 3 * sizeof(seg_desc_t) - 1;
    gdtr.desc = gdt;

    set_gdtr(gdtr);

    // TO FIX

    set_cs(0x8);
    set_ds(0x10);
    //set_es(0x20);

    for (unsigned int i = 0; i < ((gdtr.limit + 1) / sizeof(seg_desc_t)); i++) {
        printSegment(gdtr.desc + i, i);
    }

    // Q3
    /*seg_desc_t data2 = set_seg(0x600000, 0x200000, SEG_DESC_DATA_RW, 1);
    gdtr.desc[3] = data2;

    char  src[64];
    char *dst = 0;

    memset(src, 0xff, 64);

    _memcpy8(dst, src, 32);*/
}
