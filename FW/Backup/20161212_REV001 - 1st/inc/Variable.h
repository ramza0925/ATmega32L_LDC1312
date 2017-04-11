#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#define DATA_DEBUG
//#define KEY_TEST
//#define MODE_CHECK
//#define I2C_CHECK

#define TRUE 1
#define FALSE 0

#define CH0			0
#define CH1			1
#define CH2			2
#define CH3			3

#define HW_REV		0x01
#define SW_REV		0x01

#define MIN_DST		0.0f
#define MAX_DST		10.0f
#define MIN_CURR	4.0f
#define MAX_CURR	20.0f

extern uint32_t ldc[11];
#define LDC_LENGTH	11
#define UNIT_CURR	(MAX_CURR-MIN_CURR)/(LDC_LENGTH-1)

extern uint32_t ldc_data;						//Sensor data
extern float distance;							//Object Distance
extern float current;							//Output Current
extern float percent;							//output percent

extern uint16_t position;
extern uint8_t lcd_contract;
extern uint8_t reloadFlag;
extern uint16_t config, divider, settlecount, rcount, error_conf, mux_conf, i_drive, status, offset, /*gain,*/ debugPage;

extern char LCD[20];
extern uint8_t Display_Unit;				// Display Unit

extern uint16_t offset_arr[4];

#endif