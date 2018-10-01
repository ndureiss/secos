/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>

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
    gdt_reg_t gdt;
    get_gdtr(gdt);

    for (unsigned int i = 0; i < ((gdt.limit + 1) / sizeof(seg_desc_t)); i++) {
        printSegment(gdt.desc + i, i);
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
void q2()
{
    seg_desc_t null = set_seg(0x0, 0x0, 0x0, 0);
    seg_desc_t code = set_seg(0x0, 0xFFFFF, SEG_DESC_CODE_XR, 1);
    seg_desc_t data = set_seg(0x0, 0xFFFFF, SEG_DESC_DATA_RW, 1);

    seg_desc_t p[3];
    p[0] = null;
    p[1] = code;
    p[2] = data;

    gdt_reg_t gdt;
    gdt.limit = 3 * sizeof(seg_desc_t);
    gdt.desc = p;

    set_gdtr(gdt);

    // TO FIX
    /*seg_sel_t ss_sel;
    ss_sel.rpl = 0;
    ss_sel.ti = 0;
    ss_sel.index = 3;
    seg_sel_t cs_sel;
    cs_sel.rpl = 0;
    cs_sel.ti = SEG_SEL_GDT;
    cs_sel.index = 1;
    seg_sel_t ds_sel;
    ds_sel.rpl = 0;
    ds_sel.ti = SEG_SEL_GDT;
    ds_sel.index = 2;*/
    //set_ss();
    //set_cs(cs_sel);
    //set_ds(ds_sel);
    //set_es();
    //set_fs();
    //set_gs();
    
    for (unsigned int i = 0; i < ((gdt.limit + 1) / sizeof(seg_desc_t)); i++) {
        printSegment(gdt.desc + i, i);
    }
}

void tp()
{
    q2();    
}
