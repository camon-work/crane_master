#include <stdio.h>
#include <stdint.h>
#include "sdk.h"
#include "Ethernet/socket.h"
#include "loopback.h"
#include "dhcp_client.h"
#include "../NRC7394_wifi.h"
#include "../ui_globals.h"
#include "../ui_defines.h"
#include "../user_task.h"

//uint8_t destip[4] = {192, 168, 100, 40};
//uint8_t destip[4] = {192, 168, 0, 200};
//uint8_t destip[4] = {192, 168, 0, 173};

//uint16_t destport = 3333;
//uint16_t destport = 5001;
uint8_t destip[4];
uint16_t destport;

uint8_t tx_packet[1024] = {0,};

void ethernet_ctrl_read_packet_make(bool ack){
	int offset = 0, i;
	
	memset(tx_packet, 0, sizeof(tx_packet));
	
	// Device ID(ssid)
	sprintf(tx_packet, "%s", wifi_ap_state->wifi_config.ssid);
	//status
	offset = strlen(tx_packet);
	if(ack == true) strcpy(tx_packet + offset, ",OK");
	else strcpy(tx_packet + offset, "ERROR");
	
	for(i=0; i<6; i++) {
		offset = strlen(tx_packet);
		sprintf(tx_packet + offset, ",%d", ctrl_output_state(CTRL_OUT_PORT, i));
	}
	
#if 0
	// LIGH1_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LIGHT1_24V_OUT_BIT);
	
	// LIGH2_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LIGHT2_24V_OUT_BIT);
	
	// SIREN_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, SIREN_24V_OUT_BIT);
	
	// NET_OUT_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, NET_24V_OUT_BIT);
	
	// NET_220V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, NET_220V_OUT_BIT);
	
	// LOW_LIGHT_220V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LH_220V_OUT_BIT);
#endif
	
	// ALARM SW
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d", pdc->ctrl_pin_state.alarm_switch_in);
	
	// IN SW
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d", pdc->ctrl_pin_state.switch_in );
	
	// LiDAR IN
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d", pdc->ctrl_pin_state.lidar_in);
	
	// WiFi ì—°ê²° ê°œìˆ˜
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d", pdc->ctrl_pin_state.wifi_num);
}

// LIGHT1_24V , LIGHT2_24V , SIREN_24V , NET_OUT_24V, NET_OUT_220V, LOW_LIGHT_220V, ì—¬ê¸°ë¶€í„°ëŠ” ì˜µì…˜(ED0~LED5, BEEP)
void ethernet_ctrl_write_packet_make(bool ack){
	int offset = 0, i;
	
	memset(tx_packet, 0, sizeof(tx_packet));
	
	// Device ID(ssid)
	sprintf(tx_packet, "%s", wifi_ap_state->wifi_config.ssid);
	//status
	offset = strlen(tx_packet);
	if(ack == true) strcpy(tx_packet + offset, ",OK");
	else strcpy(tx_packet + offset, "ERROR");
	
	for(i = 0; i < 6; i++){
		offset = strlen(tx_packet);
		sprintf(tx_packet + offset, ",%d", ctrl_output_state(CTRL_OUT_PORT, LIGHT1_24V_OUT_BIT));
	}
#if 0
	// LIGH1_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LIGHT1_24V_OUT_BIT));
	
	// LIGH2_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LIGHT2_24V_OUT_BIT));
	
	// SIREN_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, SIREN_24V_OUT_BIT));
	
	// NET_OUT_24V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, NET_24V_OUT_BIT));
	
	// NET_220V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, NET_220V_OUT_BIT));
	
	// LOW_LIGHT_220V
	offset = strlen(tx_packet);
	sprintf(tx_packet + offset, ",%d,", ctrl_output_state(CTRL_OUT_PORT, LH_220V_OUT_BIT));
#endif
	
}

void ethernet_ctrl_write_output(char *buff){
	char *offset, i;
	
// OUTPUT
	offset = strstr(buff, "CNTR_WRITE,");
	offset += strlen("CNTR_WRITE,");
	for(i = 0; i < 6; i++){
		if(strncmp(offset, "0", 1) == 0)
			*R_GPOLOW(0) = (1 << i);
		else if(strncmp(offset, "1", 1) == 0)
			*R_GPOHIGH(0) = (1 << i);
		else debugprintf("\n\r UNKNOWN [%d] [%s]", i, offset);
		offset+=2;
	}
	
// LED
	for(i = 0; i < 6; i++){
		if(strncmp(offset, "0", 1) == 0)
			*R_GPOHIGH(1) = (1 << i);
		else if(strncmp(offset, "1", 1) == 0)
			*R_GPOLOW(1) = (1 << i);
		else debugprintf("\n\r UNKNOWN LED [%d] [%s]", i, offset);
		offset+=2;
	}
	
#if 0	
	if(strncmp(offset, '0', 1) == 0)
		LIGHT1_24V_LOW();
	else if(strncmp(offset, '1', 1) == 0)
		LIGHT1_24V_HIGH();
	else debugprintf("\n\r UNKNOWN LIGHT1_24V [%s]", offset);
	
	// LIGHT2_24V
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LIGHT2_24V_LOW();
	else if(strncmp(offset, '1', 1) == 0)
		LIGHT2_24V_HIGH();
	else debugprintf("\n\r UNKNOWN LIGHT2_24V [%s]", offset);
	
	// SIREN_24V
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		SIREN_24V_LOW();
	else if(strncmp(offset, '1', 1) == 0)
		SIREN_24V_HIGH();
	else debugprintf("\n\r UNKNOWN SIREN_24V [%s]", offset);
	
	// NET_OUT_24V
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		NET_OUT_24V_OFF();
	else if(strncmp(offset, '1', 1) == 0)
		NET_OUT_24V_ON();
	else debugprintf("\n\r UNKNOWN NET_OUT_24V [%s]", offset);
	
	
	// NET_OUT_220V
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		NET_OUT_220V_OFF();
	else if(strncmp(offset, '1', 1) == 0)
		NET_OUT_220V_ON();
	else debugprintf("\n\r UNKNOWN NET_OUT_220V [%s]", offset);
	
	// LOW_LIGHT_220V
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LOW_LIGHT_220V_OFF();
	else if(strncmp(offset, '1', 1) == 0)
		LOW_LIGHT_220V_ON();
	else debugprintf("\n\r UNKNOWN LOW_LIGHT_OUT_220V [%s]", offset);

	///////// ì•„ëž˜ëŠ” í…ŒìŠ¤íŠ¸ ìš©ìž„  ë‚˜ì¤‘ì— ì‚­ì œ ì˜ˆì •///////////////////
	// LED0
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL0);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL0);
	else debugprintf("\n\r UNKNOWN LED0 [%s]", offset);
	
	// LED1
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL1);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL1);
	else debugprintf("\n\r UNKNOWN LED1 [%s]", offset);
	
	// LED2
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL2);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL2);
	else debugprintf("\n\r UNKNOWN LED2 [%s]", offset);
	
	// LED3
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL3);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL3);
	else debugprintf("\n\r UNKNOWN LED3 [%s]", offset);
	
	// LED4
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL4);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL4;
	else debugprintf("\n\r UNKNOWN LED4[%s]", offset);
	
	// LED5
	offset+=2;
	if(strncmp(offset, '0', 1) == 0)
		LED_CONTROL_OFF(LED_CTL5);
	else if(strncmp(offset, '1', 1) == 0)
		LED_CONTROL_ON(LED_CTL5);
	else debugprintf("\n\r UNKNOWN LED5 [%s]", offset);
#endif
}



void ethernt_sensor_read_packet_make(int sens_sum){
	int offset;
	int sens_num = 0;
	
	
	memset(tx_packet, 0, sizeof(tx_packet));
	
	// Device ID(ssid)
	sprintf(tx_packet, "%s", wifi_ap_state->wifi_config.ssid);
	//status
	offset = strlen(tx_packet);
//	if(ack == true) strcpy(tx_packet + offset, ",OK,");
//	else strcpy(tx_packet + offset. "ERROR");
	
	
}

bool ethernet_packet_parsing(char *buff, int length){
//	int sens_num;
	char *ptr;
	
// Device ID(ssid)
	if(strstr(buff, wifi_ap_state->wifi_config.ssid) == NULL) return false;

// Command
	if(strstr(buff, "CNTR_READ") != NULL){// ¼­¹ö°¡ ÄÁÆ®·Ñ·¯¿¡°Ô µ¥ÀÌÅÍ ¿ä±¸
		ethernet_ctrl_read_packet_make(true);
	}
	else if(strstr(buff, "SENS_READ") != NULL){// ¼­¹ö°¡ ÄÁÆ®·Ñ·¯¿¡°Ô ¼¾½ºµ¥ÀÌÅÍ ¿ä±¸.
		ptr = strstr(buff, "SENS_READ");
		ptr += strlen("SENS_READ,");
		
		ethernt_sensor_read_packet_make(atoi(ptr));
	}
	else if(strstr(buff, "CNTR_WRITE") != NULL){// ¼­¹ö°¡ ÄÁÆ®·Ñ·¯ Á¦¾î µ¥ÀÌÅÍ
		ethernet_ctrl_write_output(buff);
		ethernet_ctrl_write_packet_make(true);
	}
	else{
		memset(tx_packet, 0, sizeof(tx_packet));
		sprintf(tx_packet, "%s%s", wifi_ap_state->wifi_config.ssid, "ERROR");
	}
	return true;
}


// TCP & UDP Loopback Test
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
//	int i;
   int32_t ret;
   uint16_t size = 0, sentsize=0;
   //uint8_t tx_buf[1024] = {0,};
//   char *ptr;
// #ifdef _LOOPBACK_DEBUG_
   // uint8_t destip[4];
   // uint16_t destport;
// #endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
        if(getSn_IR(sn) & Sn_IR_CON)
        {
#ifdef _LOOPBACK_DEBUG_
        	 getSn_DIPR(sn, destip);
        	 destport = getSn_DPORT(sn);

        	 debugprintf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
			 wiznet_state->eth_connected = true;
#endif
            setSn_IR(sn,Sn_IR_CON);
        }
        if((size = getSn_RX_RSR(sn)) > 0)
        {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recv(sn, buf, size);
            if(ret <= 0) return ret;
// data receive
			if(ethernet_packet_parsing(buf, ret) == true){// server request data format
				
			// if(strstr(buf, wifi_ap_state->wifi_config.ssid) != NULL){
				// if(strstr(buf, "CNTR_READ") != NULL){// CONTROL READ
					// PRINTLINE;
				// }
				// else if(strstr(buf, "SENS_READ") != NULL){// SENSOR READ
					// PRINTLINE;
				// }
				// else if(strstr(buf, "WRITE") != NULL){// CONTROL SET
					
					// //send(sn,tx_buf, strlen(tx_buf));
					// PRINTLINE;
				// }
				// else debugprintf("%s %s ", buf, wifi_ap_state->wifi_config.ssid);
			// }
			// for(i=0; i<ret; i++)
				// debugprintf("%x  ", buf[i]);///////////////////////////////////////
				sentsize = 0;
				size = strlen(tx_packet);
				while(size != sentsize)
				{
// data send
					ret = send(sn,tx_packet+sentsize,size-sentsize);
					if(ret < 0)
					{
						close(sn);
						return ret;
					}
					sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				}
			}
		//	else PRINTLINE;
        }
		//else PRINTLINE;
        break;
      case SOCK_CLOSE_WAIT :
		  PRINTLINE;
#ifdef _LOOPBACK_DEBUG_
         //debugprintf("%d:CloseWait\r\n",sn);
#endif
         if((ret=disconnect(sn)) != SOCK_OK){
		//	 PRINTLINE;
			return ret;
		 }
#ifdef _LOOPBACK_DEBUG_
         debugprintf("%d:Socket closed\r\n",sn);
#endif
         break;
      case SOCK_INIT :
#ifdef _LOOPBACK_DEBUG_
    	 debugprintf("%d:Listen, TCP server loopback, port [%d]\r\n",sn, port);
#endif
		if( (ret = connect(sn, destip, destport)) != SOCK_OK){
		//	PRINTLINE;
			return ret;	//	Try to TCP connect to the TCP server (destination)
		}
		PRINTLINE;
		break;
	

       ///////////////  if( (ret = listen(sn)) != SOCK_OK) return ret;
      ///////////////////////////  break;
      case SOCK_CLOSED:
		  if(wiznet_state->eth_connected == true){
			  wiznet_state->eth_connected = false;
			  wiznet_state->w5500_step = WIZNET_STEP_RESET;// ¼­¹ö¿¡¼­ ¿¬°á ²÷À½.. ´Ù½Ã ½ÃÀÛÇØ¾ß ÇÔ.
		  }
		 // PRINTLINE;
#ifdef _LOOPBACK_DEBUG_
         //debugprintf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret=socket(sn, Sn_MR_TCP, port, 0x00)) != sn)
         //if((ret=socket(sn, Sn_MR_TCP, port, Sn_MR_ND)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         //debugprintf("%d:Socket opened\r\n",sn);
#endif
       break;
      default:
         break;
   }
   return 1;
}

int32_t loopback_udps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint16_t destport;
   //uint8_t  packinfo = 0;
   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn,buf,size,destip,(uint16_t*)&destport);
            if(ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
               debugprintf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
               return ret;
            }
            size = (uint16_t) ret;
            sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn,buf+sentsize,size-sentsize,destip,destport);
               if(ret < 0)
               {
#ifdef _LOOPBACK_DEBUG_
                  debugprintf("%d: sendto error. %ld\r\n",sn,ret);
#endif
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
         }
         break;
      case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
         //debugprintf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret=socket(sn,Sn_MR_UDP,port,0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
         debugprintf("%d:Opened, UDP loopback, port [%d]\r\n",sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
}


void Ethernet_string_to_num(void)
{
	char *ptr;
	int i;
	
	ptr = wifi_ap_state->net_info.remote_ip;
	
	destip[0] = atoi(ptr);
	debugprintf("\n\r destip : %d. ", destip[0]);

	for(i = 1; i < 4; i++){
		ptr = strchr(ptr, '.');
		ptr++;
		destip[i] = atoi(ptr);
		debugprintf(" %d .", destip[i]);
	}
	
	destport = atoi(wifi_ap_state->net_info.remote_port);
	debugprintf("\n\r destport[%d]\n\r", destport);
	
}


