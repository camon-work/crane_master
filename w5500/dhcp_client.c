/*
===============================================================================
 Name        : main.c
 Author      : WIZnet
 Version     :
 Copyright   : WIZnet Co., Ltd. 2014
 Description : main definition
===============================================================================
*/

/*
 * @brief DHCP client example for W5500 EVB
 *
 * @note
 * Copyright(C) WIZnet Co., Ltd. 2014
 * All rights reserved.
 *
 */
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

//#include <cr_section_macros.h>

// TODO: insert other include files here
#include "sdk.h"
#include "w5500_spi.h"
#include "Ethernet/wizchip_conf.h"
#include "Internet/DHCP/dhcp.h"
#include "dhcp_client.h"
#include "loopback.h"
#include "../ui_globals.h"

// TODO: insert other definitions and declarations here

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ1 (1000)		/* 1000 ticks per second, for SysTick */
#define TICKRATE_HZ2 (1)		/* 1 ticks per second, for Timer0 */
volatile uint32_t msTicks; 		/* counts 1ms timeTicks */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
///////////////////////////////////////
// Debugging Message Printout enable //
///////////////////////////////////////
#define _MAIN_DEBUG_

///////////////////////////
// Demo Firmware Version //
///////////////////////////
#define VER_H		1
#define VER_L		00

//////////////////////////////////////////////////
// Socket & Port number definition for Examples //
//////////////////////////////////////////////////
#define SOCK_DHCP       6

#define SOCK_TCPS       0
#define PORT_TCPS		5000

////////////////////////////////////////////////
// Shared Buffer Definition for Loopback test //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];

///////////////////////////
// Network Configuration //
///////////////////////////
// wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            // .ip = {192, 168, 1, 2},
                            // .sn = {255, 255, 255, 0},
                            // .gw = {192, 168, 1, 1},
                            // .dns = {0, 0, 0, 0},
                            // .dhcp = NETINFO_DHCP };

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                      .ip = {0, 0, 0, 0},
                      .sn = {255, 255, 255, 0},
                      .gw = {0, 0, 0, 0},
                      .dns = {0, 0, 0, 0},
                      .dhcp = NETINFO_DHCP 
};

/////////////////////
// PHYStatus check //
/////////////////////
#define SEC_PHYSTATUS_CHECK 		1		// sec
bool PHYStatus_check_enable = false;
bool PHYStatus_check_flag = true;

/////////////////////
// RGB LED Control //
/////////////////////
static bool RGBLED_enable = true;

////////////////
// DHCP client//
////////////////
#define MY_MAX_DHCP_RETRY			2
uint8_t my_dhcp_retry = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void Net_Conf();
static void Display_Net_Conf();
static void PHYStatus_Check(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
void SysTick_Handler(void);
void Timer32_0_Init(uint32_t tickrate);
void TIMER32_0_IRQHandler(void);


// Callback function : User defined DHCP functions
void my_ip_assign(void);
void my_ip_conflict(void);

wiznet_state_t *wiznet_state;

/**
 * @brief    Main routine for W5500 EVB firmware
 * @return   Function should not exit.
 */
int maindddd(void) {


    bool run_user_applications = false;
    int32_t ret;

    /* Network Initialization */
	SPI_Init();
	W5500_Init();
	Net_Conf();

	/* Enable and setup SysTick Timer at a periodic rate */
////////////////////////////	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* Enable and setup 32-bit Timer 0 */
//////////////////////////////////	Timer32_0_Init(TICKRATE_HZ2);

	/* PHY Status check enable */
	PHYStatus_check_enable = true;

#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};

	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    debugprintf("\r\n=======================================\r\n");
	debugprintf(" WIZnet %s EVB - DHCP client v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	debugprintf("=======================================\r\n");

	//Display_Net_Conf(); // handled by dhcp_run function
#endif

	/* DHCP client Initialization */
	if(gWIZNETINFO.dhcp == NETINFO_DHCP)
	{
		DHCP_init(SOCK_DHCP, gDATABUF);
		// if you want different action instead default ip assign, update, conflict.
		// if cbfunc == 0, act as default.
		reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);

		run_user_applications = false; 	// flag for running user's code
	}
	else
	{
		// Static
#ifdef _MAIN_DEBUG_
		Display_Net_Conf();
#endif
		run_user_applications = true; 	// flag for running user's code
	}

	/* Main loop ***************************************/
	while(1)
	{
	   	/* PHY Status checker: Check every 'SEC_PHYSTATUS_CHECK' seconds */
		if(PHYStatus_check_flag)
		{
			PHYStatus_check_flag = false;
			PHYStatus_Check();
			PRINTLINE;
		}

    	/* DHCP */
		/* DHCP IP allocation and check the DHCP lease time (for IP renewal) */
    	if(gWIZNETINFO.dhcp == NETINFO_DHCP)
    	{
			// aa = DHCP_run();
			// debugprintf("\n\r aa= %x",aa);
			switch(DHCP_run())
			// switch(aa)
			{
				case DHCP_IP_ASSIGN:
				case DHCP_IP_CHANGED:
					/* If this block empty, act with default_ip_assign & default_ip_update */
					//
					// This example calls my_ip_assign in the two case.
					//
					// Add to ...
					//
					//	PRINTLINE;
					break;
				case DHCP_IP_LEASED:
					//
					// TODO: insert user's code here
					run_user_applications = true;
					//
				//	PRINTLINE;
					break;
				case DHCP_FAILED:
					/* ===== Example pseudo code =====  */
					// The below code can be replaced your code or omitted.
					// if omitted, retry to process DHCP
					my_dhcp_retry++;
					if(my_dhcp_retry > MY_MAX_DHCP_RETRY)
					{
						gWIZNETINFO.dhcp = NETINFO_STATIC;
						DHCP_stop();      // if restart, recall DHCP_init()
#ifdef _MAIN_DEBUG_
						debugprintf(">> DHCP %d Failed\r\n", my_dhcp_retry);
						Net_Conf();
						Display_Net_Conf();   // print out static netinfo to serial
#endif
						my_dhcp_retry = 0;
					}
					break;
				default:
					break;
			}
    	}

    	// TODO: insert user's code here
    	if(run_user_applications)
    	{
			//PRINTLINE;
    		// Loopback test : TCP Server
    		if ((ret = loopback_tcps(SOCK_TCPS, gDATABUF, PORT_TCPS)) < 0) // TCP server loopback test
    		{
#ifdef _MAIN_DEBUG_
    			debugprintf("SOCKET ERROR : %ld\r\n", ret);
#endif
    		}
    		;
    	} // End of user's code
	} // End of Main loop

    return 0;
}


/* Private functions *********************************************************/

static void Net_Conf()
{
	/* wizchip netconf */
	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
}

static void Display_Net_Conf()
{
#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};
#endif

	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);

#ifdef _MAIN_DEBUG_
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);

	if(gWIZNETINFO.dhcp == NETINFO_DHCP) debugprintf("\r\n===== %s NET CONF : DHCP =====\r\n",(char*)tmpstr);
		else debugprintf("\r\n===== %s NET CONF : Static =====\r\n",(char*)tmpstr);
	debugprintf(" MAC : %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	debugprintf(" IP : %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	debugprintf(" GW : %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	debugprintf(" SN : %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	debugprintf("=======================================\r\n");
#endif
}

static void PHYStatus_Check(void)
{
	uint8_t tmp;
	static bool LED_status_backup;

	LED_status_backup = RGBLED_enable;

	do
	{
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		// Error indicator: LED Green ON when no PHY link detected
		if(tmp == PHY_LINK_OFF)
		{
			RGBLED_enable = false;
			//////////////Board_LED_Set(2, true);
		}
	}while(tmp == PHY_LINK_OFF);

	RGBLED_enable = LED_status_backup;
}

/* Public functions **********************************************************/

/**
 * @brief    Handle interrupt from SysTick timer
 * @return    Nothing
 */
void SysTick_Handler(void)
{
	static uint16_t phystatus_check_cnt = 0;

	msTicks++; // increment counter

	// RGB LED Blinky
	if (msTicks % 100 == 0)
	{
		if (RGBLED_enable == true)
		{
			// Board_LED_Toggle(0);
			// Board_LED_Set(1, false);
			// Board_LED_Set(2, false);
		}
		else
		{
			// Board_LED_Set(0, false);
			// Board_LED_Set(1, false);
			// Board_LED_Set(2, false);
		}
	}

	// PHY status check counter
	if(PHYStatus_check_enable)
	{
		if (phystatus_check_cnt++ > (1000 * SEC_PHYSTATUS_CHECK))
		{
			PHYStatus_check_flag = true;
			phystatus_check_cnt = 0;
		}
	}
	else
	{
		phystatus_check_cnt = 0;
	}
}

/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
void TIMER32_0_IRQHandler(void)
{
	// if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1))
	// {
		// Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
		// ////////////////////////////////////////////////////////
		// // SHOULD BE Added DHCP Timer Handler your 1s tick timer
		// DHCP_time_handler(); 	// for DHCP timeout counter
		// ////////////////////////////////////////////////////////
	// }
}

/**
 * @brief	Initialization 32-bit Timer 0
 * @return	Nothing
 */
void Timer32_0_Init(uint32_t tickrate)
{
	// uint32_t timerFreq;

	// /* Initialize 32-bit timer 0 clock */
	// Chip_TIMER_Init(LPC_TIMER32_0);

	// /* Timer rate is system clock rate */
	// timerFreq = Chip_Clock_GetSystemClockRate();

	// /* Timer setup for match and interrupt at TICKRATE_HZ */
	// Chip_TIMER_Reset(LPC_TIMER32_0);
	// /* Enable timer to generate interrupts when time matches */
	// Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
	// /* Setup 32-bit timer's duration (32-bit match time) */
	// Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / tickrate));
	// /* Setup timer to restart when match occurs */
	// Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
	// /* Start timer */
	// Chip_TIMER_Enable(LPC_TIMER32_0);

	// /* Clear timer of any pending interrupts */
	// NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
	// /* Enable timer interrupt */
	// NVIC_EnableIRQ(TIMER_32_0_IRQn);
}


/*******************************************************
 * @ brief Call back for ip assing & ip update from DHCP
 *******************************************************/
void my_ip_assign(void)
{
   getIPfromDHCP(gWIZNETINFO.ip);
   getGWfromDHCP(gWIZNETINFO.gw);
   getSNfromDHCP(gWIZNETINFO.sn);
   getDNSfromDHCP(gWIZNETINFO.dns);
   gWIZNETINFO.dhcp = NETINFO_DHCP;
   /* Network initialization */
   Net_Conf();      // apply from DHCP
#ifdef _MAIN_DEBUG_
   Display_Net_Conf();
   debugprintf("DHCP LEASED TIME : %ld Sec.\r\n", getDHCPLeasetime());
   debugprintf("\r\n");
#endif
}

/************************************
 * @ brief Call back for ip Conflict
 ************************************/
void my_ip_conflict(void)
{
#ifdef _MAIN_DEBUG_
	debugprintf("CONFLICT IP from DHCP\r\n");
#endif
   //halt or reset or any...
   while(1); // this example is halt.
}

uint8_t wiznet_reset_step = 0;

void W5500_handler(void){
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };
	uint8_t tmpstr[6] = {0,};
	uint8_t tmp;
	uint32_t w5500_st = 0;
	
	if(wiznet_state->wait_time > 0){

		return;
	}
	
	switch(wiznet_state->w5500_step){
		case WIZNET_STEP_RESET:// reset
			if(wiznet_reset_step == 0){
				memset(wiznet_state, 0, sizeof(wiznet_state_t));
				wiznet_state->w5500_step = WIZNET_STEP_RESET;
				WIZNET_RST_LOW();
				wiznet_state->wait_time = 1;// 10 msec
				wiznet_reset_step++;
				return;
			}
			else if(wiznet_reset_step == 1){
				WIZNET_RST_HIGH();
				wiznet_state->wait_time = 5;// 50 msec
				wiznet_reset_step++;
				return;
			}
			else if(wiznet_reset_step == 2){
				wiznet_state->w5500_step = WIZNET_STEP_CHIP_INIT;
				wiznet_reset_step = 0;
			//	PRINTLINE;
				return;
			}
			break;
			
		case WIZNET_STEP_CHIP_INIT: // chip init
			if(ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1){
				debugprintf("WIZCHIP Initialized fail.\r\n");
				wiznet_state->wait_time = 5;// 50 msec
			}
			else
				wiznet_state->w5500_step = WIZNET_STEP_NET_CONF;
			break;
			
		case WIZNET_STEP_NET_CONF:// Net config
		//	PRINTLINE;
			ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
			ctlwizchip(CW_GET_ID,(void*)tmpstr);
			wiznet_state->phy_check_en = true;
			debugprintf("\r\n=======================================\r\n");
			debugprintf(" WIZnet %s EVB - DHCP client v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
			debugprintf("=======================================\r\n");
			wiznet_state->w5500_step = WIZNET_STEP_DHCIP_INIT;
			break;
		
		case WIZNET_STEP_DHCIP_INIT:
			DHCP_init(SOCK_DHCP, gDATABUF);
			// if you want different action instead default ip assign, update, conflict.
			// if cbfunc == 0, act as default.
			reg_dhcp_cbfunc(my_ip_assign, my_ip_assign, my_ip_conflict);
			wiznet_state->w5500_step = WIZNET_SETEP_PHY_LINK;
			wiznet_state->wait_time = 5;// 50 msec
			break;
// 여기부터는 실제 메인 루틴
		case WIZNET_SETEP_PHY_LINK:// 1초마다 PHY가 연결 되어있는지 확인해야 함.
			if(wiznet_state->phy_check_flag == true){
				ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);
				if(tmp == PHY_LINK_OFF){// ethernet cable 연결이 안됐을 경우.
					wiznet_state->eth_connected = false;
					//PRINTLINE;
					return;
				}
				//else PRINTLINE;
				wiznet_state->phy_check_flag = false;
			}
// uint8_t st = DHCP_run();
// debugprintf("\n\r st=0x%x", st);
//			switch(st)
			switch(DHCP_run())
			{
				case DHCP_IP_ASSIGN:
				case DHCP_IP_CHANGED:
					/* If this block empty, act with default_ip_assign & default_ip_update */
					//
					// This example calls my_ip_assign in the two case.
					//
					// Add to ...
					//
					//	PRINTLINE;
					break;
				case DHCP_IP_LEASED:
					//
					// TODO: insert user's code here
					//run_user_applications = true;
					//
					w5500_st = loopback_tcps(SOCK_TCPS, gDATABUF, PORT_TCPS);
					if (w5500_st < 0) // TCP server loopback test
					{
#ifdef _MAIN_DEBUG_
						debugprintf("SOCKET ERROR : %ld\r\n");
#endif
					}
					//debugprintf("\n\r [%x]\n\r", w5500_st);
					break;
				case DHCP_FAILED:
			//		PRINTLINE;
					/* ===== Example pseudo code =====  */
					// The below code can be replaced your code or omitted.
					// if omitted, retry to process DHCP
					my_dhcp_retry++;
					if(my_dhcp_retry > MY_MAX_DHCP_RETRY)
					{
						wiznet_state->w5500_step = WIZNET_STEP_RESET;
						wiznet_reset_step= 0;
						// gWIZNETINFO.dhcp = NETINFO_STATIC;
						// DHCP_stop();      // if restart, recall DHCP_init()
// #ifdef _MAIN_DEBUG_
						// debugprintf(">> DHCP %d Failed\r\n", my_dhcp_retry);
						// Net_Conf();
						// Display_Net_Conf();   // print out static netinfo to serial
// #endif
						my_dhcp_retry = 0;
					}
					break;
				default:
//PRINTLINE;
					break;
			}
	}
}

