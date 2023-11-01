#include <munit.h>
#include "comment.h"
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

MunitResult comment_add_test(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;
    
    int ret;
    comment_t comment;
    comment_repository_t *repository;
    
    repository = comment_repository_create();
    munit_assert_not_null(repository);

    ret = comment_repository_add(repository, 0x0002, 0x00, "comment #0");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0001, 0x00, "comment #1");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0003, 0x00, "comment #3");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0001, 0x01, "comment #2");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_find(repository, 0x0001, 0x01, &comment);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(comment.logical, ==, 0x0001);
    munit_assert_int(comment.page,  ==, 0x01);
    munit_assert_string_equal(comment.text, "comment #2");

    ret = comment_repository_find(repository, 0x0001, 0x00, &comment);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(comment.logical, ==, 0x0001);
    munit_assert_int(comment.page,  ==, 0x00);
    munit_assert_string_equal(comment.text, "comment #1");

    ret = comment_repository_find(repository, 0x0003, 0x00, &comment);
    munit_assert_int(ret, !=, 0);
    munit_assert_int(comment.logical, ==, 0x0003);
    munit_assert_int(comment.page,  ==, 0x00);
    munit_assert_string_equal(comment.text, "comment #3");

    ret = comment_repository_size(repository);
    munit_assert_int(ret, ==, 4);

    comment_repository_destroy(repository);
    
    return MUNIT_OK;
}

MunitResult comment_delete_test(const MunitParameter params[], void* fixture) {
    (void)params;
    (void)fixture;

    int ret;
    comment_t comment;
    comment_repository_t *repository;
    
    repository = comment_repository_create();
    munit_assert_not_null(repository);

    ret = comment_repository_add(repository, 0x0002, 0x03, "comment #7");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0002, 0x01, "comment #6");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0002, 0x02, "comment #5");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x000a, 0x00, "comment #4");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0008, 0x00, "comment #3");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0004, 0x00, "comment #2");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0002, 0x00, "comment #1");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_add(repository, 0x0000, 0x00, "comment #0");
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_size(repository);
    munit_assert_int(ret, ==, 8);

    ret = comment_repository_delete(repository, 0x0001, 0x0009, 0x00);
    munit_assert_int(ret, !=, 0);

    ret = comment_repository_size(repository);
    munit_assert_int(ret, ==, 5);

    comment_repository_destroy(repository);
    
    return MUNIT_OK;
}

static MunitTest comment_tests[] = {
    { "/add", comment_add_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/delete", comment_delete_test, setup, tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite comment_suite = {
    "Comment test suite", comment_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main (int argc, char* const* argv) {
    return munit_suite_main(&comment_suite, NULL, argc, argv);
}