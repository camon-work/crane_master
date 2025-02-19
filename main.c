


#include "sdk.h"

#include "ui_globals.h"
#include "user_task.h"
#include "NRC7394_wifi.h"

#include "w5500/w5500_spi.h"
#include "w5500/dhcp_client.h"

/*
 *	P0.3(OUTPUT) : NET_OUT_24V(네트웍 제어 24V)	| P0.2(OUTPUT) : SIREN_24V(사이렌 24V) | P0.1(OUTPUT) : LIGH2_24V(경광등-하 24V) | P0.0(OUTPUT) : LIGH1_24V(경광등-상 24V)
 * 	P0.7(OUTPUT) : BUZZER | P0.6(OUTPUT) : NOT USED | P0.5(OUTPUT) : (HR705-2P-24VDC) | P0.4(OUTPUT) : NET_OUT_220V(HR705-2P-24VDC)

 * 	P1.3(OUTPUT) : LED3 | P1.2(OUTPUT) : LED2 | P1.1(OUTPUT) : LED1 | P1.0(OUTPUT) : LED0
 * 	P1.7(OUTPUT) : 24V OUT CH2 | P1.6(OUTPUT) : 24V OUT CH1 | P1.5(OUTPUT) : LED5 | P1.4(OUTPUT) : LED4

 * 	P2.3(TX5) : WiFi TX5 | P2.2(RX5) : WiFi RX5 | P2.1(INPUT) : WIZNET INT | P2.0(OUTPUT) : WIZNET RST

 * 	P4.3(INPUT) : LiDAR | P4.1(INPUT) : SWITCH | P4.0(INPUT) : ALARM SWITCH 
 
* 	P5.3(OUTPUT) : NOT USED | P5.2(OUTPUT) : NOT USED | P5.1(RX0) : DEBUG UART RX0 | P5.0(TX0) : DEBUG UART TX0
* 	P5.7(SPI SCL) : WIZNET | P5.6(SPI MISO) : WIZNET | P5.5(SPI MOSI) : WIZNET | P5.4(SPI nCS : WIZNET

 * 	P6.2(OUTPUT) : NOT USED | P6.1(OUTPUT) : NOT USED | P6.0(OUTPUT) : NOT USED 

 * 	P8.3(OUTPUT) : WiFi Reset | P8.2(INPUT) : SEN2_IN | P8.1(OUTPUT) : NOT USED | P8.0(INPUT) : SEN1_IN
 * 	P8.7(OUTPUT) : NOT USED | P8.6(OUTPUT) : NOT USED | P8.5(OUTPUT) : NOT USED | P8.4(OUTPUT) : NOT USED
*/
void master_init()
{
	*R_PAF(0) = F_PAF0_0_GP0 | F_PAF0_1_GP1 | F_PAF0_2_GP2 | F_PAF0_3_GP3 | F_PAF0_4_GP4 | F_PAF0_5_GP5 | F_PAF0_6_GP6 | F_PAF0_7_GP7;
	*R_GPOLOW(0) = 0xBF;	
	*R_GPODIR(0) = 0xFB;	
	
	*R_PAF(1) = F_PAF1_0_GP0 | F_PAF1_1_GP1 | F_PAF1_2_GP2 | F_PAF1_3_GP3 | F_PAF1_4_GP4 | F_PAF1_5_GP5 | F_PAF1_6_GP6 | F_PAF1_7_GP7;
	*R_GPOHIGH(1) = 0x3F;
	*R_GPOLOW(1) = 0xC0;
	*R_GPODIR(1) = 0xFF;
	
	*R_PAF(2) = F_PAF2_0_GP0 | F_PAF2_1_GP1 | F_PAF2_2_UART5_RX | F_PAF2_3_UART5_TX;	
	*R_GPOLOW(2) = 0x09;
	*R_GPODIR(2) = 0x09;
	
	*R_PAF(4) = F_PAF4_0_GP0 | F_PAF4_1_GP1 | F_PAF4_2_GP2 | F_PAF4_3_GP3;
	*R_GPOLOW(4) = 0x04;		// x x x x 0 1 0 0
	*R_GPODIR(4) = 0x04;		// x x x x 0 1 0 0

	*R_GPPUEN(4) = (1<<2);
	
	*R_PAF(5) = F_PAF5_0_UART0_TX | F_PAF5_1_UART0_RX | F_PAF5_2_GP2 | F_PAF5_3_GP3 | F_PAF5_4_SPI1_CS | F_PAF5_5_SPI1_SDO | F_PAF5_6_SPI1_SDI | F_PAF5_7_SPI1_CLK;
	*R_GPOLOW(5) = 0x10;		// x x x 1 x x x x 
	*R_GPODIR(5) = 0xDB;		// 1 0 1 1 1 1 0 1
	
	*R_PAF(6) = F_PAF6_0_GP0 | F_PAF6_1_GP1 | F_PAF6_2_GP2;
	*R_GPOLOW(6) = 0x07;
	*R_GPODIR(6) = 0x07;

	*R_PAF(7) = F_PAF7_0_GP0 | F_PAF7_1_GP1;


	*R_PAF(8) = F_PAF8_0_GP0 | F_PAF8_1_GP1 | F_PAF8_2_GP2 | F_PAF8_3_GP3 | F_PAF8_4_GP4 | F_PAF8_5_GP5 | F_PAF8_6_GP6| F_PAF8_7_GP7;   
	*R_GPOLOW(8) = 0x0A;
	*R_GPODIR(8) = 0x0A;	//	x x x x 1 0 1 0
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	system config
//

void spi1_config(void)
{
//	*R_PAF(5) &= ~((0x3<<8) | (0x3<<10) | (0x3<<14));		// SPI		
	
	*R_SPICON(1) = SPICON_EN | SPICON_MSTR | SPICON_MSBF;
	*R_SPIBAUD(1) = 10; 									//4.8MHz
}


void system_config(void)
{
	uart_config(DEBUG_UART_CH, 115200, DATABITS_8, STOPBITS_1, UART_PARNONE);	// debug	
	uart_config(WIFI_UART_CH, 115200, DATABITS_8, STOPBITS_1, UART_PARNONE);	// Wi-Fi
	SPI_Init();
	tick_timer_init();												// TICK TIMER INIT
}

//=====================================================================================================
//	value event 
//=====================================================================================================
void value_initial(void)
{
	device_control_attach();
	
//	wdt_set();

	led_control_off(LED0_CTRL);
	led_control_off(LED1_CTRL);
	led_control_off(LED2_CTRL);
	led_control_off(LED3_CTRL);
	led_control_off(LED4_CTRL);
	
	led_control_on(LED5_CTRL, 2, 8);
	
	user_data_mem_read();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MAIN PROCESS
//

int main()
{
	master_init();
	system_config();
	value_initial();
	
#ifdef _DEBUG_MSG_
	debugstring("================================================\r\n");  
	debugprintf(" TERA Crane master.	System Clock(%dMhz)  Version 1.1.0\r\n", get_ahb_clock() / 1000000);
	debugstring("================================================\r\n");
#endif	

	while(1)
	{		
		wdt_resume();

//-----------------------------------------------------------------------------------------------
//	01. Tick Time 10ms & 100ms & 1sec condition
//-----------------------------------------------------------------------------------------------
		process_target_time_handler();

//-----------------------------------------------------------------------------------------------
//	02. Wi-Fi AT Command Handler
//-----------------------------------------------------------------------------------------------
		WiFi_AP_at_cmd_handler() ;
//-----------------------------------------------------------------------------------------------
//	03. Wi-Fi Serial Event Handler
//-----------------------------------------------------------------------------------------------
		WiFi_AP_data_receive();

//-----------------------------------------------------------------------------------------------
//	04. Wi-Fi AT data handler
//-----------------------------------------------------------------------------------------------
		WiFi_AP_data_parser();

//-----------------------------------------------------------------------------------------------
//	05. control board GPIO event handler
//-----------------------------------------------------------------------------------------------		
		control_board_handler();

//-----------------------------------------------------------------------------------------------
//	06. WIZNET W5500 Event Handler
//-----------------------------------------------------------------------------------------------
		W5500_handler();
//-----------------------------------------------------------------------------------------------
//	07. DEBUG COMMAND MESSAGE...ui_define에서 설정 가능
//-----------------------------------------------------------------------------------------------
#ifdef	_DEBUG_CMD_MSG_
		debug_command_receive();
#endif

	}
	
	return 0;
}
