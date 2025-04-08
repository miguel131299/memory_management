#include "../munit/munit.h"

typedef enum {
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_NOT_FOUND = 404,
    HTTP_TEAPOT = 418,
    HTTP_INTERNAL_SERVER_ERROR = 500
  } HttpErrorCode;
  
char *http_to_str(HttpErrorCode code) {
  switch (code) {
    case HTTP_BAD_REQUEST:
      return "400 Bad Request";
    case HTTP_UNAUTHORIZED:
      return "401 Unauthorized";
    case HTTP_NOT_FOUND:
      return "404 Not Found";
    case HTTP_TEAPOT:
      return "418 I AM A TEAPOT!";
    case HTTP_INTERNAL_SERVER_ERROR:
      return "500 Internal Server Error";
    default:
      return "Unknown HTTP status code";
  }
}
  

static MunitResult test_switch_enum(const MunitParameter params[], void* data) {
  munit_assert_string_equal(http_to_str(HTTP_BAD_REQUEST), "400 Bad Request");
  munit_assert_string_equal(http_to_str(HTTP_UNAUTHORIZED), "401 Unauthorized");
  munit_assert_string_equal(http_to_str(HTTP_NOT_FOUND), "404 Not Found");
  munit_assert_string_equal(http_to_str(HTTP_TEAPOT), "418 I AM A TEAPOT!");
  munit_assert_string_equal(http_to_str(HTTP_INTERNAL_SERVER_ERROR), "500 Internal Server Error");
  return MUNIT_OK;
}

static MunitResult test_switch_enum_default(const MunitParameter params[], void* data) {
  munit_assert_string_equal(http_to_str((HttpErrorCode)999), "Unknown HTTP status code");
  return MUNIT_OK;
}

static MunitTest tests[] = {
  { "/switch_enum", test_switch_enum, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/switch_enum_default", test_switch_enum_default, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/http", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
