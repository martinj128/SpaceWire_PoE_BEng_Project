#ifndef _tcp_socket_h_
#define _tcp_socket_h_

// TCP Socket Files
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

//ESP network IF and general files
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "uart.h"
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void sendToPuttyTask(void *pvParameters);
int transmitToPuTTY(const int, uint8_t*, int);
void tcp_server_task(void *pvParameters);

extern QueueHandle_t puttyQueue;

#endif
