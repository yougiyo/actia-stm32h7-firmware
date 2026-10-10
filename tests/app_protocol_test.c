#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "app_protocol.h"

static void test_request_sensor_1(void) {
  const uint8_t request[] = "logical=0001";
  uint8_t sensor = 0;
  uint32_t can_id = 0;

  assert(app_protocol_parse_request(request, 12, &sensor, &can_id));
  assert(sensor == 1);
  assert(can_id == 0x123);
}

static void test_request_sensor_2(void) {
  const uint8_t request[] = "logical=0002";
  uint8_t sensor = 0;
  uint32_t can_id = 0;

  assert(app_protocol_parse_request(request, 12, &sensor, &can_id));
  assert(sensor == 2);
  assert(can_id == 0x124);
}

static void test_request_rejects_wrong_length_and_content(void) {
  const uint8_t valid[] = "logical=0001";
  const uint8_t wrong_content[] = "logical=0003";
  uint8_t sensor = 9;
  uint32_t can_id = 9;

  assert(!app_protocol_parse_request(valid, 11, &sensor, &can_id));
  assert(!app_protocol_parse_request(valid, 13, &sensor, &can_id));
  assert(!app_protocol_parse_request(wrong_content, 12, &sensor, &can_id));
}

static void test_request_rejects_null_arguments(void) {
  const uint8_t request[] = "logical=0001";
  uint8_t sensor = 0;
  uint32_t can_id = 0;

  assert(!app_protocol_parse_request(NULL, 12, &sensor, &can_id));
  assert(!app_protocol_parse_request(request, 12, NULL, &can_id));
  assert(!app_protocol_parse_request(request, 12, &sensor, NULL));
}

static void test_response_is_ok_at_and_below_threshold(void) {
  const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};

  assert(app_protocol_validate_response(0x321, 4, data, 1, 0) ==
         APP_RESPONSE_OK);
  assert(app_protocol_validate_response(0x321, 4, data, 2, 100) ==
         APP_RESPONSE_OK);
}

static void test_response_reports_threshold_exceeded(void) {
  const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};

  assert(app_protocol_validate_response(0x321, 4, data, 1, 101) ==
         APP_RESPONSE_THRESHOLD_EXCEEDED);
}

static void test_response_rejects_wrong_id_length_or_payload(void) {
  const uint8_t valid[] = {0xDE, 0xAD, 0xBE, 0xEF};
  uint8_t wrong_payload[4];

  assert(app_protocol_validate_response(0x320, 4, valid, 1, 1) ==
         APP_RESPONSE_INVALID);
  assert(app_protocol_validate_response(0x321, 3, valid, 1, 1) ==
         APP_RESPONSE_INVALID);
  assert(app_protocol_validate_response(0x321, 5, valid, 1, 1) ==
         APP_RESPONSE_INVALID);

  for (size_t index = 0; index < sizeof(wrong_payload); ++index) {
    memcpy(wrong_payload, valid, sizeof(wrong_payload));
    ++wrong_payload[index];
    assert(app_protocol_validate_response(0x321, 4, wrong_payload, 1, 1) ==
           APP_RESPONSE_INVALID);
  }
}

static void test_response_rejects_null_data_and_unknown_sensor(void) {
  const uint8_t valid[] = {0xDE, 0xAD, 0xBE, 0xEF};

  assert(app_protocol_validate_response(0x321, 4, NULL, 1, 1) ==
         APP_RESPONSE_INVALID);
  assert(app_protocol_validate_response(0x321, 4, valid, 0, 1) ==
         APP_RESPONSE_INVALID);
  assert(app_protocol_validate_response(0x321, 4, valid, 3, 1) ==
         APP_RESPONSE_INVALID);
}

int main(void) {
  test_request_sensor_1();
  test_request_sensor_2();
  test_request_rejects_wrong_length_and_content();
  test_request_rejects_null_arguments();
  test_response_is_ok_at_and_below_threshold();
  test_response_reports_threshold_exceeded();
  test_response_rejects_wrong_id_length_or_payload();
  test_response_rejects_null_data_and_unknown_sensor();
  puts("app_protocol_test: all scenarios passed");
  return 0;
}
