#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

typedef struct Token {
  char* literal;
  int line;
  int column;
} token_t;

token_t** create_token_pointer_array(token_t* tokens, size_t count) {
  token_t** token_pointers = malloc(count * sizeof(token_t*));
  if (token_pointers == NULL) {
    exit(1);
  }
  for (size_t i = 0; i < count; ++i) {
    token_t* ptr = malloc(sizeof(token_t));
    *ptr = tokens[i];
    token_pointers[i] = ptr;
  }
  return token_pointers;
}

static MunitResult test_create_token_pointer_array_single(
    const MunitParameter params[], void* data) {
  token_t token = {"hello", 1, 1};
  token_t** result = create_token_pointer_array(&token, 1);

  munit_assert_not_null(result);     // Result array should not be null
  munit_assert_not_null(result[0]);  // First token pointer should not be null
  munit_assert_string_equal(result[0]->literal,
                            "hello");          // Literal should match
  munit_assert_int(result[0]->line, ==, 1);    // Line number should match
  munit_assert_int(result[0]->column, ==, 1);  // Column number should match
  munit_assert_ptr_not_equal(result[0],
                             &token);  // Should not point to original token

  free(result[0]);
  free(result);
  return MUNIT_OK;
}

static MunitResult test_create_token_pointer_array_multiple(
    const MunitParameter params[], void* data) {
  token_t tokens[3] = {{"foo", 1, 1}, {"bar", 2, 5}, {"baz", 3, 10}};
  token_t** result = create_token_pointer_array(tokens, 3);

  munit_assert_not_null(result);  // Result array should not be null
  for (int i = 0; i < 3; i++) {
    munit_assert_not_null(result[i]);  // Each token pointer should not be null
    munit_assert_string_equal(result[i]->literal,
                              tokens[i].literal);  // Literal should match
    munit_assert_int(result[i]->line, ==,
                     tokens[i].line);  // Line number should match
    munit_assert_int(result[i]->column, ==,
                     tokens[i].column);  // Column number should match
    munit_assert_ptr_not_equal(
        result[i], &tokens[i]);  // Should not point to original token
  }

  for (int i = 0; i < 3; i++) {
    free(result[i]);
  }
  free(result);
  return MUNIT_OK;
}

static MunitResult test_create_token_pointer_array_memory_allocation(
    const MunitParameter params[], void* data) {
  token_t tokens[2] = {{"test1", 1, 1}, {"test2", 2, 2}};
  token_t** result = create_token_pointer_array(tokens, 2);

  munit_assert_not_null(result);     // Result array should not be null
  munit_assert_not_null(result[0]);  // First token pointer should not be null
  munit_assert_not_null(result[1]);  // Second token pointer should not be null
  munit_assert_ptr_not_equal(result[0],
                             result[1]);  // Pointers should be different
  munit_assert_ptr_not_equal(result[0],
                             &tokens[0]);  // Should not point to original token
  munit_assert_ptr_not_equal(result[1], &tokens[1]);

  free(result[0]);
  free(result[1]);
  free(result);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/create_token_pointer_array_single",
     test_create_token_pointer_array_single, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/create_token_pointer_array_multiple",
     test_create_token_pointer_array_multiple, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/create_token_pointer_array_memory_allocation",
     test_create_token_pointer_array_memory_allocation, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/create_token_pointer_array", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
