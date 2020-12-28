/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "tcp_socket.h"
#include "uart.h"
#include "EthernetSW.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect()); //setup wifi in menuconfig, connects etc.

    setupUART();
    setupEthernet();                                                            
    //xTaskCreate(uart_rx_task, "uart_task", 4096, NULL, 7, NULL);                //create task for uart to read data
    //xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL); //create task to creat tcp/ip client on esp32
}
