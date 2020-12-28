#include "uart.h"
#include "tcp_socket.h"

#define TXD_PIN 1
#define RXD_PIN 3
#define RX_BUF_SIZE 1024

bool receive_flag = false;
QueueHandle_t puttyQueue;

void setupUART(){

const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)); 

}

void uart_rx_task(void *pvParameters){

static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* uart_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    puttyQueue = xQueueCreate(1, sizeof(uint8_t*));
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, uart_data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            uart_data[rxBytes] = (uint8_t)'\n';
            uart_data[rxBytes+1] = (uint8_t)'\r';
            uart_data[rxBytes+2] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes+2, uart_data);
            //ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, uart_data, rxBytes, ESP_LOG_INFO);
            xQueueSend(puttyQueue, (void *)&uart_data, portMAX_DELAY);
        }
    }
    free(uart_data);

}