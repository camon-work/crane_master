#ifndef __USER_TASK_H
#define __USER_TASK_H

extern void wdt_resume(void);
extern void wdt_set(void);

extern void process_target_time_handler(void);
extern void peripherial_input_state(void);
extern void LiDAR_input_state(void) ;
extern bool ctrl_output_state(uint8_t port_num, uint8_t port_bit);

extern void led_control_state(void);
extern void led_control_reset(void);
extern void led_control_on(uint8_t led_port, uint8_t on_time, uint8_t off_time);
extern void led_control_off(uint8_t led_port);

extern	void	Lidar_Direction_check(void);	

extern void control_board_handler(void);


extern  u8 	SENS_CRANE_ON;	//WIFI crane on

extern void user_data_mem_read(void);
extern void user_data_mem_write(void);
extern void user_data_buff_to_wi_fi_data(void);
extern void user_wi_fi_data_to_data_buff(void);

#endif
 
