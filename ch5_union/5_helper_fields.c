#include <stdint.h>
#include <string.h>

#include "../munit/munit.h"

typedef union PacketHeader {
  struct TCPHeader {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
  } tcp_header;
  uint8_t raw[8];
} packet_header_t;

static MunitResult test_packet_header_size(const MunitParameter params[],
                                           void* data) {
  munit_assert_size(sizeof(packet_header_t), ==,
                    8);  // PacketHeader union should be 8 bytes
  return MUNIT_OK;
}

static MunitResult test_tcp_header_fields(const MunitParameter params[],
                                          void* data) {
  packet_header_t header;
  header.tcp_header.src_port = 0x1234;
  header.tcp_header.dest_port = 0x5678;
  header.tcp_header.seq_num = 0x9ABCDEF0;

  munit_assert_uint16(header.tcp_header.src_port, ==,
                      0x1234);  // src_port should be 0x1234
  munit_assert_uint16(header.tcp_header.dest_port, ==,
                      0x5678);  // dest_port should be 0x5678
  munit_assert_uint32(header.tcp_header.seq_num, ==,
                      0x9ABCDEF0);  // seq_num should be 0x9ABCDEF0
  return MUNIT_OK;
}

static MunitResult test_field_raw_size(const MunitParameter params[],
                                       void* data) {
  munit_assert_size(sizeof(((packet_header_t*)0)->raw), ==,
                    8);  // raw field should be 8 bytes
  return MUNIT_OK;
}

static MunitResult test_field_to_raw_consistency(const MunitParameter params[],
                                                 void* data) {
  packet_header_t header = {0};
  header.tcp_header.src_port = 0x1234;
  header.tcp_header.dest_port = 0x5678;
  header.tcp_header.seq_num = 0x9ABCDEF0;

  munit_assert_uint8(header.raw[0], ==, 0x34);  // [0] LSB of src_port
  munit_assert_uint8(header.raw[1], ==, 0x12);  // [1] MSB of src_port
  munit_assert_uint8(header.raw[2], ==, 0x78);  // [2] LSB of dest_port
  munit_assert_uint8(header.raw[3], ==, 0x56);  // [3] MSB of dest_port
  munit_assert_uint8(header.raw[4], ==, 0xF0);  // [4] LSB of seq_num
  munit_assert_uint8(header.raw[5], ==, 0xDE);  // [5]
  munit_assert_uint8(header.raw[6], ==, 0xBC);  // [6]
  munit_assert_uint8(header.raw[7], ==, 0x9A);  // [7] MSB of seq_num
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/test_packet_header_size", test_packet_header_size, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_tcp_header_fields", test_tcp_header_fields, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_field_raw_size", test_field_raw_size, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_field_to_raw_consistency", test_field_to_raw_consistency, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/PacketHeader", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
