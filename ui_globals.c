
#include "sdk.h"

#include "ui_globals.h"
#include "NRC7394_wifi.h"
#include "w5500/dhcp_client.h"

device_control_t _pdc;
device_control_t *pdc;

wifi_sta_state_t	*wifi_ap_state;
debug_cmd_state_t	*debug_cmd_state;

crane_state_t	*crane_state[8];
wifi_connect_state_t	*wifi_connect_state[8];


//======================================================================
//	FACTORY RESET...
//======================================================================
/////////////////////////////////////////////////////////////////////////
//local function
//////////////////////////////////////////////////////////////////////////////
// W5500
// PORT NUM
// SEVER IP
// NRC7394
// SSID
// 

void device_control_attach(void)
{	
	int i;
	
	pdc = &_pdc;
	memset(pdc, 0, sizeof(device_control_t));

	// W5500 
	wiznet_state = (wiznet_state_t *)malloc(sizeof(wiznet_state_t));
	memset(wiznet_state, 0, sizeof(wiznet_state_t));
	wiznet_state->w5500_step = WIZNET_STEP_RESET;						// Wi Fi Module HW reset
		
	wifi_ap_state = (wifi_sta_state_t *)malloc(sizeof(wifi_sta_state_t));
	memset(wifi_ap_state, 0, sizeof(wifi_sta_state_t));
	
	for(i=0;i<8;i++) {
		crane_state[i] = (crane_state_t *)malloc(sizeof(crane_state_t));
		memset(crane_state[i], 0, sizeof(crane_state_t));
	}

	for(i=0;i<8;i++) {
		wifi_connect_state[i] = (wifi_connect_state_t *)malloc(sizeof(wifi_connect_state_t));
		memset(wifi_connect_state[i], 0, sizeof(wifi_connect_state_t));
	}


	debug_cmd_state = (debug_cmd_state_t *)malloc(sizeof(debug_cmd_state_t));
	memset(debug_cmd_state, 0, sizeof(debug_cmd_state_t));
	

//--------------------------------------------------------------------------------------------
	wifi_ap_state->at_cmd_step = WIFI_STEP_RESET;						// Wi Fi Module HW reset

	WiFi_AP_default_config();
//	W5500_default_config();
}


