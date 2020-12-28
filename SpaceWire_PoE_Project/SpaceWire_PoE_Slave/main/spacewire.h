#ifndef _SPACEWIRE_H_
#define _SPACEWIRE_H_

#include "EthernetSW.h"

#define EOP_HEADER (uint8_t)0b101010 // char '*'
#define EEP_HEADER (uint8_t)0b10 
#define NO_EOP_HEADER (uint8_t)0b00
#define CCODE_HEADER (uint8_t)0b11
#define SW_BUFFER_SIZE 128
#define EOP (uint8_t)0b1011110  // char '^'

void receiveSWPackets(esp_eth_handle_t, uint8_t*, uint32_t, void *);
void sendSWPacket(uint8_t*, uint16_t, uint8_t, uint8_t*);                          //data buffer to send, size of data, type of data (N-Char, Broadcast Code)
void processSWPacketHeader(uint8_t*);
void readSWPacket(uint8_t*);
int readSWCCode();
char* convertToCharArray(uint8_t*, uint16_t);

#endif