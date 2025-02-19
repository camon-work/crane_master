
#ifndef  _WIZCHIP_LOOPBACK_H_
#define  _WIZCHIP_LOOPBACK_H_

#define _LOOPBACK_DEBUG_

#define DATA_BUF_SIZE   2048

extern uint8_t destip[4];
extern uint16_t destport;

extern int32_t loopback_tcps(uint8_t, uint8_t*, uint16_t);
extern int32_t loopback_udps(uint8_t, uint8_t*, uint16_t);

extern void Ethernet_string_to_num(void);
#endif   // _WIZCHIP_LOOPBACK_H_
 
