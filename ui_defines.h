#ifndef __UI_DEFINES_H
#define __UI_DEFINES_H



#define	_DEBUG_MSG_
#define	_DEBUG_MENU_MSG_

#define	_DEBUG_CMD_MSG_



#define	HIGH			1
#define	LOW				0
#define	CLR				0
#define	CLEAR			0


#define	ON				1
#define	OFF				0

#define	START			1
#define	STOP			0

#define	AUTOSET_RUN		1
#define	AUTOSET_OFF		0

#define	GOOD			1
#define	BAD				0

#define	POWER_ON		1
#define	POWER_OFF		0

#define	LED_ON			1
#define	LED_OFF			0

#define	CONNECT			1
#define	DISCONNECT		0

#define	DATA_READ_OK	2
#define	DATA_SEND		1
#define	DATA_READY		0

#define	CHECK_RUN		1
#define	CHECK_STOP		0

#define	PARSER_RUN		1
#define	PARSER_STOP		0

#define SEC_TO_MIN		60
#define MS_TO_SEC		10


#define DEBUG_UART_CH	0
#define WIFI_UART_CH	5


#define	USER_MEM_BUFF_SIZE	64
#define	USER_MEM_ADDR	0x4A000			// (1024*4*74)	// USER DB



#define REQ_CMD_MAX_COUNT		120		// (120 * 0.25sec = 30sec)


#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
#define makeWord(a, b)   ((uint16_t)(((uint8_t)(b)) | ((uint16_t)((uint8_t)(a))) << 8))




/***************************************************************************************************************************
 *  P0.3(OUTPUT) : NET_OUT_24V(네트웍 제어 24V)    | P0.2(OUTPUT) : SIREN_24V(사이렌 24V) | P0.1(OUTPUT) : LIGH2_24V(경광등-하 24V) | P0.0(OUTPUT) : LIGH1_24V(경광등-상 24V)
 *  P0.7(OUTPUT) : BUZZER | P0.6(OUTPUT) : NOT USED | P0.5(OUTPUT) : (HR705-2P-24VDC) | P0.4(OUTPUT) : NET_OUT_220V(HR705-2P-24VDC)
 ***************************************************************************************************************************/
 #define CTRL_OUT_PORT 0
#define LIGHT1_24V_OUT_BIT	0
#define LIGHT2_24V_OUT_BIT	1
#define SIREN_24V_OUT_BIT	2
#define NET_24V_OUT_BIT	3
#define NET_220V_OUT_BIT 4
#define LH_220V_OUT_BIT 5
 
#define BUZZER_OUT_ON()			*R_GPOHIGH(0) = (1<<7)
#define BUZZER_OUT_OFF()		*R_GPOLOW(0) = (1<<7)

#define LOW_LIGHT_220V_ON()		*R_GPOHIGH(0) = (1<<5)
#define LOW_LIGHT_220V_OFF()	*R_GPOLOW(0) = (1<<5)

#define NET_OUT_220V_ON()		*R_GPOHIGH(0) = (1<<4)
#define NET_OUT_220V_OFF()		*R_GPOLOW(0) = (1<<4)

#define NET_OUT_24V_ON()		*R_GPOHIGH(0) = (1<<3)
#define NET_OUT_24V_OFF()		*R_GPOLOW(0) = (1<<3)

#define SIREN_24V_HIGH()		*R_GPOHIGH(0) = (1<<2)
#define SIREN_24V_LOW()			*R_GPOLOW(0) = (1<<2)

#define LIGHT2_24V_HIGH()		*R_GPOHIGH(0) = (1<<1)
#define LIGHT2_24V_LOW()		*R_GPOLOW(0) = (1<<1)

#define LIGHT1_24V_HIGH()		*R_GPOHIGH(0) = (1<<0)
#define LIGHT1_24V_LOW()		*R_GPOLOW(0) = (1<<0)

//-------------------------------------------------------------------------------------------------
#define LED_CTL0 (1<<0)
#define LED_CTL1 (1<<1)			// SENS ON
#define LED_CTL2 (1<<2)
#define LED_CTL3 (1<<3)
#define LED_CTL4 (1<<4)			
#define LED_CTL5 (1<<5)			// WI-FI connect

#define LED_CONTROL_OFF(LED_DATA) 	*R_GPOHIGH(1) = LED_DATA
#define LED_CONTROL_ON(LED_DATA) 	*R_GPOLOW(1) = LED_DATA

#define CH2_24V_OUT_HIGH()		*R_GPOHIGH(1) = (1<<7)
#define CH2_24V_OUT_LOW()		*R_GPOLOW(1) = (1<<7)
#define CH1_24V_OUT_HIGH()		*R_GPOHIGH(1) = (1<<6)
#define CH1_24V_OUT_LOW()		*R_GPOLOW(1) = (1<<6)

//-------------------------------------------------------------------------------------------------
#define WIZNET_RST_HIGH()		*R_GPOHIGH(2) = (1<<0)
#define WIZNET_RST_LOW()		*R_GPOLOW(2) = (1<<0)

#define WIZNET_INT_PIN()		((*R_GPILEV(2) >>1) & 1)	//	GP2.1

//-------------------------------------------------------------------------------------------------

#define LiDAR_IN_PIN()			((*R_GPILEV(4) >>3) & 1)	//	GP4.3
#define SWITCH_IN_PIN()			((*R_GPILEV(4) >>1) & 1)	//	GP4.1
#define ALARM_SWITCH_PIN()		((*R_GPILEV(4) >>0) & 1)	//	GP4.0

/***************************************************************************************************************************
 *  P8.3(OUTPUT) : WiFi Reset | P8.2(INPUT) : SEN2_IN | P8.1(OUTPUT) : NOT USED | P8.0(INPUT) : SEN1_IN
 *  P8.7(OUTPUT) : NOT USED | P8.6(OUTPUT) : NOT USED | P8.5(OUTPUT) : NOT USED | P8.4(OUTPUT) : NOT USED
 ***************************************************************************************************************************/
#define WI_FI_RESET_HIGH()		*R_GPOHIGH(8) = (1<<3)
#define WI_FI_RESET_LOW()		*R_GPOLOW(8) = (1<<3)

#define SENS2_IN_PIN()		((*R_GPILEV(8) >>2) & 1)	//	GP8.2
#define SENS1_IN_PIN()		((*R_GPILEV(8) >>0) & 1)	//	GP8.0


//----------------------------------------------------------------
//	FLAGS
//----------------------------------------------------------------
#define SET_WIFI_AT_CMD(val)			pdc->wifi_mode_state1.b.bit0=val
#define GET_WIFI_AT_CMD()				pdc->wifi_mode_state1.b.bit0

#define SET_WIFI_RX_MSG(val)			pdc->wifi_mode_state1.b.bit1=val
#define GET_WIFI_RX_MSG()				pdc->wifi_mode_state1.b.bit1 
 
 #define SET_WIFI_TMOUT(val)			pdc->wifi_mode_state1.b.bit2=val
#define GET_WIFI_TMOUT()					pdc->wifi_mode_state1.b.bit2 
#if 0
#define SET_WIFI_CMD_ATE(val)			pdc->wifi_mode_state1.b.bit0=val
#define GET_WIFI_CMD_ATE()				pdc->wifi_mode_state1.b.bit0

#define SET_WIFI_CMD_COUNTRY(val)		pdc->wifi_mode_state1.b.bit1=val
#define GET_WIFI_CMD_COUNTRY()			pdc->wifi_mode_state1.b.bit1

#define SET_WIFI_CMD_WSOFTAP(val)		pdc->wifi_mode_state1.b.bit2=val
#define GET_WIFI_CMD_WSOFTAP()			pdc->wifi_mode_state1.b.bit2

#define SET_WIFI_CMD_WDHCPS(val)		pdc->wifi_mode_state1.b.bit3=val
#define GET_WIFI_CMD_WDHCPS()			pdc->wifi_mode_state1.b.bit3

#define SET_WIFI_CMD_WEVENT(val)		pdc->wifi_mode_state1.b.bit4=val
#define GET_WIFI_CMD_WEVENT()			pdc->wifi_mode_state1.b.bit4

#define SET_WIFI_CMD_SOPEN(val)			pdc->wifi_mode_state1.b.bit5=val
#define GET_WIFI_CMD_SOPEN()			pdc->wifi_mode_state1.b.bit5

#define SET_WIFI_CMD_SLIST(val)			pdc->wifi_mode_state1.b.bit6=val
#define GET_WIFI_CMD_SLIST()			pdc->wifi_mode_state1.b.bit6

#define SET_WIFI_CMD_SSEND(val)			pdc->wifi_mode_state1.b.bit7=val
#define GET_WIFI_CMD_SSEND()			pdc->wifi_mode_state1.b.bit7
#endif

 ///////////// 2025.01.02  flash write add /////////////////
#define MEM_INOF_STR	"CRANE_TERA"
 #define SYS_INFO_BASE_ADDR ((512 * 1024) - (4* 1024))
#define MEM_INFO_ADDR	SYS_INFO_BASE_ADDR
#define ETH_SEVER_PORT_ADDR (MEM_INFO_ADDR + 16)
#define ETH_SEVER_IP_ADDR (ETH_SEVER_PORT_ADDR + 4)
#define ETH_CLIENT_PORT_ADDR (ETH_SEVER_IP_ADDR + 8)
#define WIFI_SSID_ADDR (ETH_CLIENT_PORT_ADDR + 4)

#endif /* __UI_DEFINES_H */
 
