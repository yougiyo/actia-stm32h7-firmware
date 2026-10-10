#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "lwip.h"
#include <cstdio>
#include <cstring>
#include <vector>

Model::Model() : modelListener(0)
{
}

void Model::tick()
{
}

/**
 * @brief Envoi d'une trame d'octets UDS/OBD2 brute via Ethernet UDP
 * 
 * Les octets sont convertis en chaîne hexadécimale avec espaces (ex: "10 01 11")
 * puis transmis de manière thread-safe à la pile LwIP via supervision_udp_send_raw().
 */
void Model::sendOBD2Request(const std::vector<uint8_t>& payload)
{
    if (payload.empty())
    {
        return;
    }

    char hexBuffer[64] = { 0 };
    int offset = 0;
    for (size_t i = 0; i < payload.size() && offset < static_cast<int>(sizeof(hexBuffer)) - 4; ++i)
    {
        offset += snprintf(hexBuffer + offset, sizeof(hexBuffer) - offset, (i == 0 ? "%02X" : " %02X"), payload[i]);
    }

    supervision_udp_send_raw(hexBuffer);
}

/**
 * @brief Envoi d'une chaîne de caractères brute via Ethernet UDP
 */
void Model::sendOBD2Request(const char* payloadStr)
{
    if (payloadStr == 0 || payloadStr[0] == '\0')
    {
        return;
    }
    supervision_udp_send_raw(payloadStr);
}
