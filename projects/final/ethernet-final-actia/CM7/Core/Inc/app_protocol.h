#ifndef APP_PROTOCOL_H
#define APP_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define APP_SENSOR_REQUEST_LENGTH 12U
#define APP_CAN_RESPONSE_ID 0x321U
#define APP_CAN_ALERT_ID 0x1FFU
#define APP_CAN_RESPONSE_LENGTH 4U
#define APP_RESPONSE_THRESHOLD 100U

typedef enum {
  APP_RESPONSE_INVALID = 0,
  APP_RESPONSE_OK,
  APP_RESPONSE_THRESHOLD_EXCEEDED
} app_response_status_t;

bool app_protocol_parse_request(const uint8_t *request, size_t length,
                                uint8_t *sensor, uint32_t *can_id);

app_response_status_t app_protocol_validate_response(
    uint32_t can_id, uint32_t data_length, const uint8_t *data,
    uint8_t sensor, uint32_t sensor_value);

#endif
