#include "sdk.h"

#include "ui_globals.h"
#include "user_task.h"
#include "NRC7394_wifi.h"
#include "w5500/dhcp_client.h"
#include "w5500/internet/DHCP/dhcp.h"
#include "w5500/loopback.h"


#define SYS_TICK_10MS		10
#define SYS_TICK_50MS		50
#define SYS_TICK_100MS		100
#define SYS_TICK_250MS		250
#define SYS_TICK_1SEC		1000



u8 	SENS_CRANE_ON=0;	//WIFI crane on
u8 	LIADAR_HIGH_ON=0;	//lidar High side on
u8 	LIADAR_LOW_ON=0;	// low side on
u8 	MANUAL_SW_ON=0;	//manual sw on

u8 	LIDAR_DIR_ON=0;		//lidar direction on 
u8	CRANE_DIR_ON=0;	//CRANE direction on HIGH ->LOW
u16	CRANE_DIR_timer=0;
u16	Dir_timewindow=0;

uint8_t user_mem_buff[USER_MEM_BUFF_SIZE ];		// 사용자 데이터 저장 영역

#define USER_MEM_CODE	 "RERA"
/********************************************************************************************************************
 *	watchdog
 ********************************************************************************************************************/
void wdt_resume(void)
{
   // Set Watchdog Count.
   *R_WDTLCK = 0x1ACCE551; // Watchdog Timer Unlock.
   *R_WDTCNT = 0x2DC6C00;   // about 1sec @ APB 60Mhz
   *R_WDTLCK = 0x0; // Watchdog Timer lock.
}
void wdt_set(void)
{
   // About 1000ms @ APB 48MHz
   // WDTCNT = ( Set_time(s) * APB Clk(Hz) )
   *R_WDTLCK = 0x1ACCE551; // Watchdog Timer Unlock.
   *R_WDTCNT = 0x2DC6C00;  // about 1sec @ APB 48Mhz
   *R_WDTCTRL = F_WDTCTRL_WDTMOD_RST | F_WDTCTRL_WDTEN_ENABLE;
   *R_WDTLCK = 0x0; // Watchdog Timer lock.
}


/********************************************************************************************************************
 *	CHECK TARGET TIME EVENT
 ********************************************************************************************************************/
unsigned int progress_time_condition(unsigned int taget_time, unsigned int check_time)
{
	unsigned int current_time_mm=get_tick_count();
	unsigned int progress_time=0;

	if(current_time_mm == taget_time)
		return 0;

	if(current_time_mm > taget_time)
		progress_time = current_time_mm -taget_time;
	else
		progress_time =(0xFFFFFFFF - taget_time) + current_time_mm;

	if(progress_time >= check_time)
	{
		if(current_time_mm == 0)
			current_time_mm++;
		return current_time_mm;
	}
	return 0;
}

/********************************************************************************************************************
 *	10ms time & 100ms time 
 ********************************************************************************************************************/
void progress_10ms_condition(void)
{
	if(wifi_ap_state->at_cmd_tmout>0)	wifi_ap_state->at_cmd_tmout--;
	if(wifi_ap_state->at_cmd_wait>0)	wifi_ap_state->at_cmd_wait--;
	if(wifi_ap_state->rxd_tmout>0)		wifi_ap_state->rxd_tmout--;				// data receive ok time out

	if(debug_cmd_state->rxd_tmout>0)	debug_cmd_state->rxd_tmout--;
	
		// w5500
	if(wiznet_state->wait_time) wiznet_state->wait_time--;
	if(wiznet_state->phy_check_en == true){
		wiznet_state->phy_link_time++;// 1sec
		if(wiznet_state->phy_link_time > 100){
			wiznet_state->phy_check_flag = true;
			wiznet_state->phy_link_time = 0;
		}
	}
	else{
		wiznet_state->phy_link_time = 0;
	}
	
}


void progress_100ms_condition(void)
{
	uint8_t i;
	
	if(wifi_ap_state->sat_device_tmout>0)	wifi_ap_state->sat_device_tmout--;
	
// W5500
	wiznet_state->dhcp_10m_time++;
	if(wiznet_state->dhcp_10m_time > 10){
		DHCP_time_handler();
		wiznet_state->dhcp_10m_time= 0;
	}


	for(i=0;i<8;i++) {
		if(wifi_connect_state[i]->connection == WI_FI_CONNECT_OK) {
			if(wifi_connect_state[i]->tmout>0)
				wifi_connect_state[i]->tmout--;
		}
	}
}

void progress_250ms_condition(void)
{

}


void progress_1sec_condition(void)
{
	
		if(CRANE_DIR_ON)
		{
		/*	if((SENS_CRANE_ON) ||LIADAR_HIGH_ON||LIADAR_LOW_ON||MANUAL_SW_ON||CRANE_DIR_ON)	
			{	
				//CRANE_DIR_timer = 600;	//10 min --> 600 sec 
				CRANE_DIR_timer = 60;	//10 min --> 600 sec 
			}
			else
			{	*/
			
				if(CRANE_DIR_timer)CRANE_DIR_timer--;
				else	CRANE_DIR_ON=0;			
			//}
		}
}


void	Lidar_Direction_check(void)		//크레인 방향 체크 	100ms routine
{

		if(LIADAR_HIGH_ON||LIADAR_LOW_ON)	
		{
			CRANE_DIR_timer= 30;//60;//60 sec 
			CRANE_DIR_ON=1;
		}

			/*
	//CRANE_DIR_ON
	
	if(Dir_timewindow)	Dir_timewindow--;
	
	if(LIADAR_HIGH_ON)
	{
			Dir_timewindow = 100;		//10 sec 이내에 LIADAR_HIGH_ON=1 , CRANE_DIR_ON=1
	}
	
	if(LIADAR_LOW_ON)
	{
		if(Dir_timewindow){
			CRANE_DIR_ON=1;	
		//	CRANE_DIR_timer = 600;	//10 min --> 600 sec 
			CRANE_DIR_timer = 60;	//1 min --> 60 sec   test 
		}		
	}
	*/	
}




/********************************************************************************************************************
 *	TARGET TIME CONDITION...10ms & 100ms & 100ms &  1sec	
 ********************************************************************************************************************/
void process_target_time_handler(void)
{
	if(progress_time_condition(pdc->target_time.tmout_10ms, SYS_TICK_10MS)) {
		pdc->target_time.tmout_10ms =get_tick_count(); 

		progress_10ms_condition();

		LiDAR_input_state();
	}

//----------------------------------------------------------------------------------------------	
//	100ms timer event handler
//----------------------------------------------------------------------------------------------
	if(progress_time_condition(pdc->target_time.tmout_100ms, SYS_TICK_100MS)) {
		pdc->target_time.tmout_100ms =get_tick_count(); 

		progress_100ms_condition();

		peripherial_input_state();

		led_control_state();
		
		Lidar_Direction_check();		//High on ->Low on ; 10 min on		//크레인이 내려왔다가 작업중이라 판단 
	}

//----------------------------------------------------------------------------------------------	
//	250ms target time... 1sec/4 ==> 1sec timer event handler
//----------------------------------------------------------------------------------------------
	if(progress_time_condition(pdc->target_time.tmout_1sec, SYS_TICK_250MS)) {
		pdc->target_time.tmout_1sec =get_tick_count(); 

		progress_250ms_condition();

		WiFi_AP_data_read_to_sat_device();

		SET_WIFI_TMOUT(ON);							// 250ms 간격으로 상태 체크...

		
//======data parsing and action =============================================================================		
#if 0				
	//	debugprintf(" crane_state[0]->sensor2_in : [%d]\r\n", crane_state[0]->sensor2_in);
/*
	u8 	SENS_CRANE_ON=0;	//WIFI crane on
	u8 	LIADAR_HIGH_ON=0;	//lidar High side on
	u8 	LIADAR_LOW_ON=0;	// low side on
	u8 	MANUAL_SW_ON=0;	//manual sw on

	u8 	LIDAR_DIR_ON=0;		//lidar direction on 
*/	
		
		SENS_CRANE_ON = crane_state[0]->sensor1_in;	
		debugprintf(" ========= >SENS_CRANE_ON : [%d]\r\n",SENS_CRANE_ON);	
		
		//if(crane_state[0]->sensor2_in) 
		if((SENS_CRANE_ON) ||LIADAR_HIGH_ON||LIADAR_LOW_ON||MANUAL_SW_ON||CRANE_DIR_ON)	
		//if((crane_state[0]->sensor2_in) ||LIADAR_HIGH_ON||LIADAR_LOW_ON||MANUAL_SW_ON)	
		{
			
			LOW_LIGHT_220V_ON();	//	#define 	*R_GPOHIGH(0) = (1<<5)
			NET_OUT_220V_ON();//		*R_GPOHIGH(0) = (1<<4)
			NET_OUT_24V_ON();//		*R_GPOHIGH(0) = (1<<3)
			SIREN_24V_HIGH();//		*R_GPOHIGH(0) = (1<<2)
			LIGHT2_24V_HIGH();//		*R_GPOHIGH(0) = (1<<1)
			LIGHT1_24V_HIGH();//		*R_GPOHIGH(0) = (1<<0)
		}
		else
		{
			
			LOW_LIGHT_220V_OFF();	//	#define 	*R_GPOHIGH(0) = (1<<5)
			NET_OUT_220V_OFF();//		*R_GPOHIGH(0) = (1<<4)
			NET_OUT_24V_OFF();//		*R_GPOHIGH(0) = (1<<3)
			SIREN_24V_LOW();//		*R_GPOHIGH(0) = (1<<2)
			LIGHT2_24V_LOW();//		*R_GPOHIGH(0) = (1<<1)
			LIGHT1_24V_LOW();//		*R_GPOHIGH(0) = (1<<0)			
		}	
#endif	
//======end of data parsing and action =============================================================================		
		
		switch(pdc->device_state.loop_sec) {
			case MAIN_LOOP_0:
				
				break;

			case MAIN_LOOP_1:
				break;

			case MAIN_LOOP_2:
				break;

			case MAIN_LOOP_3:
				progress_1sec_condition();
				break;

			default:
				break;
		}
		pdc->device_state.loop_sec++;
		pdc->device_state.loop_sec%=MAIN_LOOP_MAX;
	}
}


/******************************************************************************************************************************
 *	CNTEBOARD CONTROL INPUT STATE..250ms 간격으로 확인 혹은 RX신호를 받으면 바로 처리..
 ******************************************************************************************************************************/
void control_board_handler(void)
{
	uint8_t i;
	uint8_t wi_fi_state = 0;
	
	if((GET_WIFI_RX_MSG()) || (GET_WIFI_TMOUT())) {
		SET_WIFI_RX_MSG(CLR);
		SET_WIFI_TMOUT(CLR);
		
		SENS_CRANE_ON = crane_state[0]->sensor1_in;	
	//	debugprintf(" ========= >SENS_CRANE_ON : [%d]\r\n",SENS_CRANE_ON);	

	//if(crane_state[0]->sensor2_in) 
		//if((crane_state[0]->sensor2_in) ||LIADAR_HIGH_ON||LIADAR_LOW_ON||MANUAL_SW_ON)	
		if((SENS_CRANE_ON) ||LIADAR_HIGH_ON||LIADAR_LOW_ON||MANUAL_SW_ON||CRANE_DIR_ON)			{			
			LOW_LIGHT_220V_ON();	//	#define 	*R_GPOHIGH(0) = (1<<5)
			NET_OUT_220V_ON();//		*R_GPOHIGH(0) = (1<<4)
			NET_OUT_24V_ON();//		*R_GPOHIGH(0) = (1<<3)
			SIREN_24V_HIGH();//		*R_GPOHIGH(0) = (1<<2)
			LIGHT2_24V_HIGH();//		*R_GPOHIGH(0) = (1<<1)
			LIGHT1_24V_HIGH();//		*R_GPOHIGH(0) = (1<<0)
		}
		else {
			LOW_LIGHT_220V_OFF();	//	#define 	*R_GPOHIGH(0) = (1<<5)
			NET_OUT_220V_OFF();//		*R_GPOHIGH(0) = (1<<4)
			NET_OUT_24V_OFF();//		*R_GPOHIGH(0) = (1<<3)
			SIREN_24V_LOW();//		*R_GPOHIGH(0) = (1<<2)
			LIGHT2_24V_LOW();//		*R_GPOHIGH(0) = (1<<1)
			LIGHT1_24V_LOW();//		*R_GPOHIGH(0) = (1<<0)			
		}	
	}


	for(i=0;i<8;i++) {
		if(wifi_connect_state[i]->connection == WI_FI_CONNECT_OK) {				// 0~7 까지 연결된 상태에서 응답이 들어 오는지 확인...
		
			if(wifi_connect_state[i]->req_cmd_count>=REQ_CMD_MAX_COUNT) {
				wifi_connect_state[i]->connection = WI_FI_CONNECT_NONE;
				
				crane_state[i]->sensor1_in = 0;		// 5
				crane_state[i]->sensor2_in = 0;			// 6
				crane_state[i]->self_test_sw = 0;		// 7
				crane_state[i]->aux1_in_sw = 0;			// 8
				crane_state[i]->aux2_in_sw = 0;			// 9
				crane_state[i]->aux2_out = 0;			// 11
				crane_state[i]->aux1_out = 0;			// 10
				crane_state[i]->exp1 = 0;				// 12
				crane_state[i]->exp2 = 0;				// 13
				crane_state[i]->exp3 = 0;				// 14
				crane_state[i]->exp4 = 0;				// 15
 			}
		}
	}

	
	if(wifi_ap_state->connect == CONNECT) {				// 연결된 상태에서 10회 이상 응답이 없으면...DISCONNECT로 간주 한다..
		if(pdc->wi_fi_request_count >= 10) {
			SENS_CRANE_ON = CLR;
		}
	}
}




/******************************************************************************************************************************
 *	PERIPHERIAL INPUT STATE
 ******************************************************************************************************************************/
 void peripherial_input_state(void)
{
	if(SWITCH_IN_PIN()) {				// (LIDAR1) SWITCH IN HIGH  -->active LOW 로 변경 
		pdc->ctrl_pin_state.switch_in = HIGH;
		LIADAR_HIGH_ON = LOW;
	}
	else {
		pdc->ctrl_pin_state.switch_in = LOW;
		LIADAR_HIGH_ON = HIGH;
	}

	if(ALARM_SWITCH_PIN()) {  //알람 스우치 = 수동스위치 
		pdc->ctrl_pin_state.alarm_switch_in = HIGH;
		MANUAL_SW_ON = HIGH;
		
	}
	else {
		pdc->ctrl_pin_state.alarm_switch_in = LOW;
		MANUAL_SW_ON = LOW;
	}

	if(SENS1_IN_PIN()) {
		pdc->ctrl_pin_state.sens1_in = HIGH;
	}
	else {
		pdc->ctrl_pin_state.sens1_in = LOW;
	}

	if(SENS2_IN_PIN()) {
		pdc->ctrl_pin_state.sens2_in = HIGH;
	}
	else {
		pdc->ctrl_pin_state.sens2_in = LOW;
	}
 }

void LiDAR_input_state(void) 
{
	if(LiDAR_IN_PIN()) {				// LiDAR2  IN HIGH  --> active LOW 로 변경 
		pdc->ctrl_pin_state.lidar_in = HIGH;
		LIADAR_LOW_ON = LOW;
	}
	else {
		pdc->ctrl_pin_state.lidar_in = LOW;
		LIADAR_LOW_ON = HIGH;
	}

}
 
bool ctrl_output_state(uint8_t port_num, uint8_t port_bit){
	uint8_t port_state;
	
	port_state = *R_GPOLEV(port_num);
	if(port_state & (1 << port_bit)) return HIGH;
	else return LOW;
	
}

/******************************************************************************************************************************
 *	LED CONTROL STATE...100ms interval...
 * 	ex) on_time : 2 off_time : 8  -> LED 200ms ON | LED 800ms OFF
 * 	if on time = 0...led on.....
 ******************************************************************************************************************************/
void led_control_state(void)
{
	uint8_t i;
	uint8_t led_port;
	
	for(i=0;i<LED_CTRL_SIZE;i++) {
		if(pdc->led_control[i].power_state==POWER_ON) {
			if(pdc->led_control[i].on_time == 0) {
				pdc->led_control[i].pass_time = 0;
				pdc->led_control[i].on_time = 0xFF;
			}

			led_port = (1<<i);
			pdc->led_control[i].pass_time++;
			if(pdc->led_control[i].mode==LED_ON) {
				if(pdc->led_control[i].pass_time>=pdc->led_control[i].on_time) {// 설정 ON 시간이 지나면...다시 OFF
					pdc->led_control[i].pass_time = 0;
					pdc->led_control[i].mode = LED_OFF;
					LED_CONTROL_OFF(led_port);
				}
			}
			
			else {
				if(pdc->led_control[i].pass_time>=pdc->led_control[i].off_time) { // 설정 OFF 시간이 지나면...다시 ON
					pdc->led_control[i].pass_time = 0;
					pdc->led_control[i].mode = LED_ON;
					LED_CONTROL_ON(led_port);
				}
			}
		}	
	}
}

/******************************************************************************************************************************
 *	LED CONTROL RESET
 ******************************************************************************************************************************/
void led_control_reset(void)
{
	uint8_t i;
	uint8_t led_port;
	
	for(i=0;i<LED_CTRL_SIZE;i++) {
		pdc->led_control[i].mode = 0;
		pdc->led_control[i].power_state = 0;
		pdc->led_control[i].on_time = 0;
		pdc->led_control[i].off_time = 0;
		pdc->led_control[i].pass_time = 0;

		led_port = (1<<i);
		LED_CONTROL_OFF(led_port);
	}
}

void led_control_on(uint8_t led_port, uint8_t on_time, uint8_t off_time)
{
	
	pdc->led_control[led_port].mode = LED_ON;
	pdc->led_control[led_port].power_state = POWER_ON;
	pdc->led_control[led_port].pass_time = 0;
	pdc->led_control[led_port].on_time = on_time;
	pdc->led_control[led_port].off_time = off_time;
	LED_CONTROL_ON(1<<led_port);
}

void led_control_off(uint8_t led_port)
{
	pdc->led_control[led_port].power_state = POWER_OFF;
	pdc->led_control[led_port].pass_time = 0;
	LED_CONTROL_OFF(1<<led_port);
}


/******************************************************************************************************************************
 *	USER DATA SECTOR READ & WRITE & READ
 * 	B0 B3 	B4 B5 	B6 - B13 	B14 - B17 	B18 -  B33 	B34 -  B37 	B40 -  B55	 B56 - B59
 *  TERA  	 K1    CRANE_01       5000     xxx.xxx.xxx.xxx  50001 xxx.xxx.xxx.xxx  5000
 *   ID    CODE      SSID         PORT      REMOTE  IP    REMOTE PORT    SERVER IP    SERVER PORT
 ******************************************************************************************************************************/
 
void user_data_mem_read(void)
{
	int sector_size = sflash_get_sector_size(0);
    int sector_cnt = sflash_get_size(0)/sector_size;
    int last_sector = sector_cnt-1;
    int target_addr = last_sector*sector_size;
	int i;
	
	memset(user_mem_buff, 0, sizeof(user_mem_buff));
	
	CRITICAL_ENTER();
	sflash_read(target_addr, user_mem_buff, USER_MEM_BUFF_SIZE);			
	CRITICAL_EXIT();

	if(memcmp(&user_mem_buff[0], USER_MEM_CODE, strlen(USER_MEM_CODE))==0) {

		WiFi_AP_default_config();									// default 값을 읽어 온다..
		user_data_buff_to_wi_fi_data();								// 메모리에서 읽어 온 값을 변수에 저장 한다.

		debugprintf(" %s,  %s,  %s\r\n", wifi_ap_state->wifi_config.country_code, wifi_ap_state->wifi_config.ssid, wifi_ap_state->net_info.port_num);
		debugprintf(" %s,  %s,	%s,	%s\r\n", wifi_ap_state->net_info.client_ip, wifi_ap_state->net_info.client_port, wifi_ap_state->net_info.remote_ip, wifi_ap_state->net_info.remote_port);
	}
	else {						// default setting
		for(i = 0; i<32; i++){
			if((i % 8) == 0) debugprintf("\n\r");
			debugprintf(" 0x%02x ", user_mem_buff[i]);
		}
		debugprintf("\n\r");

		memset(user_mem_buff, 0, sizeof(user_mem_buff));
		
		WiFi_AP_default_config();									// default 값을 읽어 온다..
		user_data_mem_write();
	}
}

/******************************************************************************************************************************
 *	UPDATE 된 변수를 저장 한다..
 ******************************************************************************************************************************/
void user_data_mem_write(void)
{
	int sector_size = sflash_get_sector_size(0);
    int sector_cnt = sflash_get_size(0)/sector_size;
    int last_sector = sector_cnt-1;
    int target_addr = last_sector*sector_size;
	
	user_wi_fi_data_to_data_buff();

	CRITICAL_ENTER();

	sflash_erase_sector(0, last_sector, 1);									// 1024 * 4 * 74 = 0x4A00 -> 1 sector erase...
	delayms(100);	
	sflash_write(target_addr, user_mem_buff, USER_MEM_BUFF_SIZE);			
	
	CRITICAL_EXIT();

}

/******************************************************************************************************************************
 *	user memory-> struct value
 ******************************************************************************************************************************/
void user_data_buff_to_wi_fi_data(void)
{
	char fname[16];

	memset(wifi_ap_state->wifi_config.country_code, 0, sizeof(wifi_ap_state->wifi_config.country_code));
	strncpy((char *)wifi_ap_state->wifi_config.country_code, &user_mem_buff[4], 2);


	memset(fname, 0, sizeof(fname));
	strncpy((char *)fname, &user_mem_buff[6], 8);



	memset(wifi_ap_state->wifi_config.ssid, 0, sizeof(wifi_ap_state->wifi_config.ssid));
	strcpy((char *)wifi_ap_state->wifi_config.ssid, fname);
//	strncpy((char *)wifi_ap_state->wifi_config.ssid, &user_mem_buff[6], 8);

	debugprintf("wi_fi_data: %s, %s\r\n", fname, wifi_ap_state->wifi_config.ssid);

	memset(wifi_ap_state->net_info.port_num, 0, sizeof(wifi_ap_state->net_info.port_num));
	strncpy((char *)wifi_ap_state->net_info.port_num, &user_mem_buff[14], 4);

	memset(wifi_ap_state->net_info.client_ip, 0, sizeof(wifi_ap_state->net_info.client_ip));
	strncpy((char *)wifi_ap_state->net_info.client_ip, &user_mem_buff[18], 15);

	memset(wifi_ap_state->net_info.client_port, 0, sizeof(wifi_ap_state->net_info.client_port));
	strncpy((char *)wifi_ap_state->net_info.client_port, &user_mem_buff[34], 4);

	memset(wifi_ap_state->net_info.remote_ip, 0, sizeof(wifi_ap_state->net_info.remote_ip));
	strncpy((char *)wifi_ap_state->net_info.remote_ip, &user_mem_buff[40], 15);

	memset(wifi_ap_state->net_info.remote_port, 0, sizeof(wifi_ap_state->net_info.remote_port));
	strncpy((char *)wifi_ap_state->net_info.remote_port, &user_mem_buff[56], 4);

	Ethernet_string_to_num();			// ETHERNET IP & PORT STRING -> DECIMAL// 서버 포터번호 및 IP address를 읽어옴
}


/******************************************************************************************************************************
 *	struct value -> user memory
 ******************************************************************************************************************************/
void user_wi_fi_data_to_data_buff(void)
{
	memset(user_mem_buff, 0, sizeof(user_mem_buff));
	
	strncpy(&user_mem_buff[0], USER_MEM_CODE, strlen(USER_MEM_CODE));						// HEADER
	strncpy(&user_mem_buff[4], wifi_ap_state->wifi_config.country_code, 2);		// contry code : 2byte
	strncpy(&user_mem_buff[6], wifi_ap_state->wifi_config.ssid, 8);				// SSID : 8byte
	strncpy(&user_mem_buff[14], wifi_ap_state->net_info.port_num, 4); 			// PORT : 4byte
	strncpy(&user_mem_buff[18], wifi_ap_state->net_info.client_ip, 15);			// CLIENT IP : 15byte xxx.xxx.xxx.xxx
	strncpy(&user_mem_buff[34], wifi_ap_state->net_info.client_port, 4); 		// CLIENT PORT : 4byte
	
//	ETHERNET IP & PORT
	strncpy(&user_mem_buff[40], wifi_ap_state->net_info.remote_ip, 15);			// REMOTE IP :15byte
	strncpy(&user_mem_buff[56], wifi_ap_state->net_info.remote_port, 4); 		// REMOTE PORT :4byte
}
 
