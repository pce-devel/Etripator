#include <munit.h>
#include "section.h"
#include "message.h"
#include "message/console.h"

MunitResult SortSectionTest(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;
    
    Section section[8];
    
    section[0].output = "0002";
    section[0].page = 0;
    section[0].logical = 1;

    section[1].output = "0001";
    section[1].page = 0;
    section[1].logical = 0;

    section[2].output = "0002";
    section[2].page = 1;
    section[2].logical = 0;

    section[3].output = "0001";
    section[3].page = 1;
    section[3].logical = 0;

    section[4].output = "0000";
    section[4].page = 0;
    section[4].logical = 2;

    section[5].output = "0000";
    section[5].page = 0;
    section[5].logical = 1;

    section[6].output = "0001";
    section[6].page = 0;
    section[6].logical = 1;
    
    section[7].output = "0002";
    section[7].page = 0;
    section[7].logical = 0;

    SortSections(section, 8);

    for(int i=0; i<7; i++) {
        int cmp = strcmp(section[i].output, section[i+1].output);
        munit_assert_int(cmp, <=, 0);
        if(cmp == 0) {
            munit_assert_int(section[i].page, <=, section[i+1].page);
            if(section[i].page == section[i+1].page) {
                munit_assert_int(section[i].logical, <=, section[i+1].logical);
            }
        }
    }

    return MUNIT_OK;
}

MunitResult LoadSectionTest(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;

    static Section bank0_0[4] = {
        { "cdbios_functions", Code, 0, 0xe000, 0x0000, 0x505, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } }, 
        { "unknown.0", Data, 0, 0xe504, 0x0504, 0x05, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "ex_colorcmd.impl", Code, 0, 0xe509, 0x0509, 0xce, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "unknown.1", Data, 0, 0xe5d7, 0x05d7, 0x03, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } }
    };

    static Section bank0_1[9] = {
        { "ex_satclr.impl", Code, 0, 0xe5da, 0x05da, 0x26, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } }, 
        { "unknown.2", Data, 0, 0xf8a9, 0x18a9, 0x0f, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "bm_free.impl", Code, 0, 0xf8b8, 0x18b8, 0x575, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "jump_table.0", Data, 0, 0xfe2d, 0x1e2d, 0x2a, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "grp_bios.impl", Code, 0, 0xfe57, 0x1e57, 0x18, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "jump_table.1", Data, 0, 0xfe70, 0x1e70, 0x22, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "ex_memopen.impl", Code, 0, 0xfe92, 0x1e92, 0x30, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "unknown.3", Data, 0, 0xfec2, 0x1ec2, 0x134, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 8, 16 } },
        { "irq_vectors", Data, 0, 0xfff6, 0x1ff6, 0x0a, { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 }, "syscard.asm", { 2, 1 } }
    };

    static const Section* bank0[2] = { bank0_0, bank0_1 };
    
    Section *section = NULL;
    int count[2] = { 0, 0 };

    int i, j, k;
    int ret;
    
    ret = LoadSections("./data/bank0_0.json", &section, &count[0]);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(count[0], ==, 4);
    
    SortSections(section, count[0]);
        
    for(i=0; i<count[0]; i++) {
        munit_assert_string_equal(bank0[0][i].name, section[i].name);
        munit_assert_int(bank0[0][i].type, ==, section[i].type);
        munit_assert_uint8(bank0[0][i].page, ==, section[i].page);
        munit_assert_uint16(bank0[0][i].logical, ==, section[i].logical);
        munit_assert_uint32(bank0[0][i].offset, ==, section[i].offset);
        munit_assert_int32(bank0[0][i].size, ==, section[i].size);
        munit_assert_memory_equal(8, bank0[0][i].mpr, section[i].mpr);
        munit_assert_int32(bank0[0][i].data.element_size, ==, section[i].data.element_size);
        munit_assert_int32(bank0[0][i].data.elements_per_line, ==, section[i].data.elements_per_line);
        munit_assert_string_equal(bank0[0][i].output, section[i].output);
    }
    
    count[1] = count[0];
    ret = LoadSections("./data/bank0_1.json", &section, &count[1]);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(count[1], ==, 13);
    
    SortSections(section, count[1]);

    for(i=0, j=0; j<2; j++) {
        for(k=0; i<count[j]; i++, k++) {
            munit_assert_string_equal(bank0[j][k].name, section[i].name);
            munit_assert_int(bank0[j][k].type, ==, section[i].type);
            munit_assert_uint8(bank0[j][k].page, ==, section[i].page);
            munit_assert_uint16(bank0[j][k].logical, ==, section[i].logical);
            munit_assert_uint32(bank0[j][k].offset, ==, section[i].offset);
            munit_assert_int32(bank0[j][k].size, ==, section[i].size);
            munit_assert_memory_equal(8, bank0[j][k].mpr, section[i].mpr);
            munit_assert_int32(bank0[j][k].data.element_size, ==, section[i].data.element_size);
            munit_assert_int32(bank0[j][k].data.elements_per_line, ==, section[i].data.elements_per_line);
            munit_assert_string_equal(bank0[j][k].output, section[i].output);
        }
    }    
    DeleteSections(section, count[1]);
    
    return MUNIT_OK;
}

static MunitTest section_tests[] = {
    { "/sort", SortSectionTest, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/load", LoadSectionTest, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite section_suite = {
    "Section test suite", section_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    console_msg_printer console_printer;
    msg_printer_init();
    console_msg_printer_init(&console_printer);

    return munit_suite_main(&section_suite, NULL, argc, argv);
}