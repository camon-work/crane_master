#ifndef __NRC7394_WIFI_H
#define __NRC7394_WIFI_H

#define _DEBUG_WIFI
#define _DEF_SERVER

#define WIFI_UART_CH	5
#define WIFI_RST_PORT_NUM	8
#define WIFI_RST_PORT_BIT 3
#define WIFI_RST_HIGH() *R_GPOHIGH(WIFI_RST_PORT_NUM) = (1 << WIFI_RST_PORT_BIT)
#define WIFI_RST_LOW() *R_GPOLOW(WIFI_RST_PORT_NUM) = (1 << WIFI_RST_PORT_BIT)

#define MAKE_IPV4_ADDRESS(a, b, c, d) ((((U32) d) <<24) | (((U32)c) <<16)  | (((U32)b)<<8) |((U32)a) )

#define AT_CMD_TYPE_GET		0
#define AT_CMD_TYPE_SET		1

typedef enum {
	NRC7394_WUART=0,				// UART baudrate.
	NRC7394_WMACADDR,				// MAC address.
	NRC7394_WCOUNTRY,				// Configure the Wi-Fi country code
	NRC7394_WTXPOWER,				// Set the transmission power level.
	NRC7394_WRXSIG,					// Fetch or monitor the RSSI (dBm) and SNR (dB) values.
	NRC7394_WRATECTRL,			// Toggle the MCS rate control option.
	NRC7394_WMCS,						// Set the MCS index.
	NRC7394_WDUTYCYCLE,			// Configure duty cycle operation.
	NRC7394_WCCATHRESHOLD,	// Set CCA threshold.
	NRC7394_WTXTIME,					// Set carrier sense time and pause time.
	NRC7394_WTSF,						// Read the elapsed TSF timer duration.
	NRC7394_WBI,						// Get the beacon interval of the connected AP in STA mode.
	NRC7394_WLI,						// Set the listen interval in STA mode.
	NRC7394_WSCAN,					// Perform Wi-Fi scanning.
	NRC7394_WSCANSSID,			// Perform Wi-Fi scanning with probe request frames that specify full SSID.
	NRC7394_WCONN,					// Connect to a new AP.
	NRC7394_WDISCONN,				// Disconnect from the AP or abort an on-going connection process.
	NRC7394_WSOFTAP,				// Run as the AP mode.
	NRC7394_WSOFTAPSSID,		// Set how to specify the SSID in the beacon frame.
	NRC7394_WBSSMAXIDLE,		// Configure the BSS Max idle service for SoftAP.
	NRC7394_WSTAINFO,				// Get information of associated STAs on AP mode.
	NRC7394_WMAXSTA,				// Set the maximum number of STAs allowed in AP mode.
	NRC7394_WIPADDR,				// Configure the IPv4 address.
	NRC7394_WDNS,						// Configure the IP address for the DNS server.
	NRC7394_WDHCP						// Request dynamic IP allocation from the DHCP server.
} nrc7394_at_cmd_e;

enum {
	WIFI_MODE_NONE=0,		
	WIFI_MODE_CMD,						// AT COMMAND STATE
	WIFI_MODE_DATA,						// DATA RECEIVE MODE
	WIFI_MODE_MAX
};

typedef enum {
	WIFI_STEP_NONE=0,
	WIFI_STEP_RESET,
	WIFI_STEP_ATE,
	WIFI_STEP_COUNTRY,	
	WIFI_STEP_WSOFTAP,	
	WIFI_STEP_WDHCPS,	
	WIFI_STEP_CONNECTING,	
	WIFI_STEP_WEVENT,	
	WIFI_STEP_SOPEN,	
	WIFI_STEP_S_AT,	
	WIFI_STEP_R_AT,	
	WIFI_STEP_SLIST,	
	WIFI_STEP_SSEND,						// WRITE DATA SEND
	WIFI_STEP_SREAD							// WAITING FOR DATA READ 
}wifi_st_cmd_e;

typedef enum{
	RESULT_OK = 0,
	RESULT_COMPLITE,
	RESULT_TIME_OUT,
	RESULT_WRONG_DATA,
	RESULT_MEM_OVER,
}return_result_e;


typedef struct {
	uint8_t country_code[4];
	uint8_t ssid[16] ;
	
	float wsoftap_freq;
	uint8_t wsoftap_bandwidth ;

} wifi_config_t;


typedef struct {
	uint8_t mac_addr[8][20];	// xx:xx:xx:xx:xx:xx... �ִ� 8�� ����
	uint8_t ip_addr[16];		// xxx.xxx.xxx.xxx
	uint8_t sub_mask[16];		// xxx.xxx.xxx.xxx
	uint8_t gateway[16];		// xxx.xxx.xxx.xxx
	uint8_t port_num[8] ;
	uint8_t socket_num[4] ;

	uint8_t mac_addr_count;

	uint8_t client_socket[4] ;
	uint8_t client_ip[16] ;
	uint8_t client_port[8] ;
	uint8_t remote_ip[16] ;		// ETHERNET IP
	uint8_t remote_port[8] ;	// ETHERNET PORT
} net_info_t;

typedef struct {
	uint8_t at_cmd_step;
	uint8_t at_cmd_mode;		
	uint8_t at_cmd_tmout;		
	uint8_t at_cmd_wait;		
	
	uint8_t at_cmd_error;		
	uint8_t at_cmd_type;	
	uint8_t dummy1;		
	uint8_t dummy2;	
	
	uint8_t rxd_ready;		
	uint8_t rxd_tmout;		
	uint8_t rxd_len;
	uint8_t connect;	
	
	uint8_t sat_device_num;					// connected sat devic id
	uint8_t sat_device_sel;					// send device idx 
	uint8_t sat_device_rxd_state;			// 디바이스 수신 상태 확인..
	uint8_t sat_device_tmout;				// 디바이스 수신 대기 시간...
	
	uint8_t read_packet[64];				// rx & tx data packet
	uint8_t send_packet[64];				// rx & tx data packet
	

	wifi_config_t	wifi_config;
	net_info_t		net_info;
	

} wifi_sta_state_t;

extern wifi_sta_state_t	*wifi_ap_state;



extern void WiFi_AP_default_config(void) ;
extern void WiFi_AP_at_cmd_handler(void) ;
extern void WiFi_AP_data_parser(void) ;
extern void WiFi_AP_data_payload(uint8_t device_idx);
extern void WiFi_AP_data_receive(void);
extern void WiFi_AP_data_sread_parser(uint8_t device_idx);
extern void WiFi_AP_data_read_to_sat_device(void);

/********************************************************************************************************
 *	DEBUG TEST 
 ********************************************************************************************************/
typedef struct {
	
	uint8_t rxd_ready;		
	uint8_t rxd_tmout;		
	uint8_t rxd_len;
	uint8_t connect;	
	
	

} debug_cmd_state_t;

extern debug_cmd_state_t	*debug_cmd_state;

extern void debug_command_receive(void);



#endif
 
