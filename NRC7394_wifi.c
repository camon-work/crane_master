#include "sdk.h"

#include "ui_globals.h"
#include "user_task.h"
#include "NRC7394_wifi.h"



#define RDX_SKIP_CHAR1	0x0D	// CR \r
#define RDX_SKIP_CHAR2	0x22	// "


#define WIFI_RX_BUFF_SIZE	128
uint8_t wifi_rx_buffer[WIFI_RX_BUFF_SIZE];

#define WIFI_TOKEN_SIZE	128
uint8_t wifi_token_buff[WIFI_TOKEN_SIZE];
uint8_t wifi_token_pos=0;

#define DEBUG_CMD_SIZE	64
uint8_t debug_cmd_buff[DEBUG_CMD_SIZE];
uint8_t debug_cmd_pos=0;


uint8_t atcmd_token_buff[20][16];	// strlen : 16 , packet count : 20
uint8_t atcmd_token_pos=0;
char wifi_reset_step=0;



/*******************************************************************************************
 *	구분자 : ','   ':' '=' '\n'로 데이터 구분..최대 16개 로 구분
 *******************************************************************************************/
char wifi_data_strtok(char *s, uint8_t len)
{
	char ch;
	int i;
	int pos = 0;

	atcmd_token_pos = pos = 0;
	memset(atcmd_token_buff[atcmd_token_pos], 0, sizeof(atcmd_token_buff[atcmd_token_pos]));

	debugprintf("\r\n");
	debugprintf("rxc : %s\r\n", s);

	
	for(i=0;i<len;i++) {
		ch = *s++;
		if((ch==':')||(ch=='=')||(ch==',')||(ch=='\n')) {
			atcmd_token_buff[atcmd_token_pos++][pos] = 0x00;
			memset(atcmd_token_buff[atcmd_token_pos], 0, sizeof(atcmd_token_buff[atcmd_token_pos]));
			pos = 0;
			
		}
		else {
			atcmd_token_buff[atcmd_token_pos][pos++] = ch;
		}
	}

	return atcmd_token_pos;
}


/*******************************************************************************************
 *	지정된 구분자로 데이터 정리.
 *******************************************************************************************/
char *strtok_r(char *s, const char *delim, char **last)
{
	char *spanp;
	int c, sc;
	char *tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);
	
//------------------------------------------------------------------
//	 Skip (span) leading delimiters (s += strspn(s, delim), sort of).
 //------------------------------------------------------------------
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

//------------------------------------------------------------------
//	 Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
//	 Note that delim must have one NUL; we stop if we see that, too.
//------------------------------------------------------------------
	for (;;) {
		c = *s++;
		spanp = (char *)delim;

		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
//	 NOTREACHED 
}

char *g_strtok(char *s, const char *delim)
{
	static char *last;
	return strtok_r(s, delim, &last);
}

/********************************************************************************************************
 *	MAC ADDRESS 뒤 2 자리만 비교 한다..buffer에는 XXXXXXXXXXXX...BC:10:2F:AD:EA:33 -> BC102FADEA33
 ********************************************************************************************************/
uint8_t mac_address_duplication(const uint8_t *mac_address)
{
	uint8_t i;
	uint8_t result;


	result = FALSE;

	if(wifi_ap_state->net_info.mac_addr_count>0) {
		
		for(i=0;i<wifi_ap_state->net_info.mac_addr_count;i++) {
			if(strncmp((char *)wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count], (char *)mac_address, 17)==0) {
				result = TRUE;
				break;
			}
		}
	}

	return result;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//	데이터 송, 수신 처리...
//

void WiFi_AP_default_config(void) 
{
	strcpy((char *)wifi_ap_state->wifi_config.country_code, "K1");
	strcpy((char *)wifi_ap_state->wifi_config.ssid, "CRANE_1");
	
	strcpy((char *)wifi_ap_state->net_info.port_num, "5000");

	strcpy(wifi_ap_state->net_info.client_ip, "192.168.50.2");
	strcpy(wifi_ap_state->net_info.client_port, "5001");
	
	strcpy(wifi_ap_state->net_info.remote_ip, "192.168.0.173");
	strcpy(wifi_ap_state->net_info.remote_port, "5001");


	wifi_ap_state->wifi_config.wsoftap_freq = 921.5;
	wifi_ap_state->wifi_config.wsoftap_bandwidth = 1;

	
	
}

/*******************************************************************************************
 *	WI-FI AP at command HANDLER
 *******************************************************************************************/
void WiFi_AP_at_cmd_handler(void) 
{
	char str_buff[128] = {0,};

	if(wifi_ap_state->at_cmd_wait>0)				// AT COMMAND 이후 일정 시간이 필요 할 경우 사용...10ms 간격...
		return;

	if((wifi_ap_state->at_cmd_step>=WIFI_STEP_ATE)&&(GET_WIFI_AT_CMD()==CLR))
		return;

	switch(wifi_ap_state->at_cmd_step) {
		case WIFI_STEP_RESET:						// Wi Fi Module is hang....need to reset
			if(wifi_reset_step==0) {
				WIFI_RST_LOW();
				wifi_ap_state->at_cmd_wait = 5;		// 50ms delay
				wifi_reset_step++;
			}
			else if(wifi_reset_step==1) {
				WIFI_RST_HIGH();
				wifi_ap_state->at_cmd_wait = 10;	// 100ms delay	
				wifi_reset_step++;
			}
			else if(wifi_reset_step==2) {			// NEXT STEP NEED
				wifi_reset_step++;
				wifi_ap_state->at_cmd_wait = 200;	// 2초 동안 기다린다
			}
			else if(wifi_reset_step==3) {			// NEXT STEP NEED
				wifi_reset_step=0;
				wifi_ap_state->at_cmd_wait = 200;	// 2초 이후 다시 리셋을 한다.

			}

			break;

		case WIFI_STEP_ATE:
			
			uart_putstring(WIFI_UART_CH, "AT\n\r");
			
#ifdef _DEBUG_MSG_
			debugprintf("AT\r\n");
#endif
			break;

		case WIFI_STEP_COUNTRY:
			if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
				sprintf(str_buff, "AT+WCOUNTRY=\"%s\"\n\r", "K1");
				uart_putstring(WIFI_UART_CH, str_buff);

#ifdef _DEBUG_MSG_
				debugprintf(DEBUG_UART_CH, str_buff);
				debugprintf("\r\n");
#endif				
			}
			else {
				uart_putstring(WIFI_UART_CH, "AT+WCOUNTRY?\n\r");

#ifdef _DEBUG_MSG_
				debugprintf( "AT+WCOUNTRY?\r\n");
#endif			
			}
			break;

		case WIFI_STEP_WSOFTAP:
			if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
				sprintf(str_buff,"AT+WSOFTAP=%.01f@%d,\"%s\"\n\r", wifi_ap_state->wifi_config.wsoftap_freq,
					wifi_ap_state->wifi_config.wsoftap_bandwidth, wifi_ap_state->wifi_config.ssid);
				
				uart_putstring(WIFI_UART_CH, str_buff);
			}
			break;

		case WIFI_STEP_WDHCPS:
			if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
				uart_putstring(WIFI_UART_CH, "AT+WDHCPS\n\r");
			}
			break;

		case WIFI_STEP_WEVENT:
			break;

		case WIFI_STEP_SOPEN:
			if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
				sprintf(str_buff, "AT+SOPEN=\"UDP\",%s\n\r", wifi_ap_state->net_info.port_num);
				uart_putstring(WIFI_UART_CH, str_buff);				
			}
			break;
/*******************************************************************************************
 *	DATA SEND 를 위한 명령..
 *******************************************************************************************/
		case WIFI_STEP_S_AT:				// SSEND를 위한 AT COMMAND
			uart_putstring(WIFI_UART_CH, "AT\n\r");
			break;
		
		case WIFI_STEP_R_AT:				// error sleep일 경우 AT로 살린다..	
			uart_putstring(WIFI_UART_CH, "AT\n\r");
			break;

		case WIFI_STEP_SLIST:
			uart_putstring(WIFI_UART_CH, "AT+SLIST?\n\r");
			break;
		
		case WIFI_STEP_SSEND:

			sprintf(str_buff, "AT+SSEND=%s,\"%s\",%s,%d\n\r", wifi_ap_state->net_info.socket_num ,
					wifi_ap_state->net_info.client_ip, wifi_ap_state->net_info.client_port, 
					strlen(wifi_ap_state->send_packet)+1);

			uart_putstring(WIFI_UART_CH, str_buff);

#ifdef _DEBUG_CMD_MSG_			
			debugprintf("SSEND : ");
			debugprintf( str_buff);
			debugprintf("\r\n");
#endif			
			break;

		default:		
			break;
	}

	SET_WIFI_AT_CMD(CLR);
}


/*******************************************************************************************
 *	WI-FI AP at command HANDLER
 *******************************************************************************************/
void WiFi_AP_data_parser(void) 
{
	uint8_t dlen;
	uint8_t i, pos;
	uint8_t mac_address[24];
	

	if((wifi_ap_state->rxd_ready==ON) && (wifi_ap_state->rxd_tmout==0)) {
		wifi_ap_state->rxd_ready = OFF;

		if((wifi_token_pos>0) && (wifi_token_buff[wifi_token_pos-1]!='\n')) {	// +RDX는 \n이 없기 때문에 추가 한다..
			wifi_token_buff[wifi_token_pos++] = '\n';
		}

		dlen = wifi_data_strtok((char *)wifi_token_buff, wifi_token_pos);
		if(dlen==0)						// 귝격에 맞는데이터가 없으면...
			return;

		wifi_token_pos=0;
		memset(wifi_token_buff, 0, sizeof(wifi_token_buff));
		
		switch(wifi_ap_state->at_cmd_step) {
			case WIFI_STEP_RESET:
				if(strncmp(atcmd_token_buff[0], "+BOOT", 5)==0) {			// BOOT OK
					wifi_ap_state->at_cmd_step = WIFI_STEP_ATE;
					wifi_ap_state->at_cmd_tmout = 100;	// 10ms * 100 = 1000ms...1초 동안 응답이 없으면 다시 전송 한다..
					SET_WIFI_AT_CMD(ON);
				}
				break;

// AP MODE : #1 		
			case WIFI_STEP_ATE:
				if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {			// ATE OK
					wifi_ap_state->at_cmd_error = 0;
					
					wifi_ap_state->at_cmd_step = WIFI_STEP_COUNTRY;		// COUNTRY data get
					wifi_ap_state->at_cmd_wait = 10;				
					wifi_ap_state->at_cmd_type = AT_CMD_TYPE_GET;
					SET_WIFI_AT_CMD(ON);
				}
				else if(strncmp(atcmd_token_buff[0], "ERROR", 5)==0) {	// ATE ERROR
					wifi_ap_state->at_cmd_step = WIFI_STEP_ATE;
					wifi_ap_state->at_cmd_wait = 10;	// 10ms * 10 = 100ms...100ms 이후 명령 재 전송...
					wifi_ap_state->at_cmd_error++;		// 3회 이상 응답이 없으면 처음 부터 다시..
					SET_WIFI_AT_CMD(ON);
				}
				break;

			case WIFI_STEP_COUNTRY:
				
				if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_GET) {
					if(strncmp(atcmd_token_buff[0], "+WCOUNTRY", 9)==0) {			// COUNTRY ok
						if(strncmp(atcmd_token_buff[1], "K1", 2)==0) {			// COUNTRY ok
							wifi_ap_state->at_cmd_step = WIFI_STEP_WSOFTAP; 	// WSOFTAP CODE SET
							wifi_ap_state->at_cmd_wait = 10;					// 100ms delay
							wifi_ap_state->at_cmd_type = AT_CMD_TYPE_SET;
							SET_WIFI_AT_CMD(ON);
						}
						else {
							wifi_ap_state->at_cmd_step = WIFI_STEP_COUNTRY; 	// COUNTRY CODE SET
							wifi_ap_state->at_cmd_wait = 10;					// 100ms delay
							wifi_ap_state->at_cmd_type = AT_CMD_TYPE_SET;
							SET_WIFI_AT_CMD(ON);
						}
					}
				}
				else {
					if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {			// SET COMMAND OK
						wifi_ap_state->at_cmd_step = WIFI_STEP_COUNTRY;		// COUNTRY data get
						wifi_ap_state->at_cmd_wait = 10;				
						wifi_ap_state->at_cmd_type = AT_CMD_TYPE_GET;
						SET_WIFI_AT_CMD(ON);
					}

				}
				break;

			case WIFI_STEP_WSOFTAP:
				if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
					if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {			// SET COMMAND OK
						wifi_ap_state->at_cmd_step = WIFI_STEP_WDHCPS;		//DHCP
						wifi_ap_state->at_cmd_wait = 10;				
						wifi_ap_state->at_cmd_type = AT_CMD_TYPE_SET;
						SET_WIFI_AT_CMD(ON);
					}

				}
				break;

			case WIFI_STEP_WDHCPS:				//+WDHCPS:192.168.50.1,255.255.255.0,192.168.50.1
				if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
					if(strncmp(atcmd_token_buff[0], "+WDHCPS", 7)==0) {			// SET COMMAND OK

						memset(wifi_ap_state->net_info.ip_addr, 0, sizeof(wifi_ap_state->net_info.ip_addr));
						strcpy(wifi_ap_state->net_info.ip_addr, atcmd_token_buff[1]);

						memset(wifi_ap_state->net_info.sub_mask, 0, sizeof(wifi_ap_state->net_info.sub_mask));
						strcpy(wifi_ap_state->net_info.sub_mask, atcmd_token_buff[2]);

						memset(wifi_ap_state->net_info.gateway, 0, sizeof(wifi_ap_state->net_info.gateway));
						strcpy(wifi_ap_state->net_info.gateway, atcmd_token_buff[3]);

						
						if(strncmp(atcmd_token_buff[4], "OK", 2)==0) { 		// SET COMMAND OK

							wifi_ap_state->at_cmd_step = WIFI_STEP_WEVENT;	// STA 연결울 기다린다...
						}						
					}
				}
				break;

// AP MODE : #3			
			case WIFI_STEP_WEVENT:
				if(strncmp(atcmd_token_buff[0], "+WEVENT", 7)==0) { 		// SET COMMAND OK
					if(strncmp(atcmd_token_buff[1], "STA_CONNECT", strlen("STA_CONNECT"))==0) { 	

						memset(mac_address, 0, sizeof(mac_address));
						pos = 0;
						for(i=0;i<6;i++) {
							pos = i*3;
							strncpy(&mac_address[pos], atcmd_token_buff[i+2], 2);
							if(i<5) mac_address[pos+2] = ':';
						}

#ifdef _DEBUG_MSG_
						debugprintf( "%s\r\n", mac_address);
#endif
						
						if(mac_address_duplication((uint8_t *)mac_address)==FALSE) {	// 중복이 없는 경우에만 MAC 주소를 저장 한다.
							memset(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count], 0, sizeof(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count]));
							strcpy(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count], mac_address);
#ifdef _DEBUG_MSG_
							debugprintf( wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count]);
							debugprintf( "mac address duplication false\r\n");
#endif							

							if(wifi_ap_state->net_info.mac_addr_count<6) {			// MAX 7개 까지 가능 
								wifi_ap_state->net_info.mac_addr_count++;
							}
						}
#ifdef _DEBUG_MSG_
					 else {
							debugprintf( "mac address duplication true\r\n");
					 	}
#endif
						wifi_ap_state->at_cmd_step = WIFI_STEP_SOPEN;	
						wifi_ap_state->at_cmd_wait = 10;				
						wifi_ap_state->at_cmd_type = AT_CMD_TYPE_SET;
						SET_WIFI_AT_CMD(ON);
					}
				}
				
				break;

// AP MODE : #4
			case WIFI_STEP_SOPEN:
				if(wifi_ap_state->at_cmd_type == AT_CMD_TYPE_SET) {
					if(strncmp(atcmd_token_buff[0], "+SOPEN", strlen("+SOPEN"))==0) { 	
						memset(wifi_ap_state->net_info.socket_num, 0, sizeof(wifi_ap_state->net_info.socket_num));
						strcpy(wifi_ap_state->net_info.socket_num, atcmd_token_buff[1]);
					
						if(strncmp(atcmd_token_buff[2], "OK", 2)==0) {		
							wifi_ap_state->at_cmd_step = WIFI_STEP_SREAD;	// 수신 준비...

							debugprintf( "go to WIFI_STEP_SREAD\r\n");
							wifi_ap_state->connect = CONNECT;
							wifi_ap_state->sat_device_num = 0;				// 
							wifi_ap_state->sat_device_sel = 0;				// 
							wifi_ap_state->sat_device_rxd_state = DATA_READY;
							wifi_ap_state->sat_device_tmout = 10;			// 1초 뒤 부터 상태 정보를 읽어 온다..

							
							led_control_on(LED5_CTRL, 0, 0);
						}
					}
				}
				break;

// AP MODE : #8
			case WIFI_STEP_S_AT:				// SSEND를 위한 AT COMMAND		
				if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {			// ATE OK
					wifi_ap_state->at_cmd_step = WIFI_STEP_SLIST;	
					SET_WIFI_AT_CMD(ON);
				}
				break;

			case WIFI_STEP_R_AT:									// SSEND를 위한 AT COMMAND		
				if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {			// ATE OK
					wifi_ap_state->at_cmd_step = WIFI_STEP_SREAD;	
				}
				break;
				
			case WIFI_STEP_SLIST:
				if(strncmp(atcmd_token_buff[0], "+SLIST", 6)==0) {	
					memset(wifi_ap_state->net_info.socket_num, 0, sizeof(wifi_ap_state->net_info.socket_num));
					strcpy(wifi_ap_state->net_info.socket_num, atcmd_token_buff[1]);
				
					memset(wifi_ap_state->net_info.port_num, 0, sizeof(wifi_ap_state->net_info.port_num));
					strcpy(wifi_ap_state->net_info.port_num, atcmd_token_buff[5]);
					
					wifi_ap_state->at_cmd_step = WIFI_STEP_SSEND;	
					
					SET_WIFI_AT_CMD(ON);
				}
				
				break;
			
			case WIFI_STEP_SSEND:
				if(strncmp(atcmd_token_buff[0], "OK", 2)==0) {		
					uart_putstring(WIFI_UART_CH, wifi_ap_state->send_packet);
					
					wifi_ap_state->at_cmd_step = WIFI_STEP_SREAD;				// data 수신 준비 중...
				}
				break;

/**************************************************************************************************************
 *	"+RXD:1,10\n123456789\n"  중간에 \n 이 들어 가고 그것에 따른 길이는 10이 된다..결국 데이터는
 *	\n를 제외한 123456789 가 실제 데이터가 된다.atcmd_token_buff[1] <- socket, atcmd_token_buff[2] <- 데이터 개수..
 **************************************************************************************************************/
			case WIFI_STEP_SREAD:			
				if(strncmp(atcmd_token_buff[0], "+RXD", 4)==0) {	
					WiFi_AP_data_sread_parser(wifi_ap_state->sat_device_sel);
				}

				if(strncmp(atcmd_token_buff[0], "+EVENT", strlen("+EVENT"))==0) {	
					if(strncmp(atcmd_token_buff[1], "STA_CONNECT", strlen("STA_CONNECT"))==0) { 	
						memset(mac_address, 0, sizeof(mac_address));
						pos = 0;
						for(i=0;i<6;i++) {				// xx:xx:xx:xx:xx:xx 변환..
							pos = i*3;
							strncpy(&mac_address[pos], atcmd_token_buff[i+2], 2);
							if(i<5) mac_address[pos+2] = ':';
						}

						if(mac_address_duplication(mac_address)==FALSE) {	// 중복이 없는 경우에만 MAC 주소를 저장 한다.
							memset(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count], 0, sizeof(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count]));
							strcpy(wifi_ap_state->net_info.mac_addr[wifi_ap_state->net_info.mac_addr_count], mac_address);

							if(wifi_ap_state->net_info.mac_addr_count<6) {			// MAX 7개 까지 가능 
								wifi_ap_state->net_info.mac_addr_count++;
							}
						}
					}				
				}

				break;

			default:		
				break;
		}

//----------------------------------------------------------------------------------------------------
//	동작 중 
//----------------------------------------------------------------------------------------------------
		if(wifi_ap_state->connect == CONNECT) {	
			if(strncmp(atcmd_token_buff[0], "+RXD", 4)==0) {	
				WiFi_AP_data_sread_parser(wifi_ap_state->sat_device_sel);
				
				SET_WIFI_RX_MSG(ON);
				pdc->wi_fi_request_count = CLR;					// 요구에 대한 수신이 완료 되면...COUNT CLEAR..
			}
		}		
	}
}

/*******************************************************************************************
 *	통신 규격에 맞게 데이터를 버퍼를 구성 한다...device idx에 맞게 데이터를 전송 한다.
 *******************************************************************************************/
void WiFi_AP_data_payload(uint8_t device_idx)
{
	uint8_t mac_address[8];
	
	wifi_ap_state->at_cmd_step = WIFI_STEP_S_AT;	
	SET_WIFI_AT_CMD(ON);

	memset(mac_address, 0, sizeof(mac_address));
	memcpy(&mac_address[0], &wifi_ap_state->net_info.mac_addr[device_idx][12], 2);
	memcpy(&mac_address[2], &wifi_ap_state->net_info.mac_addr[device_idx][15], 2);

	memset(wifi_ap_state->send_packet, 0, sizeof(wifi_ap_state->send_packet));
	sprintf(wifi_ap_state->send_packet, "%s,READ,0,0", mac_address);	
}

/***********************************************************************************************************************************
 *	연결이 된 상태에서 250ms 간격으로 SAT 데이터 상태를 읽어 온다...단 수신이 되면 다음 디바이스 정보를 읽어 온다.
 *	DATA READ TIMEOUT is 100ms
 ***********************************************************************************************************************************/
void WiFi_AP_data_read_to_sat_device(void)
{
	if(wifi_ap_state->connect == CONNECT) {
		if((wifi_ap_state->sat_device_tmout == 0) || (wifi_ap_state->sat_device_rxd_state == DATA_READ_OK)) {
			wifi_ap_state->sat_device_rxd_state = DATA_SEND;
			wifi_ap_state->sat_device_tmout = 10;									// 100ms x 10 = 1sec tmout
			wifi_ap_state->sat_device_sel = wifi_ap_state->sat_device_num;			// READ 번호를 기억 한다..	
			
			WiFi_AP_data_payload(wifi_ap_state->sat_device_num);					// data request...
			wifi_connect_state[wifi_ap_state->sat_device_num]->req_cmd_count++;		

			debugprintf(" sat device : %d %d\r\n", wifi_ap_state->sat_device_num, wifi_connect_state[wifi_ap_state->sat_device_num]->req_cmd_count);

			wifi_ap_state->sat_device_num++;
			if(wifi_ap_state->sat_device_num>=wifi_ap_state->net_info.mac_addr_count) 
				wifi_ap_state->sat_device_num=0;


			pdc->wi_fi_request_count++;											// 25.01.01 ...add for wi-fi disconnect
		}
	}
}


/*******************************************************************************************
 *	M2M ...AT COMMAND 데이터 수신 하는 동안 사용 한다...ASCII 형식 AP <--> MCP
 *******************************************************************************************/
void WiFi_AP_data_receive(void)
{
	int i;
	int rx_len;

	memset(wifi_rx_buffer, 0, sizeof(wifi_rx_buffer));
	rx_len = uart_getdata(WIFI_UART_CH, wifi_rx_buffer , WIFI_RX_BUFF_SIZE);

	if(rx_len>0) {
		for(i=0;i<rx_len;i++) {
			if((wifi_rx_buffer[i]!=RDX_SKIP_CHAR1)&& (wifi_rx_buffer[i]!=RDX_SKIP_CHAR2)) {
				wifi_token_buff[wifi_token_pos++] = wifi_rx_buffer[i];
				wifi_token_pos%=WIFI_TOKEN_SIZE;
			
				wifi_ap_state->rxd_ready = ON;
				wifi_ap_state->rxd_tmout = 10;		// 100ms 동안 데이터가 없으면 다 들어 온것으로 인식 한다...
				
			}
		}
	}
}

/******************************************************************************************************************************
 *	WiFi SREAD Data Parser
 *	RXD:1,20,mac id, status,senser1,senser2,self,aux1 in, aux2 in, aux1 out,aux2 out,exp1,exp2,exp3,exp4
*********************************** *******************************************************************************************/
void WiFi_AP_data_sread_parser(uint8_t device_idx)
{
	uint8_t mac_address[8];
	uint8_t pos = 0;

	wifi_connect_state[device_idx]->connection = WI_FI_CONNECT_OK;	// TIME CLEAR
	wifi_connect_state[device_idx]->req_cmd_count = 0;

	
	memset(mac_address, 0, sizeof(mac_address));
	memcpy(&mac_address[0], &wifi_ap_state->net_info.mac_addr[device_idx][12], 2);	// xx:
	memcpy(&mac_address[2], &wifi_ap_state->net_info.mac_addr[device_idx][15], 2);	// xx	

		
	pos = 3;
	if(strncmp((char *)atcmd_token_buff[pos++], (char *)mac_address, 4)==0) {		// xx:xx		3
		if(strncmp((char *)atcmd_token_buff[pos++], "OK", 2)==0) {					// 4
		
		
			crane_state[device_idx]->sensor1_in = atoi(atcmd_token_buff[pos++]);		// 5
			crane_state[device_idx]->sensor2_in = atoi(atcmd_token_buff[pos++]);			// 6
			crane_state[device_idx]->self_test_sw = atoi(atcmd_token_buff[pos++]);		// 7
			crane_state[device_idx]->aux1_in_sw = atoi(atcmd_token_buff[pos++]);			// 8
			crane_state[device_idx]->aux2_in_sw = atoi(atcmd_token_buff[pos++]);			// 9
			crane_state[device_idx]->aux2_out = atoi(atcmd_token_buff[pos++]);			// 11
			crane_state[device_idx]->aux1_out = atoi(atcmd_token_buff[pos++]);			// 10
			crane_state[device_idx]->exp1 = atoi(atcmd_token_buff[pos++]);				// 12
			crane_state[device_idx]->exp2 = atoi(atcmd_token_buff[pos++]);				// 13
			crane_state[device_idx]->exp3 = atoi(atcmd_token_buff[pos++]);				// 14
			crane_state[device_idx]->exp4 = atoi(atcmd_token_buff[pos++]);				// 15

			wifi_ap_state->sat_device_rxd_state = DATA_READ_OK;
			
			// data get --> then action  
			
			//SENS_CRANE_ON = crane_state[0]->sensor2_in;	
			
			
			
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
 *	DEBUG를 이용한 command 
 *******************************************************************************************/
void debug_command_receive(void)
{
	int i;
	int rx_len;
	uint8_t dlen;
	uint8_t cmd_id;
	uint8_t msg_id;

	memset(wifi_rx_buffer, 0, sizeof(wifi_rx_buffer));
	rx_len = uart_getdata(DEBUG_UART_CH, wifi_rx_buffer , WIFI_RX_BUFF_SIZE);

	if(rx_len>0) {
		for(i=0;i<rx_len;i++) {
			debug_cmd_buff[debug_cmd_pos++] = wifi_rx_buffer[i];
			debug_cmd_pos%=DEBUG_CMD_SIZE;
		
			debug_cmd_state->rxd_ready = ON;
			debug_cmd_state->rxd_tmout = 10;		// 100ms 동안 데이터가 없으면 다 들어 온것으로 인식 한다...
		}
	}



	if((debug_cmd_state->rxd_ready==ON) && (debug_cmd_state->rxd_tmout==0)) {
		debug_cmd_state->rxd_ready = OFF;

		dlen = wifi_data_strtok((char *)debug_cmd_buff, debug_cmd_pos);

		if(strncmp((char *)atcmd_token_buff[0], "DEBUG_CMD", strlen("DEBUG_CMD"))==0) {
			cmd_id = atcmd_token_buff[1][0];	
			msg_id = atcmd_token_buff[2][0];	

 			memset(debug_cmd_buff, 0, sizeof(debug_cmd_buff));
			debug_cmd_pos = 0;

			
			switch(cmd_id) {
				case 'A':
				case 'a':
					break;

				case 'B':
				case 'b':
					uart_putstring(WIFI_UART_CH, "AT+WPING?\n\r");
					break;

				case 'C':
				case 'c':
					led_control_on(LED0_CTRL, 5, 5);
					led_control_on(LED1_CTRL, 1, 9);
					led_control_on(LED2_CTRL, 2, 8);
					led_control_on(LED3_CTRL, 3, 7);
					led_control_on(LED4_CTRL, 7, 3);
					led_control_on(LED5_CTRL, 9, 1);
					break;

				case 'D':
				case 'd':
					
					break;
					
				case 'E':
				case 'e':
				
					break;
			}

		}

/**************************************************************************************************************************
 *	나중에 다른 시리얼 포트로 이동을 한다...	
 **************************************************************************************************************************/

		// W5500 IP & PORT CONFIG
		else if(strncmp((char *)atcmd_token_buff[0], "MASTER_CONFIG", strlen("MASTER_CONFIG"))==0) {

			memset(wifi_ap_state->wifi_config.ssid, 0, sizeof(wifi_ap_state->wifi_config.ssid));
			memset(wifi_ap_state->net_info.remote_ip, 0, sizeof(wifi_ap_state->net_info.remote_ip));
			memset(wifi_ap_state->net_info.remote_port, 0, sizeof(wifi_ap_state->net_info.remote_port));

			strcpy(wifi_ap_state->wifi_config.ssid, (char *)atcmd_token_buff[1]);
			strcpy(wifi_ap_state->net_info.remote_ip, (char *)atcmd_token_buff[2]);
			strcpy(wifi_ap_state->net_info.remote_port, (char *)atcmd_token_buff[3]);
			
			debugprintf( "IP PORT : %s, %s\r\n", wifi_ap_state->net_info.remote_ip, wifi_ap_state->net_info.remote_port);

			Ethernet_string_to_num();

			user_data_mem_write();

		}
		else if(strncmp((char *)atcmd_token_buff[0], "W5500_CONFIG", strlen("W5500_CONFIG"))==0) {

			memset(wifi_ap_state->net_info.remote_ip, 0, sizeof(wifi_ap_state->net_info.remote_ip));
			memset(wifi_ap_state->net_info.remote_port, 0, sizeof(wifi_ap_state->net_info.remote_port));

			strcpy(wifi_ap_state->net_info.remote_ip, (char *)atcmd_token_buff[1]);
			strcpy(wifi_ap_state->net_info.remote_port, (char *)atcmd_token_buff[2]);
			
			debugprintf( "IP PORT : %s, %s\r\n", wifi_ap_state->net_info.remote_ip, wifi_ap_state->net_info.remote_port);

			Ethernet_string_to_num();

			user_data_mem_write();
		}

	// NRC7394 SSID CONFIG
		else if(strncmp((char *)atcmd_token_buff[0], "NRC7394_CONFIG", strlen("NRC7394_CONFIG"))==0) {	
			memset(wifi_ap_state->wifi_config.ssid, 0, sizeof(wifi_ap_state->wifi_config.ssid));
			strcpy(wifi_ap_state->wifi_config.ssid, (char *)atcmd_token_buff[1]);

			debugprintf( "SSID : %s\r\n", wifi_ap_state->wifi_config.ssid);

			user_data_mem_write();
		}
		else if(strncmp((char *)atcmd_token_buff[0], "CONFIG_READ", strlen("CONFIG_READ"))==0) {
		
			debugprintf( "$CONFIG_READ,%s,%s,%s\r\n", wifi_ap_state->wifi_config.ssid, wifi_ap_state->net_info.remote_ip, wifi_ap_state->net_info.remote_port);
		
		}
		
		memset(debug_cmd_buff, 0, sizeof(debug_cmd_buff));
		debug_cmd_pos = 0;
	}


}






