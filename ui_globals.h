#ifndef __UI_GLOBALS_H
#define __UI_GLOBALS_H

#include "ui_defines.h"

typedef enum {
	DEVICE_MD_NONE=0,
	DEVICE_MD_BOOT,
	DEVICE_MD_READY,
	DEVICE_MD_MAIN,
	DEVICE_MD_MAX,
} device_md_e;


typedef enum {
	MAIN_LOOP_0=0,
	MAIN_LOOP_1,
	MAIN_LOOP_2,
	MAIN_LOOP_3,
	MAIN_LOOP_MAX,
} main_loop_e;



typedef enum {
	PUSH_KEY_NONE=0,
	PUSH_KEY_EXIT,
	PUSH_KEY_ENTER_DATA,
	PUSH_KEY_UP_DATA,
	PUSH_KEY_DOWN_DATA,
	PUSH_KEY_MENU_DATA,
	PUSH_KEY_MENU_UP_DATA,
	PUSH_KEY_DOWN_UP_DATA,
	PUSH_KEY_ENTER_SHORT,
	PUSH_KEY_UP_SHORT,
	PUSH_KEY_DOWN_SHORT,
	PUSH_KEY_MENU_SHORT,
	PUSH_KEY_MENU_UP_SHORT,
	PUSH_KEY_DOWN_UP__SHORT,
	PUSH_KEY_ENTER_LONG,
	PUSH_KEY_UP_LONG,
	PUSH_KEY_DOWN_LONG,
	PUSH_KEY_MENU_LONG,
	PUSH_KEY_MENU_UP_LONG,
	PUSH_KEY_DOWN_UP_LONG,
	PUSH_KEY_COMPLETED,
	PUSH_KEY_MAX
} key_ctrl_e;

typedef enum {
	LED0_CTRL=0,
	LED1_CTRL,
	LED2_CTRL,
	LED3_CTRL,
	LED4_CTRL,
	LED5_CTRL,
	LED_CTRL_SIZE
} led_ctrl_e;

//===============================================================================================
//	FLAG
//===============================================================================================
typedef struct {
	uint8_t bit0:1;
	uint8_t bit1:1;
	uint8_t bit2:1;
	uint8_t bit3:1;
	uint8_t bit4:1;
	uint8_t bit5:1;
	uint8_t bit6:1;
	uint8_t bit7:1;
} tyFLAGBITS;

typedef union {
	uint8_t bAll;
	tyFLAGBITS b;
} tyFLAG;


//===============================================================================================


typedef struct {	
	uint8_t curr_step;
	uint8_t prev_step;
	uint8_t new_page;
	uint8_t loop_sec;
	
}device_state_t;



typedef struct {	
	uint32_t tmout_10ms;
	uint32_t tmout_25ms;
	uint32_t tmout_100ms;
	uint32_t tmout_1sec;
	
}target_time_t;


typedef struct {	
	u8 	power_state;	
	u8 	mode;	
	u8 	on_time;				// 100ms interval
	u8 	off_time;	

	u8 	pass_time;				// ê²½ê³¼ ì‹œê°„..
	
}led_control_t;

//=============================================================================
//	SENS PIN STATE
//=============================================================================
typedef struct {	
	u8 	lidar_in;	
	u8 	switch_in;	
	u8 	alarm_switch_in;	
	u8 	sens1_in;	

	u8 	sens2_in;	
	u8 	wifi_num;	// wifi¿¬°á °³¼ö
	u8 	dummy1;	
	u8 	dummy2;	
	u8 	dummy3;	
	
}ctrl_pin_state_t;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
	device_state_t	device_state;
	target_time_t	target_time;
	led_control_t	led_control[LED_CTRL_SIZE];

	ctrl_pin_state_t ctrl_pin_state;

	tyFLAG  wifi_mode_state1;
	tyFLAG  wifi_mode_state2;


	uint8_t wi_fi_request_count;

 } device_control_t;

extern device_control_t _pdc;
extern device_control_t *pdc;


typedef struct {	
	uint8_t sensor1_in;
	uint8_t sensor2_in;
	uint8_t self_test_sw;
	uint8_t dummy1;

	uint8_t aux1_in_sw;
	uint8_t aux2_in_sw;
	uint8_t aux1_out;
	uint8_t aux2_out;

	uint8_t exp1;
	uint8_t exp2;
	uint8_t exp3;
	uint8_t exp4;
	
}crane_state_t;

extern crane_state_t	*crane_state[8];

enum {
	WI_FI_CONNECT_NONE=0,
	WI_FI_CONNECT_CHECK,
	WI_FI_CONNECT_FAIL,
	WI_FI_CONNECT_OK,
	WI_FI_CONNECT_MAX
};


typedef struct {	
	uint8_t connection;
	uint8_t tmout;
	uint8_t req_cmd_count;

} wifi_connect_state_t;

extern wifi_connect_state_t	*wifi_connect_state[8];



extern void device_control_attach(void);


#endif /* __UI_GLOBALS_H */


