#ifndef PACKET_H
#define PACKET_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define MAX_PACKET_LEN          (128)

typedef struct
{
    uint32_t ofs;
    uint8_t buf[MAX_PACKET_LEN];    /* total frame buffer */
    uint16_t payload_len;
    uint16_t len;                   /* total frame len */
    uint8_t type;
} Packet_t;

/* packet Rx API */
typedef void (*OnDataReceivedEvent)(Packet_t *pkt);
void Packet_DecodeInit(Packet_t *pkt, OnDataReceivedEvent rx_handler);
uint32_t Packet_Decode(uint8_t c);
	
#ifdef __cplusplus
}
#endif
#endif
