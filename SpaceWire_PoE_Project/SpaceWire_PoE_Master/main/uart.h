#ifndef _UART_H_
#define _UART_H_

#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "tcp_socket.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setupUART(void);
void uart_rx_task(void *pvParameters);

extern bool receive_flag;
extern QueueHandle_t puttyQueue;

#endif