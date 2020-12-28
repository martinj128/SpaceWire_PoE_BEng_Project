#include "spacewire.h"

unsigned char *convertToCharArray(uint8_t*, uint32_t len);

uint16_t eth_index; //variable to hold current index in eth rx buffer
uint16_t dataRead;  //variable to hold amount of data read from eth rx buffer, updated within readSWPacket()
uint8_t checkSWHeader[4] = {EOP_HEADER, EEP_HEADER, NO_EOP_HEADER, CCODE_HEADER};
static const char *TAG = "spacwire"; 
QueueHandle_t puttyQueue;

void receiveSWPackets(esp_eth_handle_t eth_handle, uint8_t *eth_rx_buffer, uint32_t len, void *priv)
{
    eth_index = 0;

    for(int i=0; i<6; i++) //get MAC destination address from payload
    {
        if(eth_rx_buffer[eth_index] == mac_addr_ptr[i])
        { //MAC address byte is correct, do nothing
            eth_index++;
        }
        else ESP_LOGI(TAG, "Wrong address"); return; //Destination Address not correct, exit function
    }

    ESP_LOGI(TAG, "Eth Frame Rx'd");

    while ((len-6) > 0)
    { //process the ethernet buffer until all data processed/read, -6 to account for dest addr already read
        processSWPacketHeader(eth_rx_buffer);
        len = -dataRead;
    }
}

void processSWPacketHeader(uint8_t *eth_rx_buffer)
{
    
    uint8_t headerMask = eth_rx_buffer[eth_index] >> 6; //shift headerbyte to LSB
    uint8_t swPacketType = 0;

    for (int i = 0; i < 4; i++)
    {
        if (headerMask & checkSWHeader[i])
        {
            swPacketType = checkSWHeader[i];
            break;
        }
    }

    switch (swPacketType)
    {
    case EOP_HEADER:
        //contains an EOP marker within payload
        //indicate to check for EOP when processing, get length, get packet number etc.
        readSWPacket(eth_rx_buffer);
        break;
    case EEP_HEADER:
        //contains of EEP marker within payload
        //send request for packet again

        break;
    case CCODE_HEADER:
        //payload contains Control Code in payload
        //indicate c-code, change of processing
        //readSWCCode();
        break;
    case NO_EOP_HEADER:
        //payload is maxxed on buffer and unfinished, to be completed in next Eth frame
        //indicate payload not all received, keep in buffer, get number etc.
        //readSWPacket();
        break;

    default:
        //header type not found, throw error
        break;
    }
}

void readSWPacket(uint8_t *eth_rx_buffer)
{
    uint16_t sw_packet_index = 0;
    uint8_t *swPacketBuffer = (uint8_t *)malloc(SW_BUFFER_SIZE);

    eth_index++; //increase index by 1 to account for headerbyte

    while (eth_rx_buffer[eth_index] != EOP)
    { 
            swPacketBuffer[sw_packet_index] = eth_rx_buffer[eth_index];
            sw_packet_index++;
            eth_index++;
    }

    dataRead = sw_packet_index + 2;  //+2 to account for headerbyte and EOP Marker byte read
    eth_index++; //+1 to move address to next headerbyte in loop

    xQueueSend(puttyQueue, (void *)&swPacketBuffer, portMAX_DELAY); //send to queue, which sends to putty terminal like uart
}

void sendSWPacket(uint8_t* tx_buffer, uint16_t buffer_len, uint8_t sw_packet_type, uint8_t* destination_addr){

    uint8_t* sw_tx_buffer = (uint8_t*)malloc(SW_BUFFER_SIZE);
    uint16_t tx_index = 0;

    //build the spacewire packet with ethernet frame requirements
    for(int i = 0; i<6; i++)
    { // insert destination MAC address
        sw_tx_buffer[tx_index] = destination_addr[i];
        tx_index++;
    }

    for(int i = 0; i<6; i++)
    { // insert destination MAC address
        sw_tx_buffer[tx_index] = mac_addr_ptr[i];
        tx_index++;
    }    

    switch(sw_packet_type)
    {
    case EOP_HEADER:
        sw_tx_buffer[tx_index] = EOP_HEADER; //place EOP Header in tx_buffer
        for(int i = 0; i<buffer_len; i++)
        {
            sw_tx_buffer[tx_index++] = tx_buffer[i];
        }
        sw_tx_buffer[tx_index++] = EOP; //place EOP marker in send string
    default:
        break;
    }
      
    if(esp_eth_transmit(eth_handle, sw_tx_buffer, tx_index) != ESP_OK)
    { //transmit SW packet over ethernet
        ESP_LOGE(TAG, "Ethernet send packet failed");
    }

}

unsigned char *convertToCharArray(uint8_t *buffer, uint32_t len)
{
    unsigned char *char_buffer = (unsigned char *)malloc(len + 1);
    int size = 0;

    for (int i = 0; i < len; i++)
    {
        char_buffer[i] = (unsigned char)buffer[i]; //convert uint8_t to char array to print
        size++;
    }

    char_buffer[size] = '\0'; //null terminate the array to be read like string

    return char_buffer;
}
