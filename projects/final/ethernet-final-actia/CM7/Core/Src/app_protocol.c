#include "app_protocol.h"

#include <string.h>

bool app_protocol_parse_request(const uint8_t *request, size_t length,
                                uint8_t *sensor, uint32_t *can_id) {
  static const uint8_t sensor1_request[] = "logical=0001";
  static const uint8_t sensor2_request[] = "logical=0002";

  if (request == NULL || sensor == NULL || can_id == NULL ||
      length != APP_SENSOR_REQUEST_LENGTH) {
    return false;
  }

  if (memcmp(request, sensor1_request, APP_SENSOR_REQUEST_LENGTH) == 0) {
    *sensor = 1;
    *can_id = 0x123U;
    return true;
  }

  if (memcmp(request, sensor2_request, APP_SENSOR_REQUEST_LENGTH) == 0) {
    *sensor = 2;
    *can_id = 0x124U;
    return true;
  }

  return false;
}

app_response_status_t app_protocol_validate_response(
    uint32_t can_id, uint32_t data_length, const uint8_t *data,
    uint8_t sensor, uint32_t sensor_value) {
  static const uint8_t expected_data[] = {0xDE, 0xAD, 0xBE, 0xEF};

  if (data == NULL || sensor < 1U || sensor > 2U ||
      can_id != APP_CAN_RESPONSE_ID ||
      data_length != APP_CAN_RESPONSE_LENGTH ||
      memcmp(data, expected_data, APP_CAN_RESPONSE_LENGTH) != 0) {
    return APP_RESPONSE_INVALID;
  }

  return sensor_value > APP_RESPONSE_THRESHOLD
             ? APP_RESPONSE_THRESHOLD_EXCEEDED
             : APP_RESPONSE_OK;
}
