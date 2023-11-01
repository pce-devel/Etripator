#include <munit.h>
#include "label.h"
#include "message.h"
#include "message/console.h"
#include "message/file.h"

void* setup(const MunitParameter params[], void* user_data) {
    (void) params;
    (void) user_data;
    
    console_msg_printer_t *printer = (console_msg_printer_t*)malloc(sizeof(console_msg_printer_t));
    
    msg_printer_init();    
    console_msg_printer_init(printer);
    msg_printer_add((msg_printer_t*)printer);

    return (void*)printer;
}

void tear_down(void* fixture) {
    msg_printer_destroy();
    free(fixture);
}

MunitResult label_add_test(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;

    int ret;
    label_t label;
    label_repository_t* repository;
    
    repository = label_repository_create();
    munit_assert_not_null(repository);

    ret = label_repository_add(repository, "l_0001", 0x0001, 0x00, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_add(repository, "l_0020", 0x0020, 0x00, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_add(repository, "l_000a", 0x000a, 0xb1, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_add(repository, "l_cafe", 0xcafe, 0xf7, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_add(repository, "l_0001", 0x0001, 0x00, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_size(repository);
    munit_assert_int(ret, ==, 4);
    
    ret = label_repository_find(repository, 0x000a, 0xb1, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "l_000a");

    ret = label_repository_find(repository, 0x0020, 0x00, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "l_0020");

    ret = label_repository_find(repository, 0xcafe, 0xf7, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "l_cafe");

    ret = label_repository_find(repository, 0x0001, 0x00, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "l_0001");

    ret = label_repository_find(repository, 0xbeef, 0xac, &label);
    munit_assert_int(ret, ==, 0);
    
    label_repository_destroy(repository);
    return MUNIT_OK;
}

MunitResult label_delete_test(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;

    int ret;
    label_t label;
    label_repository_t* repository;
 
    repository = label_repository_create();
    munit_assert_not_null(repository);

    ret = label_repository_add(repository, "label01", 0x0110, 0x1a, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label02", 0x0220, 0x1a, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label03", 0x0330, 0x1b, "description");
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label04", 0x0440, 0x1a, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label05", 0x0550, 0x1b, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label06", 0x0553, 0x1b, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label07", 0x0555, 0x1b, NULL);
    munit_assert_int(ret, !=, 0);
    ret = label_repository_add(repository, "label08", 0x0557, 0x1b, NULL);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_size(repository);
    munit_assert_int(ret, ==, 8);

    ret = label_repository_delete(repository, 0x04a0, 0x0556, 0x1b);
    munit_assert_int(ret, !=, 0);

    ret = label_repository_size(repository);
    munit_assert_int(ret, ==, 5);

    ret = label_repository_find(repository, 0x0557, 0x1b, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "label08");
    ret = label_repository_find(repository, 0x0440, 0x1a, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "label04");
    ret = label_repository_find(repository, 0x0330, 0x1b, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "label03");
    munit_assert_string_equal(label.description, "description");
    ret = label_repository_find(repository, 0x0220, 0x1a, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "label02");
    ret = label_repository_find(repository, 0x0110, 0x1a, &label);
    munit_assert_int(ret, !=, 0);
    munit_assert_string_equal(label.name, "label01");

    ret = label_repository_find(repository, 0x0555, 0x1b, &label);
    munit_assert_int(ret, ==, 0);
    ret = label_repository_find(repository, 0x0553, 0x1b, &label);
    munit_assert_int(ret, ==, 0);
    ret = label_repository_find(repository, 0x0550, 0x1b, &label);
    munit_assert_int(ret, ==, 0);

    label_repository_destroy(repository);    

    return MUNIT_OK;
}

static MunitTest label_tests[] = {
    { "/add", label_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/delete", label_delete_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite label_suite = {
    "Label test suite", label_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&label_suite, NULL, argc, argv);
}