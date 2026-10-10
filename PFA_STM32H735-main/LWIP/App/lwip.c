/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#if (defined ( __CC_ARM ) || defined (__ARMCC_VERSION))  /* MDK ARM Compiler */
#include "lwip/sio.h"
#endif /* MDK ARM Compiler */
#include "ethernetif.h"
#include <string.h>

/* USER CODE BEGIN 0 */
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"
#include <stdio.h>

#define SUPERVISION_UDP_PORT 5000U

typedef struct
{
  char destination[40];
  char logical_address[5];
  char payload[96];
  uint8_t use_ipv6;
  ip_addr_t destination_address;
} SupervisionUdpRequest;

static SupervisionUdpRequest supervision_request;
static struct udp_pcb* supervision_udp_recv_pcb = NULL;
static void (*supervision_udp_recv_callback)(const char* response, void* context) = NULL;
static void* supervision_udp_recv_context = NULL;

static void supervision_udp_recv_callback_internal(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  if (p != NULL && supervision_udp_recv_callback != NULL)
  {
    char response[128] = {0};
    size_t copy_len = p->tot_len < sizeof(response) - 1 ? p->tot_len : sizeof(response) - 1;
    pbuf_copy_partial(p, response, copy_len, 0);
    response[copy_len] = '\0';
    supervision_udp_recv_callback(response, supervision_udp_recv_context);
  }
  if (p != NULL)
  {
    pbuf_free(p);
  }
}

int supervision_udp_set_recv_callback(void (*callback)(const char* response, void* context), void* context)
{
  supervision_udp_recv_callback = callback;
  supervision_udp_recv_context = context;
  if (supervision_udp_recv_pcb == NULL)
  {
    supervision_udp_recv_pcb = udp_new_ip_type(IPADDR_TYPE_V4);
    if (supervision_udp_recv_pcb != NULL)
    {
      ip_set_option(supervision_udp_recv_pcb, SOF_REUSEADDR);
      if (udp_bind(supervision_udp_recv_pcb, IP_ANY_TYPE, SUPERVISION_UDP_PORT) == ERR_OK)
      {
        udp_recv(supervision_udp_recv_pcb, supervision_udp_recv_callback_internal, NULL);
        return 0;
      }
      udp_remove(supervision_udp_recv_pcb);
      supervision_udp_recv_pcb = NULL;
    }
  }
  else if (callback == NULL)
  {
    // Callback removed, clean up PCB
    udp_recv(supervision_udp_recv_pcb, NULL, NULL);
    udp_remove(supervision_udp_recv_pcb);
    supervision_udp_recv_pcb = NULL;
  }
  return -1;
}

static int supervision_parse_ipv4_decimal(const char *text, ip_addr_t *address)
{
  uint16_t octet[4] = {0};
  uint8_t group = 0;
  uint8_t digits = 0;
  ip4_addr_t ipv4;

  while (*text != '\0')
  {
    if ((*text >= '0') && (*text <= '9'))
    {
      if ((digits == 3U) || (octet[group] > 25U)) return -1;
      octet[group] = (uint16_t)(octet[group] * 10U + (uint16_t)(*text - '0'));
      if (octet[group] > 255U) return -1;
      digits++;
    }
    else if ((*text == '.') && (group < 3U) && (digits > 0U))
    {
      group++;
      digits = 0;
    }
    else
    {
      return -1;
    }
    text++;
  }
  if ((group != 3U) || (digits == 0U)) return -1;
  IP4_ADDR(&ipv4, octet[0], octet[1], octet[2], octet[3]);
  ip_addr_copy_from_ip4(*address, ipv4);
  return 0;
}

static void supervision_udp_send_callback(void *argument)
{
  SupervisionUdpRequest *request = (SupervisionUdpRequest *)argument;
  struct udp_pcb *pcb;
  struct pbuf *packet;
  size_t payload_length;

  payload_length = strlen(request->payload);

  // Use the same PCB that listens on port 5000, so replies come back to port 5000
  pcb = supervision_udp_recv_pcb;
  if (pcb == NULL)
  {
    // Fallback: create temporary PCB if recv PCB not initialized yet
    pcb = udp_new_ip_type(IP_GET_TYPE(&request->destination_address));
    if (pcb == NULL)
    {
      return;
    }
  }

  packet = pbuf_alloc(PBUF_TRANSPORT, (u16_t)payload_length, PBUF_RAM);
  if (packet != NULL)
  {
    pbuf_take(packet, request->payload, (u16_t)payload_length);
    (void)udp_sendto(pcb, packet, &request->destination_address, SUPERVISION_UDP_PORT);
    pbuf_free(packet);
  }

  // Only remove if we created a temporary PCB (not the shared recv PCB)
  if (pcb != supervision_udp_recv_pcb)
  {
    udp_remove(pcb);
  }
}

int supervision_udp_configure(const char *destination, const char *logical_address, unsigned char use_ipv6)
{
  ip_addr_t parsed_address;

  if ((destination == NULL) || (logical_address == NULL))
  {
    return -1;
  }
  if (use_ipv6)
  {
    if (!ipaddr_aton(destination, &parsed_address) || !IP_IS_V6(&parsed_address)) return -1;
  }
  else if (supervision_parse_ipv4_decimal(destination, &parsed_address) != 0)
  {
    return -1;
  }

  strncpy(supervision_request.destination, destination, sizeof(supervision_request.destination) - 1U);
  supervision_request.destination[sizeof(supervision_request.destination) - 1U] = '\0';
  strncpy(supervision_request.logical_address, logical_address, sizeof(supervision_request.logical_address) - 1U);
  supervision_request.logical_address[sizeof(supervision_request.logical_address) - 1U] = '\0';
  supervision_request.use_ipv6 = use_ipv6;
  ip_addr_copy(supervision_request.destination_address, parsed_address);
  return 0;
}

int supervision_udp_send_protocol_notification(unsigned char use_ipv6)
{
  const char *message = use_ipv6 ? "on a utilisé le protocole IPv6" : "on a utilisé le protocole IPv4";

  if (supervision_request.destination[0] == '\0')
  {
    return -1;
  }
  strncpy(supervision_request.payload, message, sizeof(supervision_request.payload) - 1U);
  supervision_request.payload[sizeof(supervision_request.payload) - 1U] = '\0';
  return tcpip_callback(supervision_udp_send_callback, &supervision_request) == ERR_OK ? 0 : -1;
}

int supervision_udp_send_logical(void)
{
  if (supervision_request.destination[0] == '\0')
  {
    return -1;
  }
  snprintf(supervision_request.payload, sizeof(supervision_request.payload), "logical=%s", supervision_request.logical_address);
  return tcpip_callback(supervision_udp_send_callback, &supervision_request) == ERR_OK ? 0 : -1;
}

int supervision_udp_send_payload(const char *payload)
{
  if ((payload == NULL) || (supervision_request.destination[0] == '\0'))
  {
    return -1;
  }
  snprintf(supervision_request.payload, sizeof(supervision_request.payload), "logical=%s;%s", supervision_request.logical_address, payload);
  return tcpip_callback(supervision_udp_send_callback, &supervision_request) == ERR_OK ? 0 : -1;
}

/**
 * @brief Envoi d'une trame UDP brute vers le destinataire configuré (ou 192.168.0.5 par défaut)
 * Utilise tcpip_callback pour garantir la sécurité d'exécution multi-tâches avec FreeRTOS.
 */
int supervision_udp_send_raw(const char *payload)
{
  if (payload == NULL)
  {
    return -1;
  }
  // Si aucune adresse IP n'a été saisie dans Screen5, on applique l'IP par défaut du PC (192.168.0.5)
  if (supervision_request.destination[0] == '\0')
  {
    (void)supervision_udp_configure("192.168.0.5", "0001", 0);
  }
  strncpy(supervision_request.payload, payload, sizeof(supervision_request.payload) - 1U);
  supervision_request.payload[sizeof(supervision_request.payload) - 1U] = '\0';
  return tcpip_callback(supervision_udp_send_callback, &supervision_request) == ERR_OK ? 0 : -1;
}

/**
 * @brief Renvoie l'adresse IP de destination courante
 */
const char *supervision_udp_get_destination_ip(void)
{
  if (supervision_request.destination[0] != '\0')
  {
    return supervision_request.destination;
  }
  return "192.168.0.5";
}

/* USER CODE END 0 */
/* Private function prototypes -----------------------------------------------*/
static void ethernet_link_status_updated(struct netif *netif);
/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];
ip6_addr_t ip6addr;
/* USER CODE BEGIN OS_THREAD_ATTR_CMSIS_RTOS_V2 */
#define INTERFACE_THREAD_STACK_SIZE ( 1024 )
osThreadAttr_t attributes;
/* USER CODE END OS_THREAD_ATTR_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * LwIP initialization function
  */
void MX_LWIP_Init(void)
{
  /* IP addresses initialization */
  IP_ADDRESS[0] = 192;
  IP_ADDRESS[1] = 168;
  IP_ADDRESS[2] = 0;
  IP_ADDRESS[3] = 20;
  NETMASK_ADDRESS[0] = 255;
  NETMASK_ADDRESS[1] = 255;
  NETMASK_ADDRESS[2] = 255;
  NETMASK_ADDRESS[3] = 0;
  GATEWAY_ADDRESS[0] = 192;
  GATEWAY_ADDRESS[1] = 168;
  GATEWAY_ADDRESS[2] = 0;
  GATEWAY_ADDRESS[3] = 1;

/* USER CODE BEGIN IP_ADDRESSES */
  IP_ADDRESS[0] = 192;
  IP_ADDRESS[1] = 168;
  IP_ADDRESS[2] = 0;
  IP_ADDRESS[3] = 20;
/* USER CODE END IP_ADDRESSES */

  /* Initialize the LwIP stack with RTOS */
  tcpip_init( NULL, NULL );

  /* IP addresses initialization without DHCP (IPv4) */
  IP4_ADDR(&ipaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
  IP4_ADDR(&netmask, NETMASK_ADDRESS[0], NETMASK_ADDRESS[1] , NETMASK_ADDRESS[2], NETMASK_ADDRESS[3]);
  IP4_ADDR(&gw, GATEWAY_ADDRESS[0], GATEWAY_ADDRESS[1], GATEWAY_ADDRESS[2], GATEWAY_ADDRESS[3]);

  /* add the network interface (IPv4/IPv6) with RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /* Create IPv6 local address */
  netif_create_ip6_linklocal_address(&gnetif, 0);
  netif_ip6_addr_set_state(&gnetif, 0, IP6_ADDR_VALID);
  gnetif.ip6_autoconfig_enabled = 1;

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  /* We must always bring the network interface up connection or not... */
  netif_set_up(&gnetif);

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  /* Create the Ethernet link handler thread */
/* USER CODE BEGIN H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.name = "EthLink";
  attributes.stack_size = INTERFACE_THREAD_STACK_SIZE;
  attributes.priority = osPriorityBelowNormal;
  osThreadNew(ethernet_link_thread, &gnetif, &attributes);
/* USER CODE END H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */
}

#ifdef USE_OBSOLETE_USER_CODE_SECTION_4
/* Kept to help code migration. (See new 4_1, 4_2... sections) */
/* Avoid to use this user section which will become obsolete. */
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
#endif

/**
  * @brief  Notify the User about the network interface config status
  * @param  netif: the network interface
  * @retval None
  */
static void ethernet_link_status_updated(struct netif *netif)
{
  if (netif_is_up(netif))
  {
/* USER CODE BEGIN 5 */
/* USER CODE END 5 */
  }
  else /* netif is down */
  {
/* USER CODE BEGIN 6 */
/* USER CODE END 6 */
  }
}

#if (defined ( __CC_ARM ) || defined (__ARMCC_VERSION))  /* MDK ARM Compiler */
/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  sio_fd_t sd;

/* USER CODE BEGIN 7 */
  sd = 0; // dummy code
/* USER CODE END 7 */

  return sd;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
/* USER CODE BEGIN 8 */
/* USER CODE END 8 */
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 9 */
  recved_bytes = 0; // dummy code
/* USER CODE END 9 */
  return recved_bytes;
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 10 */
  recved_bytes = 0; // dummy code
/* USER CODE END 10 */
  return recved_bytes;
}
#endif /* MDK ARM Compiler */

