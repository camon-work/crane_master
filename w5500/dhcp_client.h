#pragma once

typedef enum {
	WIZNET_STEP_NONE = 0,
	WIZNET_STEP_RESET,
	WIZNET_STEP_CHIP_INIT,
	WIZNET_STEP_NET_CONF,
	WIZNET_STEP_DHCIP_INIT,
	WIZNET_SETEP_PHY_LINK,
}wizet_st_cmd_e;

typedef struct {
	bool eth_connected;
	uint8_t w5500_step;
	uint32_t wait_time;
	// phy
	uint32_t phy_link_time;
	bool phy_check_en;
	bool phy_check_flag;
	uint32_t dhcp_10m_time;
	
} wiznet_state_t;

extern wiznet_state_t *wiznet_state;
 
extern void W5500_handler(void);
 
