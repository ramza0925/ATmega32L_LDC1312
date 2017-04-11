#ifndef __LDC1612_H__
#define __LDC1612_H__

#include <stdint.h>

#include <ADuCM360.h>
#include <DioLib.h>
#include <I2cLib.h>

//Register Map
#define DATA_CH0_MSB			0x00				//CH0_MSB 변환결과와 에러상태
#define DATA_CH0_LSB			0x01				//CH0_LSB 변환결과
#define DATA_CH1_MSB			0x02				//CH1 변환결과와 에러상?
#define DATA_CH1_LSB			0x03				//CH1_LSB 변환결과
#define DATA_CH2_MSB			0x04				//CH2 변환결과와 에러상태	(LDC1614 only)
#define DATA_CH2_LSB			0x05				//CH2_LSB 변환결과
#define DATA_CH3_MSB			0x06				//CH3 변환결과와 에러상태	(LDC1614 only)
#define DATA_CH3_LSB			0x07				//CH3_LSB 변환결과
#define RCOUNT_CH0				0x08				//CH0 참조 카운터 설정  
#define RCOUNT_CH1				0x09				//CH1 참조 카운터 설정  
#define RCOUNT_CH2				0x0A				//CH2 참조 카운터 설정  		(LDC1614 only)
#define RCOUNT_CH3				0x0B				//CH3 참조 카운터 설정  		(LDC1614 only)
#define OFFSET_CH0				0x0C				//CH0 오프셋 값 설정 
#define OFFSET_CH1				0x0D				//CH1 오프셋 값 설정 
#define OFFSET_CH2				0x0E				//CH2 오프셋 값 설정 			(LDC1614 only)
#define OFFSET_CH3				0x0F				//CH3 오프셋 값 설정 			(LDC1614 only)
#define SETTLECOUNT_CH0			0x10				//CH0 안정화 참조 카운터 
#define SETTLECOUNT_CH1			0x11				//CH1 안정화 참조 카운터 
#define SETTLECOUNT_CH2			0x12				//CH2 안정화 참조 카운터 	(LDC1614 only)
#define SETTLECOUNT_CH3			0x13				//CH3 안정화 참조 카운터 	(LDC1614 only)
#define CLOCK_DEVIDERS_CH0		0x14				//CH0 레퍼런스와 센서의클럭 분배 설정 
#define CLOCK_DEVIDERS_CH1		0x15				//CH1 레퍼런스와 센서의클럭 분배 설정 
#define CLOCK_DEVIDERS_CH2		0x16				//CH2 레퍼런스와 센서의클럭 분배 설정 (LDC1614 only)
#define CLOCK_DEVIDERS_CH3		0x17				//CH3 레퍼런스와 센서의클럭 분배 설정 (LDC1614 only)
#define	STATUS					0x18				//디바이스 상태 보고 
#define ERROR_CONFIG			0x19				//디바이스 오류 보고 설정 
#define CONFIG					0x1A				//변환 설정
#define MUX_CONFIG				0x1B				//채널 멀티 플렉스 설정 
#define RESET_DEV				0x1C				//디바이스 리셋
#define DRIVE_CURRENT_CH0		0x1E				//CH0 센서 전류 구동 설정 
#define DRIVE_CURRENT_CH1		0x1F				//CH1 센서 전류 구동 설정 
#define DRIVE_CURRENT_CH2		0x20				//CH2 센서 전류 구동 설정 
#define DRIVE_CURRENT_CH3		0x21				//CH3 센서 전류 구동 설정 
#define MANUFACTURER_ID			0x7E				//디바이스 제조사 ID
#define DEVICE_ID				0x7F				//디바이스 ID

//DATA_CHx Register(CH0: 0x00, CH1: 0x02, Ch2: 0x04(LDC1314 Only), CH3:0x06(LDC1314 Only))
#define CHx_ERR_UR				(0x0001 << 15)		//CHx 변환 범위 미만 플래그 
#define CHx_ERR_OR				(0x0001 << 14)		//CHx 변환 범위 초과 플래그 
#define CHx_ERR_WD				(0x0001 << 13)		//CHx 변환 범위 와치독 타임아웃 플래그 
#define CHx_ERR_AE				(0x0001 << 12)		//CHx 변환 범위 진폭 오류 
#define CHx_DATA				(0x0FFF << 0)		//CHx 변환 결과 값 

//RCOUNT_CHx Register(CH0: 0x08, CH1: 0x09, Ch2: 0x0A(LDC1314 Only), CH3:0x0B(LDC1314 Only))
//#define CHx_RCOUNT				(0xFFFF << 0)

//OFFSET_CHx Register(CH0: 0x0C, CH1: 0x0D, Ch2: 0x0E(LDC1314 Only), CH3:0x0F(LDC1314 Only))
//#define CHx_OFFSET				(0xFFFF << 0)

//SETTLECOUNT_CHx Register(CH0: 0x10, CH1: 0x11, Ch2: 0x12(LDC1314 Only), CH3:0x13(LDC1314 Only))
//#define CHx_SETTLECOUNT			(0xFFFF << 0)

//CLOCK_DIVIDERS_CHx Register(CH0: 0x14, CH1: 0x15, Ch2: 0x16(LDC1314 Only), CH3:0x17(LDC1314 Only))
#define CHx_FIN_DIVIDER			(0x000F << 12)
#define CHx_FREF_DIVIDER		(0x03FF << 0)

//STATUS Register(0x18)
#define ERR_CHAN				(0x0003 << 14)		//오류 발생 채널						
#define ERR_UR					(0x0001 << 13)		//범위 미만 오류						(1: O, 0: X)
#define ERR_OR					(0x0001 << 12)		//범위 초과 오류 						(1: O, 0: X)
#define ERR_WD					(0x0001 << 11)		//와치독 타임아웃 					(1: O, 0: X)
#define ERR_AHE					(0x0001 << 10)		//진폭 높음 오류						(1: O, 0: X)
#define ERR_ALE					(0x0001 << 9)		//진폭 낮음 오류 						(1: O, 0: X)
#define ERR_ZC					(0x0001 << 8)		//카운터 제로 오류					(1: O, 0: X)
#define DRDY					(0x0001 << 6)		//데이터 준비 완료					(1: O, 0: X)
#define CH0_UNREAD_CONV			(0x0001 << 3)		//CH0에 읽지 않은 변환이 있음	(1: O, 0: X)
#define CH1_UNREAD_CONV			(0x0001 << 2)		//CH1에 읽지 않은 변환이 있음	(1: O, 0: X)
#define CH2_UNREAD_CONV			(0x0001 << 1)		//CH2에 읽지 않은 변환이 있음	(1: O, 0: X)
#define CH3_UNREAD_CONV			(0x0001 << 0)		//CH3에 읽지 않은 변환이 있음	(1: O, 0: X)

//ERROR_CONFIG Register(0x19)
#define UR_ERR2OUT				(0x0001 << 15)		//범위 미만 오류 출력 설정 		(1: O, 0: X)
#define UR_ERR2OUT_EN			(0x0001 << 15)
#define UR_ERR2OUT_DIS			(0x0000 << 15)

#define OR_ERR2OUT				(0x0001 << 14)		//범위 초과 오류 출력 설정		(1: O, 0: X)
#define OR_ERR2OUT_EN			(0x0001 << 14)
#define OR_ERR2OUT_DIS			(0x0000 << 14)

#define WD_ERR2OUT				(0x0001 << 13)		//와치독 타임아웃 출력 설정 	(1: O, 0: X)
#define WD_ERR2OUT_EN			(0x0001 << 13)
#define WD_ERR2OUT_DIS			(0x0000 << 13)

#define AH_ERR2OUT				(0x0001 << 12)		//진폭 높음 오류 출력 설정 		(1: O, 0: X)
#define AH_ERR2OUT_EN			(0x0001 << 12)
#define AH_ERR2OUT_DIS			(0x0000 << 12)

#define AL_ERR2OUT				(0x0001 << 11)		//진폭 낮음 오류 출력 설정 		(1: O, 0: X)
#define AL_ERR2OUT_EN			(0x0001 << 11)
#define AL_ERR2OUT_DIS			(0x0000 << 11)

#define UR_ERR2INT				(0x0001 << 7)		//범위 미만 오류 인터럽트 		(1: O, 0: X)
#define UR_ERR2INT_EN			(0x0001 << 7)
#define UR_ERR2INT_DIS			(0x0000 << 7)

#define OR_ERR2INT				(0x0001 << 6)		//범위 초과 오류 인터럽트 		(1: O, 0: X)
#define OR_ERR2INT_EN			(0x0001 << 6)
#define OR_ERR2INT_DIS			(0x0000 << 6)

#define WD_ERR2INT				(0x0001 << 5)		//와치독타이마웃 인터럽트 	(1: O, 0: X)
#define WD_ERR2INT_EN			(0x0001 << 5)
#define WD_ERR2INT_DIS			(0x0000 << 5)

#define AH_ERR2INT				(0x0001 << 4)		//진폭 높음 오류 인터럽트 		(1: O, 0: X)
#define AH_ERR2INT_EN			(0x0001 << 4)
#define AH_ERR2INT_DIS			(0x0000 << 4)

#define AL_ERR2INT				(0x0001 << 3)		//진폭 낮음 오류 인터럽트 		(1: O, 0: X)
#define AL_ERR2INT_EN			(0x0001 << 3)
#define AL_ERR2INT_DIS			(0x0000 << 3)

#define ZC_ERR2INT				(0x0001 << 2)		//카운터 제로 오류 인터럽트 	(1: O, 0: X)
#define ZC_ERR2INT_EN			(0x0001 << 2)
#define ZC_ERR2INT_DIS			(0x0000 << 2)

#define DRDY_2INT				(0x0001 << 0)		//데이터 준비 완료 인터럽트 	(1: O, 0: X)
#define DRDY_2INT_EN			(0x0001 << 0)
#define DRDY_2INT_DIS			(0x0000 << 0)


//CONFIG Register (0x1A)
#define ACTIVE_CHAN				(0x0003 << 14)		//활성 채널 선택(CH0(0), CH1(1), CH2(2), CH3(3))
#define ACTIVE_CHAN_CH0			(0x0000 << 14)
#define ACTIVE_CHAN_CH1			(0x0001 << 14)
#define ACTIVE_CHAN_CH2			(0x0002 << 14)
#define ACTIVE_CHAN_CH3			(0x0003	<< 14)

#define SLEEP_MODE_EN			(0x0001 << 13)		//슬립 모드 활성화(활성화(0), 슬립모드(1))
#define SLEEP_MODE_DIS			(0x0000 << 13)

#define RP_OVERRIDE_EN			(0x0001 << 12)		//센서 RP 치환 활성화(중지(0), 가동(1))
#define RP_OVERRIDE_DIS			(0x0000 << 12)

#define SENSOR_ACTIVATE_SEL		(0x0001 << 11)		//센서 활성화 모드 선택(저전력(1))
#define SENSOR_ACTIVATE_LOW		(0x0001 << 11)
#define SENSOR_ACTIVATE_FULL	(0x0000 << 11)

#define AUTO_AMP				(0x0001 << 10)		//자동 진폭 보정 활성화(활성화(0), 비활성화(1))
#define AUTO_AMP_DIS			(0x0001 << 10)		
#define AUTO_AMP_EN				(0x0000 << 10)

#define REF_CLK_SRC				(0x0001 << 9)		//참조 주파수 클럭 선택(내부(0), 외부(1))
#define REF_CLK_SRC_EXT			(0x0001 << 9)
#define REF_CLK_SRC_IN			(0x0000 << 9)

#define INTB					(0x0001 << 7)		//인터럽트 활성화(활성화(0), 비활성화(1))
#define INTB_DIS				(0x0001 << 7)		
#define INTB_EN					(0x0000 << 7)

#define HIGH_CURRENT_DRV		(0x0001 << 6)		//고 전류 센서 구동(일반(0), 고전류(1))
#define HIGH_CURRENT_DRV_EN		(0x0001 << 6)
#define HIGH_CURRENT_DRV_DIS	(0x0000 << 6)

//MUX_CONFIG Register (0x1B)
#define AUTOSCAN_EN				(0x0001 << 15)		//자동 스캔 모드 활성화(싱글채널 연속 변환(0), 다중 채널자동스캔(1))
#define AUTOSCAN_DIS			(0x0000 << 15)

#define RR_SEQUENCE				(0x0002 << 13)		//자동 스캔 채널 선택(CH0 & CH1(0), CH0 & CH1 & CH2(1), CH0 & CH1 & CH2& CH3(2), CH0 & CH1(3))
#define RR_SEQUENCE_CH0_1		(0x0000 << 13)
#define RR_SEQUENCE_CH0_2		(0x0001 << 13)
#define RR_SEQUENCE_CH0_3		(0x0002 << 13)

#define DEGLITCH				(0x0007 << 0)		//입력 Deglitch 필터 밴드 폭(1M(1), 3.3M(4), 10M(5), 33M(7))
#define DEGHITCH_1M				(0x0001 << 0)
#define DEGHITCH_3R3M			(0x0004 << 0)
#define DEGHITCH_10M			(0x0005 << 0)
#define DEGHITCH_33M			(0x0007 << 0)


//RESET_DEV Register (0x1C)
#define RESET_DEVICE			(0x0001 << 15)		//디바이스 리셋(리셋(1))
#define OUTPUT_GAIN				(0x0002 << 9)		//출력 게인 설정(1(0), 4(1), 8(2), 16(3))
#define OUTPUT_GAIN_1			(0x0000 << 9)
#define OUTPUT_GAIN_4			(0x0001 << 9)
#define OUTPUT_GAIN_8			(0x0002 << 9)
#define OUTPUT_GAIN_16			(0x0003 << 9)

//DEVICE_CURRENT_CHx Register(CH0: 0x1E, CH1: 0x1F, Ch2: 0x20(LDC1314 Only), CH3:0x21(LDC1314 Only))
#define CHx_IDRIVE				(0x001F << 11)		//CHx L-C 센서 구동 전류 
#define CHx_INIT_IDRIVE			(0x001F << 6)		//CHx 센서 구동 전류

//MANUFACTURER_ID Register (0x7E)
#define MANUFACTURERID			(0x5449 << 0)

//DEVICE_ID Register (0x7F)
#define DEVICEID				(0x3055 << 0)


#define LDC_SD_H				DioSet(pADI_GP1, 0x80)
#define LDC_SD_L			    DioClr(pADI_GP1, 0x80)
#define LDC_SCL_H				DioSet(pADI_GP2, 0x01)
#define LDC_SCL_L				DioClr(pADI_GP2, 0x01)
#define LDC_SCK_H				DioSet(pADI_GP2, 0x02)
#define LDC_SCK_L				DioClr(pADI_GP2, 0x02)

#define LDC_ADDR					0x2B

#define CH0							0
#define CH1							1
#define CH2							2
#define CH3							3

#define FIN_DIVIDER					0x0002
#define FREF_DIVIDER				0x0004
#define SETTLECOUNT					0x000A
#define RCOUNT						0x04D6
#define OFFSET_POINT				0xD8EF


void LDC_Init(uint16_t offset);
void Set_Config(uint16_t conf);
uint16_t Get_Config();
uint16_t Get_Status();
void Set_Error_Config(uint16_t err_conf);
uint16_t Get_Error_Config();
void Set_Divider(uint8_t ch, uint16_t fin_div, uint16_t fref_div);
uint16_t Get_Divider(uint8_t ch);
void Set_SettleCount(uint8_t ch, uint16_t settleCnt);
uint16_t Get_SettleCount(uint8_t ch);
void Set_RCount(uint8_t ch, uint16_t rCnt);
uint16_t Get_RCount(uint8_t ch);
void Set_IDrive(uint8_t ch, uint16_t iDrv, uint16_t init_iDrv);
uint16_t Get_IDreive(uint8_t ch);
void Set_MUX(uint16_t mux);
uint16_t Get_MUX();
void Set_Gain(uint16_t gain);
uint16_t Get_Data(uint8_t ch);
void Set_Offset(uint8_t ch, uint16_t offset);
uint16_t Get_Offset(uint8_t ch);
void Reset_Device();
void Offset_Setting(uint8_t ch, uint16_t offset);
void Gain_Setting(uint16_t gain);
uint16_t Read_Data(uint8_t addr);
void Write_Data(uint16_t data, uint8_t addr);

#endif